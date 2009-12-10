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

#include "notes.h"

static BOOL Search_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Search_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Search_OnDestroy(HWND hwnd);
static void FindProc(HWND hList);
static void ReplaceProc(void);
static void PrepareResultsList(HWND hwnd);
static LRESULT CALLBACK List_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static void ApplySearchLanguage(HWND hwnd);

static FINDTEXTEXW		m_ft;
static int				m_flags, m_count;
static wchar_t			*m_cols[] = {L"Note", L"Line", L"Column"};
static HIMAGELIST		m_hImlSearch = NULL;
static BOOL				m_hidden;

static void ApplySearchLanguage(HWND hwnd){
	wchar_t			szBuffer[256], szFile[MAX_PATH], szKey[16];
	LVCOLUMNW		lvc;

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);

	_itow(IDM_SEARCH_IN_NOTES, szKey, 10);
	GetPrivateProfileStringW(S_MENU, szKey, L"Search In Notes", szBuffer, 256, szFile);
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDC_ST_SEARCH, szFile, L"Find what:");
	SetDlgCtlText(hwnd, IDC_ST_RESULTS, szFile, L"Search results");
	SetDlgCtlText(hwnd, IDC_CHK_MATCH_CASE, szFile, L"Match case");
	SetDlgCtlText(hwnd, IDC_CHK_WHOLE_WORD, szFile, L"Whole word");
	SetDlgCtlText(hwnd, IDC_CMD_FIND, szFile, L"Find");
	SetDlgCtlText(hwnd, IDC_SRCH_IN_HIDDEN, szFile, L"Include hidden notes");
	SetDlgCtlText(hwnd, IDC_SRCH_HIST_CLEAR, szFile, L"Clear search history");
	SetDlgCtlText(hwnd, IDCANCEL, szFile, L"Cancel");
	SetDlgCtlText(hwnd, IDC_ST_REPLACE, szFile, L"Replace with:");
	SetDlgCtlText(hwnd, IDC_CMD_REPLACE_ALL, szFile, L"Replace All");
	ZeroMemory(&lvc, sizeof(lvc));
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	lvc.cchTextMax = 256;
	lvc.pszText = szBuffer;
	for(int i = 0; i < NELEMS(m_cols); i++){
		_itow(i, szKey, 10);
		lvc.pszText = szBuffer;
		SendDlgItemMessageW(hwnd, IDC_LST_RESULTS, LVM_GETCOLUMNW, i, (LPARAM)&lvc);
		GetPrivateProfileStringW(S_RESULTS_COLS, szKey, m_cols[i], szBuffer, 256, szFile);
		SendDlgItemMessageW(hwnd, IDC_LST_RESULTS, LVM_SETCOLUMNW, i, (LPARAM)&lvc);
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
			lvc.cx = 180;
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

BOOL CALLBACK Search_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
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
			CHARRANGE		chr;
			wchar_t			szVal[16];

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
			lvi.cchTextMax = 16;
			lvi.pszText = szVal;
			lvi.iSubItem = 3;
			SendMessageW(hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
			chr.cpMin = _wtol(szVal);
			lvi.iSubItem = 4;
			SendMessageW(hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
			chr.cpMax = _wtol(szVal);
			SendMessageW((HWND)GetPropW(pNote->hwnd, PH_EDIT), EM_EXSETSEL, 0, (LPARAM)&chr);
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

static void Search_OnDestroy(HWND hwnd)
{
	if(m_hImlSearch)
		ImageList_Destroy(m_hImlSearch);
	g_hSearchDialog = NULL;
}

static void Search_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
	case IDC_CBO_SEARCH:
		if(codeNotify == CBN_EDITCHANGE){
			int len = SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, WM_GETTEXTLENGTH, 0, 0);
			if(len)
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND), TRUE);
			else
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND), FALSE);
		}
		else if(codeNotify == CBN_SELENDOK){
			if(SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_GETCURSEL, 0, 0) != CB_ERR)
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND), TRUE);
			else
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND), FALSE);
		}
		break;
	case IDC_CBO_REPLACE:
		if(codeNotify == CBN_EDITCHANGE){
			int len = SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, WM_GETTEXTLENGTH, 0, 0);
			if(len)
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), TRUE);
			else
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
		}
		else if(codeNotify == CBN_SELENDOK){
			if(SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_GETCURSEL, 0, 0) != CB_ERR)
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), TRUE);
			else
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
		}
		break;
	case IDC_SRCH_HIST_CLEAR:
		if(codeNotify == BN_CLICKED){
			WritePrivateProfileSectionW(S_SEARCH_HIST, NULL, g_NotePaths.INIFile);
			SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_RESETCONTENT, 0, 0);
			EnableWindow(GetDlgItem(hwnd, IDC_CMD_FIND), FALSE);
			WritePrivateProfileSectionW(S_REPLACE_HIST, NULL, g_NotePaths.INIFile);
			SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_RESETCONTENT, 0, 0);
			EnableWindow(GetDlgItem(hwnd, IDC_CMD_REPLACE_ALL), FALSE);
		}
		break;
	case IDOK:
		if(IsWindowEnabled(GetDlgItem(hwnd, IDC_CMD_FIND))){
			SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CMD_FIND, BN_CLICKED), (LPARAM)GetDlgItem(hwnd, IDC_CMD_FIND));
		}
		break;
	case IDCANCEL:
		EndDialog(hwnd, 0);
		break;
	case IDC_CMD_REPLACE_ALL:{
		wchar_t		szBuffer[256], szMessage[512], szTemp[12];
		int			res;

		m_count = 0;
		SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, WM_GETTEXT, 256, (LPARAM)szBuffer);
		if(SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_FINDSTRINGEXACT, -1, (LPARAM)szBuffer) == CB_ERR){
			SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_INSERTSTRING, 0, (LPARAM)szBuffer);
			SaveSearchHistory(hwnd);
		}
		SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, WM_GETTEXT, 256, (LPARAM)szBuffer);
		if(SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_FINDSTRINGEXACT, -1, (LPARAM)szBuffer) == CB_ERR){
			SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_INSERTSTRING, 0, (LPARAM)szBuffer);
			SaveReplaceHistory(hwnd);
		}
		//perform serach and replace
		res = IsDlgButtonChecked(hwnd, IDC_SRCH_IN_HIDDEN);
		if(res == BST_CHECKED)
			m_hidden = TRUE;
		else
			m_hidden = FALSE;
		ZeroMemory(&m_ft, sizeof(m_ft));
		m_flags = FR_DOWN;
		
		res = IsDlgButtonChecked(hwnd, IDC_CHK_MATCH_CASE);
		if(res == BST_CHECKED)
			m_flags |= FR_MATCHCASE;
		res = IsDlgButtonChecked(hwnd, IDC_CHK_WHOLE_WORD);
		if(res == BST_CHECKED)
			m_flags |= FR_WHOLEWORD;
		m_ft.lpstrText = g_SearchString;
		GetDlgItemTextW(hwnd, IDC_CBO_SEARCH, g_SearchString, 255);
		GetDlgItemTextW(hwnd, IDC_CBO_REPLACE, g_ReplaceString, 255);
		ReplaceProc();
		//finally delete all items from list
		ListView_DeleteAllItems(GetDlgItem(hwnd, IDC_LST_RESULTS));
		GetWindowTextW(hwnd, szBuffer, 255);
		wcscpy(szMessage, g_Strings.ReplaceComplete);
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
	case IDC_CMD_FIND:{
		wchar_t		szBuffer[256], szMessage[512], szTemp[12];
		int			res;

		m_count = 0;
		SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, WM_GETTEXT, 256, (LPARAM)szBuffer);
		if(SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_FINDSTRINGEXACT, -1, (LPARAM)szBuffer) == CB_ERR){
			SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_INSERTSTRING, 0, (LPARAM)szBuffer);
			SaveSearchHistory(hwnd);
		}
		//perform search
		ListView_DeleteAllItems(GetDlgItem(hwnd, IDC_LST_RESULTS));
		res = IsDlgButtonChecked(hwnd, IDC_SRCH_IN_HIDDEN);
		if(res == BST_CHECKED)
			m_hidden = TRUE;
		else
			m_hidden = FALSE;
		ZeroMemory(&m_ft, sizeof(m_ft));
		m_flags = FR_DOWN;
		
		res = IsDlgButtonChecked(hwnd, IDC_CHK_MATCH_CASE);
		if(res == BST_CHECKED)
			m_flags |= FR_MATCHCASE;
		res = IsDlgButtonChecked(hwnd, IDC_CHK_WHOLE_WORD);
		if(res == BST_CHECKED)
			m_flags |= FR_WHOLEWORD;
		m_ft.lpstrText = g_SearchString;
		GetDlgItemTextW(hwnd, IDC_CBO_SEARCH, g_SearchString, 255);
		FindProc(GetDlgItem(hwnd, IDC_LST_RESULTS));
		//EnumThreadWindows(GetCurrentThreadId(), FindProc, (LPARAM)GetDlgItem(hwnd, IDC_LST_RESULTS));
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

