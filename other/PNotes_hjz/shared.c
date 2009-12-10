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
#include "lunar.h"
#include "gradients.h"
#include "skinlessdrawing.h"

/** Prototypes ********************************************************/
static HBRUSH GetEditBrush(HBITMAP hSkin, LPCRECT lpRc);
static HBITMAP GetBackBitmap(P_NOTE_RTHANDLES pH, LPCRECT lpRc);
static BOOL GetSkinInfo(HINSTANCE hLib, wchar_t * szBuffer);
static BOOL GetSkinVertTbr(HINSTANCE hLib);

/** Module variables ********************************************************/
static int				m_IDSizeMenu = IDI_SIZE_START;
static int				m_FSize;

/** Global functions ********************************************************/


/*-@@+@@------------------------------------------------------------------
 Procedure: DrawComboItem
 Created  : Thu Aug  9 17:41:23 2007
 Modified : Thu Aug  9 17:41:23 2007

 Synopsys : Draws item in owner-drawn combo box. By unknown reason combo 
            box cannot display properly Unicode strings on some computers, 
            if the language is not specified as "language fo non-Unicode 
            programs". All questions to microsoft.com :)
 Input    : lpd - pointer to DRAWITEMSTRUCT struct
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void DrawComboItem(const DRAWITEMSTRUCT * lpd){

	wchar_t			szBuffer[SendMessageW(lpd->hwndItem, CB_GETLBTEXTLEN, lpd->itemID, 0)];
	RECT			rc;

	CopyRect(&rc, &lpd->rcItem);
	// set back mode to transparent
	SetBkMode(lpd->hDC, TRANSPARENT);
	if((lpd->itemState & ODS_SELECTED) == ODS_SELECTED){
		SetTextColor(lpd->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
		FillRect(lpd->hDC, &lpd->rcItem, GetSysColorBrush(COLOR_HIGHLIGHT));
		if((lpd->itemState & ODS_FOCUS) == ODS_FOCUS){
			DrawFocusRect(lpd->hDC, &lpd->rcItem);
		}
	}
	else{
		if(((lpd->itemState & ODS_DISABLED) == ODS_DISABLED) || ((lpd->itemState & ODS_GRAYED) == ODS_GRAYED)){
			SetTextColor(lpd->hDC, GetSysColor(COLOR_GRAYTEXT));
		}
		else{
			SetTextColor(lpd->hDC, GetSysColor(COLOR_WINDOWTEXT));
		}
		FillRect(lpd->hDC, &lpd->rcItem, GetSysColorBrush(COLOR_WINDOW));
	}
	SendMessageW(lpd->hwndItem, CB_GETLBTEXT, lpd->itemID, (LPARAM)szBuffer);
	DrawTextExW(lpd->hDC, szBuffer, -1, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, NULL);
}

BOOL IsDateEmpty(LPSYSTEMTIME lpd){
	if(lpd->wDay == 0 && lpd->wDayOfWeek == 0 && lpd->wHour == 0 && lpd->wMilliseconds == 0 && lpd->wMinute == 0 && lpd->wMonth == 0 && lpd->wSecond == 0 && lpd->wYear == 0)
		return TRUE;
	else
		return FALSE;
}

int DateDiff(const SYSTEMTIME d1, const SYSTEMTIME d2, ddiff diffType){

	TIMEUNION 				ft1;
    TIMEUNION 				ft2;
	unsigned long long		delta;
	int						multiplier;

    SystemTimeToFileTime(&d1, &ft1.fileTime);
    SystemTimeToFileTime(&d2, &ft2.fileTime);

	if(CompareFileTime(&ft1.fileTime, &ft2.fileTime) < 0){
    	delta = ft2.ul.QuadPart - ft1.ul.QuadPart;
		multiplier = -1;
	}
	else{
		delta = ft1.ul.QuadPart - ft2.ul.QuadPart;
		multiplier = 1;
	}

	switch(diffType){
		case SECOND:
			return multiplier * delta / 10000000;
		case MINUTE:
			return multiplier * delta / 10000000 / 60;
		case HOUR:
			return multiplier * delta / 10000000 / 60 / 60;
		case DAY:
			return multiplier * delta / 10000000 / 60 / 60 / 24;
		case WEEK:
			return multiplier * delta / 10000000 / 60 / 60 / 24 / 7;
		case MONTH:
			return multiplier * delta / 10000000 / 60 / 60 / 24 / 30;
		case YEAR:
			return multiplier * delta / 10000000 / 60 / 60 / 24 / 365;
		default:
			return 0;
	}
}

short DockIndex(int dockData){
	return HIWORD(dockData);
}

int DockType(int dockData){
	return LOWORD(dockData);
}

void SaveNewDockData(HWND hwnd, int dockData){
	P_NOTE_FLAGS		pF = NoteFlags(hwnd);
	NOTE_DATA			data;

	if(pF->fromDB){
		if(GetPrivateProfileStructW(pF->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
			data.dockData = dockData;
			WritePrivateProfileStructW(pF->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
		}
		else{
			//version 3.5
			if(GetPrivateProfileStructW(pF->id, S_DATA, &data, sizeof(NOTE_DATA) - sizeof(RECT) - sizeof(BOOL), g_NotePaths.DataFile)){
				data.dockData = dockData;
				WritePrivateProfileStructW(pF->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
			}
		}
	}
}

void SetDockIndex(int * dockData, short index){
	short		tl = LOWORD(*dockData);

	*dockData = MAKELONG(tl, index);
}

HWND MoveDockWindow(HWND hwnd, int dockType, short index){
	RECT			rcDesktop, rcTray;
	int				cx, cy, sx = 0, sy = 0, mcount;
	int				tLeft = 0, tTop = 0;
	BITMAP			bm;
	HWND			hTray = NULL;

	hTray = FindWindow("Shell_TrayWnd", NULL);
	GetWindowRect(hTray, &rcTray);

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
	
	//top
	if(rcTray.left == 0 && rcTray.top == 0 && rcTray.bottom - rcTray.top < rcTray.right - rcTray.left){
		// sy -= (rcTray.bottom - rcTray.top);
		tTop = rcTray.bottom - rcTray.top;
	}
	//bottom
	else if(rcTray.left == 0 && rcTray.bottom == sy && rcTray.bottom - rcTray.top < rcTray.right - rcTray.left){
		sy -= (rcTray.bottom - rcTray.top);
	}
	//left
	else if(rcTray.left == 0 && rcTray.top == 0 && rcTray.bottom == sy){
		// sx -= (rcTray.right - rcTray.left);
		tLeft = rcTray.right - rcTray.left;
	}
	//right
	else if(rcTray.right == sx && rcTray.top == 0 && rcTray.bottom == sy){
		sx -= (rcTray.right - rcTray.left);
	}
	if(!g_RTHandles.hbSkin){
		if(g_DockSettings.fCustSize){
			cx = g_DockSettings.custSize.cx;
			cy = g_DockSettings.custSize.cy;
		}
		else{
			cx = g_RTHandles.szDef.cx;
			cy = g_RTHandles.szDef.cy;
		}
	}
	else{
		if(g_DockSettings.fCustSkin){
			GetObject(g_DRTHandles.hbSkin, sizeof(bm), &bm);
		}
		else{
			GetObject(g_RTHandles.hbSkin, sizeof(bm), &bm);
		}
		cx = bm.bmWidth;
		cy = bm.bmHeight;
	}

	switch(dockType){
	case DOCK_LEFT:
		if(g_DockSettings.fInvOrder)
			MoveWindow(hwnd, tLeft, sy - index * (cy + g_DockSettings.dist) - cy, cx, cy, TRUE);
		else
			MoveWindow(hwnd, tLeft, index * (cy + g_DockSettings.dist), cx, cy, TRUE);
		break;
	case DOCK_TOP:
		if(g_DockSettings.fInvOrder)
			MoveWindow(hwnd, sx - index * (cx + g_DockSettings.dist) - cx, tTop, cx, cy, TRUE);
		else
			MoveWindow(hwnd, index * (cx + g_DockSettings.dist), tTop, cx, cy, TRUE);
		break;
	case DOCK_RIGHT:
		if(g_DockSettings.fInvOrder)
			MoveWindow(hwnd, sx - (cx + g_DockSettings.dist), sy - index * (cy + g_DockSettings.dist) - cy, cx, cy, TRUE);
		else
			MoveWindow(hwnd, sx - (cx + g_DockSettings.dist), index * (cy + g_DockSettings.dist), cx, cy, TRUE);
		break;
	case DOCK_BOTTOM:
		if(g_DockSettings.fInvOrder)
			MoveWindow(hwnd, sx - index * (cx + g_DockSettings.dist) - cx, sy - (cy + g_DockSettings.dist), cx, cy, TRUE);
		else
			MoveWindow(hwnd, index * (cx + g_DockSettings.dist), sy - (cy + g_DockSettings.dist), cx, cy, TRUE);
		break;
	}

	return hTray;
}

PDOCKHEADER DHeader(int dockType){
	switch(dockType){
		case DOCK_LEFT:
			return g_DHeaders.pLeft;
		case DOCK_TOP:
			return g_DHeaders.pTop;
		case DOCK_RIGHT:
			return g_DHeaders.pRight;
		case DOCK_BOTTOM:
			return g_DHeaders.pBottom;
		default:
			return NULL;
	}
}

void BitOff(int * data, int position){
	*data &= ~ (1 << position);
}

void BitOn(int * data, int position){
	*data |= (1 << position);
}

BOOL IsBitOn(int data, int position){
	if((data & (1 << position)) == (1 << position))
		return TRUE;
	return FALSE;
}

int GetBit(int data, int position){
	return data & (1 << position);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ConstructDateTimeString
 Created  : Fri May 25 22:46:50 2007
 Modified : Fri May 25 22:46:50 2007

 Synopsys : Constructs date-time string
 Input    : lpSt - LPSYSTEMTIME pointer
            lpBuffer - string to return
            bWithMilliseconds - whether milliseconds should be included
 Output   : Formatted date-time string
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void ConstructDateTimeString(const LPSYSTEMTIME lpSt, wchar_t * lpBuffer, BOOL bWithMilliseconds){

	wchar_t			szTime[32], szDFormat[128], szTFormat[128];
	wchar_t			szMsec[4];

	ParseDateFormat(g_DTFormats.DateFormat, szDFormat, szTFormat);
	GetDateFormatW(LOCALE_USER_DEFAULT, 0, lpSt, szDFormat, lpBuffer, 64);
	GetTimeFormatW(LOCALE_USER_DEFAULT, 0, lpSt, szTFormat, szTime, 32);
	
	wcscat(lpBuffer, L" ");
	wcscat(lpBuffer, szTime);
	if(bWithMilliseconds){
		wsprintfW(szMsec, L"%u", lpSt->wMilliseconds);
		wcscat(lpBuffer, L".");
		wcscat(lpBuffer, szMsec);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SetDlgCtlText
 Created  : Sat May 19 12:57:18 2007
 Modified : Sat May 19 12:57:18 2007

 Synopsys : Sets dialog item text accordingly to language
 Input    : Dialog handle, dialog item id, language file, default text
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void SetDlgCtlText(HWND hwnd, int id, const wchar_t * lpFile, const wchar_t * lpDefault){

	wchar_t 		szBuffer[256], szId[12];

	_itow(id, szId, 10);
	GetPrivateProfileStringW(S_OPTIONS, szId, lpDefault, szBuffer, 256, lpFile);
	SetDlgItemTextW(hwnd, id, szBuffer);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SetFroupText
 Created  : Sat May 19 12:57:18 2007
 Modified : Sat May 19 12:57:18 2007

 Synopsys : Sets group text accordingly to language
 Input    : Dialog handle, group id, text id, language file, default text
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void SetGroupText(HWND hwnd, int idGroup, int id, const wchar_t * lpFile, const wchar_t * lpDefault){

	wchar_t 		szBuffer[256], szId[12];

	_itow(id, szId, 10);
	GetPrivateProfileStringW(S_LEFT_PANEL, szId, lpDefault, szBuffer, 256, lpFile);
	SetDlgItemTextW(hwnd, idGroup, szBuffer);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SetMenuText
 Created  : Sat May 19 12:55:45 2007
 Modified : Sat May 19 12:55:45 2007

 Synopsys : Sets menu item text from language file
 Input    : Menu handle, menu item id, language file section, language file 
            name, buffer for menu text
 Output   : Buffer with menu item text
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void SetMenuText(int id, const wchar_t * lcpSection, const wchar_t * lcpFile, wchar_t * lpDefault, wchar_t * lpBuffer){

	wchar_t 		szId[12];

	_itow(id, szId, 10);
	GetPrivateProfileStringW(lcpSection, szId, lpDefault, lpBuffer, 256, lcpFile);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SetMenuTextWithAccelerator
 Created  : Fri May 25 22:40:17 2007
 Modified : Fri May 25 22:41:14 2007

 Synopsys : Sets menu item text from language file with accelerator
 Input    : Menu handle, menu item id, language file section, language file 
            name, buffer for menu text, accelerator text
 Output   : Buffer with menu item text
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void SetMenuTextWithAccelerator(int id, const wchar_t * lpSection, const wchar_t * lpFile, wchar_t * lpDefault, wchar_t * lpBuffer, const wchar_t * lpAcc){

	wchar_t 		szId[12];

	_itow(id, szId, 10);
	GetPrivateProfileStringW(lpSection, szId, lpDefault, lpBuffer, 256, lpFile);
	wcscat(lpBuffer, L"\t");
	wcscat(lpBuffer, lpAcc);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: FileExists
 Created  : Mon May 14 12:33:55 2007
 Modified : Mon May 14 12:33:55 2007

 Synopsys : Checks file existance by directory and file name
 Input    : Directory, file name
 Output   : TRUE if exists, otherwise FALSE
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
BOOL FileExists(wchar_t * lpDir, wchar_t * lpFile){
	WIN32_FIND_DATAW 	fd;
	wchar_t				filespec[MAX_PATH];
	HANDLE 				handle = INVALID_HANDLE_VALUE;

	wcscpy(filespec, lpDir);
	wcscat(filespec, lpFile);
	handle = FindFirstFileW(filespec, &fd);
	if(handle != INVALID_HANDLE_VALUE){
		FindClose(handle);
		return TRUE;
	}
	return FALSE;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: FillSkins
 Created  : Sat May 19 12:54:14 2007
 Modified : Sat May 19 12:54:14 2007

 Synopsys : Fills skins list box
 Input    : Options dialog handle, current skin name
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void FillSkins(HWND hwnd, const wchar_t * lpSkinCurrent, BOOL fAllSkins, BOOL fShowNoSkin){
	WIN32_FIND_DATAW 	fd;
	wchar_t				filespec[MAX_PATH];
	wchar_t				szSkin[256];
	HANDLE 				handle = INVALID_HANDLE_VALUE;
	BOOL 				result = TRUE, skinSelected = FALSE;
	LRESULT 			added;

	if(fShowNoSkin)
		SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_ADDSTRING, 0, (LPARAM)DS_NO_SKIN);
	if(fAllSkins){
		wcscpy(filespec, g_NotePaths.SkinDir);
		wcscat(filespec, __T("*.skn"));
		handle = FindFirstFileW(filespec, &fd);
		if(handle != INVALID_HANDLE_VALUE){
			while(result){
				wcscpy(szSkin, fd.cFileName);
				szSkin[wcslen(szSkin) - 4] = '\0';
				added = SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_ADDSTRING, 0, (LPARAM)szSkin);
				if(wcscmp(fd.cFileName, lpSkinCurrent) == 0){
					SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_SETCURSEL, added, 0);
					skinSelected = TRUE;
				}
				result = FindNextFileW(handle, &fd);
			}
			FindClose(handle);
		}
	}
	if(!skinSelected && fShowNoSkin)
		SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_SETCURSEL, 0, 0);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: DoesAnySkinExist
 Created  : Sat May 19 12:52:28 2007
 Modified : Sat May 19 12:52:28 2007

 Synopsys : Checks whether any skin file exists
 Input    : Buffer for skin file name
 Output   : TRUE if any skin file exists, otherwise - FALSE.
            Skin file name in buffer.
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

BOOL DoesAnySkinExist(wchar_t * lpSkin){
	
	WIN32_FIND_DATAW 	fd;
	wchar_t				filespec[MAX_PATH];
	HANDLE 				handle = INVALID_HANDLE_VALUE;

	wcscpy(filespec, g_NotePaths.SkinDir);
	wcscat(filespec, __T("*.skn"));
	handle = FindFirstFileW(filespec, &fd);
	if(handle != INVALID_HANDLE_VALUE){
		wcscpy(lpSkin, fd.cFileName);
		FindClose(handle);
		return TRUE;
	}
	else
		return FALSE;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: DrawSkinPreview
 Created  : Sat May 19 12:50:51 2007
 Modified : Sat May 19 12:50:51 2007

 Synopsys : Draws skin on skin preview window
 Input    : Preview window handle, skin bitmap, DRAWITEMSTRUCT from 
            WM_DRAWITEM message
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void DrawSkinPreview(HWND hwnd, P_NOTE_RTHANDLES pH, P_NOTE_APPEARANCE pA, COLORREF crMask, const DRAWITEMSTRUCT * lpDI){

	int				state;
	RECT 			rc, rcText, rcPaint;
	HDC				hdcComp;
	BITMAP			bm;
	HBITMAP			hBm;
	int 			w, h;

	state = SaveDC(lpDI->hDC);
	SetBkMode(lpDI->hDC, TRANSPARENT);
	hdcComp = CreateCompatibleDC(lpDI->hDC);
	GetClientRect(hwnd, &rc);
	if(pH->hbSkin){
		if(GetObject(pH->hbSkin, sizeof(bm), &bm)){
			w = rc.right - rc.left - 2;
			h = rc.bottom - rc.top - 2;
			hBm = CreateCompatibleBitmap(lpDI->hDC, bm.bmWidth, bm.bmHeight);
			SelectObject(hdcComp, hBm);
			SetRect(&rcPaint ,0, 0, bm.bmWidth, bm.bmHeight);
			FillRect(hdcComp, &rcPaint, GetSysColorBrush(COLOR_WINDOW));
			DrawTransparentBitmap(pH->hbSkin, hdcComp, 0, 0, bm.bmWidth, bm.bmHeight, 0, 0, crMask);
			SetStretchBltMode(lpDI->hDC, HALFTONE);
			StretchBlt(lpDI->hDC, 1, 1, w, h, hdcComp, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
			DeleteObject(hBm);
			CopyRect(&rcText, &pH->rcEdit);

			rcText.left *= (double)w / bm.bmWidth;
			rcText.top *= (double)h / bm.bmHeight;
			rcText.right *= (double)w / bm.bmWidth;
			rcText.bottom *= (double)h / bm.bmHeight;
			SelectObject(lpDI->hDC, pH->hFont);
			SetTextColor(lpDI->hDC, pA->crFont);
			if(wcslen(pH->szSkinInfo) > 0)
				DrawTextW(lpDI->hDC, pH->szSkinInfo, -1, &rcText, DT_LEFT | DT_WORDBREAK);
			else
				DrawTextW(lpDI->hDC, g_Strings.FontSample, -1, &rcText, DT_LEFT | DT_WORDBREAK);
		}
	}
	else{
		SendMessageW(GetParent(hwnd), CHM_DRAW_SN_PVW, (WPARAM)hwnd, (LPARAM)lpDI);
	}
	DeleteDC(hdcComp);
	RestoreDC(lpDI->hDC, state);
}

void DrawSkinPreviewDock(HWND hwnd, P_NOTE_RTHANDLES pH, P_PNDOCK pA, COLORREF crMask, COLORREF crFont, const DRAWITEMSTRUCT * lpDI){

	int				state;
	RECT 			rc, rcText, rcPaint;
	HDC				hdcComp;
	BITMAP			bm;
	HBITMAP			hBm;
	int 			w, h;

	state = SaveDC(lpDI->hDC);
	SetBkMode(lpDI->hDC, TRANSPARENT);
	hdcComp = CreateCompatibleDC(lpDI->hDC);
	GetClientRect(hwnd, &rc);
	if(GetObject(pH->hbSkin, sizeof(bm), &bm)){
		w = rc.right - rc.left - 2;
		h = rc.bottom - rc.top - 2;
		hBm = CreateCompatibleBitmap(lpDI->hDC, bm.bmWidth, bm.bmHeight);
		SelectObject(hdcComp, hBm);
		SetRect(&rcPaint ,0, 0, bm.bmWidth, bm.bmHeight);
		FillRect(hdcComp, &rcPaint, GetSysColorBrush(COLOR_WINDOW));
		DrawTransparentBitmap(pH->hbSkin, hdcComp, 0, 0, bm.bmWidth, bm.bmHeight, 0, 0, crMask);
		SetStretchBltMode(lpDI->hDC, HALFTONE);
		StretchBlt(lpDI->hDC, 1, 1, w, h, hdcComp, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
		DeleteObject(hBm);
		CopyRect(&rcText, &pH->rcEdit);
		rcText.left *= (double)w / bm.bmWidth;
		rcText.top *= (double)h / bm.bmHeight;
		rcText.right *= (double)w / bm.bmWidth;
		rcText.bottom *= (double)h / bm.bmHeight;
		SelectObject(lpDI->hDC, pH->hFont);
		SetTextColor(lpDI->hDC, crFont);
		if(wcslen(pH->szSkinInfo) > 0)
			DrawTextW(lpDI->hDC, pH->szSkinInfo, -1, &rcText, DT_LEFT | DT_WORDBREAK);
		else
			DrawTextW(lpDI->hDC, g_Strings.FontSample, -1, &rcText, DT_LEFT | DT_WORDBREAK);
	}
	else{
		FillRect(lpDI->hDC, &rc, GetSysColorBrush(COLOR_BTNFACE));
	}
	DeleteDC(hdcComp);
	RestoreDC(lpDI->hDC, state);
}

static BOOL GetSkinVertTbr(HINSTANCE hLib){
	HRSRC	hRes;
	HGLOBAL	hGlob;
	LPVOID	hLock;
	int		result;

	hRes = FindResource(hLib, MAKEINTRESOURCE(IDS_VERTICAL_TBR), SKN_VERT_TBR);
	if(hRes){
		hGlob = LoadResource(hLib, hRes);
		if(hGlob){
			hLock = LockResource(hGlob);
			if(hLock){
				memcpy(&result, hLock, sizeof(int));
				if(result == 0)
					return FALSE;
				return TRUE;
			}
		}
	}
	return FALSE;
}

static BOOL GetSkinInfo(HINSTANCE hLib, wchar_t * szBuffer){
	HRSRC	hRes;
	HGLOBAL	hGlob;
	LPVOID	hLock;

	hRes = FindResourceW(hLib, MAKEINTRESOURCEW(IDS_SKIN_INFO), SKN_INFO);
	if(hRes){
		hGlob = LoadResource(hLib, hRes);
		if(hGlob){
			hLock = LockResource(hGlob);
			if(hLock){
				wcscpy(szBuffer, (const wchar_t *)hLock);
				return TRUE;
			}
		}
	}
	return FALSE;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetSkinProperties
 Created  : Fri May 25 13:12:42 2007
 Modified : Fri May 25 13:12:42 2007

 Synopsys : Loads all needed skin's properties
 Input    : Window handle, pointer to NOTE_RTHANDLES structure, skin file 
            name
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void GetSkinProperties(HWND hWnd, P_NOTE_RTHANDLES pH, wchar_t * lpSkinFile, BOOL bDeletePrevious){
	
	HINSTANCE			hLib;
	GETSKINPROC			hSProc;
	GETCTLRECTPROC		hRProc;
	wchar_t				szPath[MAX_PATH];
	HRSRC				rRes;
	HGLOBAL				rMask;
	RECT				rcTemp;

	wcscpy(szPath, g_NotePaths.SkinDir);
	wcscat(szPath, lpSkinFile);
	//load skin library
	hLib = LoadLibraryW(szPath);
	if(hLib != NULL){
		//get possible skin info
		*pH->szSkinInfo = '\0';
		GetSkinInfo(hLib, pH->szSkinInfo);
		//get possible vertical toolbar
		pH->vertTbr = GetSkinVertTbr(hLib);
		// LoadStringW(hLib, IDS_SKIN_INFO, pH->szSkinInfo, 256);
		//get proc address for bitmap loading procedure
		hSProc = (GETSKINPROC)GetProcAddress(hLib, "GetPNGSkin");
		if(NULL != hSProc){
			//load skin bitmap
			if(pH->hbSkin && bDeletePrevious)
				DeleteObject(pH->hbSkin);
			pH->hbSkin = (HBITMAP)(hSProc)(hWnd, IDS_SKIN_BODY);
			//load sysmenu bitmap
			if(pH->hbSys && bDeletePrevious)
				DeleteObject(pH->hbSys);
			pH->hbSys = (HBITMAP)(hSProc)(hWnd, IDS_SKIN_SYS);
			//load delhide bitmap
			if(pH->hbDelHide && bDeletePrevious)
				DeleteObject(pH->hbDelHide);
			pH->hbDelHide = (HBITMAP)(hSProc)(hWnd, IDS_SKIN_DEL_HIDE);
			//load commands bitmap
			if(pH->hbCommand && bDeletePrevious)
				DeleteObject(pH->hbCommand);
			pH->hbCommand = (HBITMAP)(hSProc)(hWnd, IDS_SKIN_COMMANDS);
		}
		//get proc address for rectangles loading procedure
		hRProc = (GETCTLRECTPROC)GetProcAddress(hLib, "GetRectangle");
		if(NULL != hRProc){
			//load needed rectangles
			(hRProc)(IDS_EDIT_RECT, &pH->rcEdit);
			(hRProc)(IDS_SCH_RECT, &pH->rcSchedule);
			(hRProc)(IDS_NOT_SAVED_RECT, &pH->rcNotSaved);
			(hRProc)(IDS_PROTECTED_RECT, &pH->rcProtected);
			(hRProc)(IDS_PRIORITY_RECT, &pH->rcPriority);
			(hRProc)(IDS_COMPLETED_RECT, &pH->rcCompleted);
			(hRProc)(IDS_TT_RECT, &pH->rcTooltip);
			(hRProc)(IDS_COMMANDS_RECT, &pH->rcCommands);
			(hRProc)(IDS_DH_RECT, &pH->rcDH);
			SetRectEmpty(&rcTemp);
			(hRProc)(IDS_MARKS_COUNT, &rcTemp);
			if(rcTemp.left == 0)
				//old skins
				pH->marks = 2;
			else
				pH->marks = rcTemp.left;

			if(NULL != hSProc){
				//get pattern brush for edit box transparency emulation
				if(pH->hbPattern && bDeletePrevious)
					DeleteObject(pH->hbPattern);
				pH->hbPattern = GetEditBrush(pH->hbSkin, &pH->rcEdit);
				//get back bitmap for rich edit box transparency emulation
				if(pH->hbBack && bDeletePrevious)
					DeleteObject(pH->hbBack);
				GetBackBitmap(pH, &pH->rcEdit);
			}
		}
		rRes = FindResource(hLib, MAKEINTRESOURCE(IDS_MASK_COLOR), SKN_MASK);
		if(!rRes)
			pH->crMask = CLR_MASK;
		else{
			rMask = LoadResource(hLib, rRes);
			if(rMask){
				memcpy(&pH->crMask, rMask, sizeof(COLORREF));
			}
		}
		//free library
		FreeLibrary(hLib);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ChangeSkinPreview
 Created  : Sat May 19 12:49:15 2007
 Modified : Sat May 19 12:49:15 2007

 Synopsys : Changes bitmap for skin preview and redraws preview window
 Input    : Options dialog handle, id of listbox with skin names, skin 
            preview window id, runtime handles, skin name
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void ChangeSkinPreview(HWND hwnd, int idList, int idPvw, P_NOTE_RTHANDLES pH, wchar_t * lpTempSkin, BOOL fDeletePrev){
	//get skin bitmap and show it in preview window

	wchar_t szFile[256];
	HWND	hList;

	hList = GetDlgItem(hwnd, idList);
	SendMessageW(hList, LB_GETTEXT, SendMessageW(hList, LB_GETCURSEL, 0, 0), (LPARAM)szFile);
	wcscat(szFile, __T(".skn"));
	wcscpy(lpTempSkin, szFile);
	pH->hbSkin = GetSkinById(hwnd, IDS_SKIN_BODY, lpTempSkin);
	GetSkinProperties(hwnd, pH, lpTempSkin, fDeletePrev);
	RedrawWindow(GetDlgItem(hwnd, idPvw), NULL, NULL, RDW_INVALIDATE);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: IsFontsEqual
 Created  : Sat May 19 12:46:09 2007
 Modified : Sat May 19 12:46:09 2007

 Synopsys : Checks whether two LOGFONTW structures are equal
 Input    : Two LOGFONTW structures
 Output   : TRUE if all members of structures are equal, otherwise - FALSE
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

BOOL IsFontsEqual(const LOGFONTW * lf1, const LOGFONTW * lf2){
	
	if(lf1->lfHeight != lf2->lfHeight || lf1->lfWidth != lf2->lfWidth 
		|| lf1->lfEscapement != lf2->lfEscapement || lf1->lfOrientation != lf2->lfOrientation
		|| lf1->lfWeight != lf2->lfWeight || lf1->lfItalic != lf2->lfItalic
		|| lf1->lfUnderline != lf2->lfUnderline || lf1->lfStrikeOut != lf2->lfStrikeOut
		|| lf1->lfCharSet != lf2->lfCharSet || lf1->lfOutPrecision != lf2->lfOutPrecision
		|| lf1->lfClipPrecision != lf2->lfClipPrecision || lf1->lfQuality != lf2->lfQuality
		|| lf1->lfPitchAndFamily != lf2->lfPitchAndFamily || wcscmp(lf1->lfFaceName, lf2->lfFaceName) != 0)

		return FALSE;

	return TRUE;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ShowPopUp
 Created  : Sat May 19 12:44:33 2007
 Modified : Sat May 19 12:44:33 2007

 Synopsys : Shows popup menu
 Input    : Window handle, popup menu handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void ShowPopUp(HWND hwnd, HMENU hMenu){
	POINT pt;

	GetCursorPos(&pt);
	//set window foreground for further PostMessageW call 
	SetForegroundWindow(hwnd);
	//show popup menu
	TrackPopupMenu(hMenu, TPM_RIGHTALIGN, pt.x, pt.y, 0, hwnd, NULL);
	//remove popup menu when user clicks outside it
	PostMessageW(hwnd, WM_NULL, 0, 0);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SetMIText
 Created  : Sat May 19 12:43:38 2007
 Modified : Sat May 19 12:43:38 2007

 Synopsys : Sets text of specified MItem array element
 Input    : MItem structure array, size of array, item id, text to set
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void SetMIText(MItem * lpMI, const int size, int id, const wchar_t * lpText){

	for(int i = 0; i < size; i++){
		if(lpMI->id == id){
			wcscpy(lpMI->szText, lpText);
			break;
		}
		lpMI++;
	}
}

void SetMIImageIndex(MItem * lpMI, const int size, int id, const int idImg){

	for(int i = 0; i < size; i++){
		if(lpMI->id == id){
			lpMI->xPos = idImg;
			break;
		}
		lpMI++;
	}
}

int GetMIImageIndex(MItem * lpMI, const int size, int id){
	for(int i = 0; i < size; i++){
		if(lpMI->id == id){
			return lpMI->xPos;
		}
		lpMI++;
	}
	return -1;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetPNotesMessage
 Created  : Sat May 19 12:41:52 2007
 Modified : Sat May 19 12:41:52 2007

 Synopsys : Gets message from language file
 Input    : Language file directory, language file name, message key, 
            default string, buffer for message
 Output   : Message
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void GetPNotesMessage(const wchar_t * lpDir, const wchar_t * lpFile, const wchar_t * lpKey, const wchar_t * lpDefault, wchar_t * lpMessage){

	wchar_t		szFile[MAX_PATH];

	wcscpy(szFile, lpDir);
	wcscat(szFile, lpFile);
	GetPrivateProfileStringW(__T("messages"), lpKey, lpDefault, lpMessage, 256, szFile);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CenterWindow
 Created  : Fri May 25 22:42:43 2007
 Modified : Fri May 25 22:42:43 2007

 Synopsys : Centers window on screen
 Input    : hwnd - window handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void CenterWindow(HWND hwnd, BOOL fSetDefSize){
	RECT		rcD, rcW;

	GetWindowRect(GetDesktopWindow(), &rcD);
	GetWindowRect(hwnd, &rcW);
	if(!fSetDefSize){
		MoveWindow(hwnd, ((rcD.right - rcD.left) - (rcW.right - rcW.left)) / 2, \
		((rcD.bottom - rcD.top) - (rcW.bottom - rcW.top)) / 2, \
		rcW.right - rcW.left, rcW.bottom - rcW.top, TRUE);
	}
	else{
		MoveWindow(hwnd, ((rcD.right - rcD.left) - g_RTHandles.szDef.cx) / 2, \
		((rcD.bottom - rcD.top) - g_RTHandles.szDef.cy) / 2, \
		g_RTHandles.szDef.cx, g_RTHandles.szDef.cy, TRUE);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetTooltip
 Created  : Mon May 28 11:26:49 2007
 Modified : Mon May 28 11:26:49 2007

 Synopsys : Returns tooltip text based on specified menu item text
 Input    : lpTip - string to return
            pItems - pointer to array of MItem
            size - size of array
            id - menu item id
 Output   : Tooltip text
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

void GetTooltip(wchar_t * lpTip, PMItem pItems, int size, UINT id){
	//erase string
	*lpTip = '\0';
	for(int i = 0; i < size; i++, pItems++){
		if(pItems->id == id){
			wcscpy(lpTip, pItems->szText);
			break;
		}
	}
}

/** Private functions ********************************************************/

