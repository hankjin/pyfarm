// Copyright (C) 2008 Andrey Gruber (aka lamer)

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

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include "notes.h"
#include "skin.h"

#define	_AG_DOCK_ARROW_CLASS		L"_ag_dock_arrow_class"

static LRESULT CALLBACK DArrow_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void DArrow_OnDestroy(HWND hwnd);
static void DArrow_OnPaint(HWND hwnd);
static void DArrow_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
static BOOL RegisterDArrow(HINSTANCE hInstance);
static void MoveArrow(HWND hwnd, int pos, HBITMAP hbmp);
static void ShiftNotesOnArrowClick(HWND hwnd);

HWND CreateDArrow(HINSTANCE hInstance, HWND hParent, int pos){
	HWND		hDArrow = NULL;
	HBITMAP		hbmp;

	hDArrow = CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED, _AG_DOCK_ARROW_CLASS, NULL, WS_POPUP, 0, 0, 48, 48, hParent, NULL, hInstance, NULL);
	if(hDArrow == NULL){
		if(GetLastError() == ERROR_CANNOT_FIND_WND_CLASS){
			if(!RegisterDArrow(hInstance)){
				return NULL;
			}
			hDArrow = CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED, _AG_DOCK_ARROW_CLASS, NULL, WS_POPUP, 0, 0, 48, 48, hParent, NULL, hInstance, NULL);
		}
		else{
			return NULL;
		}
	}
	
	if(hDArrow){
		SetWindowLongPtrW(hDArrow, GWLP_USERDATA, pos);
		switch(pos){
		case DA_LEFT_UP:
		case DA_RIGHT_UP:
			hbmp = LoadBitmapW(hInstance, MAKEINTRESOURCEW(IDB_ARROW_TOP));
			break;
		case DA_LEFT_DOWN:
		case DA_RIGHT_DOWN:
			hbmp = LoadBitmapW(hInstance, MAKEINTRESOURCEW(IDB_ARROW_BOTTOM));
			break;
		case DA_TOP_LEFT:
		case DA_BOTTOM_LEFT:
			hbmp = LoadBitmapW(hInstance, MAKEINTRESOURCEW(IDB_ARROW_LEFT));
			break;
		case DA_TOP_RIGHT:
		case DA_BOTTOM_RIGHT:
			hbmp = LoadBitmapW(hInstance, MAKEINTRESOURCEW(IDB_ARROW_RIGHT));
			break;
		}
		SetWindowLongPtrW(hDArrow, 0, (LONG_PTR)hbmp);
		SetLayeredWindowAttributes(hDArrow, CLR_MASK, 160, LWA_ALPHA);
		SetWindowRgn(hDArrow, RegionFromSkin(hbmp, CLR_MASK), TRUE);
		MoveArrow(hDArrow, pos, hbmp);
		ShowWindow(hDArrow, SW_HIDE);
		// RedrawWindow(hDArrow, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}
	return hDArrow;
}

