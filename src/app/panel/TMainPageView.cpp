/* --------------------------------------------------------------------------
 *
 * Panel application for little board
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
 * File: TMainPageView.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/sockios.h>

#include "TMainPageView.h"
#include "TLogoView.h"

#ifdef ETK_MAJOR_VERSION
	#define BDirectory		EDirectory
#endif

// TODO: Specified by config
#define BOARD_CPU_THERMAL_ZONE		"/sys/devices/virtual/thermal/thermal_zone0"

#define POWER_REQUESTED_CONFIRM_MSG	'pwof'
#define POWER_OFF_MSG			'pwOF'


TMainPageView::TMainPageView(const char *name)
	: LBPageView(name),
	  fTabIndex(0),
	  f24Hours(false), fShowSeconds(false), fShowTimestamp(0),
	  fInterfacesCount(0),
	  fCPUTime(NULL)
{
	SetNavButtonIcon(0, LBK_ICON_LEFT);
	SetNavButtonIcon(2, LBK_ICON_RIGHT);

	TLogoView *view = new TLogoView("logo");
	AddStickView(view);
	view->StandIn();

	BDirectory dir("/sys/bus/cpu/devices");
	if((fCPUSCount = dir.CountEntries()) > 0)
	{
		fCPUTime = (bigtime_t*)malloc(sizeof(bigtime_t) * fCPUSCount * 2);
		bzero(fCPUTime, sizeof(bigtime_t) * fCPUSCount * 2);
	}

	// TODO: monitor the count
	fInterfacesCount = GetInterfacesCount();
}


TMainPageView::~TMainPageView()
{
	if(fCPUTime != NULL)
		free(fCPUTime);
}


void
TMainPageView::GetTime(BString *strDate, BString *strTime, BString *strWeek) const
{
	time_t timer = (time_t)(real_time_clock_usecs() / (bigtime_t)1000000);
	struct tm t;
	if(localtime_r(&timer, &t) == NULL) return;

	if(strDate != NULL)
	{
		strDate->Truncate(0);
		*strDate << 1900 + t.tm_year << "年" << 1 + t.tm_mon << "月" << t.tm_mday << "日";
	}

	if(strTime != NULL)
	{
		strTime->Truncate(0);
		if(fShowSeconds == false && f24Hours == false)
		{
			if(t.tm_hour < 12)
				*strTime << (t.tm_hour < 6 ? "凌晨" : "上午");
			else
				*strTime << (t.tm_hour < 18 ? (t.tm_hour < 14 ? "中午" : "下午") : "晚上");
		}

		char buf[16];
		bzero(buf, sizeof(buf));

		if(fShowSeconds)
			snprintf(buf, sizeof(buf), "%02d:%02d:%02d",
				 (f24Hours ? t.tm_hour : (t.tm_hour % 12)), t.tm_min, t.tm_sec);
		else
			snprintf(buf, sizeof(buf), "%02d:%02d",
				 (f24Hours ? t.tm_hour : (t.tm_hour % 12)), t.tm_min);
		strTime->Append(buf);
	}

	if(strWeek != NULL)
	{
		const char *desc[] = {"日", "一", "二", "三", "四", "五", "六"};

		strWeek->SetTo("星期");
		strWeek->Append(desc[t.tm_wday]);
	}
}


void
TMainPageView::Pulse()
{
	BRect r;

	if(fTabIndex == 0) // Date And Time
	{
		BString strDate, strTime, strWeek;

		GetTime(&strDate, &strTime, &strWeek);

		if(strDate != fDate)
		{
			fDate = strDate;

			r = LBView::Bounds();
			r.bottom = 13;
			Invalidate(r);
		}

		if(strTime != fTime)
		{
			fTime = strTime;

			r = LBView::Bounds();
			r.top = 14;
			r.bottom -= 14;
			Invalidate(r);
		}

		if(strWeek != fWeek)
		{
			fWeek = strWeek;

			r = LBView::Bounds();
			r.top = r.bottom - 13;
			Invalidate(r);
		}

		if(fShowTimestamp == 0 || real_time_clock_usecs() - fShowTimestamp < 2000000) return;
		fShowTimestamp = 0;

		cast_as(Looper(), LBApplication)->SetPulseRate(fShowSeconds ? 1000000 : 10000000);
		HideNavButton(0);
		HideNavButton(2);
	}
	else if(fTabIndex == -1) // Board Info
	{
		r = Bounds();
		r.top += r.Height() / 3.f;

		Invalidate(r);
	}
	else if(fTabIndex == -2) // CPU Info
	{
		Invalidate(Bounds());
	}
}


void
TMainPageView::DrawDateAndTime(BRect rect)
{
	uint16 w;

	if(fDate.Length() == 0)
		GetTime(&fDate, &fTime, &fWeek);

	BRect r = LBView::Bounds();
	r.bottom = 13;
	if(r.Intersects(rect) && fDate.Length() > 0)
	{
		SetFontSize(12);
		w = StringWidth(fDate.String());
		DrawString(fDate.String(), BPoint(r.Center().x - w / 2.f, 1));
	}

	r.OffsetBy(0, r.Height() + 1);
	r.bottom = LBView::Bounds().bottom - 14;
	if(r.Intersects(rect) && fTime.Length() > 0)
	{
		FillRect(r & rect);

		SetFontSize(fShowSeconds ? 24 : 32);
		if(fShowSeconds == false && f24Hours == false && fTime.Length() > 6)
		{
			uint16 w1;
			BString str(fTime, 6);

			SetFontSize(12);
			w1 = StringWidth(str.String());
			SetFontSize(24);
			w = w1 + 2 + StringWidth(fTime.String() + 6);

			BPoint pt = r.Center() - BPoint(w / 2.f, 23 / 2.f);
			DrawString(fTime.String() + 6, pt + BPoint(w1 + 2, 0), true);
			SetFontSize(12);
			DrawString(str.String(), pt + BPoint(0, 10), true);
		}
		else
		{
			SetFontSize(fShowSeconds ? 24 : 32);
			w = StringWidth(fTime.String());
			DrawString(fTime.String(), r.Center() - BPoint(w / 2.f, (fShowSeconds ? 23 : 31) / 2.f), true);
		}
	}

	r.OffsetBy(0, r.Height() + 1);
	r.bottom = LBView::Bounds().bottom;
	if(r.Intersects(rect) && fWeek.Length() > 0)
	{
		SetFontSize(12);
		w = StringWidth(fWeek.String());
		DrawString(fWeek.String(),
			   r.Center() - BPoint(w / 2.f, 11 / 2.f));
	}
}


void
TMainPageView::DrawBoardInfo(BRect rect)
{
	BString str;
	uint16 w;
	BFile f;
	char buffer[128];

	SetFontSize(12);

	// Title
	BRect r = LBView::Bounds();
	r.top = Bounds().bottom + 1;
	if(r.Intersects(rect))
	{
		str.SetTo("机器信息");

		w = StringWidth(str.String());
		DrawString(str.String(),
			   r.Center() - BPoint(w / 2.f, 11 / 2.f));
	}

	r = Bounds();
	r.bottom = r.top + r.Height() / 3.f - 1.f;

	// hostname
	if(r.Intersects(rect))
	{
		str.SetTo("主机: ");

		bzero(buffer, sizeof(buffer));
		gethostname(buffer, sizeof(buffer));
		str.Append(buffer[0] == 0 ? "[无]" : buffer);

		BRect r1 = r.InsetByCopy(2, 2);
		DrawString(str.String(),
			   BPoint(r1.left, r1.top + (r1.Height() - 11.f) / 2.f));
	}

	// loadavg
	r.OffsetBy(0, r.Height() + 1);
	if(r.Intersects(rect))
	{
		str.SetTo("负载: ");

		if(f.SetTo("/proc/loadavg", B_READ_ONLY) == B_OK)
		{
			bzero(buffer, sizeof(buffer));
			f.Read(buffer, sizeof(buffer));
			f.Unset();

			if(buffer[0] != 0)
			{
				str.Append(buffer);

				int32 found = -1;
				for(int32 k = 0; k < 4; k++)
					found = str.FindFirst(" ", found + 1);
				if(found > 0)
					str.Truncate(found);
			}
		}

		BRect r1 = r.InsetByCopy(2, 2);
		DrawString(str.String(),
			   BPoint(r1.left, r1.top + (r1.Height() - 11.f) / 2.f));
	}

	// mem
	r.OffsetBy(0, r.Height() + 1);
	if(r.Intersects(rect))
	{
		str.SetTo("内存: ");

		BRect r1 = r.InsetByCopy(2, 2);
		DrawString(str.String(),
			   BPoint(r1.left, r1.top + (r1.Height() - 11.f) / 2.f));
		r1.left += StringWidth(str.String()) + 1;

		uint32 total_mem = 0; // kB
		uint32 free_mem = 0; // kB

		if(f.SetTo("/proc/meminfo", B_READ_ONLY) == B_OK)
		{
			bzero(buffer, sizeof(buffer));
			f.Read(buffer, sizeof(buffer));
			f.Unset();

			BString lines(buffer);
			int32 offset = 0;

			for(int32 k = 0 ; k < 2; k++)
			{
				int32 found = lines.FindFirst("\n", offset);
				if(found <= offset) break;

				BString line(lines.String() + offset, found - offset);
				offset = found + 1;

				found = line.FindLast(" ", line.Length() - 4);
				if(found < 0) break;

				line.Remove(0, found + 1);
				line.Truncate(line.Length() - 3);

				if(k == 0)
					total_mem = (uint32)strtoul(line.String(), NULL, 10);
				else
					free_mem = (uint32)strtoul(line.String(), NULL, 10);
			}
		}

		str.Truncate(0);
		str << ((total_mem - free_mem) >> 10) << "/" << (total_mem >> 10) << " MB";
		DrawString(str.String(),
			   r1.Center() - BPoint(StringWidth(str.String()) / 2.f, 11 / 2.f));

		r1.InsetBy(0, -1);
		if(total_mem > free_mem)
		{
			StrokeRect(r1);
			r1.InsetBy(1, 1);
			r1.right -= r1.Width() * (float)free_mem / (float)total_mem;
		}
		InvertRect(r1);
	}
}


void
TMainPageView::DrawCPUInfo(BRect rect)
{
	BString str;
	uint16 w;
	BFile f;
	char buffer[4096];

	SetFontSize(12);

	// Title
	BRect r = LBView::Bounds();
	r.top = Bounds().bottom + 1;
	if(r.Intersects(rect))
	{
		str.SetTo("CPU 信息");

		w = StringWidth(str.String());
		DrawString(str.String(),
			   r.Center() - BPoint(w / 2.f, 11 / 2.f));
	}

	if(fCPUSCount <= 0)
	{
		str.SetTo("无法获取 CPU 信息!");

		w = StringWidth(str.String());
		DrawString(str.String(),
			   Bounds().Center() - BPoint(w / 2.f, 11 / 2.f));

		return;
	}

	r = Bounds();
	r.right = r.left + r.Width() / (fCPUSCount + 1) - 1.f;

	// CPU Temperature
	if(r.Intersects(rect))
	{
		int trip_points[4];
		int trip_points_count = 0;
		int trip_point_max = 0;

		for(size_t k = 0; k < sizeof(trip_points) / sizeof(trip_points[0]); k++)
		{
			BString p(BOARD_CPU_THERMAL_ZONE);
			p << "/trip_point_" << k << "_temp";
			if(f.SetTo(p.String(), B_READ_ONLY) != B_OK) break;

			bzero(buffer, sizeof(buffer));
			f.Read(buffer, sizeof(buffer));
			f.Unset();

			if(buffer[0] == 0) break;
			trip_points[trip_points_count] = atoi(buffer) / 1000;
			if(trip_points[trip_points_count] > trip_point_max)
				trip_point_max = trip_points[trip_points_count];

			trip_points_count++;
		}
		if(trip_points_count == 0 || trip_point_max <= 0) // error
		{
			trip_points_count = 1;
			trip_point_max = trip_points[0] = 100;
		}

		int cur_temp = 0;
		if(f.SetTo(BOARD_CPU_THERMAL_ZONE "/temp", B_READ_ONLY) == B_OK)
		{
			bzero(buffer, sizeof(buffer));
			f.Read(buffer, sizeof(buffer));
			f.Unset();

			if(buffer[0] != 0)
				cur_temp = atoi(buffer) / 1000;
		}

		// bar
		BRect r1 = r.InsetByCopy((r.Width() - 5) / 2.f, 1);
		r1.bottom -= 11;
		StrokeRect(r1);
		for(int k = 0; k < trip_points_count; k++)
		{
			if(trip_points[k] <= 0 || trip_points[k] == trip_point_max) continue;

			float offset = (float)(trip_point_max - trip_points[k]) / (float)trip_point_max;
			offset *= r1.Height();

			FillRect(BRect(r1.LeftTop() + BPoint(0, offset), r1.RightTop() + BPoint(0, offset)));
		}
		r1.InsetBy(1, 1);
		if(cur_temp < trip_point_max)
			r1.top += r1.Height() * (float)(trip_point_max - max_c(cur_temp, 10)) / (float)trip_point_max;
		InvertRect(r1);

		// text
		r1 = r;
		r1.top = r1.bottom - 11;
		str.Truncate(0);
		str << cur_temp << "°C";
		if((w = StringWidth(str.String())) > r.Width())
		{
			str.Truncate(0);
			str << cur_temp << "C";
			if((w = StringWidth(str.String())) > r.Width())
			{
				str.Truncate(0);
				str << cur_temp;
				w = StringWidth(str.String());
			}
		}

		DrawString(str.String(),
			   r1.Center() - BPoint(w / 2.f, 11 / 2.f));
	}


	if(f.SetTo("/proc/stat", B_READ_ONLY) == B_OK)
	{
		bzero(buffer, sizeof(buffer));
		f.Read(buffer, sizeof(buffer));
		f.Unset();

		if(buffer[0] == 0)
		{
			str.SetTo("错误!");

			r.left += r.Width() + 1;
			r.right = Bounds().right;
			w = StringWidth(str.String());
			DrawString(str.String(),
				   r.Center() - BPoint(w / 2.f, 11 / 2.f));

			return;
		}
	}

	// CPU load
	BString strStat(buffer);
	for(int32 k = 0; k < fCPUSCount; k++)
	{
		r.OffsetBy(r.Width() + 1, 0);
		if(r.Intersects(rect) == false) continue;

		str.SetTo("cpu");
		str << k << " ";

		int32 offset = strStat.FindFirst(str.String());
		if(offset < 0) break;
		offset += str.Length();

		int32 found = strStat.FindFirst('\n', offset);
		if(found < 0) break;

		str.SetTo(strStat.String() + offset, found - offset);
		offset = 0;

		int percent = 0;
		bigtime_t idle = 0, total = 0;
		for(int m = 0; m < 7; m++)
		{
			found = str.FindFirst(' ', offset);
			if(found <= offset) break;

			BString tStr(str.String() + offset, found - offset);
			offset = found + 1;

			bigtime_t t = (bigtime_t)atoll(tStr.String());
			total += t;
			if(m == 3) idle = t;
		}
		if(fCPUTime[k * 2] < total && fCPUTime[k * 2 + 1] < idle)
			percent = (int)((bigtime_t)100 * (idle - fCPUTime[k * 2 + 1]) / (total - fCPUTime[k * 2]));
		fCPUTime[k * 2] = total;
		fCPUTime[k * 2 + 1] = idle;

		// bar
		BRect r1 = r.InsetByCopy((r.Width() - 5) / 2.f, 1);
		r1.bottom -= 11;
		StrokeRect(r1);
		r1.InsetBy(1, 1);
		if(percent > 0)
			r1.top += r1.Height() * (float)percent / 100.f;
		FillRect(r1);

		// text
		r1 = r;
		r1.top = r1.bottom - 11;
		str.SetTo("核");
		str << k + 1;
		if((w = StringWidth(str.String())) > r.Width())
		{
			str.SetTo("U");
			str << k + 1;
			if((w = StringWidth(str.String())) > r.Width())
			{
				str.Truncate(0);
				str << k + 1;
				w = StringWidth(str.String());
			}
		}

		DrawString(str.String(),
			   r1.Center() - BPoint(w / 2.f, 11 / 2.f));
	}
}


void
TMainPageView::DrawInterfaceInfo(BRect rect, int32 id)
{
	BString str;
	uint16 w;

	BString ifname;
	GetInterfaceName(ifname, id);

	SetFontSize(12);

	// Title
	BRect r = LBView::Bounds();
	r.top = Bounds().bottom + 1;
	if(r.Intersects(rect))
	{
		if(ifname.Length() > 0)
			str.Append(ifname);
		else
			str << "接口" << id + 1;

		w = StringWidth(str.String());
		DrawString(str.String(),
			   r.Center() - BPoint(w / 2.f, 11 / 2.f));
	}

	if(ifname.Length() == 0)
	{
		str.SetTo("无法获取接口信息!");

		w = StringWidth(str.String());
		DrawString(str.String(),
			   Bounds().Center() - BPoint(w / 2.f, 11 / 2.f));

		return;
	}

	r = Bounds();
	r.bottom = r.top + r.Height() / 3.f - 1.f;

	// MAC
	if(r.Intersects(rect))
	{
		str.SetTo("MAC: ");

		BString hwaddr;
		GetInterfaceHWAddr(hwaddr, ifname.String());
		str.Append(hwaddr.Length() == 0 ? "[无]" : hwaddr.String());

		BRect r1 = r.InsetByCopy(2, 2);
		DrawString(str.String(),
			   BPoint(r1.left, r1.top + (r1.Height() - 11.f) / 2.f));
	}

	// IPv4
	r.OffsetBy(0, r.Height() + 1);
	if(r.Intersects(rect))
	{
		str.SetTo("IPv4: ");

		BString ipaddr;
		GetInterfaceIPv4(ipaddr, ifname.String());
		str.Append(ipaddr.Length() == 0 ? "[无]" : ipaddr.String());

		BRect r1 = r.InsetByCopy(2, 2);
		DrawString(str.String(),
			   BPoint(r1.left, r1.top + (r1.Height() - 11.f) / 2.f));
	}

	// IPv6
	r.OffsetBy(0, r.Height() + 1);
	if(r.Intersects(rect))
	{
		str.SetTo("IPv6: ");

		BString ipaddr;
		GetInterfaceIPv6(ipaddr, ifname.String());

		if(ipaddr.Length() == 0)
		{
			str << "[无]";

			BRect r1 = r.InsetByCopy(2, 2);
			DrawString(str.String(),
				   BPoint(r1.left, r1.top + (r1.Height() - 11.f) / 2.f));
		}
		else
		{
			BRect r1 = r.InsetByCopy(2, 2);
			w = StringWidth(str.String());

			DrawString(str.String(),
				   BPoint(r1.left, r1.top + (r1.Height() - 11.f) / 2.f));
			r1.left += w + 1.f;

			BString line1, line2;
			while(ipaddr.Length() > 0)
			{
				int32 found = ipaddr.FindFirst(":");
				if(found < 0) found = ipaddr.Length();

				switch(found)
				{
					case 0:
						break;

					case 3:
						line1.Append("0");
					case 4:
						line1.Append(ipaddr.String(), found - 2);
						line2.Append(ipaddr.String() + (found - 2), 2);
						break;

					case 1:
						line2.Append("0");
					case 2:
						line1.Append("00");
						line2.Append(ipaddr.String(), found);
						break;

					default: // should never happen
						break;
				}

				if(found < ipaddr.Length())
				{
					line1.Append(".");
					line2.Append("'");
				}

				ipaddr.Remove(0, min_c(found + 1, ipaddr.Length()));
			}

			SetFontSize(8);
			DrawString(line1.String(),
				   BPoint(r1.left, r1.top + (r1.Height() - 17.f) / 2.f));
			r1.top += (r1.Height() - 17.f) / 2.f + 9.f;
			DrawString(line2.String(),
				   BPoint(r1.left, r1.top + (r1.Height() - 7.f) / 2.f));
		}
	}
}


void
TMainPageView::Draw(BRect updateRect)
{
	LBPageView::Draw(updateRect);

	switch(fTabIndex)
	{
		case -2:
			DrawCPUInfo(updateRect);
			break;

		case -1:
			DrawBoardInfo(updateRect);
			break;

		case 0:
			DrawDateAndTime(updateRect);
			break;

		default:
			DrawInterfaceInfo(updateRect, fTabIndex - 1);
	}
}


void
TMainPageView::KeyDown(uint8 key, uint8 clicks)
{
	LBPageView::KeyDown(key, clicks);

	if(clicks == 0xff && key == 1) // K2 long press
	{
		LBAlertView *view = new LBAlertView("关闭机器",
						    "是否确定\n进行关机操作?",
						    LBK_ICON_NO, LBK_ICON_YES, LBK_ICON_NONE,
						    B_WARNING_ALERT);
		view->SetInvoker(new BInvoker(new BMessage(POWER_REQUESTED_CONFIRM_MSG), this));
		view->SetName("PowerOffRequested");
		AddStickView(view);
		view->StandIn();

		printf("[TMainPageView]: Power off requested.\n");
	}

	fShowTimestamp = real_time_clock_usecs();
	if(fTabIndex > -2)
		ShowNavButton(0);
	else
		HideNavButton(0);

	if(fTabIndex < fInterfacesCount)
		ShowNavButton(2);
	else
		HideNavButton(2);
}


void
TMainPageView::KeyUp(uint8 key, uint8 clicks)
{
	LBPageView::KeyUp(key, clicks);

	int32 saveIndex = fTabIndex;

	if(clicks == 1)
	{
		if(key == 0 && fTabIndex > -2) // Left
		{
			fTabIndex--;
		}
		else if(key == 2 && fTabIndex < fInterfacesCount) // Right
		{
			fTabIndex++;
		}
		else if(key == 1) // Setup
		{
			// TODO
		}
	}
	else if(clicks != 0xff)
	{
		if(key == 1) // Home
		{ 
			fTabIndex = 0;
		}
		else if(key == 0) // Left Switch
		{
			SwitchToPrevPage();
			return;
		}
		else if(key == 2) // Right Switch
		{
			SwitchToNextPage();
			return;
		}
	}

	if(saveIndex != fTabIndex)
	{
		if(fTabIndex <= 0)
			cast_as(Looper(), LBApplication)->SetPulseRate(fTabIndex == 0 ? 500000 : 1000000);
		else
			cast_as(Looper(), LBApplication)->SetPulseRate(0);

		fShowTimestamp = real_time_clock_usecs();
		if(fTabIndex > -2)
			ShowNavButton(0);
		else
			HideNavButton(0);

		if(fTabIndex < fInterfacesCount)
			ShowNavButton(2);
		else
			HideNavButton(2);

		Invalidate();
	}
}


void
TMainPageView::Set24Hours(bool state)
{
	if(f24Hours != state)
	{
		f24Hours = state;
		if(fTabIndex == 0) Invalidate();
	}
}


void
TMainPageView::ShowSeconds(bool state)
{
	if(fShowSeconds != state)
	{
		fShowSeconds = state;
		if(fTabIndex == 0) Invalidate();
	}
}


void
TMainPageView::Activated(bool state)
{
	LBPageView::Activated(state);

	if(IsStoodIn() == false)
	{
		if(state == false || fTabIndex > 0)
			cast_as(Looper(), LBApplication)->SetPulseRate(0);
		else
			cast_as(Looper(), LBApplication)->SetPulseRate(fTabIndex == 0 ? 500000 : 1000000);
	}
	else if(strcmp(StandingInView()->Name(), "logo") == 0) // showing logo
	{
		cast_as(Looper(), LBApplication)->SetPulseRate(50000);
	}

	if(state)
		SetPowerState(true);
}


void
TMainPageView::MessageReceived(BMessage *msg)
{
	int32 which = -1;
	uint8 clicks = 0;
	LBView *view;

	switch(msg->what)
	{
		case POWER_REQUESTED_CONFIRM_MSG:
			if(msg->FindInt8("clicks", (int8*)&clicks) != B_OK) break;
			if(msg->FindInt32("which", &which) != B_OK) break;
			if(clicks > 1) break;

			view = FindStickView("PowerOffRequested");
			if(view == NULL) break;
			view->StandBack();
			RemoveStickView(view);
			delete view;

			if(which != 1) break;
			view = new LBAlertView("关闭机器",
					       "正在关机，请稍候...",
					       LBK_ICON_NONE, LBK_ICON_NONE, LBK_ICON_NONE,
					       B_EMPTY_ALERT);
			AddStickView(view);
			view->StandIn();

			Looper()->PostMessage(POWER_OFF_MSG, this);
			break;

		case POWER_OFF_MSG:
			if(msg->HasBool("delay") == false) // in order to show the "Shutting down..."
			{
				msg->AddBool("delay", true);
				Looper()->PostMessage(msg, this);
				break;
			}

			printf("[TMainPageView]: Going to power off !\n");
			system("poweroff");
			break;

		case LBK_VIEW_STOOD_BACK:
			Activated(IsActivated()); // update pulse rate
			break;

		default:
			LBPageView::MessageReceived(msg);
	}
}


int32
TMainPageView::GetInterfacesCount() const
{
	BString str;
	BFile f;

	if(f.SetTo("/proc/net/dev", B_READ_ONLY) == B_OK)
	{
		char buffer[4096];
		bzero(buffer, sizeof(buffer));
		f.Read(buffer, sizeof(buffer));
		f.Unset();

		str.SetTo(buffer);
	}

	int32 lines = 0, offset = 0, found;
	while((found = str.FindFirst('\n', offset)) > offset)
	{
		offset = found + 1;
		lines++;
	}
	if(offset < str.Length()) lines++;
	if(str.FindFirst(" lo: ") > 0) lines--; // ignore "local"

	return(lines - 2);
}


bool
TMainPageView::GetInterfaceName(BString &ifname, int32 id) const
{
	BString str;
	BFile f;

	if(id < 0) return false;

	if(f.SetTo("/proc/net/dev", B_READ_ONLY) == B_OK)
	{
		char buffer[4096];
		bzero(buffer, sizeof(buffer));
		f.Read(buffer, sizeof(buffer));
		f.Unset();

		str.SetTo(buffer);
	}

	int32 lines = 0, offset = 0, found;
	while((found = str.FindFirst('\n', offset)) > offset)
	{
		offset = found + 1;
		lines++;

		if(lines < 2 + id) continue;
		found = str.FindFirst(": ", offset);
		if(found <= offset) break;

		str.Remove(0, offset);
		str.Truncate(found - offset);
		str.RemoveAll(" ");
		if(str.Length() == 0) break;
		if(str == "lo") // ignore "local"
		{
			id++;
			continue;
		}

		ifname = str;
		return true;
	}

	return false;
}


bool
TMainPageView::GetInterfaceHWAddr(BString &hwaddr, const char *ifname) const
{
	if(ifname == NULL || *ifname == 0) return false;
	if(strcmp("lo", ifname) == 0) return false;

	struct ifreq data;
	bzero(&data, sizeof(data));
	strcpy(data.ifr_name, ifname);

	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0) return false;

	int err = ioctl(fd, SIOCGIFHWADDR, &data);
	close(fd);
	if(err != 0) return false;

	char buf[18];
	const uint8 *sa = (const uint8*)data.ifr_hwaddr.sa_data;
	snprintf(buf, sizeof(buf),
		 "%02x:%02x:%02x:%02x:%02x:%02x",
		 sa[0], sa[1], sa[2], sa[3], sa[4], sa[5]);

	hwaddr.SetTo(buf);
	return true;
}


bool
TMainPageView::GetInterfaceIPv4(BString &ipaddr, const char *ifname) const
{
	if(ifname == NULL || *ifname == 0) return false;

	struct ifreq data;
	bzero(&data, sizeof(data));
	strcpy(data.ifr_name, ifname);

	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd < 0) return false;

	int err = ioctl(fd, SIOCGIFADDR, &data);
	close(fd);
	if(err != 0) return false;

	char buf[18];
	const struct sockaddr_in *addr = (const struct sockaddr_in*)&data.ifr_addr;
	uint32 ip = ntohl(addr->sin_addr.s_addr);
	snprintf(buf, sizeof(buf),
		 "%u.%u.%u.%u",
		 ip >> 24, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);

	ipaddr.SetTo(buf);
	return true;
}


bool
TMainPageView::GetInterfaceIPv6(BString &ipaddr, const char *ifname) const
{
	if(ifname == NULL || *ifname == 0) return false;

	BString str, line;
	BFile f;

	if(f.SetTo("/proc/net/if_inet6", B_READ_ONLY) == B_OK)
	{
		char buffer[4096];
		bzero(buffer, sizeof(buffer));
		f.Read(buffer, sizeof(buffer));
		f.Unset();

		str.SetTo(buffer);
	}

	int32 offset = 0, found = 0;
	while(offset < str.Length())
	{
		found = str.FindFirst('\n', offset);

		line.SetTo(str.String() + offset);
		if(found >= offset)
			line.Truncate(found - offset);

		if(line.Length() > 46) do
		{
			int32 pos = line.FindLast(' ');

			if(pos < 45) break;
			if(strcmp(line.String() + pos + 1, ifname) != 0) break;
			if(strncmp(line.String() + 38, " 20 ", 4) != 0) break; // not Link

			ipaddr.Truncate(0);
			for(int k = 0; k < 8; k++)
			{
				ipaddr.Append(line.String() + k * 4, 4);
				if(k != 7) ipaddr << ":";
			}

			ipaddr.RemoveAll("0000");
			if((found = ipaddr.FindFirst(":::")) >= 0)
			{
				offset = found + 3;
				while(!(offset >= ipaddr.Length() || ipaddr[offset] != ':'))
					ipaddr.Remove(offset, 1);
				ipaddr.Remove(found, 1);
			}

			return true;
		} while(false);

		if(found < offset) break;
		offset = found + 1;
	}

	return false;
}