/*-@@+@@------------------------------------------------------------------
 Procedure: GetEditBrush
 Created  : Wed May  9 08:52:22 2007
 Modified : Wed May  9 08:52:22 2007

 Synopsys : Creates pattern brush for note window edit box in order to 
            simulate transparency
 Input    : Nothing
 Output   : Handle to brush created
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static HBRUSH GetEditBrush(HBITMAP hSkin, LPCRECT lpRc){
	HBRUSH		hBrush;
	HDC			hdc, hdcTemp, hdcBack;
	HBITMAP		hBmpTemp, hBOld, hBOldTemp;

	//create new DC to work with
	hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	//create compatible DCs
	hdcTemp = CreateCompatibleDC(hdc);
	hdcBack = CreateCompatibleDC(hdc);
	//create bitmap for brush
	hBmpTemp = CreateCompatibleBitmap(hdc, lpRc->right - lpRc->left, lpRc->bottom - lpRc->top);
	//select skin bitmap
	hBOld = SelectObject(hdcBack, hSkin);
	//select brush bitmap
	hBOldTemp = SelectObject(hdcTemp, hBmpTemp);
	//copy needed portion of skin bitmap into brush bitmap
	BitBlt(hdcTemp, 0, 0, lpRc->right - lpRc->left, lpRc->bottom - lpRc->top, hdcBack, lpRc->left, lpRc->top, SRCCOPY);
	//create brush
	hBrush = CreatePatternBrush(hBmpTemp);
	//clean up
	SelectObject(hdcBack, hBOld);
	DeleteObject(SelectObject(hdcTemp, hBOldTemp));
	DeleteDC(hdcBack);
	DeleteDC(hdcTemp);
	DeleteDC(hdc);
	//return created brush
	return hBrush;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetBackBitmap
 Created  : Tue Jan  8 11:59:10 2008
 Modified : Tue Jan  8 11:59:10 2008

 Synopsys : Gets bitmap for simulation of transparency
 Input    : pH - note run-time handles
            lpRc - bitmap rectangle
 Output   : Bitmap which will be drawn behind of rich edit
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static HBITMAP GetBackBitmap(P_NOTE_RTHANDLES pH, LPCRECT lpRc){
	HDC			hdc, hdcTemp, hdcBack;
	HBITMAP		hBmpTemp, hBOld, hBOldTemp;

	//create new DC to work with
	hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	//create compatible DCs
	hdcTemp = CreateCompatibleDC(hdc);
	hdcBack = CreateCompatibleDC(hdc);
	//create bitmap for brush
	hBmpTemp = CreateCompatibleBitmap(hdc, lpRc->right - lpRc->left, lpRc->bottom - lpRc->top);
	//select skin bitmap
	hBOld = SelectObject(hdcBack, pH->hbSkin);
	//select brush bitmap
	hBOldTemp = SelectObject(hdcTemp, pH->hbBack);
	//copy needed portion of skin bitmap into brush bitmap
	BitBlt(hdcTemp, 0, 0, lpRc->right - lpRc->left, lpRc->bottom - lpRc->top, hdcBack, lpRc->left, lpRc->top, SRCCOPY);
	//clean up
	SelectObject(hdcBack, hBOld);
	DeleteDC(hdcBack);
	SelectObject(hdcTemp, hBOldTemp);
	DeleteDC(hdcTemp);
	DeleteDC(hdc);
	//return created bitmap
	return hBmpTemp;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetSkinById
 Created  : Tue May  8 16:37:08 2007
 Modified : Thu May 10 22:47:56 2007

 Synopsys : Loads specified PNG resource from SKN file
 Input    : Window handle, resource identificator, path to SKN file
 Output   : Skin bitmap handle
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

HBITMAP GetSkinById(HWND hWnd, UINT id, const wchar_t* szFile){

	HINSTANCE		hLib;
	GETSKINPROC		hProc;
	HBITMAP			hBmp = NULL;
	wchar_t			szPath[MAX_PATH];

	wcscpy(szPath, g_NotePaths.SkinDir);
	wcscat(szPath, szFile);
	hLib = LoadLibraryW(szPath);
	if(hLib != NULL){
		hProc = (GETSKINPROC)GetProcAddress(hLib, "GetPNGSkin");
		if(NULL != hProc)
			hBmp = (HBITMAP)(hProc)(hWnd, id);
		FreeLibrary(hLib);
	}
	return hBmp;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetSubPathW
 Created  : Mon May 14 12:38:38 2007
 Modified : Mon May 14 12:38:38 2007

 Synopsys : Constructs path to the file, depending on executable path
 Input    : Pointer to result path, file name
 Output   : Result path sent as input
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void GetSubPathW(wchar_t * lpResult, const wchar_t * lpFile){
	GetModuleFileNameW(NULL, lpResult, MAX_PATH);
	PathRemoveFileSpecW(lpResult);
	wcscat(lpResult, lpFile);
}

void SetProperWeekday(int dow, wchar_t * lpType){

	for(int i = 1; i <= 7; i++){
		if(IsBitOn(dow, i)){
			wcscat(lpType, g_Weekdays[i - 1]);
			wcscat(lpType, L", ");
		}
	}
}
void ParseDateFormat(wchar_t * lpFormat, wchar_t * lpDFormat, wchar_t * lpTFormat){

	wchar_t			* pos;

	lpTFormat[0] = '\0';
	wcscpy(lpDFormat, lpFormat);
	pos = _wcsichr(lpDFormat, L'H');
	if(pos){
		wcscpy(lpTFormat, pos);
		*pos = '\0';
	}
	else{
		pos = _wcsichr(lpDFormat, L'h');
		if(pos){
			wcscpy(lpTFormat, pos);
			*pos = '\0';
		}
	}
}

void GetScheduleDescription(int id, wchar_t * lpBuffer, const wchar_t * lpFile, const wchar_t * lpDefault){
	wchar_t 		szId[12];

	_itow(id, szId, 10);
	//get description for schedule type
	GetPrivateProfileStringW(S_OPTIONS, szId, lpDefault, lpBuffer, 256, lpFile);
}


void ParseScheduleType(P_SCHEDULE_TYPE pS, wchar_t * lpType){
	SYSTEMTIME		st;
	wchar_t			szDDatePart[128], szDTimePart[128], szTemp[256], szStart[256], szKey[16], szFile[MAX_PATH];
	wchar_t			szDFormat[128], szTFormat[128];
	wchar_t			szY[128], szMN[128], szW[128], szD[128], szH[128], szM[128], szS[128];

	lpType[0] = '\0';
	ParseDateFormat(g_DTFormats.DateFormat, szDFormat, szTFormat);
		
	ZeroMemory(&st, sizeof(st));
	st = pS->scDate;
	switch(pS->scType){
		case SCH_ED:
			GetTimeFormatW(LOCALE_USER_DEFAULT, 0, &st, L"HH:mm", lpType, 128);
			break;
		case SCH_ONCE:
			GetDateFormatW(LOCALE_USER_DEFAULT, 0, &st, szDFormat, lpType, 128);
			GetTimeFormatW(LOCALE_USER_DEFAULT, 0, &st, szTFormat, szDTimePart, 128);
			wcscat(lpType, szDTimePart);
			break;
		case SCH_REP:
			if(st.wDay > 0)
				wsprintfW(szD, L"%d %s ", st.wDay, g_RepParts[3]);
			else
				*szD = '\0';
			if(st.wHour > 0)
				wsprintfW(szH, L"%d %s ", st.wHour, g_RepParts[4]);
			else
				*szH = '\0';
			if(st.wMinute > 0)
				wsprintfW(szM, L"%d %s ", st.wMinute, g_RepParts[5]);
			else
				*szM = '\0';
			if(st.wSecond > 0)
				wsprintfW(szS, L"%d %s", st.wSecond, g_RepParts[6]);
			else
				*szS = '\0';
			wsprintfW(lpType, L"%s%s%s%s", szD, szH, szM, szS);
			break;
		case SCH_PER_WD:
			SetProperWeekday(st.wDayOfWeek, lpType);
			if(lpType){
				lpType[wcslen(lpType) - 2] = '\0';
			}
			GetTimeFormatW(LOCALE_USER_DEFAULT, 0, &st, L"' - 'HH:mm", szDTimePart, 128);
			wcscat(lpType, szDTimePart);
			break;
		case SCH_PER_YR:
			GetDateFormatW(LOCALE_USER_DEFAULT, 0, &st, szDFormat, lpType, 128);
			GetTimeFormatW(LOCALE_USER_DEFAULT, 0, &st, szTFormat, szDTimePart, 128);
			wcscat(lpType, szDTimePart);
			if(pS->reserved == SCH_YR_LUNAR)
				wcscat(lpType,  L"-Lunar");
			else
				wcscat(lpType, L"-Solar");
			break;
		case SCH_AFTER:
			if(st.wYear > 0)
				wsprintfW(szY, L"%d %s ", st.wYear, g_RepParts[0]);
			else
				*szY = '\0';
			if(st.wMonth > 0)
				wsprintfW(szMN, L"%d %s ", st.wMonth, g_RepParts[1]);
			else
				*szMN = '\0';
			if(st.wDayOfWeek > 0)
				wsprintfW(szW, L"%d %s ", st.wDayOfWeek, g_RepParts[2]);
			else
				*szW = '\0';
			if(st.wDay > 0)
				wsprintfW(szD, L"%d %s ", st.wDay, g_RepParts[3]);
			else
				*szD = '\0';
			if(st.wHour > 0)
				wsprintfW(szH, L"%d %s ", st.wHour, g_RepParts[4]);
			else
				*szH = '\0';
			if(st.wMinute > 0)
				wsprintfW(szM, L"%d %s ", st.wMinute, g_RepParts[5]);
			else
				*szM = '\0';
			if(st.wSecond > 0)
				wsprintfW(szS, L"%d %s", st.wSecond, g_RepParts[6]);
			else
				*szS = '\0';
			wsprintfW(lpType, L"%s%s%s%s%s%s%s", szY, szMN, szW, szD, szH, szM, szS);
			wcscpy(szFile, g_NotePaths.LangDir);
			wcscat(szFile, g_NotePaths.LangFile);
			_itow(IDC_ST_STARTING, szKey, 10);
			GetPrivateProfileStringW(S_OPTIONS, szKey, L"Starting from:", szStart, 128, szFile);
			GetDateFormatW(LOCALE_USER_DEFAULT, 0, &pS->scStart, szDFormat, szDDatePart, 128);
			GetTimeFormatW(LOCALE_USER_DEFAULT, 0, &pS->scStart, szTFormat, szDTimePart, 128);
			wsprintfW(szTemp, L" %s %s %s", szStart, szDDatePart, szDTimePart);
			wcscat(lpType, szTemp);
			break;
	}
}

short RealDayOfWeek(int dof){
	wchar_t			szBuffer[128];

	GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK, szBuffer, 128);
	switch(_wtoi(szBuffer)){
		case 0:		//Monday
			switch(dof){
				case 0:
					return 6;
				default:
					return dof - 1;
			}
		case 1:		//Tuesday
			switch(dof){
				case 0:
					return 5;
				case 1:
					return 6;
				default:
					return dof - 1;
			}
		case 2:		//Wednesday
			switch(dof){
				case 0:
					return 4;
				case 1:
					return 5;
				case 2:
					return 6;
				default:
					return dof - 1;
			}
		case 3:		//Thursday
			switch(dof){
				case 0:
					return 3;
				case 1:
					return 4;
				case 2:
					return 5;
				case 3:
					return 6;
				default:
					return dof - 1;
			}
		case 4:		//Friday
			switch(dof){
				case 5:
					return 0;
				case 6:
					return 1;
				default:
					return dof + 2;
			}
		case 5:		//Saturday
			switch(dof){
				case 6:
					return 0;
				default:
					return dof + 1;
			}
		default:		//6 = Sunday
			return dof;
	}
}

void FrameSkinlessNote(HDC hdc, LPRECT lprc, COLORREF clr){
	HBRUSH		hbr;
	RECT		rc;

	CopyRect(&rc, lprc);
	// rc.right -= 20;
	// rc.bottom -= 20;
	clr = ColorAdjustLuma(clr, -95, FALSE);
	hbr = CreateSolidBrush(clr);
	FrameRect(hdc, &rc, hbr);
	DeleteObject(hbr);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: FillSkinlessNoteCaption
 Created  : Tue Jan  8 11:55:52 2008
 Modified : Tue Jan  8 11:55:52 2008

 Synopsys : Fills skinless note caption or toolbar with gradient
 Input    : hdc - device context
            lprc - drawing rectangle
            clr - note background color
            lprGrip - sizing grip rectangle (optional)
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void FillSkinlessNoteCaption(HDC hdc, LPRECT lprc, COLORREF clr, LPRECT lprGrip){
	COLORREF			clr1, clr2;
	HPEN				hPen, hOldPen;

	clr1 = clr2 = clr;
	clr1 = ColorAdjustLuma(clr1, 95, FALSE);
	clr2 = ColorAdjustLuma(clr2, -95, FALSE);

	Fill2ColorsRectangle(hdc, lprc, clr1, clr2, GRADIENT_FILL_RECT_V);

	if(lprGrip != NULL){
		//if toolbar is drawn - dra separator line
		hPen = CreatePen(PS_SOLID, 1, clr2);
		hOldPen = SelectObject(hdc, hPen);
		MoveToEx(hdc, lprGrip->right - lprGrip->left, lprGrip->top, NULL);
		LineTo(hdc, lprGrip->left - 1, lprGrip->top);
		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: DrawSimpleNotePreview
 Created  : Tue Jan  8 11:52:37 2008
 Modified : Tue Jan  8 11:52:37 2008

 Synopsys : Draws preview of skinless note
 Input    : hwnd - handle of preview window
            lpDI - pointer to DRAWITEMSTRUCT structure
            lprWindow - pointer to note's body drawing rectangle
            lprCaption - pointe to note's caption drawing rectangle
            crWindow - note background color
            crFontCaption - caption font color
            hFontCaption - caption font
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void DrawSimpleNotePreview(HWND hwnd, const DRAWITEMSTRUCT * lpDI, LPRECT lprWindow, LPRECT lprCaption, 
	P_NOTE_APPEARANCE pA, P_NOTE_RTHANDLES pH){

	RECT		rc, rcText;
	int			height;
	HFONT		hOldFont, hOldNoteFont;
	COLORREF	crOldColor, crOldNoteTextColor;
	HBRUSH		hBrush;
	wchar_t		szCaption[64], szFile[MAX_PATH];

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);
	GetPrivateProfileStringW(S_PREVIEW, L"caption", L"Caption", szCaption, 64, szFile);
	hBrush = CreateSolidBrush(pA->crWindow);
	height = GetSystemMetrics(SM_CYSMCAPTION);
	GetClientRect(hwnd, &rc);
	SetBkMode(lpDI->hDC, TRANSPARENT);
	FillRect(lpDI->hDC, &rc, GetSysColorBrush(COLOR_WINDOW));
	CopyRect(lprWindow, &rc);
	InflateRect(lprWindow, -8, -8);
	SetRect(lprCaption, lprWindow->left, lprWindow->top, lprWindow->right, height + 8);
	CopyRect(&rcText, lprWindow);
	rcText.top = rcText.top + (lprCaption->bottom - lprCaption->top) + 1;
	lprWindow->top += height;
	FillRect(lpDI->hDC, lprWindow, hBrush);
	FillSkinlessNoteCaption(lpDI->hDC, lprCaption, pA->crWindow, NULL);
	crOldNoteTextColor = SetTextColor(lpDI->hDC, pA->crFont);
	hOldNoteFont = SelectObject(lpDI->hDC, pH->hFont);
	InflateRect(&rcText, -2, -2);
	DrawTextW(lpDI->hDC, g_Strings.FontSample, -1, &rcText, DT_LEFT | DT_WORDBREAK);
	SelectObject(lpDI->hDC, hOldNoteFont);
	SetTextColor(lpDI->hDC, crOldNoteTextColor);
	crOldColor = SetTextColor(lpDI->hDC,pA->crCaption);
	hOldFont = SelectObject(lpDI->hDC, pH->hFCaption);
	DrawTextW(lpDI->hDC, szCaption, -1, lprCaption, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	SelectObject(lpDI->hDC, hOldFont);
	SetTextColor(lpDI->hDC, crOldColor);
	DeleteObject(hBrush);
	//frame note
	FrameSkinlessNote(lpDI->hDC, lprWindow, pA->crWindow);
}

void DrawSimpleNotePreviewDock(HWND hwnd, const DRAWITEMSTRUCT * lpDI, P_PNDOCK pA, COLORREF crFont, P_NOTE_RTHANDLES pH){

	RECT		rc, rcText, rWindow, rCaption;
	int			height;
	HFONT		hOldFont, hOldNoteFont;
	COLORREF	crOldColor, crOldNoteTextColor;
	HBRUSH		hBrush;
	wchar_t		szCaption[64], szFile[MAX_PATH];

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);
	GetPrivateProfileStringW(S_PREVIEW, L"caption", L"Caption", szCaption, 64, szFile);
	hBrush = CreateSolidBrush(pA->crWindow);
	height = GetSystemMetrics(SM_CYSMCAPTION);
	GetClientRect(hwnd, &rc);
	SetBkMode(lpDI->hDC, TRANSPARENT);
	FillRect(lpDI->hDC, &rc, GetSysColorBrush(COLOR_WINDOW));
	CopyRect(&rWindow, &rc);
	InflateRect(&rWindow, -8, -8);
	SetRect(&rCaption, rWindow.left, rWindow.top, rWindow.right, height + 8);
	CopyRect(&rcText, &rWindow);
	rcText.top = rcText.top + (rCaption.bottom - rCaption.top) + 1;
	rWindow.top += height;
	FillRect(lpDI->hDC, &rWindow, hBrush);
	FillSkinlessNoteCaption(lpDI->hDC, &rCaption, pA->crWindow, NULL);
	crOldNoteTextColor = SetTextColor(lpDI->hDC, crFont);
	hOldNoteFont = SelectObject(lpDI->hDC, pH->hFont);
	InflateRect(&rcText, -2, -2);
	DrawTextW(lpDI->hDC, g_Strings.FontSample, -1, &rcText, DT_LEFT | DT_WORDBREAK);
	SelectObject(lpDI->hDC, hOldNoteFont);
	SetTextColor(lpDI->hDC, crOldNoteTextColor);
	crOldColor = SetTextColor(lpDI->hDC,pA->crCaption);
	hOldFont = SelectObject(lpDI->hDC, pH->hFCaption);
	DrawTextW(lpDI->hDC, szCaption, -1, &rCaption, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	SelectObject(lpDI->hDC, hOldFont);
	SetTextColor(lpDI->hDC, crOldColor);
	DeleteObject(hBrush);
	//frame note
	FrameSkinlessNote(lpDI->hDC, &rWindow, pA->crWindow);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ChangeSkinlessColor
 Created  : Tue Jan  8 11:42:34 2008
 Modified : Tue Jan  8 11:42:34 2008

 Synopsys : Changes skinless note background color
 Input    : hwnd - note window handle
            ptA - pointer to note appearance structure
 Output   : TRUE on success, FALSE otherwise
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
BOOL ChangeSkinlessColor(HWND hwnd, LPCOLORREF lpcrWindow, int id){
	CHOOSECOLORW		cc;
	COLORREF			custcc[16];
	wchar_t 			szBuffer[256], szPath[MAX_PATH];

	wcscpy(szPath, g_NotePaths.LangDir);
	wcscat(szPath, g_NotePaths.LangFile);
	GetPrivateProfileStringW(L"captions", L"color", L"Choose color", szBuffer, 256, szPath);

	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	for(int i = 0; i < NELEMS(custcc); i++)
		custcc[i] = 0xffffff;
	cc.lpCustColors = custcc;
	cc.Flags = CC_RGBINIT | CC_FULLOPEN | CC_ENABLEHOOK;
	cc.hwndOwner = hwnd;
	cc.rgbResult = *lpcrWindow;
	cc.lCustData = (int)szBuffer;
	cc.lpfnHook = CCHookProc;
	if(ChooseColorW(&cc)){
		if(*lpcrWindow != cc.rgbResult){
			*lpcrWindow = (cc.rgbResult == 0) ? 1 : (cc.rgbResult == CLR_MASK ? CR_IN_MASK : cc.rgbResult);
			RedrawWindow(GetDlgItem(hwnd, id), NULL, NULL, RDW_INVALIDATE);
			return TRUE;
		}
	}
	return FALSE;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ChangeSkinlessCaptionFont
 Created  : Tue Jan  8 11:28:20 2008
 Modified : Tue Jan  8 11:29:32 2008

 Synopsys : Changes font of skinless note caption
 Input    : hwnd - note window handle
            ptA - pointer to note appearance structure
            ptH - pointer to note run-time handles
 Output   : changed part - font or/and font color
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
int ChangeSkinlessCaptionFont(HWND hwnd, LPLOGFONTW plfCaption, LPCOLORREF lpcrCaption, P_NOTE_RTHANDLES ptH, int id){
	CHOOSEFONTW			cf;
	int					ret = 0;
	LOGFONTW			lfw;
	wchar_t 			szBuffer[256], szPath[MAX_PATH];

	wcscpy(szPath, g_NotePaths.LangDir);
	wcscat(szPath, g_NotePaths.LangFile);
	GetPrivateProfileStringW(L"captions", L"font", L"Choose font", szBuffer, 256, szPath);

	memcpy(&lfw, plfCaption, sizeof(lfw));
	ZeroMemory(&cf, sizeof(cf));
	cf.lStructSize = sizeof(cf);
	cf.Flags = CF_SCREENFONTS | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT | CF_ENABLEHOOK;
	cf.hwndOwner = hwnd;
	cf.lpLogFont = &lfw;
	cf.rgbColors = *lpcrCaption;
	cf.lpfnHook = CFHookProc;
	cf.lCustData = (int)szBuffer;
	if(ChooseFontW(&cf)){
		if(!IsFontsEqual(plfCaption, cf.lpLogFont)){
			memcpy(plfCaption, &lfw, sizeof(lfw));
			ptH->hFCaption = CreateFontIndirectW(cf.lpLogFont);
			ret |= F_C_FONT;
		}
		if(*lpcrCaption != cf.rgbColors){
			ret |= F_C_COLOR;
			*lpcrCaption = (cf.rgbColors == 0) ? 1 : (cf.rgbColors == CLR_MASK ? CR_IN_MASK : cf.rgbColors);
		}
		RedrawWindow(GetDlgItem(hwnd, id), NULL, NULL, RDW_INVALIDATE);
	}
	return ret;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SendNoteByEmail
 Created  : Tue Jan  8 11:26:26 2008
 Modified : Tue Jan  8 11:26:26 2008

 Synopsys : Sends active note by default e-mail client
 Input    : pNote - memory note handle
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
void SendNoteByEmail(PMEMNOTE pNote){
	wchar_t				szMail[1024 * 20], szSubject[256];
	GETTEXTLENGTHEX		gtx;
	TEXTRANGEW			trg;
	long				size;
	HWND				hEdit = NULL;

	if(pNote->pData){
		if(pNote->pData->visible)
			hEdit = (HWND)GetPropW(pNote->hwnd, PH_EDIT);
		else
			hEdit = GetUnvisibleNoteEdit(pNote->pFlags->id);

		if(hEdit){
			wcscpy(szSubject, MAIL_SUBJECT);
			wcscpy(szMail, L"mailto:?subject=");
			wcscat(szSubject, pNote->pData->szName);
			wcscat(szSubject, L"'");
			wcscat(szMail, szSubject);
			gtx.flags = GTL_USECRLF;
			gtx.codepage = 1200;
			size = SendMessageW(hEdit, EM_GETTEXTLENGTHEX, (WPARAM)&gtx, 0);
			trg.chrg.cpMin = 0;
			trg.chrg.cpMax = size + 1;
			trg.lpstrText = calloc(1, (size + 1) * sizeof(wchar_t));
			SendMessageW(hEdit, EM_GETTEXTRANGE, 0, (LPARAM)&trg);
			wcscat(szMail, L"&body=");
			wcscat(szMail, trg.lpstrText);
			free(trg.lpstrText);
			if(!pNote->pData->visible)
				SendMessageW(hEdit, WM_DESTROY, 0, 0);
			ShellExecuteW(pNote->hwnd, L"open", szMail, NULL, NULL, SW_SHOWDEFAULT);
		}
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetUnvisibleNoteEdit
 Created  : Wed Jun 10 10:14:16 2009
 Modified : Wed Jun 10 10:14:16 2009

 Synopsys : Returns handle to Rich Edit control with loaded RTF content of 
            unvisible note
 Input    : lpID - note ID
 Output   : Handle to Rich edit control with loaded RTF content of 
            unvisible note. It's up to developer to destroy this handle 
            after use.
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
HWND GetUnvisibleNoteEdit(wchar_t * lpID){
	HWND	hEdit = NULL;
	
	hEdit = CreateWindowExW(0, RICHEDIT_CLASSW, NULL, WS_CHILD | ES_MULTILINE, 0, 0, 0, 0, g_hMain, NULL, g_hInstance, NULL);
	if(hEdit){
		ReadRTFFile(lpID, hEdit, NULL);
	}
	return hEdit;
}

void ReadRTFFile(wchar_t * lpID, HWND hEdit, wchar_t * lpCopyFrom){
	wchar_t				szPath[MAX_PATH];
	HANDLE				hFile;
	EDITSTREAM			esm;

	wcscpy(szPath, g_NotePaths.DataDir);
	wcscat(szPath, lpID);
	wcscat(szPath, NOTE_EXTENTION);
	if(lpCopyFrom){
		CopyFileW(lpCopyFrom, szPath, FALSE);
	}
	//load RTF file contents
	hFile = CreateFileW(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE){
		esm.dwCookie = (DWORD)hFile;
		esm.dwError = 0;
		esm.pfnCallback = InStreamCallback;
		RichEdit_StreamIn(hEdit, SF_RTF | SF_UNICODE, &esm);
		CloseHandle(hFile);
	}		
}

/*-@@+@@------------------------------------------------------------------
 Procedure: NotesDirExists
 Created  : Sun Aug 12 15:43:40 2007
 Modified : Sun Aug 12 15:43:40 2007

 Synopsys : Checks whether one of program global paths exists
 Input    : lpDir - global path
 Output   : TRUE if succeeded, otherwise - FALSE
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
BOOL NotesDirExists(wchar_t * lpDir){
	wchar_t				filespec[MAX_PATH];
	WIN32_FIND_DATAW 	fd;
	HANDLE 				handle = INVALID_HANDLE_VALUE;

	wcscpy(filespec, lpDir);
	//remove trailing backslash
	filespec[wcslen(filespec) - 1] = '\0';
	handle = FindFirstFileW(filespec, &fd);
	if(handle == INVALID_HANDLE_VALUE)
		return FALSE;
	else{
		FindClose(handle);
		return TRUE;
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: NotesDirCreate
 Created  : Sun Aug 12 15:42:23 2007
 Modified : Sun Aug 12 15:42:23 2007

 Synopsys : Creates directory, specified in one of global paths
 Input    : lpDir - global path
 Output   : TRUE if succeeded, otherwise - FALSE
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
BOOL NotesDirCreate(wchar_t * lpDir){
	wchar_t				filespec[MAX_PATH];

	wcscpy(filespec, lpDir);
	//remove trailing backslash
	filespec[wcslen(filespec) - 1] = '\0';
	return CreateDirectoryW(filespec, NULL);
}

UINT APIENTRY CCHookProc(HWND hdlg,	UINT uiMsg,	WPARAM wParam, LPARAM lParam){
	if(uiMsg == WM_INITDIALOG){
		CHOOSECOLOR *lcc = (CHOOSECOLOR *)lParam;
		SetWindowTextW(hdlg, (LPCWSTR)lcc->lCustData);
	}
	return FALSE;
}

UINT APIENTRY CFHookProc(HWND hdlg,	UINT uiMsg,	WPARAM wParam, LPARAM lParam){
	if(uiMsg == WM_INITDIALOG){
		CHOOSEFONT *lcc = (CHOOSEFONT *)lParam;
		SetWindowTextW(hdlg, (LPCWSTR)lcc->lCustData);
	}
	return FALSE;
}

void CreateColorMenu(HMENU hMenu, int * idMenu, const int idStart, wchar_t * lpCaption){

	int 		count = GetMenuItemCount(hMenu);
	int			pos = 0;
	MItem		mit;

	ZeroMemory(&mit, sizeof(mit));
	mit.xPos = -1;
	mit.yPos = -1;
	mit.xCheck = -1;
	mit.yCheck = -1;
	FreeMenus(hMenu);
	for(int i = count - 1; i >= 0; i--){
		DeleteMenu(hMenu, i, MF_BYPOSITION);
	}
	mit.id = idStart;
	wcscpy(mit.szText, lpCaption);
	AppendMenuW(hMenu, MF_STRING, mit.id++, lpCaption);
	SetMenuItemProperties(&mit, hMenu, pos++);
	mit.type = MT_COLORITEM;
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"0");
	wcscpy(mit.szText, L"0");
	SetMenuItemProperties(&mit, hMenu, pos++);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"8388608");
	wcscpy(mit.szText, L"8388608");
	SetMenuItemProperties(&mit, hMenu, pos++);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"32768");
	wcscpy(mit.szText, L"32768");
	SetMenuItemProperties(&mit, hMenu, pos++);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"8421376");
	wcscpy(mit.szText, L"8421376");
	SetMenuItemProperties(&mit, hMenu, pos++);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"128");
	wcscpy(mit.szText, L"128");
	SetMenuItemProperties(&mit, hMenu, pos++);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"8388736");
	wcscpy(mit.szText, L"8388736");
	SetMenuItemProperties(&mit, hMenu, pos++);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"32896");
	wcscpy(mit.szText, L"32896");
	SetMenuItemProperties(&mit, hMenu, pos++);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"12632256");
	wcscpy(mit.szText, L"12632256");
	SetMenuItemProperties(&mit, hMenu, pos++);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"8421504");
	wcscpy(mit.szText, L"8421504");
	SetMenuItemProperties(&mit, hMenu, pos++);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"16711680");
	wcscpy(mit.szText, L"16711680");
	SetMenuItemProperties(&mit, hMenu, pos++);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"65280");
	wcscpy(mit.szText, L"65280");
	SetMenuItemProperties(&mit, hMenu, pos++);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"16776960");
	wcscpy(mit.szText, L"16776960");
	SetMenuItemProperties(&mit, hMenu, pos++);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"255");
	wcscpy(mit.szText, L"255");
	SetMenuItemProperties(&mit, hMenu, pos++);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"16711935");
	wcscpy(mit.szText, L"16711935");
	SetMenuItemProperties(&mit, hMenu, pos++);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"65535");
	wcscpy(mit.szText, L"65535");
	SetMenuItemProperties(&mit, hMenu, pos++);
	AppendMenuW(hMenu, MF_STRING, mit.id++, L"16777215");
	wcscpy(mit.szText, L"16777215");
	SetMenuItemProperties(&mit, hMenu, pos++);
	*idMenu = --mit.id;
}

int GetMaxSizeMenuID(void){
	return m_IDSizeMenu;
}

void RemoveSizeMenu(void){
	if(g_hSizePopUp){
		DestroyMenu(g_hSizePopUp);
		g_hSizePopUp = NULL;
	}
}

void CreateSizeMenu(HMENU * pMenu, BOOL fCreate){
	wchar_t		szSize[8];
	int			size, type;

	type = MF_STRING;
	if(fCreate){
		*pMenu = CreatePopupMenu();
	}
	else{
		int count = GetMenuItemCount(*pMenu);
		for(int i = 0; i < count; i++)
			RemoveMenu(*pMenu, i, MF_BYPOSITION);
		// type = MF_OWNERDRAW;
	}
	size = 8;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 9;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 10;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 11;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 12;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 14;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 16;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 18;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 20;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 22;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 24;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 26;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 28;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 36;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 48;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
	size = 72;
	_itow(size, szSize, 10);
	AppendMenuW(*pMenu, type, IDI_SIZE_START + size, szSize);
}

void SetCFSize(int size){
	m_FSize = size;
}

int GetCFSize(void){
	return m_FSize;
}

void ConstructSaveMessage(wchar_t * lpNoteName, wchar_t * lpBuffer){

	wchar_t		szName[128];

	wcscpy(szName, __T("\n<"));
	wcscat(szName, lpNoteName);
	wcscat(szName, __T(">"));
	wcscpy(lpBuffer, g_Strings.SNote);
	wcscat(lpBuffer, szName);
	wcscat(lpBuffer, __T("\n"));
	wcscat(lpBuffer, g_Strings.SQuestion);
}

void LoadSearchHistory(HWND hwnd){
	wchar_t		szBuffer[256], szKey[16];

	for(int i = 0; i < MAX_SEARCH_COUNT; i++){
		_itow(i, szKey, 10);
		if(GetPrivateProfileStringW(S_SEARCH_HIST, szKey, NULL, szBuffer, 256, g_NotePaths.INIFile)){
			if(wcslen(szBuffer)){
				SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_ADDSTRING, 0, (LPARAM)szBuffer);
			}
		}
	}
}

void LoadReplaceHistory(HWND hwnd){
	wchar_t		szBuffer[256], szKey[16];

	for(int i = 0; i < MAX_SEARCH_COUNT; i++){
		_itow(i, szKey, 10);
		if(GetPrivateProfileStringW(S_REPLACE_HIST, szKey, NULL, szBuffer, 256, g_NotePaths.INIFile)){
			if(wcslen(szBuffer)){
				SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
			}
		}
	}

}

int SharedReplaceProc(HWND hEdit){
	TEXTRANGEW		tr;
	int				result;
	
	result = SharedFindProc(hEdit);
	if(result != -1){
		SendMessageW(hEdit, EM_EXGETSEL, 0, (LPARAM)&tr.chrg);
		SendMessageW(hEdit, EM_REPLACESEL, TRUE, (LPARAM)g_ReplaceString);
	}

	return result;
}

int SharedFindProc(HWND hEdit){
	TEXTRANGEW		tr;
	FINDTEXTEXW		ft;
	int				result;

	SendMessageW(hEdit, EM_EXGETSEL, 0, (LPARAM)&tr.chrg);

	if((g_Flags & FR_DOWN) == FR_DOWN){
		ft.chrg.cpMin = tr.chrg.cpMax;
		ft.chrg.cpMax = -1;
	}
	else{
		ft.chrg.cpMin = tr.chrg.cpMin;
		ft.chrg.cpMax = -1;
	}
	ft.lpstrText = g_SearchString;
	result = SendMessageW(hEdit, EM_FINDTEXTEXW, g_Flags, (LPARAM)&ft);
	if(result != -1){
		SendMessageW(hEdit, EM_EXSETSEL, 0, (LPARAM)&ft.chrgText);
	}
	return result;
}

void SaveSearchHistory(HWND hwnd){
	wchar_t		szBuffer[256], szKey[16];

	if(SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_GETCOUNT, 0, 0) > 0){
		_itow(MAX_SEARCH_COUNT, szBuffer, 10);
		WritePrivateProfileStringW(S_SEARCH_HIST, L"count", szBuffer, g_NotePaths.INIFile);
		for(int i = 0; i < MAX_SEARCH_COUNT; i++){
			*szBuffer = '\0';
			if(SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_GETLBTEXT, i, (LPARAM)szBuffer) != CB_ERR){
				if(wcslen(szBuffer)){
					_itow(i, szKey, 10);
					WritePrivateProfileStringW(S_SEARCH_HIST, szKey, szBuffer, g_NotePaths.INIFile);
				}
			}
		}
	}
}

void SaveReplaceHistory(HWND hwnd){
	wchar_t		szBuffer[256], szKey[16];

	if(SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_GETCOUNT, 0, 0) > 0){
		_itow(MAX_SEARCH_COUNT, szBuffer, 10);
		WritePrivateProfileStringW(S_REPLACE_HIST, L"count", szBuffer, g_NotePaths.INIFile);
		for(int i = 0; i < MAX_SEARCH_COUNT; i++){
			*szBuffer = '\0';
			if(SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_GETLBTEXT, i, (LPARAM)szBuffer) != CB_ERR){
				if(wcslen(szBuffer)){
					_itow(i, szKey, 10);
					WritePrivateProfileStringW(S_REPLACE_HIST, szKey, szBuffer, g_NotePaths.INIFile);
				}
			}
		}
	}
}

int GetMenuPosition(HMENU hMenu, int id){
	int				count = GetMenuItemCount(hMenu);
	MENUITEMINFOW	mi;

	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_ID;
	for(int i = 0; i < count; i++){
		GetMenuItemInfoW(hMenu, i, TRUE, &mi);
		if(mi.wID == id){
			return i;
		}
	}
	return -1;
}

void PrepareDockMenu(PMEMNOTE pNote, HMENU hMenu, int index){
	HMENU				hDock;
	int					count;

	hDock = GetSubMenu(hMenu, index);
	count = GetMenuItemCount(hDock);
	for(int i = 0; i < count; i++){
		EnableMenuItem(hDock, i, MF_BYPOSITION | MF_ENABLED);
	}
	if(pNote){
		switch(DockType(pNote->pData->dockData)){
		case DOCK_NONE:
			EnableMenuItem(hDock, IDM_DOCK_NONE, MF_BYCOMMAND | MF_GRAYED);
			break;
		case DOCK_LEFT:
			EnableMenuItem(hDock, IDM_DOCK_LEFT, MF_BYCOMMAND | MF_GRAYED);
			break;
		case DOCK_TOP:
			EnableMenuItem(hDock, IDM_DOCK_TOP, MF_BYCOMMAND | MF_GRAYED);
			break;
		case DOCK_RIGHT:
			EnableMenuItem(hDock, IDM_DOCK_RIGHT, MF_BYCOMMAND | MF_GRAYED);
			break;
		case DOCK_BOTTOM:
			EnableMenuItem(hDock, IDM_DOCK_BOTTOM, MF_BYCOMMAND | MF_GRAYED);
			break;
		}
	}
}

void ApplyDockColors(PMEMNOTE pNote){
	NOTE_APPEARANCE		nap;

	if(g_DockSettings.fCustColor)
		RichEdit_SetBkgndColor(GetPropW(pNote->hwnd, PH_EDIT), 0, g_DockSettings.crWindow);
	else
		RichEdit_SetBkgndColor(GetPropW(pNote->hwnd, PH_EDIT), 0, pNote->pAppearance->crWindow);
	if(g_DockSettings.fCustCaption)
		nap.crCaption = g_DockSettings.crCaption;
	else
		nap.crCaption = pNote->pAppearance->crCaption;
	if(g_DockSettings.fCustColor)
		nap.crWindow = g_DockSettings.crWindow;
	else
		nap.crWindow = pNote->pAppearance->crWindow;
	CreateSkinlessToolbarBitmap(pNote->hwnd, &nap, pNote->pRTHandles, &pNote->pRTHandles->hbCommand);
	CreateSkinlessSysBitmap(pNote->hwnd, &nap, pNote->pRTHandles, &pNote->pRTHandles->hbDelHide);
}


/*-@@+@@------------------------------------------------------------------
 Procedure: GetHotKeyByID
 Created  : Sun Aug 12 16:47:38 2007
 Modified : Sun Aug 12 16:47:38 2007

 Synopsys : Returns pointer to hot key for specified array by id
 Input    : id - hot key id
            phk - pointer to hot keys array
 Output   : Pointer to the hot key if found, otherwise - NULL
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
P_HK_TYPE GetHotKeyByID(int id, P_HK_TYPE phk, int count){
	for(int i = 0; i < count; i++){
		if(phk->id == id)
			return phk;
		phk++;
	}
	return NULL;
}

void PrepareMenuRecursiveWithAccelerators(PMItem pMenus, int size, HMENU hMenu, wchar_t * lpLangFile, wchar_t * lpSection, P_HK_TYPE lpKeys, int keysCount){
	int				count;
	MENUITEMINFOW	mi;
	wchar_t			szText[128], szDefault[128];
	PMItem			pmi;
	P_HK_TYPE		pHK;
	wchar_t			szBuffer[128];

	count = GetMenuItemCount(hMenu);
	for(int i = 0; i < count; i++){
		ZeroMemory(&mi, sizeof(mi));
		mi.cbSize = sizeof(mi);
		mi.fMask = MIIM_ID | MIIM_SUBMENU | MIIM_FTYPE;
		GetMenuItemInfoW(hMenu, i, TRUE, &mi);
		if((mi.fType & MFT_SEPARATOR) != MFT_SEPARATOR){
			if(mi.hSubMenu){
				PrepareMenuRecursiveWithAccelerators(pMenus, size, mi.hSubMenu, lpLangFile, lpSection, lpKeys, keysCount);
			}
			GetMIText(pMenus, size, mi.wID, szDefault);
			pHK = GetHotKeyByID(mi.wID, lpKeys, keysCount);
			if(pHK){
				if(pHK->fsModifiers || pHK->vk)
					SetMenuTextWithAccelerator(mi.wID, lpSection, lpLangFile, szDefault, szText, pHK->szKey);
				else{
					*szBuffer = '\0';
					GetMIReserved(pMenus, size, mi.wID, szBuffer);
					if(*szBuffer)
						SetMenuTextWithAccelerator(mi.wID, lpSection, lpLangFile, szDefault, szText, szBuffer);
					else
						SetMenuText(mi.wID, lpSection, lpLangFile, szDefault, szText);
				}
			}		
			else{
				*szBuffer = '\0';
				GetMIReserved(pMenus, size, mi.wID, szBuffer);
				if(*szBuffer)
					SetMenuTextWithAccelerator(mi.wID, lpSection, lpLangFile, szDefault, szText, szBuffer);
				else
					SetMenuText(mi.wID, lpSection, lpLangFile, szDefault, szText);
			}
			SetMIText(pMenus, size, mi.wID, szText);
			pmi = GetMItem(pMenus, size, mi.wID);
			if(pmi){
				SetMenuItemProperties(pmi, hMenu, i);
			}
		}
		if((mi.fType & MFT_OWNERDRAW) != MFT_OWNERDRAW){
			mi.fType |= MFT_OWNERDRAW;
			SetMenuItemInfoW(hMenu, i, TRUE, &mi);
		}
	}
}

void PrepareMenuRecursive(PMItem pMenus, int size, HMENU hMenu, wchar_t * lpLangFile, wchar_t * lpSection){
	int				count;
	MENUITEMINFOW	mi;
	wchar_t			szText[128], szDefault[128], szBuffer[128];
	PMItem			pmi;

	count = GetMenuItemCount(hMenu);
	for(int i = 0; i < count; i++){
		ZeroMemory(&mi, sizeof(mi));
		mi.cbSize = sizeof(mi);
		mi.fMask = MIIM_ID | MIIM_SUBMENU | MIIM_FTYPE;
		GetMenuItemInfoW(hMenu, i, TRUE, &mi);
		if((mi.fType & MFT_SEPARATOR) != MFT_SEPARATOR){
			if(mi.hSubMenu){
				PrepareMenuRecursive(pMenus, size, mi.hSubMenu, lpLangFile, lpSection);
			}
			
			GetMIText(pMenus, size, mi.wID, szDefault);
			*szBuffer = '\0';
			GetMIReserved(pMenus, size, mi.wID, szBuffer);
			if(*szBuffer)
				SetMenuTextWithAccelerator(mi.wID, lpSection, lpLangFile, szDefault, szText, szBuffer);
			else
				SetMenuText(mi.wID, lpSection, lpLangFile, szDefault, szText);
			SetMIText(pMenus, size, mi.wID, szText);
			pmi = GetMItem(pMenus, size, mi.wID);
			if(pmi){
				SetMenuItemProperties(pmi, hMenu, i);
			}
		}
		if((mi.fType & MFT_OWNERDRAW) != MFT_OWNERDRAW){
			mi.fType |= MFT_OWNERDRAW;
			SetMenuItemInfoW(hMenu, i, TRUE, &mi);
		}
	}
}

PMItem GetMItem(PMItem	lpMI, const int size, int id){
	for(int i = 0; i < size; i++){
		if(lpMI->id == id){
			return lpMI;
		}
		lpMI++;
	}
	return NULL;
}

void GetMIReserved(MItem * lpMI, const int size, int id, wchar_t * lpRes){
	for(int i = 0; i < size; i++){
		if(lpMI->id == id){
			wcscpy(lpRes, lpMI->szReserved);
			break;
		}
		lpMI++;
	}
}

void GetMIText(MItem * lpMI, const int size, int id, wchar_t * lpText){

	for(int i = 0; i < size; i++){
		if(lpMI->id == id){
			wcscpy(lpText, lpMI->szText);
			break;
		}
		lpMI++;
	}
}

int ItemImageIndex(PMEMNOTE pNote){
	if(pNote->pData->visible){
		if(pNote->pFlags->fromDB){
			//loaded note
			if(pNote->pFlags->saved){
				//unchanged or saved note
				if(pNote->pSchedule->scType == SCH_NO){
					//not scheduled note
					return 0;
				}
				else{
					//scheduled note
					return 2;
				}
			}
			else{
				//changed note
				if(pNote->pSchedule->scType == SCH_NO){
					//not scheduled note
					return 1;
				}
				else{
					//scheduled note
					return 3;
				}
			}
		}
		else{
			//new note
			if(pNote->pFlags->saved){
				//unchanged or saved note
				if(pNote->pSchedule->scType == SCH_NO){
					//not scheduled note
					return 4;
				}
				else{
					//scheduled note
					return 6;
				}
			}
			else{
				//changed note
				if(pNote->pSchedule->scType == SCH_NO){
					//not scheduled note
					return 5;
				}
				else{
					//scheduled note
					return 7;
				}
			}
		}
	}
	else{
		//hidden note
		if(pNote->pData->idGroup == GROUP_RECYCLE){
			//deleted note
			return 16;
		}
		else{
			if(pNote->pFlags->fromDB){
				//loaded note
				if(pNote->pFlags->saved){
					//unchanged or saved note
					if(pNote->pSchedule->scType == SCH_NO){
						//not scheduled note
						return 8;
					}
					else{
						//scheduled note
						return 10;
					}
				}
				else{
					//changed note
					if(pNote->pSchedule->scType == SCH_NO){
						//not scheduled note
						return 9;
					}
					else{
						//scheduled note
						return 11;
					}
				}
			}
			else{
				//new note
				if(pNote->pFlags->saved){
					//unchanged or saved note
					if(pNote->pSchedule->scType == SCH_NO){
						//not scheduled note
						return 12;
					}
					else{
						//scheduled note
						return 14;
					}
				}
				else{
					//changed note
					if(pNote->pSchedule->scType == SCH_NO){
						//not scheduled note
						return 13;
					}
					else{
						//scheduled note
						return 15;
					}
				}
			}
		}
	}
}

BOOL IsTextInClipboard(void){
	if(!IsClipboardFormatAvailable(CF_TEXT))
		if(!IsClipboardFormatAvailable(CF_UNICODETEXT))
			if(!IsClipboardFormatAvailable(CF_OEMTEXT))
				return FALSE;
	return TRUE;
}

DWORD InStreamCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb){
	ReadFile((HANDLE)dwCookie, pbBuff, cb, (PULONG)pcb, NULL);
	return 0;
}

int __cdecl GroupMenusCompare(const void *p1, const void *p2){
	P_GROUP_MENU_STRUCT		pg1, pg2;

	pg1 = (P_GROUP_MENU_STRUCT)p1;
	pg2 = (P_GROUP_MENU_STRUCT)p2;

	return _wcsicmp(pg1->pName, pg2->pName);
}

int __cdecl FavMenusCompare(const void *p1, const void *p2){
	P_FAV_MENU_STRUCT		pg1, pg2;

	pg1 = (P_FAV_MENU_STRUCT)p1;
	pg2 = (P_FAV_MENU_STRUCT)p2;

	return _wcsicmp(pg1->pName, pg2->pName);
}
