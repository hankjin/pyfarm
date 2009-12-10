// Copyright (C) 2007 Andrey Gruber (aka lamer)

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include "notes.h"
#include "skinlessdrawing.h"

/** Prototypes ********************************************************/
static void CDrawFont(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawFontSize(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawFontColor(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawBold(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawItalic(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawUnderline(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawStrikethrough(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawLeftAlign(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawRightAlign(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawCenterAlign(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawHighlight(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void CDrawBullets(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void SysDrawHide(HDC hdc, P_NOTE_APPEARANCE pA, int offset);
static void SysDrawDelete(HDC hdc, P_NOTE_APPEARANCE pA, int offset);

/*-@@+@@------------------------------------------------------------------
 Procedure: CreateSkinlessSysBitmap
 Created  : Tue Jan  8 12:11:19 2008
 Modified : Tue Jan  8 12:11:19 2008

 Synopsys : Creates bitmap for skinless "delete" and "hide" buttons
 Input    : hwnd - note window handle
            pA - note appearance
            pH - note run-time handles
            phbmp - pointer to HBITMAP will be created
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void CreateSkinlessSysBitmap(HWND hwnd, P_NOTE_APPEARANCE pA, P_NOTE_RTHANDLES pH, HBITMAP * phbmp){
	RECT		rc;
	HDC			hdcMem, hdcSrc;
	HBITMAP		hBmpOld, hBmpMem;
	HBRUSH		hBrush;
	int			offset = 0;

	hdcSrc = GetDC(hwnd);
	hdcMem = CreateCompatibleDC(hdcSrc);
	SetRect(&rc, 0, 0, SMALL_RECT * CMD_SYS_BUTTONS, SMALL_RECT);
	hBmpMem = CreateCompatibleBitmap(hdcSrc, SMALL_RECT * CMD_SYS_BUTTONS, SMALL_RECT);
	SetBkMode(hdcMem, TRANSPARENT);
	hBmpOld = SelectObject(hdcMem, hBmpMem);
	hBrush = CreateSolidBrush(pH->crMask);
	FillRect(hdcMem, &rc, hBrush);
	DeleteObject(hBrush);
	for(int i = 0; i < CMD_SYS_BUTTONS; i++){
		switch(i + IDM_HIDE){
		case IDM_HIDE:
			SysDrawHide(hdcMem, pA, offset);
			break;
		case IDM_DEL:
			SysDrawDelete(hdcMem, pA, offset);
			break;
		}
		offset += SMALL_RECT;
	}
	hBmpMem = SelectObject(hdcMem, hBmpOld);
	*phbmp = hBmpMem;
	DeleteDC(hdcMem);
	ReleaseDC(hwnd, hdcSrc);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CreateSkinlessToolbarBitmap
 Created  : Tue Jan  8 12:11:19 2008
 Modified : Tue Jan  8 12:11:19 2008

 Synopsys : Creates bitmap for skinless toolbar
 Input    : hwnd - note window handle
            pA - note appearance
            pH - note run-time handles
            phbmp - pointer to HBITMAP will be created
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void CreateSkinlessToolbarBitmap(HWND hwnd, P_NOTE_APPEARANCE pA, P_NOTE_RTHANDLES pH, HBITMAP * phbmp){

	RECT		rc;
	HDC			hdcMem, hdcSrc;
	HBITMAP		hBmpOld, hBmpMem;
	int			offset = 0;

	hdcSrc = GetDC(hwnd);
	hdcMem = CreateCompatibleDC(hdcSrc);
	SetRect(&rc, 0, 0, SMALL_RECT * CMD_TBR_BUTTONS, SMALL_RECT);
	hBmpMem = CreateCompatibleBitmap(hdcSrc, SMALL_RECT * CMD_TBR_BUTTONS, SMALL_RECT);
	SetBkMode(hdcMem, TRANSPARENT);
	hBmpOld = SelectObject(hdcMem, hBmpMem);
	FillSkinlessNoteCaption(hdcMem, &rc, pA->crWindow, NULL);
	for(int i = CMD_FONT; i < CMD_TBR_BUTTONS; i++){
		switch(i){
		case CMD_FONT:
			CDrawFont(hdcMem, pA, offset);
			break;
		case CMD_SIZE:
			CDrawFontSize(hdcMem, pA, offset);
			break;
		case CMD_COLOR:
			CDrawFontColor(hdcMem, pA, offset);
			break;
		case CMD_BOLD:
			CDrawBold(hdcMem, pA, offset);
			break;
		case CMD_ITALIC:
			CDrawItalic(hdcMem, pA, offset);
			break;
		case CMD_UNDERLINE:
			CDrawUnderline(hdcMem, pA, offset);
			break;
		case CMD_STRIKETHROUGH:
			CDrawStrikethrough(hdcMem, pA, offset);
			break;
		case CMD_BULLETS:
			CDrawBullets(hdcMem, pA, offset);
			break;
		case CMD_HIGHLIGHT:
			CDrawHighlight(hdcMem, pA, offset);
			break;
		case CMD_A_LEFT:
			CDrawLeftAlign(hdcMem, pA, offset);
			break;
		case CMD_A_CENTER:
			CDrawCenterAlign(hdcMem, pA, offset);
			break;
		case CMD_A_RIGHT:
			CDrawRightAlign(hdcMem, pA, offset);
			break;
		
		}
		offset += SMALL_RECT;
	}
	hBmpMem = SelectObject(hdcMem, hBmpOld);
	*phbmp = hBmpMem;
	DeleteDC(hdcMem);
	ReleaseDC(hwnd, hdcSrc);
}

static void SysDrawDelete(HDC hdc, P_NOTE_APPEARANCE pA, int offset){
	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectObject(hdc, hPen);

	//MoveToEx(hdc, offset + 2, 0, NULL);
	//LineTo(hdc, offset + 10, 0);
	//MoveToEx(hdc, offset + 2, 11, NULL);
	//LineTo(hdc, offset + 10, 11);
	//MoveToEx(hdc, offset + 1, 2, NULL);
	//LineTo(hdc, offset + 11, 2);
	//MoveToEx(hdc, offset + 2, 0, NULL);
	//LineTo(hdc, offset + 2, 3);
	//MoveToEx(hdc, offset + 9, 0, NULL);
	//LineTo(hdc, offset + 9, 3);
	//MoveToEx(hdc, offset + 1, 2, NULL);
	//LineTo(hdc, offset + 1, 7);
	//MoveToEx(hdc, offset + 10, 2, NULL);
	//LineTo(hdc, offset + 10, 7);
	//MoveToEx(hdc, offset + 2, 7, NULL);
	//LineTo(hdc, offset + 2, 12);
	//MoveToEx(hdc, offset + 9, 7, NULL);
	//LineTo(hdc, offset + 9, 12);
	//MoveToEx(hdc, offset + 4, 5, NULL);
	//LineTo(hdc, offset + 8, 8);
	//MoveToEx(hdc, offset + 4, 7, NULL);
	//LineTo(hdc, offset + 8, 5);

	//MoveToEx(hdc, offset + 1, 4, NULL);
	//LineTo(hdc, offset + 4, 1);
	//MoveToEx(hdc, offset + 10, 3, NULL);
	//LineTo(hdc, offset + 9, 1);
	//MoveToEx(hdc, offset + 1, 4, NULL);
	//LineTo(hdc, offset + 4, 11);
	//MoveToEx(hdc, offset + 10, 4, NULL);
	//LineTo(hdc, offset + 9, 11);

	//last good
	//MoveToEx(hdc, offset + 1, 1, NULL);
	//LineTo(hdc, offset + 11, 11);
	//MoveToEx(hdc, offset + 10, 1, NULL);
	//LineTo(hdc, offset, 11);

	MoveToEx(hdc, offset + 1, 2, NULL);
	LineTo(hdc, offset + 11, 12);
	MoveToEx(hdc, offset + 1, 3, NULL);
	LineTo(hdc, offset + 10, 12);
	MoveToEx(hdc, offset + 2, 2, NULL);
	LineTo(hdc, offset + 11, 11);
	MoveToEx(hdc, offset + 1, 11, NULL);
	LineTo(hdc, offset + 11, 1);
	MoveToEx(hdc, offset + 1, 10, NULL);
	LineTo(hdc, offset + 10, 1);
	MoveToEx(hdc, offset + 2, 11, NULL);
	LineTo(hdc, offset + 11, 2);

	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}

static void SysDrawHide(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectObject(hdc, hPen);

	//MoveToEx(hdc, offset + 1, 1, NULL);
	//LineTo(hdc, offset + 11, 11);
	//MoveToEx(hdc, offset + 10, 1, NULL);
	//LineTo(hdc, offset, 11);

	//last good
	//MoveToEx(hdc, offset + 1, 6, NULL);
	//LineTo(hdc, offset + 6, 11);
	//MoveToEx(hdc, offset + 1, 4, NULL);
	//LineTo(hdc, offset + 6, 9);
	////MoveToEx(hdc, offset + 1, 2, NULL);
	////LineTo(hdc, offset + 6, 7);
	//MoveToEx(hdc, offset + 10, 6, NULL);
	//LineTo(hdc, offset + 5, 11);
	//MoveToEx(hdc, offset + 10, 4, NULL);
	//LineTo(hdc, offset + 5, 9);
	////MoveToEx(hdc, offset + 10, 2, NULL);
	////LineTo(hdc, offset + 5, 7);

	//MoveToEx(hdc, offset + 1, 1, NULL);
	//LineTo(hdc, offset + 8, 1);
	//LineTo(hdc, offset + 8, 3);
	//MoveToEx(hdc, offset + 1, 1, NULL);
	//LineTo(hdc, offset + 1, 8);
	//LineTo(hdc, offset + 3, 8);
	//MoveToEx(hdc, offset + 3, 3, NULL);
	//LineTo(hdc, offset + 10, 3);
	//LineTo(hdc, offset + 10, 10);
	//LineTo(hdc, offset + 3, 10);
	//LineTo(hdc, offset + 3, 3);

	MoveToEx(hdc, offset + 0, 6, NULL);
	LineTo(hdc, offset + 12, 6);
	MoveToEx(hdc, offset + 1, 7, NULL);
	LineTo(hdc, offset + 11, 7);
	MoveToEx(hdc, offset + 2, 8, NULL);
	LineTo(hdc, offset + 10, 8);
	MoveToEx(hdc, offset + 3, 9, NULL);
	LineTo(hdc, offset + 9, 9);
	MoveToEx(hdc, offset + 4, 10, NULL);
	LineTo(hdc, offset + 8, 10);
	MoveToEx(hdc, offset + 5, 11, NULL);
	LineTo(hdc, offset + 7, 11);

	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}

static void CDrawFont(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 2, 9, NULL);
	LineTo(hdc, offset + 8, 3);
	LineTo(hdc, offset + 8, 9);
	MoveToEx(hdc, offset + 2, 9, NULL);
	LineTo(hdc, offset + 5, 9);
	MoveToEx(hdc, offset + 7, 9, NULL);
	LineTo(hdc, offset + 10, 9);
	MoveToEx(hdc, offset + 4, 7, NULL);
	LineTo(hdc, offset + 8, 7);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}

static void CDrawFontSize(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 1, 9, NULL);
	LineTo(hdc, offset + 6, 4);
	LineTo(hdc, offset + 6, 9);
	MoveToEx(hdc, offset + 1, 9, NULL);
	LineTo(hdc, offset + 4, 9);
	MoveToEx(hdc, offset + 5, 9, NULL);
	LineTo(hdc, offset + 8, 9);
	MoveToEx(hdc, offset + 3, 7, NULL);
	LineTo(hdc, offset + 6, 7);
	MoveToEx(hdc, offset + 9, 1, NULL);
	LineTo(hdc, offset + 9, 9);
	MoveToEx(hdc, offset + 8, 2, NULL);
	LineTo(hdc, offset + 11, 2);
	MoveToEx(hdc, offset + 8, 7, NULL);
	LineTo(hdc, offset + 11, 7);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}

static void CDrawFontColor(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 2, 3, NULL);
	LineTo(hdc, offset + 10, 3);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(255, 128, 64));
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 2, 4, NULL);
	LineTo(hdc, offset + 10, 4);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 2, 5, NULL);
	LineTo(hdc, offset + 10, 5);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 2, 6, NULL);
	LineTo(hdc, offset + 10, 6);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 2, 7, NULL);
	LineTo(hdc, offset + 10, 7);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 2, 8, NULL);
	LineTo(hdc, offset + 10, 8);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(128, 0, 255));
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 2, 9, NULL);
	LineTo(hdc, offset + 10, 9);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}

static void CDrawBold(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 3, 3, NULL);
	LineTo(hdc, offset + 8, 3);
	MoveToEx(hdc, offset + 3, 4, NULL);
	LineTo(hdc, offset + 9, 4);
	MoveToEx(hdc, offset + 3, 5, NULL);
	LineTo(hdc, offset + 5, 5);
	MoveToEx(hdc, offset + 7, 5, NULL);
	LineTo(hdc, offset + 9, 5);
	MoveToEx(hdc, offset + 3, 6, NULL);
	LineTo(hdc, offset + 8, 6);
	MoveToEx(hdc, offset + 3, 7, NULL);
	LineTo(hdc, offset + 5, 7);
	MoveToEx(hdc, offset + 7, 7, NULL);
	LineTo(hdc, offset + 9, 7);
	MoveToEx(hdc, offset + 3, 8, NULL);
	LineTo(hdc, offset + 9, 8);
	MoveToEx(hdc, offset + 3, 9, NULL);
	LineTo(hdc, offset + 8, 9);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}

static void CDrawItalic(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 6, 2, NULL);
	LineTo(hdc, offset + 8, 2);
	MoveToEx(hdc, offset + 4, 4, NULL);
	LineTo(hdc, offset + 7, 4);
	MoveToEx(hdc, offset + 5, 5, NULL);
	LineTo(hdc, offset + 7, 5);
	MoveToEx(hdc, offset + 5, 6, NULL);
	LineTo(hdc, offset + 7, 6);
	MoveToEx(hdc, offset + 5, 7, NULL);
	LineTo(hdc, offset + 7, 7);
	MoveToEx(hdc, offset + 4, 8, NULL);
	LineTo(hdc, offset + 6, 8);
	MoveToEx(hdc, offset + 4, 9, NULL);
	LineTo(hdc, offset + 7, 9);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}

static void CDrawUnderline(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 3, 2, NULL);
	LineTo(hdc, offset + 3, 7);
	MoveToEx(hdc, offset + 4, 2, NULL);
	LineTo(hdc, offset + 4, 7);
	MoveToEx(hdc, offset + 7, 2, NULL);
	LineTo(hdc, offset + 7, 7);
	MoveToEx(hdc, offset + 8, 2, NULL);
	LineTo(hdc, offset + 8, 7);
	MoveToEx(hdc, offset + 4, 7, NULL);
	LineTo(hdc, offset + 8, 7);
	MoveToEx(hdc, offset + 3, 9, NULL);
	LineTo(hdc, offset + 9, 9);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}

static void CDrawStrikethrough(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 4, 3, NULL);
	LineTo(hdc, offset + 7, 3);
	MoveToEx(hdc, offset + 3, 4, NULL);
	LineTo(hdc, offset + 5, 4);
	MoveToEx(hdc, offset + 7, 4, NULL);
	LineTo(hdc, offset + 8, 4);
	MoveToEx(hdc, offset + 3, 5, NULL);
	LineTo(hdc, offset + 6, 5);
	MoveToEx(hdc, offset + 1, 6, NULL);
	LineTo(hdc, offset + 11, 6);
	MoveToEx(hdc, offset + 5, 7, NULL);
	LineTo(hdc, offset + 8, 7);
	MoveToEx(hdc, offset + 3, 8, NULL);
	LineTo(hdc, offset + 4, 8);
	MoveToEx(hdc, offset + 6, 8, NULL);
	LineTo(hdc, offset + 8, 8);
	MoveToEx(hdc, offset + 4, 9, NULL);
	LineTo(hdc, offset + 7, 9);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}

static void CDrawLeftAlign(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 2, 9, NULL);
	LineTo(hdc, offset + 10, 9);
	MoveToEx(hdc, offset + 2, 7, NULL);
	LineTo(hdc, offset + 6, 7);
	MoveToEx(hdc, offset + 2, 5, NULL);
	LineTo(hdc, offset + 10, 5);
	MoveToEx(hdc, offset + 2, 3, NULL);
	LineTo(hdc, offset + 6, 3);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}

static void CDrawRightAlign(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 2, 9, NULL);
	LineTo(hdc, offset + 10, 9);
	MoveToEx(hdc, offset + 6, 7, NULL);
	LineTo(hdc, offset + 10, 7);
	MoveToEx(hdc, offset + 2, 5, NULL);
	LineTo(hdc, offset + 10, 5);
	MoveToEx(hdc, offset + 6, 3, NULL);
	LineTo(hdc, offset + 10, 3);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}

static void CDrawCenterAlign(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 2, 9, NULL);
	LineTo(hdc, offset + 10, 9);
	MoveToEx(hdc, offset + 4, 7, NULL);
	LineTo(hdc, offset + 8, 7);
	MoveToEx(hdc, offset + 2, 5, NULL);
	LineTo(hdc, offset + 10, 5);
	MoveToEx(hdc, offset + 4, 3, NULL);
	LineTo(hdc, offset + 8, 3);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}

static void CDrawHighlight(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;

	hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 1, 8, NULL);
	LineTo(hdc, offset + 9, 0);
	MoveToEx(hdc, offset + 1, 9, NULL);
	LineTo(hdc, offset + 10, 0);
	MoveToEx(hdc, offset + 1, 10, NULL);
	LineTo(hdc, offset + 11, 0);
	MoveToEx(hdc, offset + 2, 10, NULL);
	LineTo(hdc, offset + 11, 1);
	MoveToEx(hdc, offset + 3, 10, NULL);
	LineTo(hdc, offset + 11, 2);
	MoveToEx(hdc, offset + 4, 10, NULL);
	LineTo(hdc, offset + 11, 3);
	MoveToEx(hdc, offset + 5, 10, NULL);
	LineTo(hdc, offset + 11, 4);
	MoveToEx(hdc, offset + 6, 10, NULL);
	LineTo(hdc, offset + 11, 5);

	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 1, 9, NULL);
	LineTo(hdc, offset + 6, 4);
	LineTo(hdc, offset + 6, 9);
	MoveToEx(hdc, offset + 1, 9, NULL);
	LineTo(hdc, offset + 4, 9);
	MoveToEx(hdc, offset + 5, 9, NULL);
	LineTo(hdc, offset + 8, 9);
	MoveToEx(hdc, offset + 3, 7, NULL);
	LineTo(hdc, offset + 6, 7);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}

static void CDrawBullets(HDC hdc, P_NOTE_APPEARANCE pA, int offset){

	HPEN		hPen, hOldPen;
	RECT		rc;
	HBRUSH		br;

	br = CreateSolidBrush(pA->crCaption);
	SetRect(&rc, offset + 1, 2, offset + 3, 4);
	FillRect(hdc, &rc, br);
	hPen = CreatePen(PS_SOLID, 1, pA->crCaption);
	hOldPen = SelectObject(hdc, hPen);
	MoveToEx(hdc, offset + 5, 3, NULL);
	LineTo(hdc, offset + 11, 3);
	SetRect(&rc, offset + 1, 7, offset + 3, 9);
	FillRect(hdc, &rc, br);
	MoveToEx(hdc, offset + 5, 8, NULL);
	LineTo(hdc, offset + 11, 8);
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
	DeleteObject(br);
}


