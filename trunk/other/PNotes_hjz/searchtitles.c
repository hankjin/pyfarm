// Copyright (C) 2009 Andrey Gruber (aka lamer)

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

static BOOL Search_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Search_OnDestroy(HWND hwnd);
static void Search_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void PrepareResultsList(HWND hwnd);
static LRESULT CALLBACK List_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static void ApplySearchLanguage(HWND hwnd);
static void FindTitles(HWND hList);

static wchar_t			*m_cols[] = {L"Title"};
static HIMAGELIST		m_hImlSearch = NULL;
static int				m_count;

BOOL CALLBACK SearchTitles_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		HANDLE_MSG (hwnd, WM_INITDIALOG, Search_OnInitDialog);
		HANDLE_MSG (hwnd, WM_COMMAND, Search_OnCommand);
		HANDLE_MSG (hwnd, WM_DESTROY, Search_OnDestroy);
		case WM_NOTIFY:{
			NMHDR *lpnm = (NMHDR *)lParam;
			if(lpnm->code == NM_DBLCLK && lpnm->idFrom == IDC_LST_RESULTS){
				HWND			hList = GetDlgItem(hwnd, IDC_LST_RESULTS);
				PMEMNOTE		pNote;
				NMITEMACTIVATE *lpi = (NMITEMACTIVATE *)lParam;
				LVITEMW			lvi;

				ZeroMemory(&lvi, sizeof(lvi));
				lvi.mask = LVIF_TEXT | LVIF_PARAM;
				lvi.iItem = lpi->iItem;
				SendMessageW(hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
				pNote = (PMEMNOTE)lvi.lParam;
				if(pNote->pData->idGroup == GROUP_RECYCLE){
					return TRUE;
				}
				if(!pNote->pData->visible){
					SendMessageW(hwnd, CHM_SHOW_AFTER_SEARCH, (WPARAM)pNote, 0);
					if(g_hCPDialog)
						SendMessageW(g_hCPDialog, CHM_CTRL_UPD, 0, (LPARAM)pNote);
				}
				ShowNote(pNote);
			}
			return TRUE;
		}
		case CHM_SHOW_AFTER_SEARCH:{
			HWND	hList = GetDlgItem(hwnd, IDC_LST_RESULTS);
			LVITEMW	lv;
			int		count;
			wchar_t	szBuffer[256];

			ZeroMemory(&lv, sizeof(lv));
			lv.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE | LVIF_STATE;
			lv.cchTextMax = 256;
			lv.pszText = szBuffer;
			count = ListView_GetItemCount(hList);
			for(int i = 0; i < count; i++){
				lv.iItem = i;
				SendMessageW(hList, LVM_GETITEMW, 0, (LPARAM)&lv);
				if(lv.lParam == wParam){
					lv.iImage = 0;
					SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lv);
				}
			}
			return TRUE;
		}
		case CHM_BECOMES_HIDDEN:{
			HWND	hList = GetDlgItem(hwnd, IDC_LST_RESULTS);
			LVITEMW	lv;
			int		count;
			wchar_t	szBuffer[256];

			ZeroMemory(&lv, sizeof(lv));
			lv.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE | LVIF_STATE;
			lv.cchTextMax = 256;
			lv.pszText = szBuffer;
			count = ListView_GetItemCount(hList);
			for(int i = 0; i < count; i++){
				lv.iItem = i;
				SendMessageW(hList, LVM_GETITEMW, 0, (LPARAM)&lv);
				if(lv.lParam == wParam){
					lv.iImage = 8;
					SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lv);
				}
			}
			return TRUE;
		}
		case CHM_CTRL_UPD_LANG:
			ApplySearchLanguage(hwnd);
			return TRUE;
		default: return FALSE;
	}
}

static BOOL Search_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	g_hSearchTitlesDialog = hwnd;
	CenterWindow(hwnd, FALSE);
	PrepareResultsList(hwnd);
	ApplySearchLanguage(hwnd);
	SendDlgItemMessageW(hwnd, IDC_EDI_TITLE_SEARCH, EM_LIMITTEXT, 255, 0);
	SetFocus(GetDlgItem(hwnd, IDC_EDI_TITLE_SEARCH));
	return FALSE;
}

static void Search_OnDestroy(HWND hwnd)
{
	if(m_hImlSearch)
		ImageList_Destroy(m_hImlSearch);
	g_hSearchTitlesDialog = NULL;
}

static void Search_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify){
	switch(id){
		case IDCANCEL:
			EndDialog(hwnd, 0);
			break;
		case IDOK:
			if(IsWindowEnabled(GetDlgItem(hwnd, IDC_CMD_FIND))){
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CMD_FIND, BN_CLICKED), (LPARAM)GetDlgItem(hwnd, IDC_CMD_FIND));
			}
			break;
		case IDC_EDI_TITLE_SEARCH:
			if(codeNotify == EN_CHANGE){
				int len = SendDlgItemMessageW(hwnd, IDC_EDI_TITLE_SEARCH, WM_GETTEXTLENGTH, 0, 0);
				if(len)
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND), TRUE);
				else
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND), FALSE);
			}
			break;
		case IDC_CMD_FIND:{
			wchar_t		szBuffer[256], szMessage[512], szTemp[12];

			ListView_DeleteAllItems(GetDlgItem(hwnd, IDC_LST_RESULTS));
			FindTitles(GetDlgItem(hwnd, IDC_LST_RESULTS));
			GetWindowTextW(hwnd, szBuffer, 255);
			wcscpy(szMessage, g_Strings.SearchComplete);
			wcscat(szMessage, L"\n");
			if(m_count > 0){
				wcscat(szMessage, g_Strings.MatchesFound);
				wcscat(szMessage, L" ");
				_itow(m_count, szTemp, 10);
				wcscat(szMessage, szTemp);
			}
			else
				wcscat(szMessage, g_Strings.NoMatchesFound);
			MessageBoxW(hwnd, szMessage, szBuffer, MB_OK | MB_ICONINFORMATION);
			break;
		}
	}
}