static BOOL Search_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	g_hSearchDialog = hwnd;
	CenterWindow(hwnd, FALSE);
	PrepareResultsList(hwnd);
	ApplySearchLanguage(hwnd);
	CheckDlgButton(hwnd, IDC_SRCH_IN_HIDDEN, BST_CHECKED);
	//limit combo box
	SendDlgItemMessageW(hwnd, IDC_CBO_SEARCH, CB_LIMITTEXT, 255, 0);
	SendDlgItemMessageW(hwnd, IDC_CBO_REPLACE, CB_LIMITTEXT, 255, 0);
	//load search history
	LoadSearchHistory(hwnd);
	//load replace history
	LoadReplaceHistory(hwnd);
	SetFocus(GetDlgItem(hwnd, IDC_CBO_SEARCH));
	return FALSE;
}

static void ReplaceProc(void){
	PMEMNOTE		pNote = MemoryNotes();
	HWND			hEdit;
	int				result;
	
	while(pNote){
		hEdit = NULL;
		if(!pNote->pData->visible && m_hidden){
			hEdit = GetUnvisibleNoteEdit(pNote->pFlags->id);
		}
		else{
			hEdit = (HWND)GetPropW(pNote->hwnd, PH_EDIT);
		}
		if(hEdit){
			result = 0;
			m_ft.chrg.cpMin = 0;
			m_ft.chrg.cpMax = -1;
			while(result != -1){
				result = SendMessageW(hEdit, EM_FINDTEXTEXW, m_flags, (LPARAM)&m_ft);
				if(result != -1){
					SendMessageW(hEdit, EM_EXSETSEL, 0, (LPARAM)&m_ft.chrgText);
					SendMessageW(hEdit, EM_REPLACESEL, TRUE, (LPARAM)g_ReplaceString);
					m_ft.chrg.cpMin = m_ft.chrgText.cpMax;
					m_ft.chrg.cpMax = -1;

					m_count++;
				}
			}
		}
		pNote = pNote->next;
	}
}