static void MoveArrow(HWND hwnd, int pos, HBITMAP hbmp){
	RECT			rcDesktop;
	int				sx = 0, sy = 0, mcount;
	BITMAP			bm;

	mcount = GetSystemMetrics(SM_CMONITORS);
	if(mcount == 1){
		GetClientRect(GetDesktopWindow(), &rcDesktop);
		sx = rcDesktop.right - rcDesktop.left;
		sy = rcDesktop.bottom - rcDesktop.top;
	}
	else{
		sx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		sy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	}
	GetObject(hbmp, sizeof(bm), &bm);

	switch(pos){
	case DA_LEFT_UP:
	case DA_TOP_LEFT:
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
		break;
	case DA_LEFT_DOWN:
	case DA_BOTTOM_LEFT:
		SetWindowPos(hwnd, HWND_TOPMOST, 0, sy - bm.bmHeight, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
		break;
	case DA_RIGHT_UP:
	case DA_TOP_RIGHT:
		SetWindowPos(hwnd, HWND_TOPMOST, sx - bm.bmWidth, 0, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
		break;
	case DA_RIGHT_DOWN:
	case DA_BOTTOM_RIGHT:
		SetWindowPos(hwnd, HWND_TOPMOST, sx - bm.bmWidth, sy - bm.bmHeight, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
		break;
	}
}

static BOOL RegisterDArrow(HINSTANCE hInstance){
	WNDCLASSEXW		wcx;

	ZeroMemory(&wcx, sizeof(wcx));
	wcx.cbSize = sizeof(wcx);
	wcx.hInstance = hInstance;
	wcx.cbWndExtra = 4;
	wcx.lpszClassName = _AG_DOCK_ARROW_CLASS;
	wcx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcx.lpfnWndProc = DArrow_WndProc;
	wcx.hCursor = LoadCursor(NULL, IDC_HAND);
	// wcx.hbrBackground = CreateSolidBrush(RGB(255,0,0));
	if(RegisterClassExW(&wcx)){
		return TRUE;
	}
	else{
		if(GetLastError() == ERROR_CLASS_ALREADY_EXISTS){
			return TRUE;
		}
		return FALSE;
	}
}

static LRESULT CALLBACK DArrow_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_DESTROY, DArrow_OnDestroy);
	// HANDLE_MSG (hwnd, WM_CREATE, Bell_OnCreate);
	HANDLE_MSG (hwnd, WM_PAINT, DArrow_OnPaint);
	HANDLE_MSG (hwnd, WM_LBUTTONUP, DArrow_OnLButtonUp);

	// HANDLE_MSG (hwnd, WM_LBUTTONDOWN, Bell_OnLButtonDown);

	default: return DefWindowProc (hwnd, msg, wParam, lParam);
	}
}

static void DArrow_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	ShowWindow(hwnd, SW_HIDE);
	ShiftNotesOnArrowClick(hwnd);
}

static void DArrow_OnDestroy(HWND hwnd)
{
	HBITMAP		hbmp = (HBITMAP)GetWindowLongPtrW(hwnd, 0);
	if(hbmp)
		DeleteObject(hbmp);
}

static void DArrow_OnPaint(HWND hwnd)
{
	PAINTSTRUCT		ps;
	RECT			rc;
	HBITMAP			hbmp;

	BeginPaint(hwnd, &ps);
	hbmp = (HBITMAP)GetWindowLongPtrW(hwnd, 0);
	if(hbmp){
		GetClientRect(hwnd, &rc);
		DrawState(ps.hdc, NULL, NULL, (LPARAM)hbmp, 0, 0, 0, rc.right - rc.left, rc.bottom - rc.top, DST_BITMAP);
	}
	EndPaint(hwnd, &ps);
}

static void ShiftNotesOnArrowClick(HWND hwnd){
	PDOCKITEM		pItem;
	PDOCKHEADER 	pHeader;
	int				pos = GetWindowLongPtrW(hwnd, GWLP_USERDATA);
	int				offset, dockType;
	// HWND			hTray = NULL;

	switch(pos){
	case DA_LEFT_UP:
		pHeader = DHeader(DOCK_LEFT);
		dockType = DOCK_LEFT;
		offset = 1;
		break;
	case DA_LEFT_DOWN:
		pHeader = DHeader(DOCK_LEFT);
		dockType = DOCK_LEFT;
		offset = -1;
		break;
	case DA_TOP_LEFT:
		pHeader = DHeader(DOCK_TOP);
		dockType = DOCK_TOP;
		offset = 1;
		break;
	case DA_TOP_RIGHT:
		pHeader = DHeader(DOCK_TOP);
		dockType = DOCK_TOP;
		offset = -1;
		break;
	case DA_RIGHT_UP:
		pHeader = DHeader(DOCK_RIGHT);
		dockType = DOCK_RIGHT;
		offset = 1;
		break;
	case DA_RIGHT_DOWN:
		pHeader = DHeader(DOCK_RIGHT);
		dockType = DOCK_RIGHT;
		offset = -1;
		break;
	case DA_BOTTOM_LEFT:
		pHeader = DHeader(DOCK_BOTTOM);
		dockType = DOCK_BOTTOM;
		offset = 1;
		break;
	case DA_BOTTOM_RIGHT:
		pHeader = DHeader(DOCK_BOTTOM);
		dockType = DOCK_BOTTOM;
		offset = -1;
		break;
	}
	pItem = pHeader->pNext;
	while(pItem){
		pItem->index += offset;
		MoveDockWindow(pItem->hwnd, dockType, pItem->index);
		pItem = pItem->pNext;
	}
	// if(hTray)
		// SetWindowPos(hTray, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
}
