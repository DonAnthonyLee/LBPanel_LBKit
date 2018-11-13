/* --------------------------------------------------------------------------
 *
 * Little Board Application Kit
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
 * File: LBAppSettings.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <string.h>

#include <lbk/LBAppSettings.h>

#ifdef ETK_MAJOR_VERSION
	#undef SEEK_END
	#undef SEEK_CUR
	#undef SEEK_SET

	#define SEEK_SET	E_SEEK_SET
	#define SEEK_CUR	E_SEEK_CUR
	#define SEEK_END	E_SEEK_END
#endif


LBAppSettings::LBAppSettings()
{
}


LBAppSettings::~LBAppSettings()
{
	MakeEmpty();
}


bool
LBAppSettings::AddItem(const char *item, int32 index)
{
	char *data = (item == NULL || *item == 0) ? NULL : strdup(item);

	if(index < 0)
		index = fSettings.CountItems();

	if(fSettings.AddItem(data, index) == false)
	{
		if(data != NULL) free(data);
		return false;
	}

	return true;
}


bool
LBAppSettings::AddItems(BFile *f, int32 index)
{
	if(f == NULL || f->InitCheck() != B_OK) return false;

	if(index < 0)
		index = fSettings.CountItems();

	char *buf = NULL;
	off_t fsize = (off_t)f->Seek(0, SEEK_END);
	f->Seek(0, SEEK_SET);

	if(fsize == (off_t)-1 || fsize > 65535 ||
	   (buf = (char*)malloc((size_t)fsize)) == NULL ||
	   f->Read(buf, fsize) != (ssize_t)fsize)
	{
		if(buf != NULL) free(buf);
		return false;
	}
	*(buf + fsize) = 0;

	BList cfg;
	BString str(buf);
	str.RemoveAll("\r");
	free(buf);

	int32 offset = 0, found;
	char *data;

	while(offset < str.Length())
	{
		found = str.FindFirst("\n", offset);
		if(found < offset)
			found = str.Length();

		data = NULL;

		if(found > offset)
		{
#if 0
			data = strndup(str.String() + offset, found - offset);
#else
			if((data = (char*)malloc(found - offset + 1)) != NULL)
			{
				memcpy(data, str.String() + offset, found - offset);
				*(data + found - offset) = 0;
			}
#endif
		}

		if((found > offset && data == NULL) || cfg.AddItem(data) == false)
		{
			if(data != NULL) free(data);
			goto failed;
		}

		offset = found + 1;
	}

	if(fSettings.AddList(&cfg, index)) return true;

failed:
	for(int32 k = 0; k < cfg.CountItems(); k++)
	{
		data = (char*)cfg.ItemAt(k);
		if(data != NULL) free(data);
	}

	return false;
}


bool
LBAppSettings::AddItems(const LBAppSettings &settings, int32 index)
{
	if(settings.CountItems() == 0) return false;
	if(index < 0)
		index = fSettings.CountItems();

	BList cfg;
	for(int32 k = 0; k < settings.CountItems(); k++)
	{
		char *data = (settings.ItemAt(k) == NULL) ? NULL : strdup(settings.ItemAt(k));
		if(cfg.AddItem(data) == false)
		{
			if(data != NULL) free(data);
			goto failed;
		}
	}

	if(fSettings.AddList(&cfg, index)) return true;

failed:
	for(int32 k = 0; k < cfg.CountItems(); k++)
	{
		char *data = (char*)cfg.ItemAt(k);
		if(data != NULL) free(data);
	}

	return false;
}


void
LBAppSettings::MakeEmpty()
{
	for(int32 k = 0; k < fSettings.CountItems(); k++)
	{
		char *data = (char*)fSettings.ItemAt(k);
		if(data != NULL) free(data);
	}
	fSettings.MakeEmpty();
}


LBAppSettings&
LBAppSettings::operator+=(const LBAppSettings &settings)
{
	AddItems(settings);
	return *this;
}


LBAppSettings&
LBAppSettings::operator=(const LBAppSettings &settings)
{
	MakeEmpty();
	return operator+=(settings);
}


const char*
LBAppSettings::operator[](int index) const
{
	return ItemAt(index);
}


const char*
LBAppSettings::ItemAt(int32 index) const
{
	return (const char*)fSettings.ItemAt(index);
}


status_t
LBAppSettings::GetItemAt(int32 index,
			 BString *name,
			 BString *value,
			 BString *options) const
{
	if(index < 0 || index >= fSettings.CountItems()) return B_BAD_INDEX;
	if(name == NULL) return B_BAD_VALUE;

	do
	{
		BString item((char*)fSettings.ItemAt(index));
		if(item.Length() == 0) break;
		if(item.ByteAt(0) == '#' || item.FindFirst("//") == 0) break; // comment

		int32 found = item.FindFirst("=");
		if(found <= 0 || found == item.Length() - 1) break;

		BString strName(item.String(), found);
		BString strValue(item.String() + found + 1);

		if(options != NULL)
		{
			found = strValue.FindFirst(",");
			if(found == 0) break;
			if(found > 0)
			{
				if(found < strValue.Length() - 1)
					options->SetTo(strValue.String() + found + 1);
				strValue.Truncate(found);
			}
		}

		*name = strName;
		if(value) *value = strValue;

		return B_OK;
	} while(false);

	return B_ERROR;
}


int32
LBAppSettings::CountItems() const
{
	return fSettings.CountItems();
}