static void FindProc(HWND hList){
	wchar_t			szNumber[16];
	PMEMNOTE		pNote = MemoryNotes();
	HWND			hEdit;
	int				result, count, line, col;
	LVITEMW			lvi;

	while(pNote){
		hEdit = NULL;
		if(!pNote->pData->visible && m_hidden){
			hEdit = GetUnvisibleNoteEdit(pNote->pFlags->id);
		}
		else{
			hEdit = (HWND)GetPropW(pNote->hwnd, PH_EDIT);
		}
		if(hEdit){
			result = 0;
			m_ft.chrg.cpMin = 0;
			m_ft.chrg.cpMax = -1;
			while(result != -1){
				result = SendMessageW(hEdit, EM_FINDTEXTEXW, m_flags, (LPARAM)&m_ft);
				if(result != -1){
					count = ListView_GetItemCount(hList);
					ZeroMemory(&lvi, sizeof(lvi));
					lvi.iItem = count;
					lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
					lvi.lParam = (int)pNote;
					lvi.pszText = pNote->pData->szName;
					lvi.iImage = ItemImageIndex(pNote);
					//if(pNote->pData->visible)
						//lvi.iImage = 0;
					//else
						//lvi.iImage = 8;
					SendMessageW(hList, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
					line = SendMessageW(hEdit, EM_EXLINEFROMCHAR, 0, m_ft.chrgText.cpMin);
					lvi.mask = LVIF_TEXT;
					lvi.iSubItem = 1;
					lvi.pszText = szNumber;
					_itow(++line, szNumber, 10);
					SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
					col = m_ft.chrgText.cpMin - SendMessageW(hEdit, EM_LINEINDEX, --line, 0);
					lvi.iSubItem = 2;
					lvi.pszText = szNumber;
					_itow(++col, szNumber, 10);
					SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
					lvi.iSubItem = 3;
					lvi.pszText = szNumber;
					_ltow(m_ft.chrgText.cpMin, szNumber, 10);
					SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
					lvi.iSubItem = 4;
					lvi.pszText = szNumber;
					_ltow(m_ft.chrgText.cpMax, szNumber, 10);
					SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&lvi);

					m_ft.chrg.cpMin = m_ft.chrgText.cpMax;
					m_ft.chrg.cpMax = -1;

					m_count++;
				}
			}
			if(!pNote->pData->visible){
				SendMessageW(hEdit, WM_DESTROY, 0, 0);
			}
		}
		pNote = pNote->next;
	}
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

