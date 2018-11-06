/* --------------------------------------------------------------------------
 *
 * Commands using Little Board Application Kit
 * Copyright (C) 2018, Anthony Lee, All Rights Reserved
 *
 * This software is a freeware; it may be used and distributed according to
 * the terms of The MIT License.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * File: lbk-uci2msg.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <stdio.h>
#include <lbk/LBKit.h>


static void show_usage(void)
{
	printf("lbk-uci2msg - Convert uci config to *Message flattened data.\n\n");
	printf("Usage: lbk-uci2msg [-d] uci_config_file msg_data\n\
    -d                         Output the content of *Message.\n\
    uci_config_file            Path of uci config file to read.\n\
    msg_data                   Path of *Message flattened data to write.\n");
}


static status_t file_get_line(BFile &f, BString &line, BString &buf)
{
	int32 found = buf.FindFirst("\n");
	if(found < 0)
	{
		char buffer[2048];
		bzero(&buffer[0], sizeof(buffer));

		ssize_t n = f.Read((void*)&buffer[0], sizeof(buffer) - 1);
		if(n <= 0) return B_ERROR;
		buf.Append(&buffer[0], n);
		if((found = buf.FindFirst("\n")) < 0) return B_ERROR;
	}

	line.Truncate(0);
	if(found > 0) line.SetTo(buf.String(), found);
	line.RemoveAll("\r");

	buf.Remove(0, found + 1);

	return B_OK;
}


static status_t uci_get_next_item(BString &line, int32 &offset, BString &item)
{
	BString retStr, str;
	int32 found;
	char sep;

	do
	{
		if(offset < 0 || offset >= line.Length() - 1) break;

		str.SetTo(line.String() + offset);
		sep = (str[0] == '\"' || str[0] == '\'') ? str[0] : ' ';

		if((found = str.FindFirst(sep, sep == ' ' ? 0 : 1)) <= 0)
		{
			if(sep != ' ') break;
			found = str.Length();
		}
		if(sep != ' ' && !(found >= str.Length() - 1 || str[found + 1] == ' ')) break;

		str.Truncate(found);
		if(sep != ' ') str.Remove(0, 1);

		offset += found + (sep == ' ' ? 1 : 2);
		retStr.SetTo(str);
	} while(false);

	item = retStr;
	return(retStr.Length() > 0 ? B_OK : B_ERROR);
}


static status_t uci_cvt_msg(BFile &fIn, BMessage *msg)
{
	int32 count = 0, found, offset;
	BString tmpBuf, line;
	status_t st;

	while(file_get_line(fIn, line, tmpBuf) == B_OK)
	{
		BString domain, section;

		found = line.FindFirst("config ");
		if(found != 0) continue;

		offset = 7;
		st = uci_get_next_item(line, offset, domain);
		if(st != B_OK) break;

		uci_get_next_item(line, offset, section);

		while(file_get_line(fIn, line, tmpBuf) == B_OK)
		{
			if((found = line.FindFirst("option ")) < 0)
				found = line.FindFirst("list ");

			if(found > 0)
			{
				BString tmpStr(line, found);
				tmpStr.RemoveSet(" \t");
				if(tmpStr.Length() != 0) continue;
				line.Remove(0, found);
				found = 0;
			}

			if(found == 0)
			{
				BString name, value;

				line.ReplaceAll("'\\''", "&quote;");

				offset = (line[0] == 'o' ? 7 : 5);
				st = uci_get_next_item(line, offset, name);
				if(st != B_OK) continue;
				if(name.FindFirst("/") >= 0) continue; // refuse to parse if name contains "/"
				if(name.FindFirst("@") >= 0) continue; // refuse to parse if name contains "@"

				st = uci_get_next_item(line, offset, value);
				if(st != B_OK) continue;

				if(section.Length() > 0)
				{
					name.Prepend("/");
					name.Prepend(section);
				}
				name.Prepend("/");
				name.Prepend(domain);

				value.ReplaceAll("&quote;", "'");

				if(msg->AddString(name.String(), value.String()) != B_OK) return B_ERROR;
				count++;
			}
			else if(line.FindFirst("config ") == 0)
			{
				tmpBuf.Prepend("\n");
				tmpBuf.Prepend(line);
				break;
			}
			else
			{
				line.RemoveSet(" \t");
				if(line.Length() == 0) break;
			}
		}
	}

	return(count > 0 ? B_OK : B_ERROR);
}

extern "C" {

#ifdef CMD_ALL_IN_ONE
int cmd_uci2msg(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	BFile fIn, fOut;
	BMessage msg;
	bool output = false;
	int n;

	for (n = 1; n < argc; n++) {
		if (n < argc - 1 && strcmp(argv[n], "-d") == 0) {
			output = true;
		} else {
			break;
		}
	}
	argc -= (--n);

	if(argc != 3)
	{
		show_usage();
		exit(1);
	}

	fIn.SetTo(argv[n + 1], B_READ_ONLY);
	fOut.SetTo(argv[n + 2], B_CREATE_FILE | B_ERASE_FILE | B_WRITE_ONLY);

	if(fIn.InitCheck() != B_OK || fOut.InitCheck() != B_OK)
	{
		fprintf(stderr, "Unable to open file to read or write !\n");
		exit(1);
	}

	if(uci_cvt_msg(fIn, &msg) != B_OK)
	{
		fprintf(stderr, "Failed to convert uci config to *Message !\n");
		exit(1);
	}

	size_t len = msg.FlattenedSize();
	char *data = (char*)malloc(len);

	if(data == NULL)
		fprintf(stderr, "Unable to alloc buffer !\n");
	else if(msg.Flatten(data, len) == false)
		fprintf(stderr, "Failed to flatten *Message !\n");
	else if(fOut.Write(data, len) != (ssize_t)len)
		fprintf(stderr, "Failed to write !\n");

	if(data != NULL) free(data);

	if(output)
		msg.PrintToStream();

	return 0;
}

}; // extern "C"

