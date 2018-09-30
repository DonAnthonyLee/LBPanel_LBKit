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
 * File: LBScopeItem.cpp
 * Description:
 *
 * --------------------------------------------------------------------------*/

#include <lbk/LBScopeItem.h>
#include <lbk/LBScopeHandler.h>


LBScopeItem::LBScopeItem()
	: fHandler(NULL), fVisible(true)
{
}


LBScopeItem::~LBScopeItem()
{
	/*
	 * WARNING: NO GUARANTEE for ScopeHandler !!!
	 */
#if 0
	if(fHandler != NULL)
		fprintf(stderr, "[LBScopeItem]: Item should be removed from handler before deleting !\n");
	}
#endif
}


LBScopeHandler*
LBScopeItem::Handler() const
{
	return fHandler;
}


void
LBScopeItem::Show()
{
	if(fVisible == false)
	{
		if(fHandler != NULL)
			fHandler->ShowItem(this);
		else
			fVisible = true;
	}
}


void
LBScopeItem::Hide()
{
	if(fVisible)
	{
		if(fHandler != NULL)
			fHandler->HideItem(this);
		else
			fVisible = false;
	}
}


bool
LBScopeItem::IsVisible() const
{
	if(fVisible == false) return false;
	if(fHandler != NULL) return fHandler->IsItemVisible(this);
	return true;
}