static void PrepareResultsList(HWND hwnd){
	HWND		hList;
	LVCOLUMNW	lvc;
	HBITMAP		hBmp;
	wchar_t		szHidden[] = L"Hidden";
	int			j;

	//prepare and set list view image lists
	hBmp = LoadBitmapW(g_hInstance, MAKEINTRESOURCEW(IDB_CTRL_SMALL));
	m_hImlSearch = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 0, 1);
	ImageList_AddMasked(m_hImlSearch, hBmp, CLR_MASK);
	DeleteObject(hBmp);
	DeleteObject((void *)CLR_MASK);

	hList = GetDlgItem(hwnd, IDC_LST_RESULTS);
	SetWindowLongPtrW(hList, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(hList, GWLP_WNDPROC, (LONG_PTR)List_Proc));
	ListView_SetImageList(hList, m_hImlSearch, LVSIL_SMALL);
	SendMessageW(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES, LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES);
	ZeroMemory(&lvc, sizeof(lvc));
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	for(int i = 0; i < NELEMS(m_cols); i++){
		if(i > 0){
			lvc.fmt = LVCFMT_RIGHT;
			lvc.cx = 96;
		}
		else{
			lvc.cx = 360;
		}
		lvc.iSubItem = i;
		lvc.pszText = m_cols[i];
		SendMessageW(hList, LVM_INSERTCOLUMNW, i, (LPARAM)&lvc);
		j = i;
	}
	j++;
	lvc.cx = 0;
	lvc.iSubItem = j;
	lvc.pszText = szHidden;
	SendMessageW(hList, LVM_INSERTCOLUMNW, j++, (LPARAM)&lvc);
	lvc.iSubItem = j;
	lvc.pszText = szHidden;
	SendMessageW(hList, LVM_INSERTCOLUMNW, j++, (LPARAM)&lvc);
}

static LRESULT CALLBACK List_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){

	LPNMHEADER		lpnm;

	switch(uMsg){
	case WM_NOTIFY:
		lpnm = (LPNMHEADER)lParam;
		switch(lpnm->hdr.code){
		case HDN_DIVIDERDBLCLICK:
		case HDN_DIVIDERDBLCLICKW:
		case HDN_BEGINTRACK:
		case HDN_BEGINTRACKW:
			if(lpnm->iItem > NELEMS(m_cols) - 1)
				return TRUE;
			else
				return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
		default:
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
		}
	default:
		return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
	}
}

static void ApplySearchLanguage(HWND hwnd){
	wchar_t			szBuffer[256], szFile[MAX_PATH], szKey[16];
	LVCOLUMNW		lvc;

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);

	_itow(DLG_TITLES_SEARCH, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Find by title", szBuffer, 256, szFile);
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDC_ST_TITLE_SEARCH, szFile, L"Find notes with title equal to/contains:");
	SetDlgCtlText(hwnd, IDC_ST_RESULTS, szFile, L"Search results");
	SetDlgCtlText(hwnd, IDC_CMD_FIND, szFile, L"Find");
	SetDlgCtlText(hwnd, IDCANCEL, szFile, L"Cancel");
	ZeroMemory(&lvc, sizeof(lvc));
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	lvc.cchTextMax = 256;
	lvc.pszText = szBuffer;
	for(int i = 0; i < NELEMS(m_cols); i++){
		_itow(i, szKey, 10);
		lvc.pszText = szBuffer;
		SendDlgItemMessageW(hwnd, IDC_LST_RESULTS, LVM_GETCOLUMNW, i, (LPARAM)&lvc);
		GetPrivateProfileStringW(S_TITLES_COLS, szKey, m_cols[i], szBuffer, 256, szFile);
		SendDlgItemMessageW(hwnd, IDC_LST_RESULTS, LVM_SETCOLUMNW, i, (LPARAM)&lvc);
	}
}

static void FindTitles(HWND hList){
	PMEMNOTE		pNote = MemoryNotes();
	wchar_t			szBuffer[256];
	int				count;
	LVITEMW			lvi;

	m_count = 0;
	GetDlgItemTextW(g_hSearchTitlesDialog, IDC_EDI_TITLE_SEARCH, szBuffer, 255);

	while(pNote){
		if(_wcsistr(pNote->pData->szName, szBuffer)){
			count = ListView_GetItemCount(hList);
			ZeroMemory(&lvi, sizeof(lvi));
			lvi.iItem = count;
			lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
			lvi.lParam = (int)pNote;
			lvi.pszText = pNote->pData->szName;
			lvi.iImage = ItemImageIndex(pNote);
			SendMessageW(hList, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
			m_count++;
		}
		pNote = pNote->next;
	}
}
