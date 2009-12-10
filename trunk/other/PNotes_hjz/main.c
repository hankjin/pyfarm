
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

#include <stdio.h>
#include "notes.h"
#include "hotkeys.h"
#include "registryclean.h"
#include "linklabel.h"
#include "update.h"
#include "skinlessdrawing.h"
#include "print.h"

/** Constants *********************************************************/
#define	MAIN_HOTKEYS_IND	100
#define	NOTE_HOTKEYS_IND	200
#define	NEXT_HOTKEYS_IND	300
/** Structures ********************************************************/
typedef struct {
	HWND	hAppearance;
	HWND	hSkins;
	HWND	hSchedule;
	HWND 	hDocks;
	HWND	hBehavior;
	HWND	hMisc;
}DLG_WNDS;

typedef struct _CUST_FONT	*P_CUST_FONT;
typedef struct _CUST_FONT {
	wchar_t		*lpName;
	P_CUST_FONT	next;
}CUST_FONT;

/** Prototypes ********************************************************/
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static void Main_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void Main_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem);
static void Main_OnClose(HWND hwnd);
static void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL Main_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
static void Main_OnHotKey(HWND hwnd, int idHotKey, UINT fuModifiers, UINT vk);
static void Main_OnNCDestroy(HWND hwnd);
static void Main_OnEndSession(HWND hwnd, BOOL fEnding);
static void Main_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu);
static void CreateDataFile(void);
static BOOL CreateAppearance(HWND hwnd);
static void GetAllNotes(HWND hwnd);
static void CreateDefaultFont(PLOGFONTW plfFont, BOOL fBold);
static void CreateMenuFont(void);
static void Main_OnDestroy(HWND hwnd);
static LRESULT CALLBACK OptionsDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static void Options_OnClose(HWND hwnd);
static void Options_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL Options_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static BOOL CALLBACK Hot_DlgProc (HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL Hot_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Hot_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Hot_OnClose(HWND hwnd);
static BOOL CALLBACK EditHotProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void ApplyHotDialogLanguage(HWND hwnd, wchar_t * lpFile);
static BOOL ApplyChanges(HWND hwnd, wchar_t * lpFile);
static void ShowAllNotes(void);
static void AddNotifyIcon(void);
static void ApplyMainDlgLanguage(HWND hwnd, wchar_t * lpFile);
static void CleanUp(void);
static void CreatePopUp(wchar_t * lpFile);
static void AddPNMenuItem(int size, int id, wchar_t * lpDef, wchar_t * lpFile, int * pIndex, BOOL addToCommands, HMENU hMenu);
static void AddPNMenuItemForDockAll(int size, int id, wchar_t * lpDef, wchar_t * lpFile, int * pIndex, int pos);
static void PrepareDateFormatsMessages(void);
static BOOL FileExistsByFullPath(wchar_t * lpFile);
static LRESULT CALLBACK Font_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void RestoreDefaultSettings(void);
static void EnableSkinlessProperties(BOOL fEnabled);
static HRESULT CreatePNotesShortcut(void);
static void LoadMenuLanguages(HMENU	hMenu);
static void CreateShowHideGroupsMenus(HMENU hMenu);
static void LangPathByID(int id, wchar_t * lpPath);
static void GetFileNameFromPath(wchar_t * lpPath, wchar_t * lpName);
static void AddOptionsTabs(HWND hwnd);
static void SetMainDlgSizes(HWND hwnd);
static void SetShowDateTime(HWND hwnd);
static BOOL ShowOpenFileDlg(HWND hwnd, wchar_t *pstrFileName, wchar_t *pstrTitleName, wchar_t *pstrFilter, wchar_t *pstrTitle);
static void PrepareMessages(wchar_t * lpFile);
static void RegisterHKeys(HWND hwnd, P_HK_TYPE lpKeys, int count);
static void UnregisterHKeys(HWND hwnd, P_HK_TYPE lpKeys, int count);
static void PrepareHotKeys(void);
static void PrepareNoteHotKeys(void);
static void SaveHotKeys(const wchar_t * lpSection, P_HK_TYPE lpKeys, int count);
static void AddCommand(int index, wchar_t * lpCommand);
static void DrawListItem(const DRAWITEMSTRUCT * lpd);
static int GetHKId(P_HK_TYPE lpKeys, int count, int identifier);
static void HKeysAlreadyRegistered(wchar_t * lpKey);
static BOOL CheckHotKeysChanges(P_HK_TYPE lpKeys, P_HK_TYPE lpTempKeys, int count);
static void LoadSounds(HWND hwnd);
static void PreviewSound(wchar_t * lpSound);
static BOOL CopySoundFile(wchar_t * lpSrcFull, wchar_t * lpSrcFile);
static BOOL DeleteSoundFile(wchar_t * lpFile);
static int ParsePNCommandLine(wchar_t * pINIPath, wchar_t * pDataPath, wchar_t * pProgPath);
static BOOL CALLBACK EnumPNotes(HWND hwnd, LPARAM lParam);
static void ReloadNotes(void);
static void ClearOnOptionsClose(void);
static BOOL IsLastBackslash(wchar_t * src);
static void CALLBACK AutosaveTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static void LoadAutosaved(void);
static void DeleteAutosaved(void);
static void GetPrevious(wchar_t * lpFile);
static void ReplaceCharacters(wchar_t * dst, wchar_t * src);
static BOOL CALLBACK Skins_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Skins_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void Skins_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL Skins_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void InitDlgSkins(HWND hwnd);
static BOOL CALLBACK Appearance_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Appearance_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Appearance_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
static HBRUSH Appearance_OnCtlColorStatic(HWND hwnd, HDC hdc, HWND hwndChild, int type);
static void InitDlgAppearance(HWND hwnd);
static BOOL CALLBACK Schedule_DlgProc (HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static void Schedule_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Schedule_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void InitDlgSchedule(HWND hwnd);
static BOOL CALLBACK Misc_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Misc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void InitDlgMisc(HWND hwnd);
static BOOL CALLBACK Docks_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Docks_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void Docks_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Docks_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
static void InitDlgDocks(HWND hwnd);
static void FillComboSkins(HWND hwnd, int id, const wchar_t * lpSkinCurrent);
// static void SetDockDistance(HWND hwnd, int val);
static BOOL CALLBACK Behavior_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void InitDlgBehavior(HWND hwnd);
static void Behavior_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void Behavior_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static HWND CreateNewNote(void);
static void MakeDefaultGroup(void);
static void RefreshDefaultGroupName(void);
static void PrepareTabListView(HWND hwnd);
static void PrepareLeftImageList(void);
static void ShowHideOptionsDialogs(int index);
static void MoveOptionsDialogs(int x, int y, int cx, int cy);
static void SetTransValue(HWND hwnd, int value);
static void GetVersionNumber(void);
static void StartUpdateProcess(void);
static void ShowNewVersionBaloon(wchar_t * szNewVersion);
static BOOL IsNewDLLVersion(char * szLib);
static void MoveButtonOnOptions(HWND hwnd, int id, int offset);
static int WinVer(void);
static void ChangeDockingDefSkin(void);
static void ChangeDockingDefSize(int id, int val);
static void InitDockHeaders(void);
static void FreeDockHeaders(void);
static void RearrangeDockWindowsOnstart(dock_type dockType);
static BOOL CALLBACK DockAllProc(HWND hwnd, LPARAM lParam);
static BOOL CALLBACK NoteHotkeysProc(HWND hwnd, LPARAM lParam);
static void GetDockSkinProperties(HWND hwnd, P_NOTE_RTHANDLES pH, wchar_t * lpSkin, BOOL fDeletePrev);
static LRESULT CALLBACK HiddenProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static void UnregisterLogonMessage(HWND hwnd);
static void RegisterLogonMessage(HWND hwnd);
static void BuildFavoritesMenu(void);
static void FreeFavoritesMenu(void);
static void ShowFavorite(int id);
static void RegisterCustomFonts(void);
static void ClearCustomFonts(void);
static BOOL IsAnyAutosavedExists(void);
static void FindFavorite(wchar_t * id);
static void LoadWeekdays(wchar_t * lpLang);
static void SetWeekday(int day, int index, const wchar_t * lpFile);
static void FreeWeekdays(void);
static HWND NoteFromClipboard(void);
static void AddDblCommands(wchar_t * lpFile);
static int GetIndexOfDblClick(void);
static void SetDefaultItemForDblClick(void);
static BOOL CheckDblClickChanges(void);
static void PrepareNoteCommands(wchar_t * lpFile);
static void CheckOverdueReminders(void);
static void PrepareRepParts(wchar_t * lpFile);
static void FreeRepParts(void);
static int DaysBetweenAlerts(const SYSTEMTIME stLast, const SYSTEMTIME stNow, const SYSTEMTIME stDate);
static BOOL RestrictedHotKey(wchar_t * szKey);
// static void CleanPrefetch(const wchar_t * lpcProgName);

/** Module variables ********************************************************/
static HWND				m_hDlg = 0, m_hHotDlg = 0;
static NOTIFYICONDATAW	m_nData;
static HMENU			m_hMenu = 0, m_hPopUp;
static BOOL				m_InTray = FALSE;
static wchar_t			m_sMainINI[MAX_PATH];
static wchar_t			m_sTempSkinFile[256];
static wchar_t			m_sNoWindowMessage[256];
static wchar_t			m_sInvalidDate[256], m_sInvalidTime[256], m_sUnsuccessfull[512];
static wchar_t			m_sHK1[128], m_sHK2[128], m_sHK3[128];
static wchar_t			m_sDefSize[256], m_sTempdefBrowser[MAX_PATH];
static wchar_t			m_sSound1[128], m_sSound2[128], m_sSound3[128], m_sSound4[128], m_sSound5[128];
static RECT				m_rWindow, m_rCaption;
static NOTE_RTHANDLES	m_TempRHandles, m_TempDRTHandles;
static NOTE_APPEARANCE	m_TempAppearance;
static NOTE_SETTINGS	m_TempNoteSettings;
static NT_DT_FORMATS	m_TempFormats;
static PNDOCK			m_TempDockingSettings;
static HANDLE			m_Mutex;
static SOUND_TYPE		m_TempSound, m_PrevSound;
static HK_TYPE			m_HKCurrent;
static int				m_Args;				//command line arguments
static int				m_Timer = 0;		//autosave timer id
static PMItem			m_PMenus = NULL;
static HIMAGELIST		m_hImlLeft = NULL;
static P_CUST_FONT		m_pFonts = NULL;
static MItem			m_MainMenus[] = {{IDM_NEW, 0, 0, -1, -1, MT_REGULARITEM, FALSE, L"New Note", L""}, 
								{IDM_LOAD_NOTE, 71, 0, -1, -1, MT_REGULARITEM, FALSE, L"Load Note", L""}, 
								{IDM_NOTE_FROM_CLIPBOARD, 76, 0, -1, -1, MT_REGULARITEM, FALSE, L"New Note From Clipboard", L""}, 
								{IDM_OPTIONS, 1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Preferences", L""}, 
								{IDM_LANGUAGES, 23, 0, -1, -1, MT_REGULARITEM, FALSE, L"Language", L""},
								{IDM_SHOW_GROUPS, 3, 0, -1, -1, MT_REGULARITEM, FALSE, L"Show", L""}, 
								{IDM_HIDE_GROUPS, 2, 0, -1, -1, MT_REGULARITEM, FALSE, L"Hide", L""},
								{IDM_SHOW_ALL, -1, -1, -1, -1, MT_REGULARITEM, FALSE, L"Show All", L""},
								{IDM_HIDE_ALL, -1, -1, -1, -1, MT_REGULARITEM, FALSE, L"Hide All", L""},
								{IDM_ALL_TO_FRONT, 77, -1, -1, -1, MT_REGULARITEM, FALSE, L"Bring All To Front", L""},
								{IDM_SAVE_ALL, 27, 0, -1, -1, MT_REGULARITEM, FALSE, L"Save All", L""},
								{IDM_DOCK_ALL, 48, 0, -1, -1, MT_REGULARITEM, FALSE, L"Dock All", L""},
								{IDM_DOCK_NONE_ALL, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"None", L""},
								{IDM_DOCK_LEFT_ALL, 49, 0, -1, -1, MT_REGULARITEM, FALSE, L"Left", L""},
								{IDM_DOCK_TOP_ALL, 50, 0, -1, -1, MT_REGULARITEM, FALSE, L"Top", L""},
								{IDM_DOCK_RIGHT_ALL, 51, 0, -1, -1, MT_REGULARITEM, FALSE, L"Right", L""},
								{IDM_DOCK_BOTTOM_ALL, 52, 0, -1, -1, MT_REGULARITEM, FALSE, L"Bottom", L""},
								{IDM_SEARCH_SUBMENU, 43, 0, -1, -1, MT_REGULARITEM, FALSE, L"Search", L""},
								{IDM_SEARCH_IN_NOTES, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Search In Notes", L""},
								{IDM_SEARCH_TITLE, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Search By Title", L""},
								{IDM_CTRL, 11, 0, -1, -1, MT_REGULARITEM, FALSE, L"Control Panel", L""},
								{IDM_FAVORITES, 67, 0, -1, -1, MT_REGULARITEM, FALSE, L"Favorites", L""},
								{IDM_HELP, 5, 0, -1, -1, MT_REGULARITEM, FALSE, L"Help", L""}, 
								{IDM_ABOUT, 4, 0, -1, -1, MT_REGULARITEM, FALSE, L"About", L""}, 
								{IDM_ON_THE_WEB, 26, -1, -1, -1, MT_REGULARITEM, FALSE, PROG_PAGE_MENU_ITEM, L""},
								{IDM_EXIT, -1, -1, -1, -1, MT_REGULARITEM, FALSE, L"Exit", L""}};
static wchar_t			* m_PanelDefs[] = {L"Appearance", L"Skins", L"Schedule", L"Docking", L"Behavior", L"Misc"};
static HK_TYPE			m_Hotkeys[NELEMS(m_MainMenus)], m_TempHotkeys[NELEMS(m_MainMenus)];
static PNCOMMAND 		m_Commands[NELEMS(m_MainMenus)], m_DblCommands[4];
static P_PNCOMMAND		m_NoteCommands = NULL;
static P_HK_TYPE		m_NoteHotkeys = NULL, m_TempNoteHotkeys = NULL;
static DLG_WNDS			m_Dialogs;
static char				m_Version[12];
static BOOL				m_CheckingFromButton = FALSE, m_TrackBaloonClick, m_NewDLLVersion, m_TrackLogon;
static int				m_WinVer;

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    INITCOMMONCONTROLSEX 	icc;
    WNDCLASSEXW 			wcx;
	MSG						msg;
	// HACCEL					hAccel;
	int						args;
	wchar_t					szINIPath[MAX_PATH], szDataPath[MAX_PATH], szProgPath[MAX_PATH];

	// SetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK, L"6");
	args = ParsePNCommandLine(szINIPath, szDataPath, szProgPath);

	m_Mutex = CreateMutexW(NULL, TRUE, NOTES_MUTEX);
	if(GetLastError() == ERROR_ALREADY_EXISTS){
		if((args & ARG_EXIT) == ARG_EXIT)
			EnumWindows(EnumPNotes, args); 
		return 0;
	}

	//check whether main INI path has been sent as command line argument
	g_NotePaths.DataDir[0] = '\0';
	m_sMainINI[0] = '\0';
	if((args & ARG_INI_PATH) == ARG_INI_PATH){
		wcscpy(m_sMainINI, szINIPath);
		if(!_wcsistr(m_sMainINI, L"Notes.ini")){
			if(IsLastBackslash(m_sMainINI))
				wcscat(m_sMainINI, L"Notes.ini");
			else
				wcscat(m_sMainINI, L"\\Notes.ini");
		}
	}
	if((args & ARG_DATA_PATH) == ARG_DATA_PATH){
		if(!IsLastBackslash(szDataPath))
			wcscat(szDataPath, L"\\");
		wcscpy(g_NotePaths.DataDir, szDataPath);
	}
	if((args & ARG_PROG_PATH) == ARG_PROG_PATH){
		wcscpy(g_NotePaths.ProgFullPath, szProgPath);
	}
	else{
		GetModuleFileNameW(NULL, g_NotePaths.ProgFullPath, MAX_PATH);
	}

	g_hInstance = hInstance;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES | ICC_DATE_CLASSES | ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icc);

	LoadLibraryW(__T("riched20.dll"));  // Rich Edit v2.0, v3.0

	m_NewDLLVersion = IsNewDLLVersion("shell32.dll");
	m_WinVer = WinVer();

	ZeroMemory(&wcx, sizeof(wcx));
    //register main window class
    wcx.cbSize = sizeof(wcx);
    wcx.hInstance = hInstance;
	wcx.lpfnWndProc = WindowProc;
	wcx.hIcon = LoadIconW(g_hInstance, MAKEINTRESOURCEW(IDR_ICO_MAIN));
	wcx.hIconSm = LoadIconW(g_hInstance, MAKEINTRESOURCEW(IDR_ICO_MAIN));
    wcx.lpszClassName = NOTES_PROG_MAIN_CLASS;
    if (!RegisterClassExW(&wcx))
        return 1;

	//load control panel cursors
	g_CurV = LoadCursorW(g_hInstance, MAKEINTRESOURCEW(IDR_CUR_V_SPLIT));
	g_CurH = LoadCursorW(g_hInstance, MAKEINTRESOURCEW(IDR_CUR_H_SPLIT));

	//create invisible main window
	g_hMain = CreateWindowExW(0, NOTES_PROG_MAIN_CLASS, NULL, 0, 0, 0, 0, 0, NULL, NULL, g_hInstance, NULL);
	if(g_hMain){
		//get version for updates checking
		GetVersionNumber();
		//add notification icon
		AddNotifyIcon();
		g_SearchProc = Search_DlgProc;
		g_SearchTitlesProc = SearchTitles_DlgProc;
		if(g_NoteSettings.checkOnStart)
			StartUpdateProcess();
		//check overdue reminders
		if(IsBitOn(g_NoteSettings.reserved1, SB1_TRACK_OVERDUE)){
			CheckOverdueReminders();
		}
		// CreateMenuColors(m_WinVer);
	}
	// hAccel = LoadAcceleratorsW(g_hInstance, MAKEINTRESOURCEW(IDA_TABLE));

	//start message loop
	while(GetMessageW(&msg, NULL, 0, 0)){
		// if(!TranslateAcceleratorW(g_hMain, hAccel, &msg)){
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		// }
	}

	//exit program
	return msg.wParam;
}

static void ReloadNotes(void){
	if(g_NoteSettings.saveOnExit & (m_Args & ARG_NO_SAVE) != ARG_NO_SAVE){
		ApplySaveOnUnload(m_Args, TRUE);
	}
	//close all notes in order to clean up memory
	ApplyCloseAllNotes();
	//free previous docking headers
	FreeDockHeaders();
	//free memory notes list
	FreeMemNotes();
	//prepare docking headers
	InitDockHeaders();
	GetAllNotes(g_hMain);
	if(g_Empties.count){
		LPPOINT	lpp = g_Empties.pPoint;
		LPINT	lpi = g_Empties.pDockData;
		for(int i = 0; i < g_Empties.count; i++){
			HWND hNote = CreateNewNote();
			if(hNote){
				SetWindowPos(hNote, HWND_TOP, lpp->x, lpp->y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOZORDER);
				lpp++;
				if(DockType(*lpi) != DOCK_NONE){
					P_NOTE_DATA		pD = NoteData(hNote);
					pD->dockData = *lpi;
					AddDockItem(DHeader(DockType(*lpi)), hNote, DockIndex(*lpi));
				}
				lpi++;
			}
		}
		free(g_Empties.pPoint);
		g_Empties.pPoint = 0;
		free(g_Empties.pDockData);
		g_Empties.pDockData = 0;
		g_Empties.count = 0;
	}
	//rearrange all dock notes
	RearrangeDockWindowsOnstart(DOCK_LEFT);
	RearrangeDockWindowsOnstart(DOCK_TOP);
	RearrangeDockWindowsOnstart(DOCK_RIGHT);
	RearrangeDockWindowsOnstart(DOCK_BOTTOM);
	if(g_hCPDialog)
		SendMessageW(g_hCPDialog, CHM_RELOAD, 0, 0);
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){

	switch(uMsg){
		HANDLE_MSG (hwnd, WM_DESTROY, Main_OnDestroy);
		HANDLE_MSG (hwnd, WM_CLOSE, Main_OnClose);
		HANDLE_MSG (hwnd, WM_COMMAND, Main_OnCommand);
		HANDLE_MSG (hwnd, WM_CREATE, Main_OnCreate);
		HANDLE_MSG (hwnd, WM_DRAWITEM, Main_OnDrawItem);
		HANDLE_MSG (hwnd, WM_MEASUREITEM, Main_OnMeasureItem);
		HANDLE_MSG (hwnd, WM_HOTKEY, Main_OnHotKey);
		HANDLE_MSG (hwnd, WM_NCDESTROY, Main_OnNCDestroy);
		HANDLE_MSG (hwnd, WM_ENDSESSION, Main_OnEndSession);
		HANDLE_MSG (hwnd, WM_INITMENUPOPUP, Main_OnInitMenuPopup);
		case WM_WTSSESSION_CHANGE:
			if(m_TrackLogon){
				if(wParam == WTS_SESSION_UNLOCK || wParam == WTS_REMOTE_DISCONNECT){
					ApplySessionLogOn();
				}
			}
			break;
		case UPDM_INETERROR:
			if(m_CheckingFromButton){
				MessageBox(hwnd, (char *)wParam, 0, 0);
			}
			break;
		case UPDM_UPDATEFOUND:{
			wchar_t		temp[12];
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char *)wParam, 12, temp, 12);
			ShowNewVersionBaloon(temp);
			break;
		}
		case UPDM_SAMEVERSION:
			if(m_CheckingFromButton)
				MessageBoxW(m_Dialogs.hMisc, g_Strings.SameVersion, g_Strings.CheckUpdate, MB_OK | MB_ICONINFORMATION);
			break;
		case UPDM_GETVERSION:
			strcpy((char *)wParam, m_Version);
			break;
		case UPDM_GETSTRINGS:
			strcpy((char *)wParam, UPDATE_CHECK_URL);
			strcpy((char *)lParam, "/version.txt");
			break;
		// case WM_SYSCOLORCHANGE:
			// CreateMenuColors(m_WinVer);
			// break;
		case WM_SHELLNOTIFY:
			if(wParam == IDI_TRAY){
				if(lParam == WM_LBUTTONDBLCLK){
					//choose default action
					switch(GetIndexOfDblClick()){
						case 0:
							SendMessageW(hwnd, WM_COMMAND, IDM_NEW, 0);
							break;
						case 1:
							SendMessageW(hwnd, WM_COMMAND, IDM_CTRL, 0);
							break;
						case 2:
							SendMessageW(hwnd, WM_COMMAND, IDM_OPTIONS, 0);
							break;
						case 3:
							SendMessageW(hwnd, WM_COMMAND, IDM_SEARCH_IN_NOTES, 0);
							break;
						default:
							SendMessageW(hwnd, WM_COMMAND, IDM_NEW, 0);
							break;
					}
				}
				else if (lParam == WM_RBUTTONDOWN){
					//show popup menu
					BuildFavoritesMenu();
					//build sho/hide menus
					CreateShowHideGroupsMenus(m_hPopUp);
					ShowPopUp(hwnd, m_hPopUp);
				}
			}
			break;
		case CHM_MAIN_CLOSE:
			m_Args = lParam;
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
		case CHM_GET_RT_HANDLES:
			return (LRESULT)&g_RTHandles;
			break;
		case CHM_ON_TOP_CHANGED:
			ApplyOnTopChanged();
			break;
		default:
			return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

static void Main_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu)
{
	PrepareMenuGradientColors(GetSysColor(COLOR_BTNFACE));
	if(IsTextInClipboard())
		EnableMenuItem(m_hPopUp, IDM_NOTE_FROM_CLIPBOARD, MF_BYCOMMAND | MF_ENABLED);
	else
		EnableMenuItem(m_hPopUp, IDM_NOTE_FROM_CLIPBOARD, MF_BYCOMMAND | MF_GRAYED);
}

static void Main_OnEndSession(HWND hwnd, BOOL fEnding)
{
	//save all when Windows session ends
	CleanUp();
	if(g_NoteSettings.saveOnExit){
		if((m_Args & ARG_SILENT) != ARG_SILENT){
			m_Args |= ARG_SILENT;
		}
		ApplySaveOnUnload(m_Args, FALSE);;
	}
}

static void Main_OnNCDestroy(HWND hwnd)
{
	//unregister hot keys
	UnregisterHKeys(hwnd, m_Hotkeys, NELEMS(m_Hotkeys));
	//save hot keys
	SaveHotKeys(S_HOTKEYS, m_Hotkeys, NELEMS(m_Hotkeys));
}

static void Main_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	DrawMItem(lpDrawItem, g_hBmpMenuNormal, g_hBmpMenuGray, CLR_MASK);
}

static void Main_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem)
{
	MeasureMItem(g_hMenuFont, lpMeasureItem);
}

static void Main_OnClose(HWND hwnd)
{
	if(g_NoteSettings.saveOnExit & (m_Args & ARG_NO_SAVE) != ARG_NO_SAVE){
		ApplySaveOnUnload(m_Args, FALSE);
	}
	//close all notes in order to clean up memory
	ApplyCloseAllNotes();
	if(g_hCPDialog)
		SendMessageW(g_hCPDialog, WM_CLOSE, 0, 0);
	CleanUp();
	DestroyWindow(hwnd);
}

static void Main_OnHotKey(HWND hwnd, int idHotKey, UINT fuModifiers, UINT vk)
{
	int 	id;

	if(idHotKey >= MAIN_HOTKEYS_IND && idHotKey < NOTE_HOTKEYS_IND){
		id = GetHKId(m_Hotkeys, NELEMS(m_Hotkeys), idHotKey);
		if(id){
			if(m_hHotDlg != 0)
				//user press the hot key on the hot key creation dialog - just warn
				HKeysAlreadyRegistered(GetHotKeyByID(id, m_Hotkeys, NELEMS(m_Hotkeys))->szKey);
			else
				//process hot key
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(id, 0), 0);
		}
	}
	else if(idHotKey >= NOTE_HOTKEYS_IND && idHotKey < NEXT_HOTKEYS_IND){
		id = GetHKId(m_NoteHotkeys, NoteMenuCount(), idHotKey);
		if(id){
			if(m_hHotDlg != 0)
				//user press the hot key on the hot key creation dialog - just warn
				HKeysAlreadyRegistered(GetHotKeyByID(id, m_NoteHotkeys, NoteMenuCount())->szKey);
			else
				//process hot key
				EnumThreadWindows(GetCurrentThreadId(), NoteHotkeysProc, id);
		}
	}
}

static void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	wchar_t				szDir[256];
	HMENU				hLang = NULL;
	int 				pos;
	MENUITEMINFOW		mi;
	wchar_t				szTemp[MAX_PATH], szBuffer[256], szLang[256];
	wchar_t				szFileName[MAX_PATH], szFileTitle[MAX_PATH];
	HWND 				hNote;
	switch (id){
		case IDM_NEW:{
			hNote = CreateNewNote();
			PMEMNOTE	pNote = MEMNOTE(hNote);
			if(hNote && codeNotify != 0){
				pNote->pData->idGroup = codeNotify;
			}
			if(g_hCPDialog){
				SendMessageW(g_hCPDialog, CHM_CTRL_INS, 0, (LPARAM)pNote);
				SendMessageW(g_hCPDialog, CHM_CTRL_GROUP_UPDATE, pNote->pData->idGroup, pNote->pData->idGroup);
			}
			break;
		}
		case IDM_NOTE_FROM_CLIPBOARD:
			if(IsTextInClipboard()){
				hNote = NoteFromClipboard();
				if(hNote){
					PMEMNOTE	pNote = MEMNOTE(hNote);
					if(codeNotify != 0){
						pNote->pData->idGroup = codeNotify;
					}
					if(g_hCPDialog){
						SendMessageW(g_hCPDialog, CHM_CTRL_INS, 0, (LPARAM)pNote);
						SendMessageW(g_hCPDialog, CHM_CTRL_GROUP_UPDATE, pNote->pData->idGroup, pNote->pData->idGroup);
					}
				}
			}
			break;
		case IDM_LOAD_NOTE:{
			wcscpy(szLang, g_NotePaths.LangDir);
			wcscat(szLang, g_NotePaths.LangFile);
			GetPrivateProfileStringW(L"captions", L"load", L"Load Note", szBuffer, 256, szLang);
			if(ShowOpenFileDlg(hwnd, szFileName, szFileTitle, LOAD_NOTE_FILTER, szBuffer)){
				PathRemoveExtensionW(szFileTitle);
				GetPrivateProfileStringW(szFileTitle, S_DATA, NULL, szTemp, 256, g_NotePaths.DataFile);
				if(wcslen(szTemp) > 0){
					GetPrivateProfileStringW(L"messages", L"already_exists", L"The note with the same id already exists. Please, choose another file.", szFileName, MAX_PATH, szLang);
					GetPrivateProfileStringW(L"captions", L"loading_error", L"Loading Error", szBuffer, 256, szLang);
					MessageBoxW(hwnd, szFileName, szBuffer, MB_OK | MB_ICONEXCLAMATION);
					break;
				}
				PMEMNOTE	pNote = AddMemNote();
				LoadNoteProperties(pNote, &g_RTHandles, szFileTitle, TRUE, szFileName);
				hNote = CreateNote(pNote, g_hInstance, hwnd, TRUE, szFileName);
				if(g_hCPDialog){
					SendMessageW(g_hCPDialog, CHM_CTRL_INS, 0, (LPARAM)pNote);
					SendMessageW(g_hCPDialog, CHM_CTRL_GROUP_UPDATE, pNote->pData->idGroup, pNote->pData->idGroup);
				}
			}
			break;
		}
		case IDM_ALL_TO_FRONT:
			BringNotesToFront();
			break;
    	case IDM_EXIT:		//exit application
			g_IsClosing = TRUE;
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
		case IDM_SEARCH_IN_NOTES:
			if(!g_hSearchDialog)
				DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_FIND), NULL, (DLGPROC)Search_DlgProc, 0);
			else
				BringWindowToTop(g_hSearchDialog);
			break;
		case IDM_SEARCH_TITLE:
			if(!g_hSearchTitlesDialog)
				DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_TITLES_SEARCH), NULL, (DLGPROC)SearchTitles_DlgProc, 0);
			else
				BringWindowToTop(g_hSearchTitlesDialog);
			break;
		case IDM_OPTIONS:	//restore or show options dialog
			if(!m_hDlg){
				DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_MAIN), NULL, (DLGPROC)OptionsDlgProc, 0);
			}
			else{
				BringWindowToTop(m_hDlg);
			}
			break;
		case IDM_CTRL:
			if(!g_hCPDialog)
				ShowControlPanel();
			else
				BringWindowToTop(g_hCPDialog);
			break;
		case IDM_SHOW_ALL:	//show all notes
			ShowAllNotes();
			break;
		case IDM_HIDE_ALL:	//hide all notes
			ApplyHideAllNotes();
			break;
		case IDM_ABOUT:		//shohw about dialog
			CreateAboutDialog(hwnd, g_hInstance, m_nData.hIcon);
			break;
		case IDM_HELP:		//show help
			GetSubPathW(szDir, HELP_SUBPATH);
			ShellExecuteW(hwnd, L"open", szDir, NULL, NULL, SW_SHOWNORMAL);
			break;
		case IDM_ON_THE_WEB:
			if(wcslen(g_DefBrowser) == 0)
				ShellExecuteW(hwnd, L"open", HOME_PAGE, NULL, NULL, SW_SHOWDEFAULT);
			else
				if((int)ShellExecuteW(hwnd, NULL, g_DefBrowser, HOME_PAGE, NULL, SW_SHOWDEFAULT) <= 32){
					ShellExecuteW(hwnd, L"open", HOME_PAGE, NULL, NULL, SW_SHOWDEFAULT);
				}
			break;
		// case IDM_SAVE:
		// case IDM_SEARCH_NOTE:
		// case IDM_REPLACE_NOTE:
			// EnumThreadWindows(GetCurrentThreadId(), EnumActiveNotesProc, MAKELPARAM(id, 0));
			// break;
		case IDM_SAVE_ALL:
			ApplySaveAllNotes();
			break;
		case IDM_DOCK_LEFT_ALL:
			EnumThreadWindows(GetCurrentThreadId(), DockAllProc, MAKELPARAM(IDM_DOCK_LEFT, DOCK_LEFT));
			break;
		case IDM_DOCK_TOP_ALL:
			EnumThreadWindows(GetCurrentThreadId(), DockAllProc, MAKELPARAM(IDM_DOCK_TOP, DOCK_TOP));
			break;
		case IDM_DOCK_RIGHT_ALL:
			EnumThreadWindows(GetCurrentThreadId(), DockAllProc, MAKELPARAM(IDM_DOCK_RIGHT, DOCK_RIGHT));
			break;
		case IDM_DOCK_BOTTOM_ALL:
			EnumThreadWindows(GetCurrentThreadId(), DockAllProc, MAKELPARAM(IDM_DOCK_BOTTOM, DOCK_BOTTOM));
			break;
		case IDM_DOCK_NONE_ALL:
			EnumThreadWindows(GetCurrentThreadId(), DockAllProc, MAKELPARAM(IDM_DOCK_NONE, DOCK_NONE));
			break;
		default:
			if(hwndCtl == 0 && codeNotify == 0){
				if(id >= LANG_ADDITION){	//sent from the languages menu
					hLang = GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_LANGUAGES));
					if((GetMenuState(hLang, id, MF_BYCOMMAND) & MF_CHECKED) != MF_CHECKED){
						PMItem		pmi;

						for(pos = 0; pos < GetMenuItemCount(hLang); pos++){
							if(GetMenuItemID(hLang, pos) == id)
								break;
						}
						ZeroMemory(&mi, sizeof(mi));
						mi.cbSize = sizeof(mi);
						mi.fMask = MIIM_DATA;
						GetMenuItemInfoW(hLang, pos, TRUE, &mi);
						pmi = (PMItem)mi.dwItemData;
						wcscpy(szTemp, pmi->szReserved);
						GetFileNameFromPath(szTemp, szLang);
						CheckMenuRadioItem(hLang, 0, GetMenuItemCount(hLang) - 1, pos, MF_BYPOSITION);
						wcscpy(g_NotePaths.LangFile, szLang);
						PrepareMessages(szLang);
						//get program name for tray icon tooltip
						GetPrivateProfileStringW(__T("program"), __T("name"), NULL, g_NotePaths.ProgName, 256, szTemp);
						wcscpy(m_nData.szTip, g_NotePaths.ProgName);
						Shell_NotifyIconW(NIM_MODIFY, &m_nData);
						//refresh default group name
						RefreshDefaultGroupName();
						//re-arrange menu
						CreatePopUp(g_NotePaths.LangFile);
						if(m_hDlg)
							//apply new language
							ApplyMainDlgLanguage(m_hDlg, g_NotePaths.LangFile);
						if(g_hCPDialog)
							//apply new language
							SendMessageW(g_hCPDialog, CHM_CTRL_UPD_LANG, 0, 0);
						if(m_hHotDlg)
							//apply new language
							ApplyHotDialogLanguage(m_hHotDlg, g_NotePaths.LangFile);
						if(g_hSearchDialog)
							//apply new language
							SendMessageW(g_hSearchDialog, CHM_CTRL_UPD_LANG, 0, 0);
						if(g_hSearchTitlesDialog)
							//apply new language
							SendMessageW(g_hSearchTitlesDialog, CHM_CTRL_UPD_LANG, 0, 0);
						if(g_hOverdueDialog)
							//apply new language
							SendMessageW(g_hOverdueDialog, CHM_CTRL_UPD_LANG, 0, 0);
						//store new language
						WritePrivateProfileStringW(__T("lang"), S_FILE, g_NotePaths.LangFile, m_sMainINI);
						//notify all notes about new language
						ApplyNewLanguage();
					}
				}
				else if(id >= SHOW_GROUP_ADDITION && id < (SHOW_GROUP_ADDITION + MAX_GROUP_COUNT)){
					ShowGroupOfNotes(id - SHOW_GROUP_ADDITION);
				}
				else if(id >= HIDE_GROUP_ADDITION && id < (HIDE_GROUP_ADDITION + MAX_GROUP_COUNT)){
					HideGroupOfNotes(id - HIDE_GROUP_ADDITION);
				}
				else if(id >= FAVORITES_ADDITION && id < (FAVORITES_ADDITION + FAVORITES_MAX)){	//favorites menu
					ShowFavorite(id);
				}
			}
	}
}

static BOOL Main_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	wchar_t		szTemp[MAX_PATH], szMessage[256];
	int			exitParam;

	//get process heap
	g_hHeap = GetProcessHeap();
	// //register for logon/logoff messages
	if(m_TrackLogon)
		RegisterLogonMessage(hwnd);
	
	//get main INI file path
	if(wcslen(m_sMainINI) == 0)
		GetSubPathW(m_sMainINI, __T("\\Notes.ini"));
	wcscpy(g_NotePaths.INIFile, m_sMainINI);
	//get data path
	if(wcslen(g_NotePaths.DataDir) == 0)
		GetSubPathW(g_NotePaths.DataDir, __T("\\data\\"));
	wcscpy(g_NotePaths.DataFile, g_NotePaths.DataDir);
	wcscat(g_NotePaths.DataFile, FN_DATA_FILE);
	//check whether paths exist
	wcscpy(szTemp, m_sMainINI);
	PathRemoveFileSpecW(szTemp);
	if(!PathFileExistsW(szTemp)){
		SHCreateDirectoryExW(NULL, szTemp, NULL);
	}
	wcscpy(szTemp, g_NotePaths.DataDir);
	PathRemoveFileSpecW(szTemp);
	if(!PathFileExistsW(szTemp)){
		SHCreateDirectoryExW(NULL, szTemp, NULL);
	}
	//get skins path
	GetSubPathW(g_NotePaths.SkinDir, __T("\\skins\\"));
	//get fonts path
	GetSubPathW(g_NotePaths.FontsPath, __T("\\fonts\\"));
	//get language file short name
	GetPrivateProfileStringW(__T("lang"), S_FILE, L"english.lng", g_NotePaths.LangFile, 256, m_sMainINI);
	//get language files directory
	GetSubPathW(g_NotePaths.LangDir, __T("\\lang\\"));
	//get sound files directory
	GetSubPathW(g_NotePaths.SoundDir, __T("\\sound\\"));
	//get def browser
	GetPrivateProfileStringW(L"def_browser", L"def_browser", NULL, g_DefBrowser, MAX_PATH, m_sMainINI);
	//prepare messages
	PrepareMessages(g_NotePaths.LangFile);
	//allocate memory for note commands
	m_NoteCommands = (P_PNCOMMAND)calloc(NoteMenuCount(), sizeof(PNCOMMAND));
	//allocate memory for note hotkeys
	m_NoteHotkeys = (P_HK_TYPE)calloc(NoteMenuCount(), sizeof(HK_TYPE));
	//prepare note hotkeys
	PrepareNoteHotKeys();
	//register note hot keys
	RegisterHKeys(hwnd, m_NoteHotkeys, NoteMenuCount());
	//prepare hot keys
	PrepareHotKeys();
	//register hot keys
	RegisterHKeys(hwnd, m_Hotkeys, NELEMS(m_Hotkeys));
	//get program name for tray icon tooltip
	wcscpy(szTemp, g_NotePaths.LangDir);
	wcscat(szTemp, g_NotePaths.LangFile);
	GetPrivateProfileStringW(__T("program"), __T("name"), DEF_PROG_TOOLTIP, g_NotePaths.ProgName, 256, szTemp);
	
	//register note window class
	if(!RegisterNoteClass(g_hInstance)){
		GetPNotesMessage(g_NotePaths.LangDir, g_NotePaths.LangFile, _T("no_note_registered"), __T("Unable to register note window class"), szMessage);
		MessageBoxW(hwnd, szMessage, NULL, MB_OK);
		CleanUp();
		return FALSE;
	}

	//create date-time format strings
	GetPrivateProfileStringW(S_FORMAT, __T("date_format"), DTS_DATE_FORMAT, g_DTFormats.DateFormat, 128, m_sMainINI);
	GetPrivateProfileStringW(S_FORMAT, __T("time_format"), DTS_TIME_FORMAT, g_DTFormats.TimeFormat, 128, m_sMainINI);
	
	//get sound information
	g_Sound.allowSound = GetPrivateProfileIntW(S_SOUND, DSK_ALLOW, 1, m_sMainINI);
	g_Sound.defSound = GetPrivateProfileIntW(S_SOUND, DSK_SOUND_DEF, 0, m_sMainINI);
	GetPrivateProfileStringW(S_SOUND, DSK_SOUND_CUST, DS_DEF_SOUND, g_Sound.custSound, MAX_PATH, m_sMainINI);

	//try version 4.0 settings
	if(!GetPrivateProfileStructW(S_NOTE, S_SETTINGS, &g_NoteSettings, sizeof(NOTE_SETTINGS), m_sMainINI)){
		g_NoteSettings.newOnTop = TRUE;
		//try version 3.0.0 settings
		if(!GetPrivateProfileStructW(S_NOTE, S_SETTINGS, &g_NoteSettings, sizeof(NOTE_SETTINGS) - sizeof(BOOL) - sizeof(int) * 2, m_sMainINI)){
			g_NoteSettings.autoSave = FALSE;
			g_NoteSettings.autoSValue = 5;
			// g_NoteSettings.allOnTop = FALSE;
			g_NoteSettings.visualAlert = TRUE;
			g_NoteSettings.rollOnDblClick = TRUE;
			//try version 2.5.0 settings
			if(!GetPrivateProfileStructW(S_NOTE, S_SETTINGS, &g_NoteSettings, sizeof(NOTE_SETTINGS) - sizeof(BOOL) * 4 - sizeof(BYTE), m_sMainINI)){
				g_NoteSettings.confirmDelete = TRUE;
				//try version 2.0.0 settings
				if(!GetPrivateProfileStructW(S_NOTE, S_SETTINGS, &g_NoteSettings, sizeof(NOTE_SETTINGS) - sizeof(BOOL), m_sMainINI)){
					//try version 1.0.1 structure size
					if(!GetPrivateProfileStructW(S_NOTE, S_SETTINGS, &g_NoteSettings, sizeof(NOTE_SETTINGS) - (sizeof(BOOL) * 2), m_sMainINI)){
						// g_NoteSettings.showOnStart = TRUE;
						g_NoteSettings.saveOnExit = TRUE;
						g_NoteSettings.confirmSave = TRUE;
					}
				}
			}
		}
	}
	//set default action of double click in case of no action is set before
	if(!IsBitOn(g_NoteSettings.reserved2, SB2_NEW_NOTE) 
	&& !IsBitOn(g_NoteSettings.reserved2, SB2_CONTROL_PANEL) 
	&& !IsBitOn(g_NoteSettings.reserved2, SB2_PREFS) 
	&& !IsBitOn(g_NoteSettings.reserved2, SB2_SEARCH_NOTES)){
		BitOn(&g_NoteSettings.reserved2, SB2_NEW_NOTE);
	}

	//prepare popup menu
	CreatePopUp(g_NotePaths.LangFile);
	CreateMenuFont();

	//register custom fonts
	if(IsBitOn(g_NoteSettings.reserved1, SB1_CUST_FONTS))
		RegisterCustomFonts();
	//prepare default appearance
	if(!CreateAppearance(hwnd)){
		CleanUp();
		return FALSE;
	}
	//get exit flag
	exitParam = GetPrivateProfileIntW(S_EXITFLAG, DSK_CURRENT_STATE, 0, m_sMainINI);
	if(exitParam != 0){
		if(IsAnyAutosavedExists()){
			if(MessageBoxW(hwnd, m_sUnsuccessfull, PROG_NAME, MB_YESNO | MB_ICONEXCLAMATION) == IDYES){
				LoadAutosaved();
			}
		}
	}
	DeleteAutosaved();
	//write exit state parameter ("1" on start, "0" on succesfull end)
	WritePrivateProfileStringW(S_EXITFLAG, DSK_CURRENT_STATE, L"1", m_sMainINI);
	//load groups
	MakeDefaultGroup();
	//start autosave timer
	if(g_NoteSettings.autoSave)
		m_Timer = SetTimer(hwnd, 11, g_NoteSettings.autoSValue * 1000 * 60, AutosaveTimerProc);
	//get docking settings
	GetPrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), m_sMainINI);
	//prepare dock appearance
	memcpy(&g_DRTHandles, &g_RTHandles, sizeof(NOTE_RTHANDLES));
	if(g_DockSettings.fCustSkin){
		GetDockSkinProperties(hwnd, &g_DRTHandles, g_DockSettings.szCustSkin, FALSE);
	}
	if(!g_DockSettings.fCustCaption){
		memcpy(&g_DockSettings.lfCaption, &g_Appearance.lfCaption, sizeof(g_Appearance.lfCaption));
		g_DockSettings.crCaption = g_Appearance.crCaption;
	}
	if(!g_DockSettings.fCustColor){
		g_DockSettings.crWindow = g_Appearance.crWindow;
	}
	// if(!g_DockSettings.fCustFont){
		// memcpy(&g_DockSettings.lfFont, &g_Appearance.lfFont, sizeof(g_Appearance.lfFont));
	// }
	//prepare docking headers
	InitDockHeaders();
	//load existing notes
	GetAllNotes(hwnd);
	//rearrange all dock notes
	RearrangeDockWindowsOnstart(DOCK_LEFT);
	RearrangeDockWindowsOnstart(DOCK_TOP);
	RearrangeDockWindowsOnstart(DOCK_RIGHT);
	RearrangeDockWindowsOnstart(DOCK_BOTTOM);
	//create dock arrows
	g_DArrows.hlu = CreateDArrow(g_hInstance, hwnd, DA_LEFT_UP);
	g_DArrows.hld = CreateDArrow(g_hInstance, hwnd, DA_LEFT_DOWN);
	g_DArrows.hru = CreateDArrow(g_hInstance, hwnd, DA_RIGHT_UP);
	g_DArrows.hrd = CreateDArrow(g_hInstance, hwnd, DA_RIGHT_DOWN);
	g_DArrows.htl = CreateDArrow(g_hInstance, hwnd, DA_TOP_LEFT);
	g_DArrows.htr = CreateDArrow(g_hInstance, hwnd, DA_TOP_RIGHT);
	g_DArrows.hbl = CreateDArrow(g_hInstance, hwnd, DA_BOTTOM_LEFT);
	g_DArrows.hbr = CreateDArrow(g_hInstance, hwnd, DA_BOTTOM_RIGHT);
	//create data file if it does not exist
	CreateDataFile();
	//get bitmaps for menu
	g_hBmpMenuNormal = LoadBitmapW(g_hInstance, MAKEINTRESOURCEW(IDB_MENU_NORMAL));
	g_hBmpMenuGray = LoadBitmapW(g_hInstance, MAKEINTRESOURCEW(IDB_MENU_GRAY));
	//initialize empty notes structure
	g_Empties.pPoint = 0;
	g_Empties.pDockData = 0;
	g_Empties.count = 0;
	//prepare groups
	GetAllGroups();
	//prepare find flags
	g_Flags |= FR_DOWN;
	//initialize printing
	InitPrintMembers();
	//show control panel
	if(IsBitOn(g_NoteSettings.reserved1, SB1_SHOW_CP))
		SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_CTRL, 0), 0);

	return TRUE;
}

static void CheckOverdueReminders(void){
	PMEMNOTE		* parr, pTemp = MemoryNotes();
	int				count = 0, days;
	BOOL			fFound;
	SYSTEMTIME		st;
	FILETIME		ftNow, ftRem, ftLast;

	GetLocalTime(&st);
	SystemTimeToFileTime(&st, &ftNow);

	while(pTemp){
		fFound = FALSE;
		//check notes with schedule type, not from recycle bin and those should be tracked
		if(pTemp->pSchedule->scType != SCH_NO && pTemp->pData->idGroup != GROUP_RECYCLE && 
			!IsBitOn(pTemp->pData->res1, NB_NOT_TRACK)){
			switch(pTemp->pSchedule->scType){
				case SCH_ONCE:
					SystemTimeToFileTime(&pTemp->pSchedule->scDate, &ftRem);
					if(CompareFileTime(&ftRem, &ftNow) < 0)
						fFound = TRUE;
					break;
				case SCH_ED:
					SystemTimeToFileTime(&pTemp->pSchedule->scLastRun, &ftLast);
					if(IsDateEmpty(&pTemp->pSchedule->scLastRun)){
						if(st.wMonth > pTemp->pSchedule->scStart.wMonth || st.wDay > pTemp->pSchedule->scStart.wDay 
						|| st.wHour > pTemp->pSchedule->scStart.wHour || (st.wHour == pTemp->pSchedule->scStart.wHour && st.wMinute > pTemp->pSchedule->scStart.wMinute)){
							fFound = TRUE;
						}
					}
					else if(CompareFileTime(&ftLast, &ftNow) < 0){
						if(DateDiff(st, pTemp->pSchedule->scLastRun, DAY) > 0){
							if(st.wHour > pTemp->pSchedule->scDate.wHour || (st.wHour == pTemp->pSchedule->scDate.wHour && st.wMinute > pTemp->pSchedule->scDate.wMinute)){
								fFound = TRUE;
							}
						}
					}
					break;
				case SCH_REP:
					SystemTimeToFileTime(&pTemp->pSchedule->scLastRun, &ftLast);
					if(!IsDateEmpty(&pTemp->pSchedule->scLastRun) && CompareFileTime(&ftLast, &ftNow) < 0){
						if(DateDiff(st, pTemp->pSchedule->scLastRun, DAY) > pTemp->pSchedule->scDate.wDay)
							fFound = TRUE;
						else if((DateDiff(st, pTemp->pSchedule->scLastRun, DAY) == pTemp->pSchedule->scDate.wDay) 
						&& (DateDiff(st, pTemp->pSchedule->scLastRun, HOUR) > pTemp->pSchedule->scDate.wHour))
							fFound = TRUE;
						else if((DateDiff(st, pTemp->pSchedule->scLastRun, DAY) == pTemp->pSchedule->scDate.wDay) 
						&& (DateDiff(st, pTemp->pSchedule->scLastRun, HOUR) == pTemp->pSchedule->scDate.wHour)
						&& (DateDiff(st, pTemp->pSchedule->scLastRun, MINUTE) > pTemp->pSchedule->scDate.wMinute))
							fFound = TRUE;
						else if((DateDiff(st, pTemp->pSchedule->scLastRun, DAY) == pTemp->pSchedule->scDate.wDay) 
						&& (DateDiff(st, pTemp->pSchedule->scLastRun, HOUR) == pTemp->pSchedule->scDate.wHour)
						&& (DateDiff(st, pTemp->pSchedule->scLastRun, MINUTE) == pTemp->pSchedule->scDate.wMinute)
						&& (DateDiff(st, pTemp->pSchedule->scLastRun, SECOND) > pTemp->pSchedule->scDate.wSecond))
							fFound = TRUE;
					}
					break;
				case SCH_AFTER:
					if(DateDiff(st, pTemp->pSchedule->scStart, YEAR) > pTemp->pSchedule->scDate.wYear)
						fFound = TRUE;
					else if((DateDiff(st, pTemp->pSchedule->scStart, YEAR) == pTemp->pSchedule->scDate.wYear) 
					&& (DateDiff(st, pTemp->pSchedule->scStart, MONTH) > pTemp->pSchedule->scDate.wMonth))
						fFound = TRUE;
					else if((DateDiff(st, pTemp->pSchedule->scStart, YEAR) == pTemp->pSchedule->scDate.wYear) 
					&& (DateDiff(st, pTemp->pSchedule->scStart, MONTH) == pTemp->pSchedule->scDate.wMonth)
					&& (DateDiff(st, pTemp->pSchedule->scStart, WEEK) > pTemp->pSchedule->scDate.wDayOfWeek))
						fFound = TRUE;
					else if((DateDiff(st, pTemp->pSchedule->scStart, YEAR) == pTemp->pSchedule->scDate.wYear) 
					&& (DateDiff(st, pTemp->pSchedule->scStart, MONTH) == pTemp->pSchedule->scDate.wMonth)
					&& (DateDiff(st, pTemp->pSchedule->scStart, WEEK) == pTemp->pSchedule->scDate.wDayOfWeek)
					&& (DateDiff(st, pTemp->pSchedule->scStart, DAY) > pTemp->pSchedule->scDate.wDay))
						fFound = TRUE;
					else if((DateDiff(st, pTemp->pSchedule->scStart, YEAR) == pTemp->pSchedule->scDate.wYear) 
					&& (DateDiff(st, pTemp->pSchedule->scStart, MONTH) == pTemp->pSchedule->scDate.wMonth)
					&& (DateDiff(st, pTemp->pSchedule->scStart, WEEK) == pTemp->pSchedule->scDate.wDayOfWeek)
					&& (DateDiff(st, pTemp->pSchedule->scStart, DAY) == pTemp->pSchedule->scDate.wDay)
					&& (DateDiff(st, pTemp->pSchedule->scStart, HOUR) > pTemp->pSchedule->scDate.wHour))
						fFound = TRUE;
					else if((DateDiff(st, pTemp->pSchedule->scStart, YEAR) == pTemp->pSchedule->scDate.wYear) 
					&& (DateDiff(st, pTemp->pSchedule->scStart, MONTH) == pTemp->pSchedule->scDate.wMonth)
					&& (DateDiff(st, pTemp->pSchedule->scStart, WEEK) == pTemp->pSchedule->scDate.wDayOfWeek)
					&& (DateDiff(st, pTemp->pSchedule->scStart, DAY) == pTemp->pSchedule->scDate.wDay)
					&& (DateDiff(st, pTemp->pSchedule->scStart, HOUR) == pTemp->pSchedule->scDate.wHour)
					&& (DateDiff(st, pTemp->pSchedule->scStart, MINUTE) > pTemp->pSchedule->scDate.wMinute))
						fFound = TRUE;
					else if((DateDiff(st, pTemp->pSchedule->scStart, YEAR) == pTemp->pSchedule->scDate.wYear) 
					&& (DateDiff(st, pTemp->pSchedule->scStart, MONTH) == pTemp->pSchedule->scDate.wMonth)
					&& (DateDiff(st, pTemp->pSchedule->scStart, WEEK) == pTemp->pSchedule->scDate.wDayOfWeek)
					&& (DateDiff(st, pTemp->pSchedule->scStart, DAY) == pTemp->pSchedule->scDate.wDay)
					&& (DateDiff(st, pTemp->pSchedule->scStart, HOUR) == pTemp->pSchedule->scDate.wHour)
					&& (DateDiff(st, pTemp->pSchedule->scStart, MINUTE) == pTemp->pSchedule->scDate.wMinute)
					&& (DateDiff(st, pTemp->pSchedule->scStart, SECOND) > pTemp->pSchedule->scDate.wSecond))
						fFound = TRUE;
					break;
				case SCH_PER_WD:
					days = DaysBetweenAlerts(pTemp->pSchedule->scLastRun, st, pTemp->pSchedule->scDate);
					for(int i = 0; i <=6; i++){
						if(IsBitOn(days, i)){
							int dof = RealDayOfWeek(i);
							if(IsBitOn(pTemp->pSchedule->scDate.wDayOfWeek, dof + 1)){
								fFound = TRUE;
								break;
							}
						}
					}
					break;
				case SCH_PER_YR:
					SystemTimeToFileTime(&pTemp->pSchedule->scDate, &ftRem);
					if(CompareFileTime(&ftRem, &ftNow) < 0)
						fFound = TRUE;
					break;
			}
		}
		if(fFound){
			if(count == 0)
				parr = malloc(sizeof(PMEMNOTE));
			else
				parr = realloc(parr, sizeof(PMEMNOTE) * (count + 1));
			parr[count] = pTemp;
			count++;
		}
		pTemp = pTemp->next;
	}
	if(count > 0){
		g_OverdueCount = count;
		DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_OVERDUE), g_hMain, Overdue_DlgProc, (LPARAM)parr);
		free(parr);
	}
}

static int DaysBetweenAlerts(const SYSTEMTIME stLast, const SYSTEMTIME stNow, const SYSTEMTIME stDate){
	int		days = 0, day, from, to;
	
	if(stNow.wDayOfWeek > stLast.wDayOfWeek){
		if(stLast.wHour < stDate.wHour || (stLast.wHour == stDate.wHour && stLast.wMinute < stDate.wMinute)){
			from = stLast.wDayOfWeek;
		}
		else{
			from = stLast.wDayOfWeek + 1;
		}
		if(stNow.wHour > stDate.wHour || (stNow.wHour == stDate.wHour && stNow.wMinute > stDate.wMinute)){
			to = stNow.wDayOfWeek;
		}
		else{
			to = stNow.wDayOfWeek - 1;
		}
		for(day = from; day <= to; day++){
			BitOn(&days, day);
		}
	}
	else if(stNow.wDayOfWeek < stLast.wDayOfWeek){
		if(stLast.wHour < stDate.wHour || (stLast.wHour == stDate.wHour && stLast.wMinute < stDate.wMinute)){
			from = stLast.wDayOfWeek;
		}
		else{
			from = stLast.wDayOfWeek + 1;
		}
		for(day = from; day <= 6; day++){
			BitOn(&days, day);
		}
		if(stNow.wHour > stDate.wHour || (stNow.wHour == stDate.wHour && stNow.wMinute > stDate.wMinute)){
			to = stNow.wDayOfWeek;
		}
		else{
			to = stNow.wDayOfWeek - 1;
		}
		for(day = 0; day <= from; day++){
			BitOn(&days, day);
		}
	}
	else{
		if((stLast.wHour < stDate.wHour || (stLast.wHour == stDate.wHour && stLast.wMinute < stDate.wMinute)) && (stNow.wHour > stDate.wHour || (stNow.wHour == stDate.wHour && stNow.wMinute > stDate.wMinute))){
			BitOn(&days, stNow.wDayOfWeek);
		}
	}
	return days;
}

static void InitDockHeaders(void){
	g_DHeaders.pLeft = (PDOCKHEADER)calloc(1, sizeof(DOCKHEADER));
	g_DHeaders.pTop = (PDOCKHEADER)calloc(1, sizeof(DOCKHEADER));
	g_DHeaders.pRight = (PDOCKHEADER)calloc(1, sizeof(DOCKHEADER));
	g_DHeaders.pBottom = (PDOCKHEADER)calloc(1, sizeof(DOCKHEADER));
}

static void FreeDockHeaders(void){
	FreeDockList(g_DHeaders.pLeft);
	FreeDockList(g_DHeaders.pTop);
	FreeDockList(g_DHeaders.pRight);
	FreeDockList(g_DHeaders.pBottom);
}

static void MakeDefaultGroup(void){
	PNGROUP		png;
	wchar_t 	szFile[MAX_PATH];

	ZeroMemory(&png, sizeof(png));
	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);
	if(!GetPrivateProfileStructW(S_GROUPS, L"0", &png, sizeof(png), g_NotePaths.INIFile)){
		png.parent = -1;
		GetPrivateProfileStringW(S_CAPTIONS, L"group_default", DS_GENERAL_GROUP_NAME, png.szName, 128, szFile);
		WritePrivateProfileStructW(S_GROUPS, L"0", &png, sizeof(png), g_NotePaths.INIFile);
	}
}

static void RefreshDefaultGroupName(void){
	wchar_t 	szFile[MAX_PATH];
	PNGROUP		png, *ppg;

	ZeroMemory(&png, sizeof(png));
	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);
	if(!GetPrivateProfileStructW(S_GROUPS, L"0", &png, sizeof(png), g_NotePaths.INIFile)){
		png.parent = -1;
	}
	GetPrivateProfileStringW(S_CAPTIONS, L"group_default", DS_GENERAL_GROUP_NAME, png.szName, 128, szFile);
	WritePrivateProfileStructW(S_GROUPS, L"0", &png, sizeof(png), g_NotePaths.INIFile);
	ppg = GetGroup(png.id);
	if(ppg){
		wcscpy(ppg->szName, png.szName);
	}
}

static void GetFileNameFromPath(wchar_t * lpPath, wchar_t * lpName){
	while(*lpPath++)
		;
	while(*lpPath != '\\')
		*lpPath--;
	lpPath++;
	wcscpy(lpName, lpPath);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CreateDataFile
 Created  : Mon May 14 12:24:55 2007
 Modified : Mon May 14 12:24:55 2007

 Synopsys : Creates file for notes if it does not exist. Adds BOM bytes to 
            the beginning of file, since it must be Unicode
 Input    : Nothing
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void CreateDataFile(void){
	HANDLE		hFile;
	wchar_t		szTemp[MAX_PATH];

	wcscpy(szTemp, g_NotePaths.DataDir);
	if(!NotesDirExists(szTemp))
		NotesDirCreate(szTemp);
	if(!FileExistsByFullPath(g_NotePaths.DataFile)){
		hFile = CreateFileW(g_NotePaths.DataFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile != INVALID_HANDLE_VALUE){
			CloseHandle(hFile);
		}
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CreateAppearance
 Created  : Mon May 14 12:26:19 2007
 Modified : Mon May 14 12:26:19 2007

 Synopsys : Loads skin, font and text color for general notes settings
 Input    : Main window handle
 Output   : TRUE on success, FALSE if skin file does not exist
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static BOOL CreateAppearance(HWND hwnd){

	wchar_t		szMessage[256];

	//get skin file name
	GetPrivateProfileStringW(S_APPEARANCE, S_SKIN, NULL,g_Appearance.szSkin, 64, m_sMainINI);
	if(wcslen(g_Appearance.szSkin)){
		//check skin existance
		if(!FileExists(g_NotePaths.SkinDir, g_Appearance.szSkin)){
			if(!DoesAnySkinExist(g_Appearance.szSkin)){
				GetPNotesMessage(g_NotePaths.LangDir, g_NotePaths.LangFile, __T("no_skin_file"), __T("The skin file does not exist.\nThe program will now quit."), szMessage);
				MessageBoxW(hwnd, szMessage, NULL, MB_OK);
				return FALSE;
			}
		}
	}

	//get font data
	if(!GetPrivateProfileStructW(S_APPEARANCE, S_FONT, &g_Appearance.lfFont, sizeof(g_Appearance.lfFont), m_sMainINI)){
		//prepare default font
		CreateDefaultFont(&g_Appearance.lfFont, FALSE);
	}
	g_Appearance.crFont = GetPrivateProfileIntW(S_APPEARANCE, S_COLOR, GetSysColor(COLOR_WINDOWTEXT), m_sMainINI);
	if(g_Appearance.crFont == 0)
		g_Appearance.crFont = 1;
	g_RTHandles.hFont = CreateFontIndirectW(&g_Appearance.lfFont);

	//get window caption font data
	if(!GetPrivateProfileStructW(S_APPEARANCE, S_CAPTION_FONT, &g_Appearance.lfCaption, sizeof(g_Appearance.lfCaption), m_sMainINI)){
		//prepare default font
		CreateDefaultFont(&g_Appearance.lfCaption, TRUE);
	}
	g_Appearance.crCaption = GetPrivateProfileIntW(S_APPEARANCE, S_CAPTION_COLOR, GetSysColor(COLOR_WINDOWTEXT), m_sMainINI);
	if(g_Appearance.crCaption == 0)
		g_Appearance.crCaption = 1;
	g_RTHandles.hFCaption = CreateFontIndirectW(&g_Appearance.lfCaption);
	//get window background color
	g_Appearance.crWindow = GetPrivateProfileIntW(S_APPEARANCE, S_BCKG_COLOR, RGB(242, 221, 116), m_sMainINI);

	//Load skin
	GetSkinProperties(hwnd, &g_RTHandles, g_Appearance.szSkin, TRUE);

	//get default skinless note size
	g_RTHandles.szDef.cx = GetPrivateProfileIntW(S_DEFSIZE, L"w", DEF_FLAT_WIDTH, m_sMainINI);
	g_RTHandles.szDef.cy = GetPrivateProfileIntW(S_DEFSIZE, L"h", DEF_FLAT_HEIGHT, m_sMainINI);
	return TRUE;
}
/*-@@+@@------------------------------------------------------------------
 Procedure: RestoreDefaultSettings
 Created  : Thu Aug  9 17:36:43 2007
 Modified : Thu Aug  9 17:36:43 2007

 Synopsys : Restores default program settings (except language)
 Input    : Nothing
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void RestoreDefaultSettings(void){

	int			index;
	wchar_t		szSkin[64];

	//-----------	appearance dialog	-----------
	//set default transparency
	// SetDlgItemInt(m_Dialogs.hAppearance, IDC_EDT_TRANS, 0, FALSE);
	SendDlgItemMessageW(m_Dialogs.hAppearance, IDC_CHK_TRANS, BM_SETCHECK, BST_UNCHECKED, 0);
	EnableWindow(GetDlgItem(m_Dialogs.hAppearance, IDC_TRANS_LEFT_ST), FALSE);
	EnableWindow(GetDlgItem(m_Dialogs.hAppearance, IDC_TRANS_RIGHT_ST), FALSE);
	EnableWindow(GetDlgItem(m_Dialogs.hAppearance, IDC_TRANS_TRACK_BAR), FALSE);
	EnableWindow(GetDlgItem(m_Dialogs.hAppearance, IDC_GRP_TRANS), FALSE);
	// EnableWindow(GetDlgItem(m_Dialogs.hAppearance, IDC_EDT_TRANS), FALSE);
	EnableWindow(GetDlgItem(m_Dialogs.hAppearance, IDC_TRANS_WARNING), FALSE);
	m_TempNoteSettings.transAllow = FALSE;
	m_TempNoteSettings.transValue = 255;
	SetTransValue(m_Dialogs.hAppearance, 0);
	SendDlgItemMessageW(m_Dialogs.hAppearance, IDC_TRANS_TRACK_BAR, TBM_SETPOS, TRUE, 0);
	//set default font
	CreateDefaultFont(&m_TempAppearance.lfFont, FALSE);
	if(g_RTHandles.hFont != m_TempRHandles.hFont)
		DeleteObject(m_TempRHandles.hFont);
	m_TempRHandles.hFont = CreateFontIndirectW(&m_TempAppearance.lfFont);
	m_TempAppearance.crFont = 1;
	SendDlgItemMessageW(m_Dialogs.hAppearance, IDC_CHK_SCROLLBAR, BM_SETCHECK, BST_UNCHECKED, 0);
	m_TempNoteSettings.showScrollbar = FALSE;
	CheckDlgButton(m_Dialogs.hAppearance, IDC_HIDE_TOOLBAR, BST_UNCHECKED);
	BitOff(&m_TempNoteSettings.reserved1, SB1_HIDETOOLBAR);
	CheckDlgButton(m_Dialogs.hAppearance, IDC_USE_CUST_FONTS, BST_UNCHECKED);
	BitOff(&m_TempNoteSettings.reserved1, SB1_CUST_FONTS);
	//-----------	skins dialog	-----------
	//select default skin (none)
	SendDlgItemMessageW(m_Dialogs.hSkins, IDC_LST_SKIN, LB_SETCURSEL, 0, 0);
	SendMessageW(m_Dialogs.hSkins, WM_COMMAND, MAKEWPARAM(IDC_LST_SKIN, LBN_SELCHANGE), (LPARAM)GetDlgItem(m_Dialogs.hAppearance, IDC_LST_SKIN));	
	//restore default skinless settings
	m_TempAppearance.crWindow = RGB(242, 221, 116);
	CreateDefaultFont(&m_TempAppearance.lfCaption, TRUE); 
	if(g_RTHandles.hFCaption != m_TempRHandles.hFCaption)
		DeleteObject(m_TempRHandles.hFCaption);
	m_TempRHandles.hFCaption = CreateFontIndirectW(&m_TempAppearance.lfCaption);
	m_TempAppearance.crCaption = 1;
	m_TempRHandles.szDef.cx = DEF_FLAT_WIDTH;
	m_TempRHandles.szDef.cy = DEF_FLAT_HEIGHT;
	SetDlgItemInt(m_Dialogs.hSkins, IDC_EDT_DEF_SIZE_W, DEF_FLAT_WIDTH, FALSE);
	SetDlgItemInt(m_Dialogs.hSkins, IDC_EDT_DEF_SIZE_H, DEF_FLAT_HEIGHT, FALSE);
	//set default roll/unroll property
	SendDlgItemMessageW(m_Dialogs.hSkins, IDC_REDUCE_TO_CAPTION, BM_SETCHECK, BST_CHECKED, 0);
	m_TempNoteSettings.rollOnDblClick = TRUE;
	//set default random color property
	SendDlgItemMessageW(m_Dialogs.hSkins, IDC_CHK_RANDOM_COLOR, BM_SETCHECK, BST_UNCHECKED, 0);
	BitOff(&m_TempNoteSettings.reserved1, SB1_RANDCOLOR);
	//set default inverting text color
	SendDlgItemMessageW(m_Dialogs.hSkins, IDC_CHK_INVERT_COLOR, BM_SETCHECK, BST_UNCHECKED, 0);
	BitOff(&m_TempNoteSettings.reserved1, SB1_INVERT_TEXT);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_CHK_INVERT_COLOR), FALSE);
	//enable skinless properties
	EnableSkinlessProperties(TRUE);

	//-----------	docking dialog	-----------
	CheckDlgButton(m_Dialogs.hDocks, IDC_OPT_DOCK_DEF_SKIN, BST_CHECKED);
	CheckDlgButton(m_Dialogs.hDocks, IDC_OPT_DOCK_RND_SKIN, BST_UNCHECKED);
	EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_CBO_SKINS), FALSE);
	wcscpy(szSkin, g_Appearance.szSkin);
	szSkin[wcslen(szSkin) - 4] = '\0';
	index = SendDlgItemMessageW(m_Dialogs.hDocks, IDC_CBO_SKINS, CB_FINDSTRING, -1, (LPARAM)szSkin);
	SendDlgItemMessageW(m_Dialogs.hDocks, IDC_CBO_SKINS, CB_SETCURSEL, index, 0);
	m_TempDockingSettings.fCustSkin = FALSE;
	// CheckDlgButton(m_Dialogs.hDocks, IDC_OPT_DOCK_DEF_SIZE, BST_CHECKED);
	CheckDlgButton(m_Dialogs.hDocks, IDC_OPT_DOCK_RND_SIZE, BST_UNCHECKED);
	EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_DEF_SIZE_W), FALSE);
	EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_DEF_SIZE_H), FALSE);
	EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_EDT_DEF_SIZE_W), FALSE);
	EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_EDT_DEF_SIZE_H), FALSE);
	
	m_TempDockingSettings.fCustSize = FALSE;
	if(m_TempRHandles.hbSkin){
		SetDlgItemInt(m_Dialogs.hDocks, IDC_EDT_DEF_SIZE_W, DEF_FLAT_WIDTH, FALSE);
		SetDlgItemInt(m_Dialogs.hDocks, IDC_EDT_DEF_SIZE_H, DEF_FLAT_HEIGHT, FALSE);
	}
	else{
		SetDlgItemInt(m_Dialogs.hDocks, IDC_EDT_DEF_SIZE_W, m_TempRHandles.szDef.cx, FALSE);
		SetDlgItemInt(m_Dialogs.hDocks, IDC_EDT_DEF_SIZE_H, m_TempRHandles.szDef.cy, FALSE);
	}
	SendDlgItemMessageW(m_Dialogs.hDocks, IDC_DOCK_SLIDER, TBM_SETPOS, TRUE, 0);
	m_TempDockingSettings.dist = 0;
	m_TempDockingSettings.fCustCaption = FALSE;
	m_TempDockingSettings.fCustColor = FALSE;
	CheckDlgButton(m_Dialogs.hDocks, IDC_CHK_DOCK_C, BST_UNCHECKED);
	CheckDlgButton(m_Dialogs.hDocks, IDC_CHK_DOCK_F, BST_UNCHECKED);
	EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_CMD_SKNLESS_BCOLOR), FALSE);
	EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_CMD_SKNLESS_CFONT), FALSE);
	// CheckDlgButton(m_Dialogs.hDocks, IDC_CHK_DOCK_CUST_F, BST_UNCHECKED);
	// EnableWindow(GetDlgItem(m_Dialogs.hDocks, IDC_CMD_DOCK_FONT), FALSE);
	// if(g_DRTHandles.hFont != m_TempDRTHandles.hFont)
		// DeleteObject(m_TempDRTHandles.hFont);
	// m_TempDRTHandles.hFont = CreateFontIndirectW(&m_TempDockingSettings.lfFont);
	// m_TempDockingSettings.crFont = 1;
	m_TempDockingSettings.crWindow = RGB(242, 221, 116);
	CreateDefaultFont(&m_TempDockingSettings.lfCaption, TRUE); 
	if(g_DRTHandles.hFCaption != m_TempDRTHandles.hFCaption)
		DeleteObject(m_TempDRTHandles.hFCaption);
	m_TempDRTHandles.hFCaption = CreateFontIndirectW(&m_TempDockingSettings.lfCaption);
	m_TempDockingSettings.crCaption = 1;

	//-----------	misc dialog	-----------
	//set default misc options
	SendDlgItemMessageW(m_Dialogs.hMisc, IDC_CHK_NEW_V, BM_SETCHECK, BST_UNCHECKED, 0);
	m_TempNoteSettings.checkOnStart = FALSE;
	// SendDlgItemMessageW(m_Dialogs.hMisc, IDC_CHK_SHOW, BM_SETCHECK, BST_CHECKED, 0);
	// m_TempNoteSettings.showOnStart = TRUE;
	SendDlgItemMessageW(m_Dialogs.hMisc, IDC_CHK_AUTORUN, BM_SETCHECK, BST_UNCHECKED, 0);
	m_TempNoteSettings.onStartup = FALSE;
	SetDlgItemTextW(m_Dialogs.hMisc, IDC_EDT_DEF_BROWSER, NULL);
	*m_sTempdefBrowser = '\0';
	CheckDlgButton(m_Dialogs.hMisc, IDC_CHK_SHOW_CP, BST_UNCHECKED);
	BitOff(&m_TempNoteSettings.reserved1, SB1_SHOW_CP);
	//-----------	Behavior dialog	-----------
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_ALWAYS_SAVE, BM_SETCHECK, BST_CHECKED, 0);
	m_TempNoteSettings.saveOnExit = TRUE;
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_ASK_B_SAVE, BM_SETCHECK, BST_CHECKED, 0);
	m_TempNoteSettings.confirmSave = TRUE;
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CHK_DEL_CONFIRM, BM_SETCHECK, BST_CHECKED, 0);
	m_TempNoteSettings.confirmDelete = TRUE;
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CHK_NEW_ONTOP, BM_SETCHECK, BST_CHECKED, 0);
	m_TempNoteSettings.newOnTop = TRUE;
	//restore default autosave settings
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CHK_AUTOSAVE, BM_SETCHECK, BST_UNCHECKED, 0);
	EnableWindow(GetDlgItem(m_Dialogs.hBehavior, IDC_EDT_AUTOSAVE), FALSE);
	EnableWindow(GetDlgItem(m_Dialogs.hBehavior, IDC_UPD_AUTOSAVE), FALSE);
	EnableWindow(GetDlgItem(m_Dialogs.hBehavior, IDC_ST_AUTOSAVE), FALSE);
	m_TempNoteSettings.autoSave = FALSE;
	m_TempNoteSettings.autoSValue = 5;
	SetWindowTextW(GetDlgItem(m_Dialogs.hBehavior, IDC_EDT_AUTOSAVE), L"5");
	//set default hot keys (remove all)
	for(int i = 0; i < NELEMS(m_TempHotkeys); i++){
		m_TempHotkeys[i].fsModifiers = 0;
		m_TempHotkeys[i].vk = 0;
		m_TempHotkeys[i].szKey[0] = '\0';
	}
	for(int i = 0; i < NoteMenuCount(); i++){
		m_TempNoteHotkeys[i].fsModifiers = 0;
		m_TempNoteHotkeys[i].vk = 0;
		m_TempNoteHotkeys[i].szKey[0] = '\0';
	}
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_COMMANDS, CB_SETCURSEL, 0, 0);
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_SINGLE_NOTE, CB_SETCURSEL, 0, 0);
	SetDlgItemTextW(m_Dialogs.hBehavior, IDC_ST_HOTKEY, NULL);
	EnableWindow(GetDlgItem(m_Dialogs.hBehavior, IDC_CMD_DEL_HOT), FALSE);
	CheckDlgButton(m_Dialogs.hBehavior, IDC_OPT_MAIN_MENU, BST_CHECKED);
	CheckDlgButton(m_Dialogs.hBehavior, IDC_OPT_SINGLE_NOTE, BST_UNCHECKED);
	EnableWindow(GetDlgItem(m_Dialogs.hBehavior, IDC_CBO_SINGLE_NOTE), FALSE);
	CheckDlgButton(m_Dialogs.hBehavior, IDC_CHK_REL_POSITION, BST_UNCHECKED);
	BitOff(&m_TempNoteSettings.reserved1, SB1_RELPOSITION);
	//erase double click action settings
	BitOff(&m_TempNoteSettings.reserved2, SB2_NEW_NOTE);
	BitOff(&m_TempNoteSettings.reserved2, SB2_CONTROL_PANEL);
	BitOff(&m_TempNoteSettings.reserved2, SB2_PREFS);
	BitOff(&m_TempNoteSettings.reserved2, SB2_SEARCH_NOTES);
	//set default settings
	BitOn(&m_TempNoteSettings.reserved2, 0);
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_DBL_CLICK, CB_SETCURSEL, 0, 0);
	//-----------	schedule dialog	-----------
	//set default date formats
	wcscpy(m_TempFormats.DateFormat, DTS_DATE_FORMAT);
	wcscpy(m_TempFormats.TimeFormat, DTS_TIME_FORMAT);
	SetDlgItemTextW(m_Dialogs.hSchedule, IDC_EDT_DATE_FMTS, m_TempFormats.DateFormat);
	SetDlgItemTextW(m_Dialogs.hSchedule, IDC_EDT_TIME_FMTS, m_TempFormats.TimeFormat);
	SetShowDateTime(m_Dialogs.hSchedule);
	//set default sound settings
	m_TempSound.allowSound = TRUE;
	m_TempSound.defSound = 0;
	wcscpy(m_TempSound.custSound, DS_DEF_SOUND);
	SendDlgItemMessageW(m_Dialogs.hSchedule, IDC_CHK_ALLOW_SOUND, BM_SETCHECK, BST_CHECKED, 0);
	SendDlgItemMessageW(m_Dialogs.hSchedule, IDC_LST_SOUND, LB_SETCURSEL, 0, 0);
	EnableWindow(GetDlgItem(m_Dialogs.hSchedule, IDC_CMD_DEL_SOUND), FALSE);
	//set default visual alert property
	SendDlgItemMessageW(m_Dialogs.hSchedule, IDC_VISUAL_NOTIFY, BM_SETCHECK, BST_CHECKED, 0);
	m_TempNoteSettings.visualAlert = TRUE;
	//set default overdue tracking
	CheckDlgButton(m_Dialogs.hSchedule, IDC_CHK_OVERDUE, BST_UNCHECKED);
	BitOff(&m_TempNoteSettings.reserved1, SB1_TRACK_OVERDUE);
}

static void EnableSkinlessProperties(BOOL fEnabled){
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_GRP_SKINLESS_PROPS), fEnabled);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_GRP_DEF_SIZE), fEnabled);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_EDT_DEF_SIZE_W), fEnabled);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_ST_DEF_SIZE_W), fEnabled);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_EDT_DEF_SIZE_H), fEnabled);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_ST_DEF_SIZE_H), fEnabled);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_REDUCE_TO_CAPTION), fEnabled);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_CMD_SKNLESS_BCOLOR), fEnabled);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_CMD_SKNLESS_CFONT), fEnabled);
	EnableWindow(GetDlgItem(m_Dialogs.hSkins, IDC_CHK_RANDOM_COLOR), fEnabled);
}

static void LoadAutosaved(void){
	wchar_t				szBuffer[10 * 1024], szSection[10 * 1024], *pw;
	wchar_t				szTemp[MAX_PATH], szPathNote[MAX_PATH], szTempNote[MAX_PATH];

	wcscpy(szTemp, g_NotePaths.DataDir);
	wcscat(szTemp, AUTOSAVE_INDEX_FILE);
	if(PathFileExistsW(szTemp)){
		GetPrivateProfileSectionNamesW(szBuffer, 10 * 1024, szTemp);
		pw = szBuffer;
		if(wcslen(pw) == 0)
			return;
		while(*pw || *(pw - 1)){
			if(*pw && *pw != 31888){
				wcscpy(szTempNote, g_NotePaths.DataDir);
				wcscat(szTempNote, L"~");
				wcscat(szTempNote, pw);
				wcscat(szTempNote, L".no~");
				wcscpy(szPathNote, g_NotePaths.DataDir);
				wcscat(szPathNote, pw);
				wcscat(szPathNote, NOTE_EXTENTION);
				CopyFileW(szTempNote, szPathNote, FALSE);
				SetFileAttributesW(szPathNote, FILE_ATTRIBUTE_NORMAL);
				GetPrivateProfileSectionW(pw, szSection, 10 * 1024, szTemp);
				WritePrivateProfileSectionW(pw, szSection, g_NotePaths.DataFile);
			}
			while(*pw++)
				;
		}
	}
	// wcscpy(szTemp, g_NotePaths.DataDir);
	// wcscat(szTemp, L"~");
	// wcscpy(szTempNote, g_NotePaths.DataDir);
	// if(FileExistsByFullPath(g_NotePaths.DataFile)){
		// GetPrivateProfileSectionNamesW(szBuffer, 10 * 1024, g_NotePaths.DataFile);
			// pw = szBuffer;
			// if(wcslen(pw) == 0)
				// return;
			// while(*pw || *(pw - 1)){
				// if(*pw && *pw != 31888){
					// wcscpy(szPath, szTemp);
					// wcscat(szPath, pw);
					// wcscat(szPath, L".no~");
					// wcscpy(szPathNote, szTempNote);
					// wcscat(szPathNote, pw);
					// wcscat(szPathNote, NOTE_EXTENTION);
					// if(FileExistsByFullPath(szPath)){
						// CopyFileW(szPath, szPathNote, FALSE);
						// SetFileAttributesW(szPathNote, FILE_ATTRIBUTE_NORMAL);
					// }
				// }
				// while(*pw++)
					// ;
			// }
	// }
}

static void GetPrevious(wchar_t * lpFile){
	HWND				hwnd;
	wchar_t				szText[2048], szTemp[2048], szBuffer[4096], *pw;
	PMEMNOTE			pNote;

	GetPrivateProfileSectionNamesW(szBuffer, 4096, lpFile);
	pw = szBuffer;
	if(wcslen(pw) == 0)
		return;
	while(*pw || *(pw - 1)){
		if(*pw && *pw != 31888){
			pNote = AddMemNote();
			LoadNoteProperties(pNote, &g_RTHandles, NULL, FALSE, NULL);
			if(!(hwnd = CreateNote(pNote, g_hInstance, g_hMain, FALSE, NULL))){
				MessageBoxW(hwnd, m_sNoWindowMessage, NULL, MB_OK);
			}
			if(GetPrivateProfileStringW(pw, L"text", NULL, szTemp, 2048, lpFile) > 0){
				ReplaceCharacters(szText, szTemp);
				SetWindowTextW((HWND)GetPropW(hwnd, PH_EDIT), szText);
				SetScheduleForPrevious(hwnd, pw, lpFile);
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_SAVE, 0), 0);
				ShowWindow(hwnd, SW_SHOW);
			}
		}
		while(*pw++)
			;
	}
	if(MessageBoxW(g_hMain, g_Strings.DPrevQuestion, PROG_NAME, MB_YESNO | MB_ICONQUESTION) == IDYES)
		DeleteFileW(lpFile);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetAllNotes
 Created  : Thu May 10 16:59:11 2007
 Modified : Mon May 14 12:27:31 2007

 Synopsys : Loads all saved notes on proghram start
 Input    : Main window handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void GetAllNotes(HWND hwnd){

	wchar_t				szBuffer[4096], *pw;
	PMEMNOTE			pNote;

	if(FileExistsByFullPath(g_NotePaths.DataFile)){
		GetPrivateProfileSectionNamesW(szBuffer, 4096, g_NotePaths.DataFile);
		pw = szBuffer;
		if(wcslen(pw) == 0)
			return;
		while(*pw || *(pw - 1)){
			if(*pw && *pw != 31888){
				pNote = AddMemNote();
				LoadNoteProperties(pNote, &g_RTHandles, pw, FALSE, NULL);
				if(pNote->pData->visible){
					if(!CreateNote(pNote, g_hInstance, hwnd, FALSE, NULL)){
						MessageBoxW(hwnd, m_sNoWindowMessage, NULL, MB_OK);
					}
				}
			}
			while(*pw++)
				;
		}
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CreateDefaultFont
 Created  : Thu May 10 17:00:50 2007
 Modified : Thu May 10 17:00:50 2007

 Synopsys : Creates default notes font, if no special font specified
 Input    : Nothing
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void CreateDefaultFont(PLOGFONTW plfFont, BOOL fBold){
	
	NONCLIENTMETRICSW	nc;
	
	ZeroMemory(&nc, sizeof(nc));
	nc.cbSize = sizeof(nc);
	SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(nc), &nc, 0);
	memcpy(plfFont, &nc.lfMessageFont, sizeof(LOGFONTW));
	if(fBold)
		plfFont->lfWeight = FW_HEAVY;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CreateMenuFont
 Created  : Thu Aug  9 17:38:24 2007
 Modified : Thu Aug  9 17:38:24 2007

 Synopsys : Creates menu font
 Input    : Nothing
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void CreateMenuFont(void){

	NONCLIENTMETRICSW	nc;

	ZeroMemory(&nc, sizeof(nc));
	nc.cbSize = sizeof(nc);
	SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(nc), &nc, 0);
	g_hMenuFont = CreateFontIndirectW(&nc.lfMenuFont);
}

static void Main_OnDestroy(HWND hwnd)
{
	if(m_TrackLogon)
		UnregisterLogonMessage(hwnd);
	PostQuitMessage(0);
}

static void DrawListItem(const DRAWITEMSTRUCT * lpd){

	wchar_t			szBuffer[SendMessageW(lpd->hwndItem, LB_GETTEXTLEN, lpd->itemID, 0)];
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
		SetTextColor(lpd->hDC, GetSysColor(COLOR_WINDOWTEXT));
		FillRect(lpd->hDC, &lpd->rcItem, GetSysColorBrush(COLOR_WINDOW));
	}
	SendMessageW(lpd->hwndItem, LB_GETTEXT, lpd->itemID, (LPARAM)szBuffer);
	//offset rectangle - make room for bitmap
	OffsetRect(&rc, 18, 0);
	//draw bitmap
	DrawTransparentBitmap(g_hBmpMenuNormal, lpd->hDC, 1, rc.top + 1, 16, 16, 24 * 16, 0, CLR_MASK);
	DrawTextExW(lpd->hDC, szBuffer, -1, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER, NULL);
}

static void ShowHideOptionsDialogs(int index){
	HWND		*lpDlg;
	int			size;

	lpDlg = (HWND *)&m_Dialogs;
	size = sizeof(m_Dialogs) / sizeof(HWND);
	for(int i = 0; i < size; i++){
		if(i == index)
			ShowWindow(*lpDlg, SW_SHOW);
		else
			ShowWindow(*lpDlg, SW_HIDE);
		lpDlg++;
	}
}

static LRESULT CALLBACK OptionsDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	NMHDR		* nm;

    switch (uMsg)
    {
		HANDLE_MSG (hwnd, WM_CLOSE, Options_OnClose);
		HANDLE_MSG (hwnd, WM_COMMAND, Options_OnCommand);
		HANDLE_MSG (hwnd, WM_INITDIALOG, Options_OnInitDialog);
		
		case WM_NOTIFY:
			nm = (NMHDR *)lParam;
			if(nm->code == TCN_SELCHANGE){
				ShowHideOptionsDialogs(SendDlgItemMessageW(hwnd, IDC_TAB_OPTIONS, TCM_GETCURSEL, 0, 0));
			}
			// if(nm->code == LVN_ITEMCHANGED){
				// NMLISTVIEW	*nml = (NMLISTVIEW *)lParam;
				// if(nml->uNewState == 3 || nml->uNewState == 2){
					// ShowHideOptionsDialogs(nml->iItem);
				// }
			// }
			// if(nm->code == TCN_SELCHANGE){
				// //show/hide controls on tabs selection
				// switch(SendDlgItemMessageW(hwnd, IDC_TAB_OPTIONS, TCM_GETCURSEL, 0, 0)){
					// case 0:
						// ShowWindow(m_Dialogs.hMisc, SW_HIDE);
						// ShowWindow(m_Dialogs.hSchedule, SW_HIDE);
						// ShowWindow(m_Dialogs.hAppearance, SW_SHOW);
						// break;
					// case 1:
						// ShowWindow(m_Dialogs.hMisc, SW_HIDE);
						// ShowWindow(m_Dialogs.hAppearance, SW_HIDE);
						// ShowWindow(m_Dialogs.hSchedule, SW_SHOW);
						// break;
					// case 2:
						// ShowWindow(m_Dialogs.hAppearance, SW_HIDE);
						// ShowWindow(m_Dialogs.hSchedule, SW_HIDE);
						// ShowWindow(m_Dialogs.hMisc, SW_SHOW);
						// break;
				// }
			// }
			return TRUE;
		default:
			return FALSE;
    }
}

static void Options_OnClose(HWND hwnd)
{
	//destoy image list
	if(m_hImlLeft)
		ImageList_Destroy(m_hImlLeft);

	m_hDlg = 0;
	m_Dialogs.hAppearance = 0;
	m_Dialogs.hSchedule = 0;
	m_Dialogs.hBehavior = 0;
	m_Dialogs.hMisc = 0;

	EndDialog(hwnd, 0);
}

static void Options_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id){
		case IDCANCEL:		//close options dialog
			ClearOnOptionsClose();
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
		case IDOK:			//close options dialog while applying changes
			if(ApplyChanges(hwnd, g_NotePaths.LangFile)){
				ReloadNotes();
			}
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
		case IDAPPLY:		//apply changes
			if(ApplyChanges(hwnd, g_NotePaths.LangFile)){
				ReloadNotes();
			}
			break;
		case IDC_CMD_DEF:
			if(MessageBoxW(hwnd, g_Strings.DefaultSettingsWarning, m_sSound2, MB_YESNO | MB_ICONQUESTION) == IDYES)
				RestoreDefaultSettings();
			break;
	}
}

static BOOL Options_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	m_hDlg = hwnd;
	//set icon
	SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIconW(g_hInstance, MAKEINTRESOURCEW(IDR_ICO_MAIN)));
	CenterWindow(hwnd, FALSE);
	
	//prepare image list for left pane
	PrepareLeftImageList();
	PrepareTabListView(hwnd);

	m_Dialogs.hAppearance = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_APPEARANCE), hwnd, Appearance_DlgProc, 0); 
	m_Dialogs.hSkins = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_SKINS), hwnd, Skins_DlgProc, 0); 
	m_Dialogs.hSchedule = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_SCHEDULE), hwnd, Schedule_DlgProc, 0); 
	m_Dialogs.hBehavior = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_BEHAVIOR), hwnd, Behavior_DlgProc, 0); 
	m_Dialogs.hMisc = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_MISC), hwnd, Misc_DlgProc, 0); 
	m_Dialogs.hDocks = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_DOCKING), hwnd, Docks_DlgProc, 0); 

	//copy note hotkeys
	m_TempNoteHotkeys = (P_HK_TYPE)calloc(NoteMenuCount(), sizeof(HK_TYPE));
	memcpy(m_TempNoteHotkeys, m_NoteHotkeys, sizeof(HK_TYPE) * NoteMenuCount());
	//show skin
	m_TempRHandles.hbSkin = g_RTHandles.hbSkin;
	//copy data
	memcpy(&m_TempRHandles, &g_RTHandles, sizeof(NOTE_RTHANDLES));
	memcpy(&m_TempAppearance, &g_Appearance, sizeof(NOTE_APPEARANCE));
	// memcpy(&gTempFData, &gfData, sizeof(gfData));
	wcscpy(m_sTempSkinFile, g_Appearance.szSkin);
	//copy date/time formats
	wcscpy(m_TempFormats.DateFormat, g_DTFormats.DateFormat);
	wcscpy(m_TempFormats.TimeFormat, g_DTFormats.TimeFormat);
	//apply note settings
	memcpy(&m_TempNoteSettings, &g_NoteSettings, sizeof(NOTE_SETTINGS));
	//hot keys
	memcpy(m_TempHotkeys, m_Hotkeys, sizeof(HK_TYPE) * NELEMS(m_Hotkeys));
	//docking settings
	memcpy(&m_TempDockingSettings, &g_DockSettings, sizeof(PNDOCK));
	memcpy(&m_TempDRTHandles, &g_DRTHandles, sizeof(NOTE_RTHANDLES));
	//save default browser
	wcscpy(m_sTempdefBrowser, g_DefBrowser);

	InitDlgAppearance(m_Dialogs.hAppearance);
	InitDlgSkins(m_Dialogs.hSkins);
	InitDlgSchedule(m_Dialogs.hSchedule);
	InitDlgBehavior(m_Dialogs.hBehavior);
	InitDlgMisc(m_Dialogs.hMisc);
	InitDlgDocks(m_Dialogs.hDocks);
	AddOptionsTabs(hwnd);

	// ApplyMainDlgLanguage(hwnd, g_NotePaths.LangFile);
		
	SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	return FALSE;
}

static void PrepareLeftImageList(void){
	HICON		hIcon;

	m_hImlLeft = ImageList_Create(24, 24, ILC_COLOR32 | ILC_MASK, 0, 1);
	for(int i = IDR_ICO_APPEARANCE; i <= IDR_ICO_MISC; i++){
		hIcon = LoadIconW(g_hInstance, MAKEINTRESOURCEW(i));
		ImageList_AddIcon(m_hImlLeft, hIcon);
	}
}

static void PrepareTabListView(HWND hwnd){
	wchar_t		szFile[MAX_PATH];
	// LVITEMW		lvi;
	// HWND		hList;

	// hList = GetDlgItem(hwnd, IDC_LIST_TAB);
	// ListView_SetIconSpacing(hList, 96, 64);
	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);
	// ListView_SetImageList(hList, m_hImlLeft, LVSIL_NORMAL);
	// ZeroMemory(&lvi, sizeof(lvi));
	// lvi.mask = LVIF_TEXT | LVIF_IMAGE;

	// for(int i = IDR_ICO_APPEARANCE; i <= IDR_ICO_MISC; i++){
		// lvi.pszText = szBuffer;
		// _itow(i, szKey, 10);
		// GetPrivateProfileStringW(S_LEFT_PANEL, szKey, m_PanelDefs[i - IDR_ICO_APPEARANCE], szBuffer, 256, szFile);
		// lvi.iImage = i - IDR_ICO_APPEARANCE;
		// lvi.iItem = i - IDR_ICO_APPEARANCE;
		// SendMessageW(hList, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
	// }
	// SetFocus(hList);
	// ListView_SetItemState(hList, 0, LVIS_SELECTED, LVIS_SELECTED);
}

static BOOL ShowOpenFileDlg(HWND hwnd, wchar_t *pstrFileName, wchar_t *pstrTitleName, 
	wchar_t *pstrFilter, wchar_t *pstrTitle){

	OPENFILENAMEW 		ofn;

	pstrFileName[0] = '\0';
	pstrTitleName[0] = '\0';
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = GetModuleHandle(0);
	ofn.lpstrFilter	= pstrFilter;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.lpstrTitle = pstrTitle;
	ofn.nFilterIndex = 1;
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH; 

	// flags to control appearance of open-file dialog
	ofn.Flags =	OFN_EXPLORER | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;

	return GetOpenFileNameW(&ofn);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SetShowDateTime
 Created  : Thu Aug  9 17:51:51 2007
 Modified : Thu Aug  9 17:51:51 2007

 Synopsys : Shows dates and times on preferences dialog according to 
            current formats
 Input    : hwnd - preferences dialog handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void SetShowDateTime(HWND hwnd){

	SYSTEMTIME	st;
	wchar_t			szDate[128], szTime[128], szDTimePart[128];
	wchar_t			szDFormat[128], szTFormat[128];

	szDTimePart[0] = '\0';
	ParseDateFormat(m_TempFormats.DateFormat, szDFormat, szTFormat);
		
	ZeroMemory(&st, sizeof(st));
	GetLocalTime(&st);
	GetDateFormatW(LOCALE_USER_DEFAULT, 0, &st, szDFormat, szDate, 128);
	GetTimeFormatW(LOCALE_USER_DEFAULT, 0, &st, szTFormat, szDTimePart, 128);
	GetTimeFormatW(LOCALE_USER_DEFAULT, 0, &st, m_TempFormats.TimeFormat, szTime, 128);
	wcscat(szDate, szDTimePart);
	SetDlgItemTextW(hwnd, IDC_DATE_SAMPLE, szDate);
	SetDlgItemTextW(hwnd, IDC_TIME_SAMPLE, szTime);
}

static BOOL CheckDblClickChanges(void){
	if(GetBit(g_NoteSettings.reserved2, SB2_NEW_NOTE) != GetBit(m_TempNoteSettings.reserved2, SB2_NEW_NOTE))
		return TRUE;
	else if(GetBit(g_NoteSettings.reserved2, SB2_CONTROL_PANEL) != GetBit(m_TempNoteSettings.reserved2, SB2_CONTROL_PANEL))
		return TRUE;
	else if(GetBit(g_NoteSettings.reserved2, SB2_PREFS) != GetBit(m_TempNoteSettings.reserved2, SB2_PREFS))
		return TRUE;
	else if(GetBit(g_NoteSettings.reserved2, SB2_SEARCH_NOTES) != GetBit(m_TempNoteSettings.reserved2, SB2_SEARCH_NOTES))
		return TRUE;
	else
		return FALSE;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CheckHotKeysChanges
 Created  : Thu Aug  9 17:52:41 2007
 Modified : Thu Aug  9 17:52:41 2007

 Synopsys : Checks whether hot keys are changed
 Input    : Nothing
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static BOOL CheckHotKeysChanges(P_HK_TYPE lpKeys, P_HK_TYPE lpTempKeys, int count){

	BOOL			result = FALSE;

	for(int i = 0; i < count; i++){
		if(lpKeys[i].fsModifiers != lpTempKeys[i].fsModifiers || lpKeys[i].vk != lpTempKeys[i].vk){
			result = TRUE;
			if(lpKeys[i].fsModifiers != 0 || lpKeys[i].vk != 0)
				//if the hot key was registered before - unregister it
				UnregisterHotKey(g_hMain, lpKeys[i].identifier);
			memcpy(&lpKeys[i], &lpTempKeys[i], sizeof(HK_TYPE));
			if(lpKeys[i].fsModifiers != 0 || lpKeys[i].vk != 0){
				//if the new hot key is not empty - register it
				RegisterHotKey(g_hMain, lpKeys[i].identifier, lpKeys[i].fsModifiers, lpKeys[i].vk);
			}
		}
	}
	return result;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ApplyChanges
 Created  : Mon May 14 12:30:26 2007
 Modified : Mon May 14 12:30:26 2007

 Synopsys : Applies new settings
 Input    : Options dialog handle, skin file name
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static BOOL ApplyChanges(HWND hwnd, wchar_t * lpFile){
	wchar_t 		szColor[32], szKey[12], szMessage[280];
	BOOL			fChanged = FALSE, fSkinBefore, fReload = FALSE, fAutoSVChanged = FALSE;
	BOOL			fW = FALSE, fH = FALSE, fSetDefSize = TRUE, fDockSizeChanged = FALSE, fDockSkinChanged = FALSE;
	BOOL			fDockBGColorChanged = FALSE, fDockCaptionFontChanged = FALSE, fHotkeysChanged = FALSE;
	char			szLink[MAX_PATH];

	//def browser
	if(wcscmp(g_DefBrowser, m_sTempdefBrowser)){
		wcscpy(g_DefBrowser, m_sTempdefBrowser);
		WritePrivateProfileStringW(L"def_browser", L"def_browser", g_DefBrowser, m_sMainINI);
	}
	//hot keys
	if(CheckHotKeysChanges(m_Hotkeys, m_TempHotkeys, NELEMS(m_Hotkeys))){
		SaveHotKeys(S_HOTKEYS, m_Hotkeys, NELEMS(m_Hotkeys));
		fHotkeysChanged = TRUE;
	}
	//note hotkeys
	if(CheckHotKeysChanges(m_NoteHotkeys, m_TempNoteHotkeys, NoteMenuCount())){
		SaveHotKeys(S_NOTE_HOTKEYS, m_NoteHotkeys, NoteMenuCount());
		fHotkeysChanged = TRUE;
	}
	if(fHotkeysChanged)
		CreatePopUp(g_NotePaths.LangFile);
	//sounds
	if(m_TempSound.allowSound != g_Sound.allowSound || m_TempSound.defSound != g_Sound.defSound 
	|| wcscmp(m_TempSound.custSound, g_Sound.custSound) != 0){
		g_Sound.allowSound = m_TempSound.allowSound;
		g_Sound.defSound = m_TempSound.defSound;
		wcscpy(g_Sound.custSound, m_TempSound.custSound);
		memcpy(&m_PrevSound, &g_Sound, sizeof(SOUND_TYPE));
		if(m_TempSound.allowSound)
			WritePrivateProfileStringW(S_SOUND, DSK_ALLOW, __T("1"), m_sMainINI);
		else
			WritePrivateProfileStringW(S_SOUND, DSK_ALLOW, __T("0"), m_sMainINI);

		_itow(m_TempSound.defSound, szKey, 10);
		WritePrivateProfileStringW(S_SOUND, DSK_SOUND_DEF, szKey, m_sMainINI);
		WritePrivateProfileStringW(S_SOUND, DSK_SOUND_CUST, m_TempSound.custSound, m_sMainINI);
	}
	//formats
	if(wcscmp(m_TempFormats.DateFormat, g_DTFormats.DateFormat) != 0){
		if(!wcschr(m_TempFormats.DateFormat, L'd') || !wcschr(m_TempFormats.DateFormat, L'M') || !wcschr(m_TempFormats.DateFormat, L'y') 
		|| !_wcsichr(m_TempFormats.DateFormat, L'h') || !wcschr(m_TempFormats.DateFormat, L'm')){
			MessageBoxW(hwnd, m_sInvalidDate, __T("Invalid format"), MB_OK);
			return FALSE;
		}
		wcscpy(g_DTFormats.DateFormat, m_TempFormats.DateFormat);
		WritePrivateProfileStringW(S_FORMAT, __T("date_format"), m_TempFormats.DateFormat, m_sMainINI);
	}
	if(wcscmp(m_TempFormats.TimeFormat, g_DTFormats.TimeFormat) != 0){
		if(!_wcsichr(m_TempFormats.TimeFormat, L'h') || !wcschr(m_TempFormats.TimeFormat, L'm')){
			MessageBoxW(hwnd, m_sInvalidTime, __T("Invalid format"), MB_OK);
			return FALSE;
		}
		wcscpy(g_DTFormats.TimeFormat, m_TempFormats.TimeFormat);
		WritePrivateProfileStringW(S_FORMAT, __T("time_format"), m_TempFormats.TimeFormat, m_sMainINI);
	}
	SetShowDateTime(m_Dialogs.hSchedule);

	//default skinless size
	if(g_RTHandles.szDef.cx != m_TempRHandles.szDef.cx)
		fW = TRUE;
	if(g_RTHandles.szDef.cy != m_TempRHandles.szDef.cy)
		fH = TRUE;
	if(fW || fH){
		if(m_TempRHandles.szDef.cx < DEF_SIZE_MIN || m_TempRHandles.szDef.cx > DEF_SIZE_MAX 
		|| m_TempRHandles.szDef.cy < DEF_SIZE_MIN || m_TempRHandles.szDef.cy > DEF_SIZE_MAX){
			wcscpy(szMessage, m_sDefSize);
			wcscat(szMessage, L" ");
			_itow(m_TempRHandles.szDef.cx, szKey, 10);
			wcscat(szMessage, szKey);
			wcscat(szMessage, L"x");
			_itow(m_TempRHandles.szDef.cy, szKey, 10);
			wcscat(szMessage, szKey);
			wcscat(szMessage, L"?");
			if(MessageBoxW(hwnd, szMessage, PROG_NAME, MB_YESNO | MB_ICONQUESTION) == IDNO)
				fSetDefSize = FALSE;
		}
	}
	if(fSetDefSize){
		if(fW){
			g_RTHandles.szDef.cx = m_TempRHandles.szDef.cx;
			_itow(g_RTHandles.szDef.cx, szKey, 10);
			WritePrivateProfileStringW(S_DEFSIZE, L"w", szKey, m_sMainINI);
		}
		if(fH){
			g_RTHandles.szDef.cy = m_TempRHandles.szDef.cy;
			_itow(g_RTHandles.szDef.cy, szKey, 10);
			WritePrivateProfileStringW(S_DEFSIZE, L"h", szKey, m_sMainINI);
		}
		if(g_DockSettings.fCustSize == FALSE && m_TempDockingSettings.fCustSize == FALSE){
			fDockSizeChanged = TRUE;
		}
	}
	
	//skin changed
	fSkinBefore = g_RTHandles.hbSkin ? TRUE : FALSE;
	if(g_RTHandles.hbSkin != m_TempRHandles.hbSkin){	
		if(m_TempRHandles.hbSkin){
			fReload = g_RTHandles.crMask == m_TempRHandles.crMask ? FALSE : TRUE;
			wcscpy(g_Appearance.szSkin, m_sTempSkinFile);
			GetSkinProperties(hwnd, &g_RTHandles, g_Appearance.szSkin, TRUE);
			WritePrivateProfileStringW(S_APPEARANCE, S_SKIN, g_Appearance.szSkin, m_sMainINI);
			if(fSkinBefore){
				if(!fReload){
					ApplyNewCommonSkin();
				}
			}
			else{
				fReload = TRUE;
			}
		}
		else{
			g_Appearance.szSkin[0] = '\0';
			g_RTHandles.hbSkin = NULL;
			WritePrivateProfileStringW(S_APPEARANCE, S_SKIN, NULL, m_sMainINI);
			fReload = TRUE;
		}
	}

	//font changed
	if(!IsFontsEqual(&g_Appearance.lfFont, &m_TempAppearance.lfFont)){	
		memcpy(&g_Appearance.lfFont, &m_TempAppearance.lfFont, sizeof(LOGFONTW));	
		DeleteObject(g_RTHandles.hFont);
		memcpy(&g_RTHandles, &m_TempRHandles, sizeof(NOTE_RTHANDLES));
		WritePrivateProfileStructW(S_APPEARANCE, S_FONT, &g_Appearance.lfFont, sizeof(g_Appearance.lfFont), m_sMainINI);
		ApplyNewFont();	
	}

	//font color changed
	if(g_Appearance.crFont != m_TempAppearance.crFont){
		g_Appearance.crFont = m_TempAppearance.crFont;
		_ltow(g_Appearance.crFont, szColor, 10);
		WritePrivateProfileStringW(S_APPEARANCE, S_COLOR, szColor, m_sMainINI);
		ApplyNewTextColor();
	}

	//window caption font changed
	if(!IsFontsEqual(&g_Appearance.lfCaption, &m_TempAppearance.lfCaption)){

		memcpy(&g_Appearance.lfCaption, &m_TempAppearance.lfCaption, sizeof(LOGFONTW));
		DeleteObject(g_RTHandles.hFCaption);
		memcpy(&g_RTHandles, &m_TempRHandles, sizeof(NOTE_RTHANDLES));
		WritePrivateProfileStructW(S_APPEARANCE, S_CAPTION_FONT, &g_Appearance.lfCaption, sizeof(g_Appearance.lfCaption), m_sMainINI);
		ApplyNewCaptionFont();
	}
	
	//window caption font color changed
	if(g_Appearance.crCaption != m_TempAppearance.crCaption){
		g_Appearance.crCaption = m_TempAppearance.crCaption;
		_ltow(g_Appearance.crCaption, szColor, 10);
		WritePrivateProfileStringW(S_APPEARANCE, S_CAPTION_COLOR, szColor, m_sMainINI);
		ApplyNewCaptionColor();
	}

	//window background color changed
	if(g_Appearance.crWindow != m_TempAppearance.crWindow){
		g_Appearance.crWindow = m_TempAppearance.crWindow;
		_ltow(g_Appearance.crWindow, szColor, 10);
		WritePrivateProfileStringW(S_APPEARANCE, S_BCKG_COLOR, szColor, m_sMainINI);
		ApplyNewBackgroundColor();
	}

	//dock skin settings changed
	if(g_DockSettings.fCustSkin != m_TempDockingSettings.fCustSkin){
		fDockSkinChanged = TRUE;
		g_DockSettings.fCustSkin = m_TempDockingSettings.fCustSkin;
		if(g_DockSettings.fCustSkin){
			wcscpy(g_DockSettings.szCustSkin, m_TempDockingSettings.szCustSkin);
			GetDockSkinProperties(m_Dialogs.hDocks, &g_DRTHandles, m_TempDockingSettings.szCustSkin, FALSE);
		}
		//TODO
	}
	else{
		if(g_DockSettings.fCustSkin){
			if(wcscmp(g_DockSettings.szCustSkin, m_TempDockingSettings.szCustSkin) != 0){
				fDockSkinChanged = TRUE;
				wcscpy(g_DockSettings.szCustSkin, m_TempDockingSettings.szCustSkin);
				GetDockSkinProperties(m_Dialogs.hDocks, &g_DRTHandles, m_TempDockingSettings.szCustSkin, TRUE);
			}
		}
	}
	if(fDockSkinChanged){
		WritePrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), m_sMainINI);
		ApplyNewDockSkin();
	}
	//dock skinless size changed
	if(g_DockSettings.fCustSize == FALSE && m_TempDockingSettings.fCustSize == TRUE){
		g_DockSettings.fCustSize = m_TempDockingSettings.fCustSize;
		memcpy(&g_DockSettings.custSize, &m_TempDockingSettings.custSize, sizeof(SIZE));
		fDockSizeChanged = TRUE;	
		WritePrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), m_sMainINI);
	}
	else if(g_DockSettings.fCustSize == TRUE && m_TempDockingSettings.fCustSize == TRUE){
		if(g_DockSettings.custSize.cx != m_TempDockingSettings.custSize.cx || 
			g_DockSettings.custSize.cy != m_TempDockingSettings.custSize.cy){
			memcpy(&g_DockSettings.custSize, &m_TempDockingSettings.custSize, sizeof(SIZE));;
			fDockSizeChanged = TRUE;
			WritePrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), m_sMainINI);
		}
	}
	else if(g_DockSettings.fCustSize == TRUE && m_TempDockingSettings.fCustSize == FALSE){
		g_DockSettings.fCustSize = m_TempDockingSettings.fCustSize;
		fDockSizeChanged = TRUE;
		WritePrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), m_sMainINI);
	}
	//dock distance changed
	if(g_DockSettings.dist != m_TempDockingSettings.dist){
		g_DockSettings.dist = m_TempDockingSettings.dist;
		fDockSizeChanged = TRUE;
		WritePrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), m_sMainINI);
	}
	if(fDockSizeChanged){
		ApplyDockSizeOrderChanged();
	}
	//dock order changed
	if(g_DockSettings.fInvOrder != m_TempDockingSettings.fInvOrder){
		g_DockSettings.fInvOrder = m_TempDockingSettings.fInvOrder;
		WritePrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), m_sMainINI);
		ApplyDockSizeOrderChanged();
	}
	// //docking font
	// if(g_DockSettings.fCustFont != m_TempDockingSettings.fCustFont){
		// g_DockSettings.fCustFont = m_TempDockingSettings.fCustFont;
		// fDockFontChanged = TRUE;
	// }
	// if(g_DRTHandles.hFont != m_TempDRTHandles.hFont){
		// g_DRTHandles.hFont = m_TempDRTHandles.hFont;
		// memcpy(&g_DockSettings.lfFont, &m_TempDockingSettings.lfFont, sizeof(LOGFONTW));
		// fDockFontChanged = TRUE;
	// }
	// if(fDockFontChanged){
		// WritePrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), m_sMainINI);
		// EnumThreadWindows(GetCurrentThreadId(), EnumWindowsProc, APP_DOCK_FONT_CHANGED);
	// }
	//docking caption font
	if(g_DockSettings.fCustCaption != m_TempDockingSettings.fCustCaption){
		g_DockSettings.fCustCaption = m_TempDockingSettings.fCustCaption;
		fDockCaptionFontChanged = TRUE;
	}
	if(g_DRTHandles.hFCaption != m_TempDRTHandles.hFCaption){
		g_DRTHandles.hFCaption = m_TempDRTHandles.hFCaption;
		g_DockSettings.crCaption = m_TempDockingSettings.crCaption;
		memcpy(&g_DockSettings.lfCaption, &m_TempDockingSettings.lfCaption, sizeof(LOGFONTW));
		fDockCaptionFontChanged = TRUE;
	}
	if(g_DockSettings.crCaption != m_TempDockingSettings.crCaption){
		g_DockSettings.crCaption = m_TempDockingSettings.crCaption;
		fDockCaptionFontChanged = TRUE;
	}
	if(fDockCaptionFontChanged){
		WritePrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), m_sMainINI);
		ApplyDockingColors();
	}
	//docking bg color
	if(g_DockSettings.fCustColor != m_TempDockingSettings.fCustColor){
		g_DockSettings.fCustColor = m_TempDockingSettings.fCustColor;
		fDockBGColorChanged = TRUE;
	}
	if(g_DockSettings.crWindow != m_TempDockingSettings.crWindow){
		g_DockSettings.crWindow = m_TempDockingSettings.crWindow;
		fDockBGColorChanged = TRUE;
	}
	if(fDockBGColorChanged){
		WritePrivateProfileStructW(S_DOCKING, DSK_SETTINGS, &g_DockSettings, sizeof(PNDOCK), m_sMainINI);
		ApplyDockingColors();
	}

	fChanged = FALSE;
	if(g_NoteSettings.onStartup != m_TempNoteSettings.onStartup){
		g_NoteSettings.onStartup = m_TempNoteSettings.onStartup;			//store startup settings
		fChanged = TRUE;
		if(m_TempNoteSettings.onStartup){
			//create shortcut in Startup directory
			CreatePNotesShortcut();
		}
		else{
			if(SHGetSpecialFolderPath(NULL, szLink, CSIDL_STARTUP, FALSE)){
				//remove shortcut from Startup directory
				strcat(szLink, LINK_NAME);
				remove(szLink);
			}
		}
	}
	if(g_NoteSettings.reserved2 != m_TempNoteSettings.reserved2){
		//default double click action
		if(CheckDblClickChanges()){
			g_NoteSettings.reserved2 = m_TempNoteSettings.reserved2;
			SetDefaultItemForDblClick();
			fChanged = TRUE;
		}
	}
	//reserved1 changed
	if(g_NoteSettings.reserved1 != m_TempNoteSettings.reserved1){
		//custome fonts
		if(IsBitOn(g_NoteSettings.reserved1, SB1_CUST_FONTS) == TRUE && IsBitOn(m_TempNoteSettings.reserved1, SB1_CUST_FONTS) == FALSE){
			ClearCustomFonts();
			ApplyJustRedraw();
		}
		if(IsBitOn(g_NoteSettings.reserved1, SB1_CUST_FONTS) == FALSE && IsBitOn(m_TempNoteSettings.reserved1, SB1_CUST_FONTS) == TRUE){
			RegisterCustomFonts();
			ApplyJustRedraw();
		}
		g_NoteSettings.reserved1 = m_TempNoteSettings.reserved1;			//store reserved1 settings
		fChanged = TRUE;
	}
	if(g_NoteSettings.checkOnStart != m_TempNoteSettings.checkOnStart){
		g_NoteSettings.checkOnStart = m_TempNoteSettings.checkOnStart;			//store startup settings
		fChanged = TRUE;
	}
	// if(g_NoteSettings.showOnStart != m_TempNoteSettings.showOnStart){
		// g_NoteSettings.showOnStart = m_TempNoteSettings.showOnStart;			//store startup settings
		// fChanged = TRUE;
	// }
	if(g_NoteSettings.saveOnExit != m_TempNoteSettings.saveOnExit){
		g_NoteSettings.saveOnExit = m_TempNoteSettings.saveOnExit;			//store startup settings
		fChanged = TRUE;
	}
	if(g_NoteSettings.confirmSave != m_TempNoteSettings.confirmSave){
		g_NoteSettings.confirmSave = m_TempNoteSettings.confirmSave;			//store startup settings
		fChanged = TRUE;
	}
	if(g_NoteSettings.confirmDelete != m_TempNoteSettings.confirmDelete){
		g_NoteSettings.confirmDelete = m_TempNoteSettings.confirmDelete;			//store deletion settings
		fChanged = TRUE;
	}
	if(g_NoteSettings.newOnTop != m_TempNoteSettings.newOnTop){
		g_NoteSettings.newOnTop = m_TempNoteSettings.newOnTop;			//store new note settings
		fChanged = TRUE;
	}
	if(g_NoteSettings.autoSValue != m_TempNoteSettings.autoSValue){
		fAutoSVChanged = TRUE;
		g_NoteSettings.autoSValue = m_TempNoteSettings.autoSValue;
		fChanged = TRUE;
	}
	if(g_NoteSettings.autoSave != m_TempNoteSettings.autoSave){
		g_NoteSettings.autoSave = m_TempNoteSettings.autoSave;
		if(g_NoteSettings.autoSave){
			m_Timer = SetTimer(g_hMain, 11, g_NoteSettings.autoSValue * 1000 * 60, AutosaveTimerProc);
		}
		else{
			//kill autosave timer
			if(m_Timer)
				KillTimer(g_hMain, m_Timer);
		}
		fChanged = TRUE;
	}
	else{
		if(fAutoSVChanged && g_NoteSettings.autoSave){
			//kill autosave timer
			if(m_Timer)
				KillTimer(g_hMain, m_Timer);
			m_Timer = SetTimer(g_hMain, 11, g_NoteSettings.autoSValue * 1000 * 60, AutosaveTimerProc);
		}
	}
	//visual alert
	if(g_NoteSettings.visualAlert != m_TempNoteSettings.visualAlert){
		g_NoteSettings.visualAlert = m_TempNoteSettings.visualAlert;
		fChanged = TRUE;
	}
	//roll on double click
	if(g_NoteSettings.rollOnDblClick != m_TempNoteSettings.rollOnDblClick){
		g_NoteSettings.rollOnDblClick = m_TempNoteSettings.rollOnDblClick;
		fChanged = TRUE;
	}
	//scrollbar
	if(g_NoteSettings.showScrollbar != m_TempNoteSettings.showScrollbar){
		g_NoteSettings.showScrollbar = m_TempNoteSettings.showScrollbar;
		fReload = TRUE;
		fChanged = TRUE;
	}
	
	if(g_NoteSettings.transAllow != m_TempNoteSettings.transAllow){		//allow transparency changed
		g_NoteSettings.transAllow = m_TempNoteSettings.transAllow;
		g_NoteSettings.transValue = m_TempNoteSettings.transValue;
		if(!g_NoteSettings.transAllow)		//becomes not allowed
			ApplyRestrictTransparency();
		else							//becomes allowed
			ApplyTransparencyValue();
		WritePrivateProfileStructW(S_NOTE, S_SETTINGS, &g_NoteSettings, sizeof(NOTE_SETTINGS), m_sMainINI);
	}
	else if(g_NoteSettings.transValue != m_TempNoteSettings.transValue){
		g_NoteSettings.transValue = m_TempNoteSettings.transValue;
		if(g_NoteSettings.transAllow){
			ApplyTransparencyValue();
		}
		WritePrivateProfileStructW(S_NOTE, S_SETTINGS, &g_NoteSettings, sizeof(NOTE_SETTINGS), m_sMainINI);
	}
	else if(fChanged)		//if only startup settings changed
		WritePrivateProfileStructW(S_NOTE, S_SETTINGS, &g_NoteSettings, sizeof(NOTE_SETTINGS), m_sMainINI);

	if(fReload)
		GetSkinProperties(hwnd, &g_RTHandles, g_Appearance.szSkin, TRUE);

	RedrawWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_DOCK_SKIN), NULL, NULL, RDW_INVALIDATE);
	RedrawWindow(GetDlgItem(m_Dialogs.hDocks, IDC_ST_DOCK_SKINLESS), NULL, NULL, RDW_INVALIDATE);
	RedrawWindow(GetDlgItem(m_Dialogs.hSkins, IDC_ST_BMP), NULL, NULL, RDW_INVALIDATE);
	return fReload;
}

static BOOL CALLBACK NoteHotkeysProc(HWND hwnd, LPARAM lParam){
	wchar_t				szClass[256];

	GetClassNameW(hwnd, szClass, 256);
	if(wcscmp(szClass, NWC_SKINNABLE_CLASS) == 0){
		if(GetForegroundWindow() == hwnd){
			SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(lParam, 0), 0);
		}
	}
	return TRUE;
}

static BOOL CALLBACK DockAllProc(HWND hwnd, LPARAM lParam){
	wchar_t				szClass[256];
	P_NOTE_DATA			pD;
	dock_type			dockType;

	GetClassNameW(hwnd, szClass, 256);
	if(wcscmp(szClass, NWC_SKINNABLE_CLASS) == 0){
		if(IsWindowVisible(hwnd)){
			pD = NoteData(hwnd);
			dockType = HIWORD(lParam);
			if(DockType(pD->dockData) != dockType){
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(LOWORD(lParam), 0), 0);
			}
		}
	}

	return TRUE;
}

static void ShowAllNotes(void){
	PMEMNOTE	pNote = MemoryNotes();

	while(pNote){
		if(pNote->pData->idGroup > GROUP_RECYCLE){
			ShowNote(pNote);
		}
		pNote = pNote->next;
	}
	//if(g_hCPDialog){
		////SendMessageW(g_hCPDialog, CHM_CTRL_UPD, 0, (LPARAM)pNote);
		//SendMessageW(g_hCPDialog, CHM_RELOAD, 0, 0);
	//}
}

static void AddNotifyIcon(void){
	//add tray notify icon and set it up

	m_InTray = TRUE;
	if(m_NewDLLVersion)
		m_nData.cbSize = sizeof(NOTIFYICONDATAW);
	else
		m_nData.cbSize = NOTIFYICONDATAW_V1_SIZE;

	m_nData.hWnd = g_hMain;
	m_nData.uID = IDI_TRAY;
	m_nData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nData.uCallbackMessage = WM_SHELLNOTIFY;
	m_nData.hIcon = LoadIconW(g_hInstance, MAKEINTRESOURCEW(IDR_ICO_MAIN));
	wcscpy(m_nData.szTip, g_NotePaths.ProgName);
	Shell_NotifyIconW(NIM_ADD, &m_nData);
}

static void AddOptionsTabs(HWND hwnd){
	TCITEMW		tci;
	wchar_t 	szFile[MAX_PATH];

	HWND		hTab;

	hTab = GetDlgItem(hwnd, IDC_TAB_OPTIONS);
	TabCtrl_SetImageList(hTab, m_hImlLeft);

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);

	ZeroMemory(&tci, sizeof(tci));
	tci.mask = TCIF_TEXT | TCIF_IMAGE;
	for(int i = IDR_ICO_APPEARANCE; i <= IDR_ICO_MISC; i++){
		tci.iImage = i - IDR_ICO_APPEARANCE;
		SendDlgItemMessageW(hwnd, IDC_TAB_OPTIONS, TCM_INSERTITEMW, i - IDR_ICO_APPEARANCE, (LPARAM)&tci);
	}
	ApplyMainDlgLanguage(hwnd, g_NotePaths.LangFile);
	SetMainDlgSizes(hwnd);
}

static void MoveOptionsDialogs(int x, int y, int cx, int cy){
	HWND		*lpDlg;
	int			size;

	size = sizeof(m_Dialogs) / sizeof(HWND);
	lpDlg = (HWND *)&m_Dialogs;
	for(int i = 0; i < size - 1 ; i++){
		lpDlg++;
	}

	for(int i = size; i > 0; i--){
		MoveWindow(*lpDlg, x, y, cx, cy, TRUE);
		lpDlg--;
	}
}

static void SetMainDlgSizes(HWND hwnd){
	
	RECT		rc, rcd, rct, rcw;
	int			cx, cy;
	// HWND		hList;
	HWND		hTab;
	int			count, offset = 0;

	hTab = GetDlgItem(hwnd, IDC_TAB_OPTIONS);

	GetWindowRect(GetDlgItem(hwnd, IDC_CMD_DEF), &rcd);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rcd, 2);

	GetClientRect(hwnd, &rc);
	cx = rc.right - rc.left;
	cy = (rc.bottom - rc.top) - (rcd.bottom - rcd.top) - 16;
	MoveWindow(hTab, 0, 0, cx, cy, TRUE);

	count = TabCtrl_GetRowCount(hTab);
	if(count > 2){
		//increase dialog height
		TabCtrl_GetItemRect(hTab, 0, &rct);
		GetWindowRect(hwnd, &rcw);
		offset = (rct.bottom - rct.top) * (count - 2);
		rcw.bottom += offset;
		cy += offset;
		MoveWindow(hwnd, rcw.left, rcw.top, rcw.right - rcw.left, rcw.bottom - rcw.top, TRUE);
		MoveWindow(hTab, 0, 0, cx, cy, TRUE);
		MoveButtonOnOptions(hwnd, IDC_CMD_DEF, offset);
		MoveButtonOnOptions(hwnd, IDOK, offset);
		MoveButtonOnOptions(hwnd, IDCANCEL, offset);
		MoveButtonOnOptions(hwnd, IDAPPLY, offset);
	}
	

	SendMessageW(hTab, TCM_ADJUSTRECT, FALSE, (LPARAM)&rc);

	rc.bottom -= ((rcd.bottom - rcd.top) + 16);
	MoveWindow(GetDlgItem(hwnd, IDC_ST_OPTIONS), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top + offset, TRUE);
	MoveOptionsDialogs(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top + offset);
}

static void MoveButtonOnOptions(HWND hwnd, int id, int offset){
	HWND	hButton = GetDlgItem(hwnd, id);
	RECT	rc;

	GetWindowRect(hButton, &rc);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
	MoveWindow(hButton, rc.left, rc.top + offset, rc.right - rc.left, rc.bottom - rc.top, TRUE);
}

static void ApplyHotDialogLanguage(HWND hwnd, wchar_t * lpFile){
	wchar_t 	szBuffer[256], szFile[MAX_PATH], szKey[12];

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, lpFile);
	_itow(IDC_GRP_HOTKEYS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, __T("Hot keys"), szBuffer, 256, szFile);
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDOK, szFile, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, szFile, DS_CANCEL);
	SetDlgCtlText(hwnd, IDC_ST_CHNGE_HK, szFile, __T("Type the keys to be used as hot keys for selected command"));
}

static void ApplyMainDlgLanguage(HWND hwnd, wchar_t * lpFile){
	
	wchar_t 	szBuffer[256], szFile[MAX_PATH], szKey[16];
	TCITEMW		tci;
	int			index, selMain, selNote;
	HWND		hTab;

	hTab = GetDlgItem(hwnd, IDC_TAB_OPTIONS);

	LockWindowUpdate(hwnd);
	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, lpFile);
	//set dialog and controls text
	GetPrivateProfileStringW(S_OPTIONS, __T("1001"), DS_CAPTION, szBuffer, 256, szFile);
	SetWindowTextW(hwnd, szBuffer);

	ZeroMemory(&tci, sizeof(tci));
	tci.mask = TCIF_TEXT | TCIF_IMAGE;
	tci.pszText = szBuffer;
	tci.cchTextMax = 256;
	for(int i = IDR_ICO_APPEARANCE; i <= IDR_ICO_MISC; i++){
		_itow(i, szKey, 10);
		tci.iImage = i - IDR_ICO_APPEARANCE;
		GetPrivateProfileStringW(S_LEFT_PANEL, szKey, m_PanelDefs[i - IDR_ICO_APPEARANCE], szBuffer, 256, szFile);
		SendMessageW(hTab, TCM_SETITEMW, i - IDR_ICO_APPEARANCE, (LPARAM)&tci);
	}
	// GetPrivateProfileStringW(__T("tabs_options"), __T("0"), __T("Appearance"), szBuffer, 256, szFile);
	// SendDlgItemMessageW(hwnd, IDC_TAB_OPTIONS, TCM_SETITEMW, 0, (LPARAM)&tci);
	// GetPrivateProfileStringW(__T("tabs_options"), __T("1"), __T("Schedule"), szBuffer, 256, szFile);
	// SendDlgItemMessageW(hwnd, IDC_TAB_OPTIONS, TCM_SETITEMW, 1, (LPARAM)&tci);
	// GetPrivateProfileStringW(__T("tabs_options"), __T("2"), __T("Misc"), szBuffer, 256, szFile);
	// SendDlgItemMessageW(hwnd, IDC_TAB_OPTIONS, TCM_SETITEMW, 2, (LPARAM)&tci);
	//main dialog
	SetDlgCtlText(hwnd, IDOK, szFile, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, szFile, DS_CANCEL);
	SetDlgCtlText(hwnd, IDAPPLY, szFile, DS_APPLY);
	SetDlgCtlText(hwnd, IDC_CMD_DEF, szFile, DS_O_DEFAULT);

	//appearance dialog
	SetGroupText(m_Dialogs.hAppearance, IDC_GRP_APPEARANCE, IDR_ICO_APPEARANCE, szFile, m_PanelDefs[0]);
	SetDlgCtlText(m_Dialogs.hAppearance, IDC_CHK_TRANS, szFile, DS_TRANS);
	SetDlgCtlText(m_Dialogs.hAppearance, IDC_TRANS_WARNING, szFile, DS_TR_WARNING);
	SetDlgCtlText(m_Dialogs.hAppearance, IDC_CHK_SCROLLBAR, szFile, DS_SHOW_SCROLLBAR);
	SetDlgCtlText(m_Dialogs.hAppearance, IDC_GRP_TRANS, szFile, __T("Transparency"));
	SetDlgCtlText(m_Dialogs.hAppearance, IDC_USE_CUST_FONTS, szFile, __T("Use custom fonts"));
	SetDlgCtlText(m_Dialogs.hAppearance, IDC_HIDE_TOOLBAR, szFile, __T("Hide note's toolbar"));

	//skins dialog
	SetGroupText(m_Dialogs.hSkins, IDC_GRP_SKIN, IDR_ICO_SKINS, szFile, m_PanelDefs[1]);
	SetDlgCtlText(m_Dialogs.hSkins, IDC_GRP_DEF_SIZE, szFile, __T("Default size"));
	SetDlgCtlText(m_Dialogs.hSkins, IDC_CMD_FONT, szFile, __T("Change font"));
	SetDlgCtlText(m_Dialogs.hSkins, IDC_ST_DEF_SIZE_W, szFile, __T("Width"));
	SetDlgCtlText(m_Dialogs.hSkins, IDC_ST_DEF_SIZE_H, szFile, __T("Height"));
	SetDlgCtlText(m_Dialogs.hSkins, IDC_LBL_MORE_SKINS, szFile, __T("More skins"));
	SetDlgCtlText(m_Dialogs.hSkins, IDC_REDUCE_TO_CAPTION, szFile, __T("Roll/unroll note by double click on caption"));
	SetDlgCtlText(m_Dialogs.hSkins, IDC_GRP_SKINLESS_PROPS, szFile, __T("Skinless notes"));
	SetDlgCtlText(m_Dialogs.hSkins, IDC_CMD_SKNLESS_BCOLOR, szFile, __T("Background color"));
	SetDlgCtlText(m_Dialogs.hSkins, IDC_CMD_SKNLESS_CFONT, szFile, __T("Caption font"));
	SetDlgCtlText(m_Dialogs.hSkins, IDC_CHK_RANDOM_COLOR, szFile, __T("Use random background color"));
	SetDlgCtlText(m_Dialogs.hSkins, IDC_CHK_INVERT_COLOR, szFile, __T("Invert text color"));

	//schedule dialog
	SetGroupText(m_Dialogs.hSchedule, IDC_GRP_SCHEDULE, IDR_ICO_SCHEDULE, szFile, m_PanelDefs[2]);
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_GRP_SOUND, szFile, __T("Sound alert"));
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_CHK_ALLOW_SOUND, szFile, __T("Allow sound alert"));
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_CMD_ADD_SOUND, szFile, __T("Add"));
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_CMD_DEL_SOUND, szFile, __T("Remove"));
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_CMD_LISTEN, szFile, __T("Listen"));
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_ST_DATE_FMTS, szFile, __T("Date sample"));
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_ST_TIME_FMTS, szFile, __T("Time sample"));
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_GRP_FORMATS, szFile, __T("Date/time formats"));
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_SINGLE_ALARM, szFile, __T("For single alarm"));
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_REPEATED_ALARM, szFile, __T("For repeated alarm"));
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_VISUAL_NOTIFY, szFile, __T("Enable visual notification"));
	SetDlgCtlText(m_Dialogs.hSchedule, IDC_CHK_OVERDUE, szFile, L"Track overdue reminders");

	//docking dialog
	SetGroupText(m_Dialogs.hDocks, IDC_GRP_DOCKING, IDR_ICO_DOCK, szFile, m_PanelDefs[3]);
	SetDlgCtlText(m_Dialogs.hDocks, IDC_GRP_DOCK_SKINS, szFile, __T("Skinnable notes"));
	SetDlgCtlText(m_Dialogs.hDocks, IDC_GRP_DOCK_SKINLESS, szFile, __T("Skinless notes"));
	SetDlgCtlText(m_Dialogs.hDocks, IDC_OPT_DOCK_DEF_SKIN, szFile, __T("Common skin"));
	SetDlgCtlText(m_Dialogs.hDocks, IDC_OPT_DOCK_RND_SKIN, szFile, __T("Custom skin"));
	// SetDlgCtlText(m_Dialogs.hDocks, IDC_OPT_DOCK_DEF_SIZE, szFile, __T("Common size"));
	SetDlgCtlText(m_Dialogs.hDocks, IDC_OPT_DOCK_RND_SIZE, szFile, __T("Custom size"));
	SetDlgCtlText(m_Dialogs.hDocks, IDC_ST_DEF_SIZE_W, szFile, __T("Width"));
	SetDlgCtlText(m_Dialogs.hDocks, IDC_ST_DEF_SIZE_H, szFile, __T("Height"));
	SetDlgCtlText(m_Dialogs.hDocks, IDC_DOCK_ST_DISTANCE, szFile, __T("Distance between notes"));
	SetDlgCtlText(m_Dialogs.hDocks, IDC_INV_DOCK_ORDER, szFile, __T("Inverse docking order"));
	SetDlgCtlText(m_Dialogs.hDocks, IDC_CMD_SKNLESS_BCOLOR, szFile, __T("Background color"));
	SetDlgCtlText(m_Dialogs.hDocks, IDC_CMD_SKNLESS_CFONT, szFile, __T("Caption font"));
	SetDlgCtlText(m_Dialogs.hDocks, IDC_GRP_DOCK_SZ, szFile, __T("Size"));
	SetDlgCtlText(m_Dialogs.hDocks, IDC_GRP_DOCK_FC, szFile, __T("Color and caption font"));
	SetDlgCtlText(m_Dialogs.hDocks, IDC_CHK_DOCK_C, szFile, __T("Custom back color"));
	SetDlgCtlText(m_Dialogs.hDocks, IDC_CHK_DOCK_F, szFile, __T("Custom caption font"));
	// SetDlgCtlText(m_Dialogs.hDocks, IDC_CHK_DOCK_CUST_F, szFile, __T("Custom note font"));
	// SetDlgCtlText(m_Dialogs.hDocks, IDC_CMD_DOCK_FONT, szFile, __T("Choose font"));

	//misc dialog
	SetGroupText(m_Dialogs.hMisc, IDC_GRP_MISC, IDR_ICO_MISC, szFile, m_PanelDefs[5]);
	SetDlgCtlText(m_Dialogs.hMisc, IDC_GRP_UPDATE, szFile, __T("New version"));
	SetDlgCtlText(m_Dialogs.hMisc, IDC_CHK_NEW_V, szFile, __T("Check for new version on program start"));
	SetDlgCtlText(m_Dialogs.hMisc, IDC_CMD_NEW_V, szFile, __T("Check now"));
	SetDlgCtlText(m_Dialogs.hMisc, IDC_CMD_PREV_VERSION, szFile, __T("Get notes from previous versions"));
	// SetDlgCtlText(m_Dialogs.hMisc, IDC_CHK_SHOW, szFile, DS_SHOW_START);
	SetDlgCtlText(m_Dialogs.hMisc, IDC_CHK_AUTORUN, szFile, DS_ON_STARTUP);
	SetDlgCtlText(m_Dialogs.hMisc, IDC_GRP_START_OPT, szFile, __T("Startup options"));
	SetDlgCtlText(m_Dialogs.hMisc, IDC_ST_DEF_BROWSER, szFile, __T("Specify your preferred browser (leave the field blank in order to use default system browser)."));
	SetDlgCtlText(m_Dialogs.hMisc, IDC_CHK_SHOW_CP, szFile, __T("Show Control Panel on program start"));

	//Behavior dialog
	SetGroupText(m_Dialogs.hBehavior, IDC_GRP_BEHAVIOR, IDR_ICO_BEHAVIOR, szFile, m_PanelDefs[4]);
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_ALWAYS_SAVE, szFile, DS_SAVE_EXIT);
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_ASK_B_SAVE, szFile, DS_SAVE_CONF);
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_CHK_DEL_CONFIRM, szFile, DS_CONF_DELETE);
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_GRP_HOTKEYS, szFile, __T("Hot keys"));
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_CMD_HOTKEY, szFile, __T("Add/change hot keys"));
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_CMD_DEL_HOT, szFile, __T("Remove"));
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_GRP_AUTOSAVE, szFile, __T("Autosave options"));
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_CHK_AUTOSAVE, szFile, __T("Autosave every:"));
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_ST_AUTOSAVE, szFile, __T("minute(s)"));
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_GRP_SAVE_OPT, szFile, __T("Saving/Deletion"));
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_CHK_NEW_ONTOP, szFile, __T("New note always on top"));
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_CHK_REL_POSITION, szFile, L"Relational notes positioning");
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_GRP_DBL_CLICK, szFile, L"Default action after double click on system tray icon");
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_OPT_MAIN_MENU, szFile, L"Main menu");
	SetDlgCtlText(m_Dialogs.hBehavior, IDC_OPT_SINGLE_NOTE, szFile, L"Note");
	//add menu commands for double click action
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_DBL_CLICK, CB_RESETCONTENT, 0, 0);
	for(int i = 0; i < NELEMS(m_DblCommands); i++){
		index = SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_DBL_CLICK, CB_ADDSTRING, 0, (LPARAM)m_DblCommands[i].szCommand);
		SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_DBL_CLICK, CB_SETITEMDATA, index, index);
	}
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_DBL_CLICK, CB_SETCURSEL, GetIndexOfDblClick(), 0);
	//add note commands to combo box
	selNote = SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_SINGLE_NOTE, CB_GETCURSEL, 0, 0);
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_SINGLE_NOTE, CB_RESETCONTENT, 0, 0);
	for(int i = 0; i < NoteMenuCount(); i++){
		switch(m_TempNoteHotkeys[i].id){
			case IDM_SAVE:
			case IDM_PRINT_NOTE:
			case IDM_DOCK:
			case IDM_NOTE_GROUPS:
				continue;
		}
		if(wcslen(m_NoteCommands[i].szCommand) > 0){
			index = SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_SINGLE_NOTE, CB_ADDSTRING, 0, (LPARAM)m_NoteCommands[i].szCommand);
			SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_SINGLE_NOTE, CB_SETITEMDATA, index, m_TempNoteHotkeys[i].id);
		}
	}
	if(selNote < 0)
		selNote = 0;
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_SINGLE_NOTE, CB_SETCURSEL, selNote, 0);
	//add menu commands to combo box (behavior dialog)
	selMain = SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_COMMANDS, CB_GETCURSEL, 0, 0);
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_COMMANDS, CB_RESETCONTENT, 0, 0);
	for(int i = 0; i < NELEMS(m_Commands); i++){
		switch(m_TempHotkeys[i].id){
			case IDM_LANGUAGES:
			case IDM_SHOW_GROUPS:
			case IDM_HIDE_GROUPS:
			case IDM_DOCK_ALL:
			case IDM_FAVORITES:
			case IDM_SEARCH_SUBMENU:
				continue;
		}
		if(wcslen(m_Commands[i].szCommand) > 0){
			index = SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_COMMANDS, CB_ADDSTRING, 0, (LPARAM)m_Commands[i].szCommand);
			SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_COMMANDS, CB_SETITEMDATA, index, m_TempHotkeys[i].id);
		}
	}
	if(selMain < 0)
		selMain = 0;
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_COMMANDS, CB_SETCURSEL, selMain, 0);
	//check what part should be shown in static window
	if(IsDlgButtonChecked(m_Dialogs.hBehavior, IDC_OPT_MAIN_MENU) == BST_CHECKED){
		SetDlgItemTextW(m_Dialogs.hBehavior, IDC_ST_HOTKEY, m_TempHotkeys[selMain].szKey);
		if(!wcslen(m_TempHotkeys[selMain].szKey))
			EnableWindow(GetDlgItem(m_Dialogs.hBehavior, IDC_CMD_DEL_HOT), FALSE);
	}
	else{
		SetDlgItemTextW(m_Dialogs.hBehavior, IDC_ST_HOTKEY, m_TempNoteHotkeys[selNote].szKey);
		if(!wcslen(m_TempNoteHotkeys[selNote].szKey))
			EnableWindow(GetDlgItem(m_Dialogs.hBehavior, IDC_CMD_DEL_HOT), FALSE);
	}
	LockWindowUpdate(NULL);
}

static void CleanUp(void){
	//write exit state parameter ("1" on start, "0" on succesfull end)
	WritePrivateProfileStringW(S_EXITFLAG, DSK_CURRENT_STATE, L"0", m_sMainINI);
	//delete autosaved files
	DeleteAutosaved();
	//destroy temp skin bitmap
	if(m_TempRHandles.hbSkin && m_TempRHandles.hbSkin != g_RTHandles.hbSkin)
		DeleteObject(m_TempRHandles.hbSkin);
	//destroy dock windows bitmap
	if(wcscmp(g_Appearance.szSkin, g_DockSettings.szCustSkin) != 0){
		DeleteObject(g_DRTHandles.hbSkin);
		DeleteObject(g_DRTHandles.hbSys);
		DeleteObject(g_DRTHandles.hbBack);
		DeleteObject(g_DRTHandles.hbPattern);
		DeleteObject(g_DRTHandles.hbDelHide);
		DeleteObject(g_DRTHandles.hbCommand);
	}
	if(g_DockSettings.fCustCaption){
		DeleteObject(g_DRTHandles.hFCaption);
	}
	//free repeat parts
	FreeRepParts();
	//free weekdays
	FreeWeekdays();
	if(g_DockSettings.fCustColor){
		DeleteObject(g_DRTHandles.hbBackground);
	}
	//remove custom fonts
	ClearCustomFonts();
	//destroy skin bitmap
	if(g_RTHandles.hbSkin)
		DeleteObject(g_RTHandles.hbSkin);
	//destroy sys menu bitmap
	if(g_RTHandles.hbSys)
		DeleteObject(g_RTHandles.hbSys);
	//delete pattern brush
	if(g_RTHandles.hbPattern)
		DeleteObject(g_RTHandles.hbPattern);
	//destroy delhide bitmap
	if(g_RTHandles.hbDelHide)
		DeleteObject(g_RTHandles.hbDelHide);
	//destroy commands bitmap
	if(g_RTHandles.hbCommand)
		DeleteObject(g_RTHandles.hbCommand);
	//free favorites menu
	FreeFavoritesMenu();
		//free menus list
	if(m_PMenus){
		free(m_PMenus);
	}
	//free note commands
	if(m_NoteCommands){
		free(m_NoteCommands);
		m_NoteCommands = NULL;
	}
	//unregister note hot keys
	UnregisterHKeys(g_hMain, m_NoteHotkeys, NoteMenuCount());
	//save note hot keys
	SaveHotKeys(S_NOTE_HOTKEYS, m_NoteHotkeys, NoteMenuCount());
	//free hotkeys
	if(m_NoteHotkeys)
		free(m_NoteHotkeys);
	if(m_TempNoteHotkeys)
		free(m_TempNoteHotkeys);
	//destroy popup menu
	if(m_hMenu){
		FreeMenus(m_hPopUp);
		DestroyMenu(m_hMenu);
	}
	if(g_hNoteMainMenu){
		FreeMenus(g_hNotePopUp);
		DestroyMenu(g_hNoteMainMenu);
	}
	if(g_hEditMainMenu){
		FreeMenus(g_hEditPopUp);
		DestroyMenu(g_hEditMainMenu);
	}
	if(g_hDropMenu){
		FreeMenus(g_hDropPopUp);
		DestroyMenu(g_hDropMenu);
	}
	//destroy font
	if(g_RTHandles.hFont)
		DeleteObject(g_RTHandles.hFont);
	if(g_RTHandles.hFCaption)
		DeleteObject(g_RTHandles.hFCaption);
	//destroy menu bitmaps
	if(g_hBmpMenuNormal)
		DeleteObject(g_hBmpMenuNormal);
	if(g_hBmpMenuGray)
		DeleteObject(g_hBmpMenuGray);
	if(g_hMenuFont)
		DeleteObject(g_hMenuFont);
	//delete tray icon
	if(m_InTray){
		m_InTray = FALSE;
		Shell_NotifyIconW(NIM_DELETE, &m_nData);
	}
	//kill timer
	if(m_Timer)
		KillTimer(g_hMain, m_Timer);
	//free empty notes memory
	if(g_Empties.pPoint)
		free(g_Empties.pPoint);
	if(g_Empties.pDockData)
		free(g_Empties.pDockData);
	//remove groups
	FreeGroups();
	//clean docking lists
	FreeDockHeaders();
	//free memory notes list
	FreeMemNotes();
	//delete cursors
	if(g_CurV)
		DestroyCursor(g_CurV);
	if(g_CurH)
		DestroyCursor(g_CurH);
	//clean registry
	CleanRegMUICache(PROG_EXE_NAME);
	CleanRegMRU(PROG_EXE_NAME);
	CleanRegOpenWithList(PROG_EXE_NAME);
	CleanRegOpenSaveMRU(PROG_EXE_NAME);
	// CleanPrefetch(L"PNotes.exe");
}

static void PrepareMessages(wchar_t * lpFile){

	//prepare messages
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("no_note_created"), __T("Unable to create note window"), m_sNoWindowMessage);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("delete_confirm"), __T("Do you really want to delete this note?"), g_Strings.DMessage);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("delete_confirm_multiple"), __T("Do you really want to delete selected item(s)?"), g_Strings.DMessageMultiple);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("delete_bin"), __T("Do you want to send this note to Recycle Bin?"), g_Strings.DMessageBin);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("delete_complete"), __T("Do you want to completely delete this note?"), g_Strings.DMessageComplete);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("empty_bin"), __T("Empty recycle bin (the operation cannot be undone)?"), g_Strings.EmptyBinMessage);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("delete_caption"), __T("Deletion"), g_Strings.DCaption);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("too_many_files"), __T("Only one file can be dropped onto the note."), g_Strings.TooManyFiles);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("save_caption"), __T("Saving"), g_Strings.SCaption);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("save_note"), __T("Note has been changed"), g_Strings.SNote);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("save_question"), __T("Do you want to save it?"), g_Strings.SQuestion);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("delete_previous"), __T("Would you like to delete the database file of previous version?"), g_Strings.DPrevQuestion);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("invalid_date"), __T("The date format is invalid. It should include day, month, year, hour and minute."), m_sInvalidDate);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("invalid_time"), __T("The time format is invalid. It should include hour and minute."), m_sInvalidTime);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("hk_1"), __T("The hot keys combination"), m_sHK1);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("hk_2"), __T("is already registered. Please, choose another one."), m_sHK2);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("hk_3"), __T("Delete current hot keys?"), m_sHK3);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("no_success"), __T("Program did not finish correctly last time. Would you like to load autosaved notes?"), m_sUnsuccessfull);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("sound_1"), __T("Add sound"), m_sSound1);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("sound_2"), __T("Confirmation"), m_sSound2);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("sound_3"), __T("The file already exists in your 'sound' directory. Copy anyway?"), m_sSound3);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("sound_4"), __T("The sound file not found. Reset to default."), m_sSound4);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("sound_5"), __T("Delete selected sound?"), m_sSound5);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("def_size"), __T("Are you sure you want to set the default size of note to"), m_sDefSize);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("delete_group_0"), __T("Group deletion"), g_Strings.DeleteGroup0);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("delete_group_1"), __T("CAUTION! The selected group, including all its notes, is about to be deleted."), g_Strings.DeleteGroup1);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("delete_group_2"), __T("All its subgroups and their notes will be deleted as well."), g_Strings.DeleteGroup2);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("delete_group_3"), __T("The operation cannot be rolled back. Continue?"), g_Strings.DeleteGroup3);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("search_complete"), __T("Search complete"), g_Strings.SearchComplete);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("replace_complete"), __T("Replace complete"), g_Strings.ReplaceComplete);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("matches_found"), __T("Matches found:"), g_Strings.MatchesFound);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("no_matches"), __T("No matches found:"), g_Strings.NoMatchesFound);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("check_for_update"), __T("Checking for update"), g_Strings.CheckUpdate);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("new_version_1"), __T("The new version of PNotes is available"), g_Strings.NewVersion1);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("new_version_2"), __T("Click 'OK' in order to proceed to download page."), g_Strings.NewVersion2);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("same_version"), __T("You are using the latest version of PNotes."), g_Strings.SameVersion);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("font_sample"), __T("The quick brown fox jumps over the lazy dog"), g_Strings.FontSample);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("save_before_send"), __T("The note has to be saved before sending as attachment. Save now?"), g_Strings.SaveBeforeSend);
	GetPNotesMessage(g_NotePaths.LangDir, lpFile, __T("default_warning"), __T("You are about to reset ALL program settings to their default values. Continue?"), g_Strings.DefaultSettingsWarning);

	PrepareDateFormatsMessages();
}

static void PrepareDateFormatsMessages(void){
	wchar_t			szFile[MAX_PATH], szBuffer[256];
	wchar_t			szKeys[4096], *pw;

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);
	g_Strings.DFormats[0] = '\0';
	g_Strings.TFormats[0] = '\0';
	GetPrivateProfileStringW(S_CAPTIONS, L"date_formats", L"Possible date formats", g_Strings.DFCaption, 128, szFile);
	GetPrivateProfileStringW(S_CAPTIONS, L"time_formats", L"Possible time formats", g_Strings.TFCaption, 128, szFile);
	//date formats
	GetPrivateProfileStringW(S_DATE_FORMAT_CHARS, NULL, NULL, szKeys, 4096, szFile);
	if(wcslen(szKeys) == 0) {
		wcscpy(g_Strings.DFormats, DATE_FORMAT_MASKS);
	}
	else{
		pw = szKeys;
		if(*pw){
			while(*pw || *(pw - 1)){
				if(*pw && *pw != 31888){
					GetPrivateProfileStringW(S_DATE_FORMAT_CHARS, pw, L"\n", szBuffer, 256, szFile);
					wcscat(g_Strings.DFormats, pw);
					wcscat(g_Strings.DFormats, L"\t");
					wcscat(g_Strings.DFormats, szBuffer);
					wcscat(g_Strings.DFormats, L"\n");
				}
				while(*pw++)
					;
			}
		}
	}
	//time formats
	GetPrivateProfileStringW(S_TIME_FORMAT_CHARS_H12, NULL, NULL, szKeys, 4096, szFile);
	if(wcslen(szKeys) == 0) {
		wcscpy(g_Strings.TFormats, TIME_FORMAT_MASKS);
		return;
	}
	else{
		pw = szKeys;
		if(*pw){
			while(*pw || *(pw - 1)){
				if(*pw && *pw != 31888){
					GetPrivateProfileStringW(S_TIME_FORMAT_CHARS_H12, pw, L"\n", szBuffer, 256, szFile);
					wcscat(g_Strings.TFormats, pw);
					wcscat(g_Strings.TFormats, L"\t");
					wcscat(g_Strings.TFormats, szBuffer);
					wcscat(g_Strings.TFormats, L"\n");
				}
				while(*pw++)
					;
			}
		}
	}
	GetPrivateProfileStringW(S_TIME_FORMAT_CHARS_H24, NULL, NULL, szKeys, 4096, szFile);
	if(wcslen(szKeys) == 0) {
		return;
	}
	else{
		pw = szKeys;
		if(*pw){
			while(*pw || *(pw - 1)){
				if(*pw && *pw != 31888){
					GetPrivateProfileStringW(S_TIME_FORMAT_CHARS_H24, pw, L"\n", szBuffer, 256, szFile);
					wcscat(g_Strings.TFormats, pw);
					wcscat(g_Strings.TFormats, L"\t");
					wcscat(g_Strings.TFormats, szBuffer);
					wcscat(g_Strings.TFormats, L"\n");
				}
				while(*pw++)
					;
			}
		}
	}
	GetPrivateProfileStringW(S_TIME_FORMAT_CHARS, NULL, NULL, szKeys, 4096, szFile);
	if(wcslen(szKeys) == 0) {
		return;
	}
	else{
		pw = szKeys;
		if(*pw){
			while(*pw || *(pw - 1)){
				if(*pw && *pw != 31888){
					GetPrivateProfileStringW(S_TIME_FORMAT_CHARS, pw, L"\n", szBuffer, 256, szFile);
					wcscat(g_Strings.TFormats, pw);
					wcscat(g_Strings.TFormats, L"\t");
					wcscat(g_Strings.TFormats, szBuffer);
					wcscat(g_Strings.TFormats, L"\n");
				}
				while(*pw++)
					;
			}
		}
	}
}

static void AddPNMenuItemForDockAll(int size, int id, wchar_t * lpDef, wchar_t * lpFile, int * pIndex, int pos){

	wchar_t 		szBuffer[256];
	P_HK_TYPE		pHK;
	int				idTemp = 0;
	PMItem			pmi;

	pHK = GetHotKeyByID(id, m_Hotkeys, NELEMS(m_Hotkeys));
	//replace docking menu items (in order to load right text)
	switch(id){
	case IDM_DOCK_NONE_ALL:
		idTemp = IDM_DOCK_NONE;
		break;
	case IDM_DOCK_LEFT_ALL:
		idTemp = IDM_DOCK_LEFT;
		break;
	case IDM_DOCK_TOP_ALL:
		idTemp = IDM_DOCK_TOP;
		break;
	case IDM_DOCK_RIGHT_ALL:
		idTemp = IDM_DOCK_RIGHT;
		break;
	case IDM_DOCK_BOTTOM_ALL:
		idTemp = IDM_DOCK_BOTTOM;
		break;
	}
	if(pHK){
		if(pHK->fsModifiers || pHK->vk)
			SetMenuTextWithAccelerator(idTemp, S_MENU, lpFile, lpDef, szBuffer, pHK->szKey);
		else
			SetMenuText(idTemp, S_MENU, lpFile, lpDef, szBuffer);
	}
	else{
		SetMenuText(idTemp, S_MENU, lpFile, lpDef, szBuffer);
	}
	SetMIText(m_MainMenus, size, id, szBuffer);
	AddCommand((*pIndex)++, szBuffer);
	pmi = GetMItem(m_MainMenus, size, id);
	if(pmi){
		SetMenuItemProperties(pmi, GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_DOCK_ALL)), pos);
	}
}

static void AddPNMenuItem(int size, int id, wchar_t * lpDef, wchar_t * lpFile, int * pIndex, BOOL addToCommands, HMENU hMenu){

	wchar_t 		szBuffer[256];
	P_HK_TYPE		pHK;
	PMItem			pmi;

	if(addToCommands){
		pHK = GetHotKeyByID(id, m_Hotkeys, NELEMS(m_Hotkeys));
		if(pHK){
			if(pHK->fsModifiers || pHK->vk)
				SetMenuTextWithAccelerator(id, S_MENU, lpFile, lpDef, szBuffer, pHK->szKey);
			else
				SetMenuText(id, S_MENU, lpFile, lpDef, szBuffer);
		}
		else{
			SetMenuText(id, S_MENU, lpFile, lpDef, szBuffer);
		}
		SetMIText(m_MainMenus, size, id, szBuffer);
		AddCommand((*pIndex)++, szBuffer);
	}
	else{
		SetMenuText(id, S_MENU, lpFile, lpDef, szBuffer);
		SetMIText(m_MainMenus, size, id, szBuffer);
	}
	pmi = GetMItem(m_MainMenus, size, id);
	if(pmi){
		SetMenuItemProperties(pmi, hMenu, GetMenuPosition(hMenu, id));
	}
}

static void AddDblCommands(wchar_t * lpFile){
	wchar_t 		szBuffer[256];

	GetPrivateProfileStringW(S_MENU, L"10001", L"New Note", szBuffer, 256, lpFile);
	wcscpy(m_DblCommands[0].szCommand, szBuffer);
	GetPrivateProfileStringW(S_MENU, L"10008", L"Control Panel", szBuffer, 256, lpFile);
	wcscpy(m_DblCommands[1].szCommand, szBuffer);
	GetPrivateProfileStringW(S_MENU, L"10002", L"Preferences", szBuffer, 256, lpFile);
	wcscpy(m_DblCommands[2].szCommand, szBuffer);
	GetPrivateProfileStringW(S_MENU, L"10034", L"Search In Notes", szBuffer, 256, lpFile);
	wcscpy(m_DblCommands[3].szCommand, szBuffer);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CreatePopUp
 Created  : Thu Aug  9 18:00:32 2007
 Modified : Thu Aug  9 18:00:32 2007

 Synopsys : Prepares menus and menu commands array
 Input    : lpFile - language file name
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void CreatePopUp(wchar_t * lpFile){
	wchar_t 		szFile[MAX_PATH], szKey[12], szTemp[128];
	int				size, index = 0;

	if(m_hPopUp){
		FreeMenus(m_hPopUp);
	}
	if(m_hMenu){
		DestroyMenu(m_hMenu);
	}
	m_hMenu = LoadMenuW(g_hInstance, MAKEINTRESOURCEW(IDR_MAIN));
	m_hPopUp = GetSubMenu(m_hMenu, 0);

	if(g_hNoteMainMenu){
		FreeMenus(g_hNotePopUp);
		DestroyMenu(g_hNoteMainMenu);
	}
	g_hNoteMainMenu = LoadMenuW(g_hInstance, MAKEINTRESOURCEW(IDR_NOTE));
	g_hNotePopUp = GetSubMenu(g_hNoteMainMenu, 0);
	if(g_hEditMainMenu){
		FreeMenus(g_hEditPopUp);
		DestroyMenu(g_hEditMainMenu);
	}
	g_hEditMainMenu = LoadMenuW(g_hInstance, MAKEINTRESOURCEW(IDR_NOTE_POPUP));
	g_hEditPopUp = GetSubMenu(g_hEditMainMenu, 0);
	if(g_hDropMenu){
		FreeMenus(g_hDropPopUp);
		DestroyMenu(g_hDropMenu);
	}
	g_hDropMenu = LoadMenuW(g_hInstance, MAKEINTRESOURCEW(IDR_DROP_MENU));
	g_hDropPopUp = GetSubMenu(g_hDropMenu, 0);
	//-----------	main popup menu	-----------
	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, lpFile);
	size = NELEMS(m_MainMenus);
	AddPNMenuItem(size, IDM_NEW, L"New Note", szFile, &index, TRUE, m_hPopUp);
	AddPNMenuItem(size, IDM_LOAD_NOTE, L"Load Note", szFile, &index, TRUE, m_hPopUp);
	AddPNMenuItem(size, IDM_NOTE_FROM_CLIPBOARD, L"New Note From Clipboard", szFile, &index, TRUE, m_hPopUp);
	AddPNMenuItem(size, IDM_OPTIONS, L"Preferences", szFile, &index, TRUE, m_hPopUp);
	AddPNMenuItem(size, IDM_LANGUAGES, L"Language", szFile, &index, TRUE, m_hPopUp);
	AddPNMenuItem(size, IDM_SHOW_GROUPS, L"Show", szFile, &index, TRUE, m_hPopUp);
	AddPNMenuItem(size, IDM_HIDE_GROUPS, L"Hide", szFile, &index, TRUE, m_hPopUp);
	_itow(IDM_SHOW_ALL, szKey, 10);
	GetPrivateProfileStringW(S_MENU, szKey, L"Show All", szTemp, 128, szFile);
	AddCommand(index++, szTemp);
	_itow(IDM_HIDE_ALL, szKey, 10);
	GetPrivateProfileStringW(S_MENU, szKey, L"Hide All", szTemp, 128, szFile);
	AddCommand(index++, szTemp);
	AddPNMenuItem(size, IDM_ALL_TO_FRONT, L"Bring All To Front", szFile, &index, TRUE, m_hPopUp);
	AddPNMenuItem(size, IDM_SAVE_ALL, L"Save All", szFile, &index, TRUE, m_hPopUp);
	AddPNMenuItem(size, IDM_DOCK_ALL, L"Dock All", szFile, &index, TRUE, m_hPopUp);
	AddPNMenuItemForDockAll(size, IDM_DOCK_NONE_ALL, L"None", szFile, &index, 0);
	AddPNMenuItemForDockAll(size, IDM_DOCK_LEFT_ALL, L"Left", szFile, &index, 1);
	AddPNMenuItemForDockAll(size, IDM_DOCK_TOP_ALL, L"Top", szFile, &index, 2);
	AddPNMenuItemForDockAll(size, IDM_DOCK_RIGHT_ALL, L"Right", szFile, &index, 3);
	AddPNMenuItemForDockAll(size, IDM_DOCK_BOTTOM_ALL, L"Bottom", szFile, &index, 4);
	AddPNMenuItem(size, IDM_SEARCH_SUBMENU, L"Search", szFile, &index, TRUE, m_hPopUp);
	AddPNMenuItem(size, IDM_SEARCH_IN_NOTES, L"Search In Notes", szFile, &index, TRUE, GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_SEARCH_SUBMENU)));
	AddPNMenuItem(size, IDM_SEARCH_TITLE, L"Search By Title", szFile, &index, TRUE, GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_SEARCH_SUBMENU)));
	AddPNMenuItem(size, IDM_CTRL, L"Control Panel", szFile, &index, TRUE, m_hPopUp);
	AddPNMenuItem(size, IDM_FAVORITES, L"Favorites", szFile, &index, TRUE, m_hPopUp);
	AddPNMenuItem(size, IDM_HELP, L"Help", szFile, &index, TRUE, m_hPopUp);
	AddPNMenuItem(size, IDM_ABOUT, L"About", szFile, &index, TRUE, m_hPopUp);
	AddPNMenuItem(size, IDM_ON_THE_WEB, PROG_PAGE_MENU_ITEM, szFile, &index, TRUE, m_hPopUp);
	AddPNMenuItem(size, IDM_EXIT, L"Exit", szFile, &index, TRUE, m_hPopUp);
	
	//first of all prepare note commands
	PrepareNoteCommands(szFile);

	//-----------	note popup menu	-----------
	PrepareMenuRecursiveWithAccelerators(NoteMenu(), NoteMenuCount(), g_hNotePopUp, szFile, S_MENU, m_NoteHotkeys, NoteMenuCount());
	//PrepareMenuRecursive(NoteMenu(), NoteMenuCount(), g_hNotePopUp, szFile, S_MENU);
	
	//edit popup menu
	PrepareMenuRecursive(EditMenu(), EditMenuCount(), g_hEditPopUp, szFile, S_MENU);
	
	//formatting menu
	PrepareMenuRecursive(EditMenu(), EditMenuCount(), GetSubMenu(g_hEditPopUp, GetMenuPosition(g_hEditPopUp, IDM_FORMAT_NOTE_TEXT)), szFile, S_COMMANDS);
	
	//drop popup menu
	PrepareMenuRecursive(DropMenu(), DropMenuCount(), g_hDropPopUp, szFile, S_MENU);

	if(m_PMenus){
		free(m_PMenus);
	}
	m_PMenus = (PMItem)calloc(NELEMS(m_MainMenus), NELEMS(m_MainMenus) * sizeof(MItem));
	memcpy(m_PMenus, m_MainMenus, NELEMS(m_MainMenus) * sizeof(MItem));

	LoadMenuLanguages(m_hPopUp);

	SetMenuOwnerDraw(m_hPopUp);
	
	AddColorsMenu();

	LoadWeekdays(szFile);

	AddDblCommands(szFile);

	SetDefaultItemForDblClick();

	PrepareRepParts(szFile);
}

static void PrepareRepParts(wchar_t * lpFile){
	wchar_t		szKey[16];

	for(int i = 0; i < NELEMS(g_RepParts); i++){
		if(!g_RepParts[i])
			g_RepParts[i] = calloc(128, sizeof(wchar_t));
	}
	_itow(IDC_ST_YEARS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Years", g_RepParts[0], 128, lpFile);
	_itow(IDC_ST_MONTHS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Months", g_RepParts[1], 128, lpFile);
	_itow(IDC_ST_WEEKS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Weeks", g_RepParts[2], 128, lpFile);
	_itow(IDC_ST_DAYS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Days", g_RepParts[3], 128, lpFile);
	_itow(IDC_ST_HOURS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Hours", g_RepParts[4], 128, lpFile);
	_itow(IDC_ST_MINS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Minutes", g_RepParts[5], 128, lpFile);
	_itow(IDC_ST_SECS, szKey, 10);
	GetPrivateProfileStringW(S_OPTIONS, szKey, L"Seconds", g_RepParts[6], 128, lpFile);
}

static void PrepareNoteCommands(wchar_t * lpFile){
	P_PNCOMMAND		pCommand = m_NoteCommands;
	wchar_t			szBuffer[128];
	PMItem			pmi = NoteMenu();

	for(int i = 0; i < NoteMenuCount(); i++){
		SetMenuText(pmi[i].id, S_MENU, lpFile, pmi[i].szText, szBuffer);
		wcscpy(pCommand->szCommand, szBuffer);
		pCommand++;
	}
}

static void SetDefaultItemForDblClick(void){
	SetMenuDefaultItem(m_hPopUp, -1, FALSE);
	SetMenuDefaultItem(GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_SEARCH_SUBMENU)), -1, FALSE);
	switch(GetIndexOfDblClick()){
		case 0:
			SetMenuDefaultItem(m_hPopUp, IDM_NEW, FALSE);
			break;
		case 1:
			SetMenuDefaultItem(m_hPopUp, IDM_CTRL, FALSE);
			break;
		case 2:
			SetMenuDefaultItem(m_hPopUp, IDM_OPTIONS, FALSE);
			break;
		case 3:
			SetMenuDefaultItem(GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_SEARCH_SUBMENU)), IDM_SEARCH_IN_NOTES, FALSE);
			break;
		default:
			SetMenuDefaultItem(m_hPopUp, IDM_NEW, FALSE);
			break;
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: FileExistsByFullPath
 Created  : Mon May 14 12:34:40 2007
 Modified : Mon May 14 12:34:40 2007

 Synopsys : Checks file existance by full file path
 Input    : Full file path
 Output   : TRUE if exists, otherwise - FALSE
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static BOOL FileExistsByFullPath(wchar_t * lpFile){
	WIN32_FIND_DATAW 	fd;
	HANDLE 				handle = INVALID_HANDLE_VALUE;

	handle = FindFirstFileW(lpFile, &fd);
	if(handle != INVALID_HANDLE_VALUE){
		FindClose(handle);
		return TRUE;
	}
	return FALSE;
}

static void RegisterCustomFonts(void){
	WIN32_FIND_DATAW 			fd;
	wchar_t						filespec[MAX_PATH], szTemp[MAX_PATH];
	HANDLE 						handle = INVALID_HANDLE_VALUE;
	BOOL 						result = TRUE;
	const wchar_t				*suffixes[] = {L"fon", L"fnt", L"ttf", L"ttc", L"fot", L"otf"};
	P_CUST_FONT					pTemp, pNext;

	wcscpy(filespec, g_NotePaths.FontsPath);
	wcscat(filespec, __T("*.*"));
	handle = FindFirstFileW(filespec, &fd);
	if(handle != INVALID_HANDLE_VALUE){
		while(result){
			wcscpy(szTemp, g_NotePaths.FontsPath);
			wcscat(szTemp, fd.cFileName);
			if(PathFileExistsW(szTemp)){
				if(PathFindSuffixArrayW(szTemp, suffixes, NELEMS(suffixes))){
					if(AddFontResourceExW(szTemp, FR_PRIVATE, 0) > 0){
						pTemp = calloc(1, sizeof(CUST_FONT));
						pTemp->lpName = calloc(wcslen(szTemp) + 1, sizeof(wchar_t));
						wcscpy(pTemp->lpName, szTemp);
						if(m_pFonts == NULL){
							m_pFonts = pTemp;
						}
						else{
							pNext = m_pFonts;
							while(pNext->next){
								pNext = pNext->next;
							}
							pNext->next = pTemp;
						}
					}
				}
			}
			result = FindNextFileW(handle, &fd);
		}
		FindClose(handle);
	}
}

static void ClearCustomFonts(void){
	P_CUST_FONT					pTemp, pNext;
	
	pNext = m_pFonts;
	while(pNext){
		pTemp = pNext->next;
		RemoveFontResourceExW(pNext->lpName, FR_PRIVATE, 0);
		free(pNext->lpName);
		free(pNext);
		pNext = pTemp;
	}
	m_pFonts = NULL;
}

static void CreateShowHideGroupsMenus(HMENU hMenu){
	HMENU					h1, h2;
	int						count, j = 0, groupsCount;
	P_PNGROUP				ppg;
	wchar_t					szB1[128], szB2[128], szLang[256], szKey[12];
	MItem					mit;
	P_HK_TYPE				pHK;
	P_GROUP_MENU_STRUCT		pGroups, pTemp;

	ZeroMemory(&mit, sizeof(mit));
	mit.xPos = -1;
	mit.yPos = -1;
	mit.xCheck = 22;
	wcscpy(szLang, g_NotePaths.LangDir);
	wcscat(szLang, g_NotePaths.LangFile);
	_itow(IDM_SHOW_ALL, szKey, 10);
	GetPrivateProfileStringW(S_MENU, szKey, L"Show All", szB1, 128, szLang);
	pHK =  GetHotKeyByID(IDM_SHOW_ALL, m_Hotkeys, NELEMS(m_Hotkeys));
	if(pHK){
		wcscat(szB1, L"\t");
		wcscat(szB1, pHK->szKey);
	}
	_itow(IDM_HIDE_ALL, szKey, 10);
	GetPrivateProfileStringW(S_MENU, szKey, L"Hide All", szB2, 128, szLang);
	pHK =  GetHotKeyByID(IDM_HIDE_ALL, m_Hotkeys, NELEMS(m_Hotkeys));
	if(pHK){
		wcscat(szB2, L"\t");
		wcscat(szB2, pHK->szKey);
	}
	h1 = GetSubMenu(hMenu, GetMenuPosition(hMenu, IDM_SHOW_GROUPS));
	h2 = GetSubMenu(hMenu, GetMenuPosition(hMenu, IDM_HIDE_GROUPS));
	count = GetMenuItemCount(h1);
	for(int i = count - 1; i >=0; i--){
		FreeSingleMenu(h1, i);
		FreeSingleMenu(h2, i);
		DeleteMenu(h1, i, MF_BYPOSITION);
		DeleteMenu(h2, i, MF_BYPOSITION);
	}
	mit.id = IDM_SHOW_ALL;
	wcscpy(mit.szText, szB1);
	AppendMenuW(h1, MF_STRING, mit.id, mit.szText);
	SetMenuItemProperties(&mit, h1, 0);
	mit.id = IDM_HIDE_ALL;
	wcscpy(mit.szText, szB2);
	AppendMenuW(h2, MF_STRING, mit.id, mit.szText);
	SetMenuItemProperties(&mit, h2, 0);
	j++;
	AppendMenuW(h1, MF_SEPARATOR | MF_OWNERDRAW, 0, NULL);
	AppendMenuW(h2, MF_SEPARATOR | MF_OWNERDRAW, 0, NULL);
	j++;
	ppg = GetGroups();
	groupsCount = GetGroupsCount();
	if(ppg){
		pGroups = calloc(groupsCount, sizeof(GROUP_MENU_STRUCT));
		if(pGroups){
			pTemp = pGroups;
			while(ppg){
				pTemp->id = ppg->id;
				pTemp->pName = ppg->szName;
				pTemp++;
				ppg = ppg->next;
			}
			pTemp = pGroups;
			if(groupsCount > 1){
				qsort(pTemp, groupsCount, sizeof(GROUP_MENU_STRUCT), GroupMenusCompare);
			}
			for(int i = 0; i < groupsCount; i++){
				wcscpy(mit.szText, pTemp->pName);
				mit.id = pTemp->id + SHOW_GROUP_ADDITION;
				//insert menu item
				AppendMenuW(h1, MF_STRING, mit.id, mit.szText);
				SetMenuItemProperties(&mit, h1, j);
				mit.id = pTemp->id + HIDE_GROUP_ADDITION;
				//insert menu item
				AppendMenuW(h2, MF_STRING, mit.id, mit.szText);
				SetMenuItemProperties(&mit, h2, j);
				//move next
				j++;
				pTemp++;
			}
			free(pGroups);
		}
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: LoadMenuLanguages
 Created  : Thu Aug  9 18:01:54 2007
 Modified : Thu Aug  9 18:01:54 2007

 Synopsys : Loads languages names for language menu
 Input    : hMenu - main popup menu
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void LoadMenuLanguages(HMENU	hMenu){
	WIN32_FIND_DATAW 	fd;
	wchar_t				filespec[MAX_PATH], szTemp[MAX_PATH];
	wchar_t				szLang[256];
	HANDLE 				handle = INVALID_HANDLE_VALUE;
	BOOL 				result = TRUE;
	int 				index = 0, position = -1, i = 0;
	HMENU				hLang;
	MItem				mit;

	ZeroMemory(&mit, sizeof(mit));
	mit.xPos = -1;
	mit.yPos = -1;
	mit.xCheck = 22;

	hLang = GetSubMenu(hMenu, GetMenuPosition(hMenu, IDM_LANGUAGES));

	wcscpy(filespec, g_NotePaths.LangDir);
	wcscat(filespec, __T("*.lng"));

	handle = FindFirstFileW(filespec, &fd);
	//find language file
	if(handle != INVALID_HANDLE_VALUE){
		//remove "empty" menu
		FreeSingleMenu(hLang, 0);
		RemoveMenu(hLang, IDM_L_EMPTY, MF_BYCOMMAND);
		while(result){
			//get language name
			wcscpy(szTemp, g_NotePaths.LangDir);
			wcscat(szTemp, fd.cFileName);
			GetPrivateProfileStringW(__T("language"), __T("name"), NULL, szLang, 256, szTemp);
			index = GetPrivateProfileIntW(__T("language"), __T("id"), 0, szTemp);
			//set up menu item
			mit.id = index + LANG_ADDITION;
			wcscpy(mit.szText, szLang);
			wcscpy(mit.szReserved, szTemp);
			//insert menu item
			AppendMenuW(hLang, MF_STRING, mit.id, szLang);
			SetMenuItemProperties(&mit, hLang, i);
			if(wcscmp(fd.cFileName, g_NotePaths.LangFile) == 0)
				position = i;
			i++;
			result = FindNextFileW(handle, &fd);
		}
		if(position >= 0){
			//check currently selected item
			CheckMenuItem(hLang, position, MF_BYPOSITION | MF_CHECKED);
		}
		FindClose(handle);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CreatePNotesShortcut
 Created  : Thu Aug  9 18:04:50 2007
 Modified : Thu Aug  9 18:04:50 2007

 Synopsys : Creates program shortcut in StartUp directory
 Input    : Nothing
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static HRESULT CreatePNotesShortcut(void){

	char			szWorkingDir[MAX_PATH], szLink[MAX_PATH], szProgDir[MAX_PATH];
	
	if(SHGetSpecialFolderPath(NULL, szLink, CSIDL_STARTUP, FALSE)){
		strcat(szLink, LINK_NAME);
		WideCharToMultiByte(CP_ACP, 0, g_NotePaths.ProgFullPath, -1, szProgDir, MAX_PATH, NULL, NULL);
		strcpy(szWorkingDir, szProgDir);
		PathRemoveFileSpec(szWorkingDir);
		return CreateShortcut(szLink, szProgDir, szWorkingDir, NULL, SW_SHOWNORMAL, szProgDir, 0, NULL, 0);
	}
	else
		return -1;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetHKId
 Created  : Sun Aug 12 16:02:24 2007
 Modified : Sun Aug 12 16:02:24 2007

 Synopsys : Returns hot keys ID by identifier
 Input    : identifier - hot keys identifier
 Output   : Hot keys id
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static int GetHKId(P_HK_TYPE lpKeys, int count, int identifier){
	for(int i = 0; i < count; i++){
		if(lpKeys[i].identifier == identifier)
			return lpKeys[i].id;
	}
	return 0;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: HKeysAlreadyRegistered
 Created  : Sun Aug 12 16:00:20 2007
 Modified : Sun Aug 12 16:00:20 2007

 Synopsys : Dispays message on hot keys already registered
 Input    : lpKey - hot keys text
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void HKeysAlreadyRegistered(wchar_t * lpKey){
	wchar_t			szMessage[256];

	szMessage[0] = '\0';
	wcscpy(szMessage, m_sHK1);
	wcscat(szMessage, L" ");
	wcscat(szMessage, lpKey);
	wcscat(szMessage, L" ");
	wcscat(szMessage, m_sHK2);
	MessageBoxW(g_hMain, szMessage, L"Re-register hot key", MB_OK | MB_ICONEXCLAMATION);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: RegisterHKeys
 Created  : Sun Aug 12 15:59:22 2007
 Modified : Sun Aug 12 15:59:22 2007

 Synopsys : Registers hot keys
 Input    : hwnd - main window handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void RegisterHKeys(HWND hwnd, P_HK_TYPE lpKeys, int count){
	P_HK_TYPE	pKeys = lpKeys;
	for(int i = 0; i < count; i++){
		if(pKeys->fsModifiers != 0 || pKeys->vk != 0){
			if(RestrictedHotKey(pKeys->szKey)){
				HKeysAlreadyRegistered(pKeys->szKey);
			}
			else if(!RegisterHotKey(hwnd, pKeys->identifier, pKeys->fsModifiers, pKeys->vk)){
				HKeysAlreadyRegistered(pKeys->szKey);
			}
		}
		pKeys++;
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: UnregisterHKeys
 Created  : Sun Aug 12 15:58:51 2007
 Modified : Sun Aug 12 15:58:51 2007

 Synopsys : Unregisters hot keys
 Input    : hwnd - main window handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void UnregisterHKeys(HWND hwnd, P_HK_TYPE lpKeys, int count){
	P_HK_TYPE	pKeys = lpKeys;
	for(int i = 0; i < count; i++){
		if(pKeys->fsModifiers != 0 || pKeys->vk != 0){
			UnregisterHotKey(hwnd, pKeys->identifier);
		}
		pKeys++;
	}
}

static void PrepareNoteHotKeys(void){
	wchar_t		szKey[12];
	HK_TYPE		tKey, *pKeys = m_NoteHotkeys;
	int			count = NoteMenuCount();
	PMItem		menus = NoteMenu();

	for(int i = 0; i < count; i++){
		pKeys->id = menus[i].id;
		_itow(pKeys->id, szKey, 10);
		pKeys->identifier = NOTE_HOTKEYS_IND + i;
		//use temporary key instead of array member, 
		//because identifiers can change, when menu items are added or deleted
		if(!GetPrivateProfileStructW(S_NOTE_HOTKEYS, szKey, &tKey, sizeof(HK_TYPE), m_sMainINI)){
			pKeys->fsModifiers = 0;
			pKeys->vk = 0;
			pKeys->szKey[0] = '\0';
		}
		else{
			pKeys->fsModifiers = tKey.fsModifiers;
			pKeys->vk = tKey.vk;
			wcscpy(pKeys->szKey, tKey.szKey);
		}
		pKeys++;
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: PrepareHotKeys
 Created  : Sun Aug 12 15:57:27 2007
 Modified : Sun Aug 12 15:57:27 2007

 Synopsys : Prepares hot keys array, according to menu items
 Input    : Nothing
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void PrepareHotKeys(void){
	wchar_t		szKey[12];
	HK_TYPE		tKey;

	for(int i = 0; i < NELEMS(m_Hotkeys); i++){
		m_Hotkeys[i].id = m_MainMenus[i].id;
		_itow(m_Hotkeys[i].id, szKey, 10);
		m_Hotkeys[i].identifier = MAIN_HOTKEYS_IND + i;
		//use temporary key instead of array member, 
		//because identifiers can change, when menu items are added or deleted
		if(!GetPrivateProfileStructW(S_HOTKEYS, szKey, &tKey, sizeof(HK_TYPE), m_sMainINI)){
			m_Hotkeys[i].fsModifiers = 0;
			m_Hotkeys[i].vk = 0;
			m_Hotkeys[i].szKey[0] = '\0';
		}
		else{
			m_Hotkeys[i].fsModifiers = tKey.fsModifiers;
			m_Hotkeys[i].vk = tKey.vk;
			wcscpy(m_Hotkeys[i].szKey, tKey.szKey);
		}
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SaveHotKeys
 Created  : Sun Aug 12 15:54:06 2007
 Modified : Sun Aug 12 15:54:06 2007

 Synopsys : Saves/deletes hot keys in INI file
 Input    : Nothing
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void SaveHotKeys(const wchar_t * lpSection, P_HK_TYPE lpKeys, int count){
	wchar_t		szKey[12];
	P_HK_TYPE	pKeys = lpKeys;

	for(int i = 0; i < count; i++){
		_itow(pKeys->id, szKey, 10);
		if(pKeys->fsModifiers != 0 || pKeys->vk != 0)
			//save hot key
			WritePrivateProfileStructW(lpSection, szKey, pKeys, sizeof(HK_TYPE), m_sMainINI);
		else
			//delete hot key
			WritePrivateProfileStructW(lpSection, szKey, NULL, sizeof(HK_TYPE), m_sMainINI);
		pKeys++;
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: AddCommand
 Created  : Sun Aug 12 15:50:16 2007
 Modified : Sun Aug 12 15:50:16 2007

 Synopsys : Adds menu command to array, removing possible tab characters
 Input    : index - array index
            lpCommand - menu command text
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void AddCommand(int index, wchar_t * lpCommand){
	wchar_t * tmp = lpCommand;

	while(*tmp){
		if(*tmp == '\t')
			*tmp++ = '\0';
		else
			*tmp++;
	}
	wcscpy(m_Commands[index].szCommand, lpCommand);
}

static BOOL RestrictedHotKey(wchar_t * szKey){
	if(_wcsicmp(szKey, L"Ctrl+S") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+C") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+X") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+V") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+O") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+P") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+F") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+A") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+Z") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+Y") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"F3") == 0)
		return TRUE;
	else if(_wcsicmp(szKey, L"Ctrl+G") == 0)
		return TRUE;
	return FALSE;
}

static BOOL CALLBACK EditHotProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	wchar_t			szModifiers[24], szKey[8], szBuffer[24];
	int				idMod = 0, key = 0;
	BOOL			proceed = FALSE;

	switch(msg){
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			//get modifiers
			idMod = GetModifiers(szModifiers);
			//get the very first other key pressed
			key = GetOtherKey(szKey);
			if(idMod == 0 && (key >= VK_F1 && key <= VK_F24)){
				if(m_HKCurrent.fsModifiers != idMod || m_HKCurrent.vk != key){
					proceed = TRUE;
				}
			}
			else if(idMod && key){
				if(m_HKCurrent.fsModifiers != idMod || m_HKCurrent.vk != key){
					proceed = TRUE;
				}
			}
			if(proceed){
				wcscpy(szBuffer, szModifiers);
				wcscat(szBuffer, szKey);
				if(RestrictedHotKey(szBuffer)){
					HKeysAlreadyRegistered(szBuffer);
					return 0;
				}
				//check whether hot keys are already registered
				if(!RegisterHotKey(g_hMain, 1001, idMod, key)){
					HKeysAlreadyRegistered(szBuffer);
				}
				else{
					UnregisterHotKey(g_hMain, 1001);
				}
				//apply to current hot key
				wcscpy(m_HKCurrent.szKey, szBuffer);
				m_HKCurrent.fsModifiers = idMod;
				m_HKCurrent.vk = key;
				SetWindowTextW(hwnd, szBuffer);
			}
			return 0;
		default:
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
	}
}

static BOOL CALLBACK Hot_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_INITDIALOG, Hot_OnInitDialog);
	HANDLE_MSG (hwnd, WM_COMMAND, Hot_OnCommand);
	HANDLE_MSG (hwnd, WM_CLOSE, Hot_OnClose);
	default: return FALSE;
	}
}

static void Hot_OnClose(HWND hwnd)
{
	m_hHotDlg = 0;
	EndDialog(hwnd, 0);
}

static void Hot_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDCANCEL:
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
		case IDOK:
			m_hHotDlg = 0;
			EndDialog(hwnd, IDOK);
			break;
	}
}

static BOOL Hot_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	RECT			rc;
	HWND			hEdit;

	m_hHotDlg = hwnd;
	hEdit = GetDlgItem(hwnd, IDC_EDT_HOT);
	//subclass edit box window
	SetWindowLongPtrW(hEdit, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(hEdit, GWLP_WNDPROC, (LONG_PTR)EditHotProc));
	//move window according to commands combo box
	GetWindowRect(GetDlgItem(m_Dialogs.hBehavior, IDC_CBO_COMMANDS), &rc);
	SetWindowPos(hwnd, 0, rc.left, rc.bottom, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOZORDER);
	//apply language
	ApplyHotDialogLanguage(hwnd, g_NotePaths.LangFile);
	//display current hot keys
	SetDlgItemTextW(hwnd, IDC_EDT_HOT, m_HKCurrent.szKey);
	SetFocus(hEdit);
	return FALSE;
}

static BOOL IsAnyAutosavedExists(void){
	wchar_t				filespec[MAX_PATH];
	WIN32_FIND_DATAW 	fd;
	HANDLE 				handle;

	wcscpy(filespec, g_NotePaths.DataDir);
	wcscat(filespec, __T("~*.no~"));
	handle = FindFirstFileW(filespec, &fd);
	if(handle == INVALID_HANDLE_VALUE){
		return FALSE;
	}
	else{
		FindClose(handle);
		return TRUE;
	}
}

static void DeleteAutosaved(void){
	//delete all autosaved notes
	wchar_t				filespec[MAX_PATH], szPath[MAX_PATH], szTemp[MAX_PATH];
	WIN32_FIND_DATAW 	fd;
	HANDLE 				handle = INVALID_HANDLE_VALUE;
	BOOL				result = TRUE;

	if(!NotesDirExists(g_NotePaths.DataDir))
		return;
	wcscpy(szTemp, g_NotePaths.DataDir);
	wcscpy(filespec, g_NotePaths.DataDir);
	wcscat(filespec, __T("~*.no~"));
	handle = FindFirstFileW(filespec, &fd);
	if(handle == INVALID_HANDLE_VALUE)
		return;
	while(result){
		wcscpy(szPath, szTemp);
		wcscat(szPath, fd.cFileName);
		DeleteFileW(szPath);
		result = FindNextFileW(handle, &fd);
	}
	FindClose(handle);
	wcscat(szTemp, AUTOSAVE_INDEX_FILE);
	DeleteFileW(szTemp);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: LoadSounds
 Created  : Sun Aug 12 15:47:06 2007
 Modified : Sun Aug 12 15:47:06 2007

 Synopsys : Loads all sound files from 'sound' directory into listbox on 
            Preferences window
 Input    : hwnd - Preferences window handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void LoadSounds(HWND hwnd){
	wchar_t				filespec[MAX_PATH];
	WIN32_FIND_DATAW 	fd;
	HANDLE 				handle = INVALID_HANDLE_VALUE;
	BOOL				result = TRUE;
	HWND				hList;

	//if 'sound' directory does not exist - return
	if(!NotesDirExists(g_NotePaths.SoundDir))
		return;
	//search for all *.wav files
	wcscpy(filespec, g_NotePaths.SoundDir);
	wcscat(filespec, __T("*.wav"));
	handle = FindFirstFileW(filespec, &fd);
	if(handle == INVALID_HANDLE_VALUE)
		return;
	hList = GetDlgItem(hwnd, IDC_LST_SOUND);
	while(result){
		SendMessageW(hList, LB_ADDSTRING, 0, (LPARAM)fd.cFileName);
		result = FindNextFileW(handle, &fd);
	}
	FindClose(handle);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CopySoundFile
 Created  : Sun Aug 12 15:40:56 2007
 Modified : Sun Aug 12 15:40:56 2007

 Synopsys : Copies sound file into 'sound' directory
 Input    : lpSrcFull - full source path
            lpSrcFile - sound file short name
 Output   : TRUE if succeeded, otherwise - FALSE
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static BOOL CopySoundFile(wchar_t * lpSrcFull, wchar_t * lpSrcFile){
	wchar_t				szDest[MAX_PATH];

	wcscpy(szDest, g_NotePaths.SoundDir);
	wcscat(szDest, lpSrcFile);
	return CopyFileW(lpSrcFull, szDest, FALSE);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: DeleteSoundFile
 Created  : Sun Aug 12 15:38:55 2007
 Modified : Sun Aug 12 15:38:55 2007

 Synopsys : Deletes sound file
 Input    : lpFile - sound file short name
 Output   : TRUE if succeeded, otherwise - FALSE
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static BOOL DeleteSoundFile(wchar_t * lpFile){
	wchar_t				szDest[MAX_PATH];

	wcscpy(szDest, g_NotePaths.SoundDir);
	wcscat(szDest, lpFile);
	return DeleteFileW(szDest);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: PreviewSound
 Created  : Sun Aug 12 15:37:56 2007
 Modified : Sun Aug 12 15:37:56 2007

 Synopsys : Plays selected sound
 Input    : lpSound - sound file short name
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void PreviewSound(wchar_t * lpSound){
	wchar_t			szPath[MAX_PATH];

	if(wcscmp(lpSound, DS_DEF_SOUND) == 0)
		PlaySoundW(__T("Notify"), 0, SND_ALIAS | SND_ASYNC);
	else{
		wcscpy(szPath, g_NotePaths.SoundDir);
		wcscat(szPath, lpSound);
		PlaySoundW(szPath, 0, SND_FILENAME | SND_ASYNC);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ParsePNCommandLine
 Created  : Sun Aug 12 15:33:55 2007
 Modified : Sun Aug 12 15:33:55 2007

 Synopsys : Parses program command line arguments
 Input    : pINIPath - output string for main INI file in case of -conf or -config flag
 Output   : Combination of arguments flags
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static int ParsePNCommandLine(wchar_t * pINIPath, wchar_t * pDataPath, wchar_t * pProgPath){
	LPWSTR 			* szArglist;
   	int 			nArgs;
	wchar_t			* szArg;
	int				c, result = 0;

	pINIPath[0] = '\0';
	pDataPath[0] = '\0';
	//parse command line into WCHAR array
   	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if(szArglist != 0){
		if(nArgs > 1){			//the first argument is a program name
			for(int i = 1; i < nArgs; i++){
				if((_wcsicmp(szArglist[i], L"-conf") == 0) || (_wcsicmp(szArglist[i], L"-config") == 0)){
					if(nArgs >= 3){
						if(wcslen(szArglist[i + 1]) > 0){
							wcscpy(pINIPath, szArglist[i + 1]);
							PathUnquoteSpacesW(pINIPath);
							result |= ARG_INI_PATH;
						}
					}
					if(nArgs >= 4){
						if(wcslen(szArglist[i + 2]) > 0){
							wcscpy(pDataPath, szArglist[i + 2]);
							PathUnquoteSpacesW(pDataPath);
							result |= ARG_DATA_PATH;
						}
					}
					if(nArgs == 5){
						if(wcslen(szArglist[i + 3]) > 0){
							wcscpy(pProgPath, szArglist[i + 3]);
							PathUnquoteSpacesW(pProgPath);
							result |= ARG_PROG_PATH;
						}
					}
					break;
				}
				szArg = szArglist[i];
				if(*szArg == '-'){			//arguments should start from '-'
					while((c = *szArg++) != '\0'){
						switch(c){
							case 'x':				//close program
								result |= ARG_EXIT;
								break;
							case 's':				//silent close
								result |= ARG_SILENT;
								break;
							case 'n':				//close without saving
								result |= ARG_NO_SAVE;
								break;
						}
					}
				}
			}
		}
		LocalFree(szArglist);
	}
	return result;
}

static BOOL CALLBACK EnumPNotes(HWND hwnd, LPARAM lParam){
	wchar_t				szClass[256];

	GetClassNameW(hwnd, szClass, 256);
	if(wcscmp(szClass, NOTES_PROG_MAIN_CLASS) == 0){
		//if main PNotes window found
		if((lParam & ARG_EXIT) == ARG_EXIT){
			//if command line argument = exit - close window
			SendMessageW(hwnd, CHM_MAIN_CLOSE, 0, lParam);
			return FALSE;
		}
	}
	return TRUE;
}

static void ClearOnOptionsClose(void){
	if(g_RTHandles.hFCaption != m_TempRHandles.hFCaption)
		DeleteObject(m_TempRHandles.hFCaption);
	if(g_RTHandles.hFont != m_TempRHandles.hFont)
		DeleteObject(m_TempRHandles.hFont);
	if(g_RTHandles.hbSkin != m_TempRHandles.hbSkin)
		DeleteObject(m_TempRHandles.hbSkin);
	if(g_RTHandles.hbPattern != m_TempRHandles.hbPattern)
		DeleteObject(m_TempRHandles.hbPattern);
	if(g_RTHandles.hbDelHide != m_TempRHandles.hbDelHide)
		DeleteObject(m_TempRHandles.hbDelHide);
	if(g_RTHandles.hbSys != m_TempRHandles.hbSys)
		DeleteObject(m_TempRHandles.hbSys);
	if(g_RTHandles.hbCommand != m_TempRHandles.hbCommand)
		DeleteObject(m_TempRHandles.hbCommand);
	if(m_TempDockingSettings.fCustCaption && g_DRTHandles.hFCaption != m_TempDRTHandles.hFCaption)
		DeleteObject(m_TempDRTHandles.hFCaption);
	// if(m_TempDockingSettings.fCustFont && g_DRTHandles.hFont != m_TempDRTHandles.hFont)
		// DeleteObject(m_TempDRTHandles.hFont);
	if(m_TempDockingSettings.fCustSkin){
		if(g_DRTHandles.hbSkin != m_TempDRTHandles.hbSkin)
			DeleteObject(m_TempDRTHandles.hbSkin);
		if(g_DRTHandles.hbPattern != m_TempDRTHandles.hbPattern)
			DeleteObject(m_TempDRTHandles.hbPattern);
		if(g_DRTHandles.hbDelHide != m_TempDRTHandles.hbDelHide)
			DeleteObject(m_TempDRTHandles.hbDelHide);
		if(g_DRTHandles.hbSys != m_TempDRTHandles.hbSys)
			DeleteObject(m_TempDRTHandles.hbSys);
		if(g_DRTHandles.hbCommand != m_TempDRTHandles.hbCommand)
			DeleteObject(m_TempDRTHandles.hbCommand);
	}
	if(m_TempNoteHotkeys){
		free(m_TempNoteHotkeys);
		m_TempNoteHotkeys = NULL;
	}
}

static BOOL IsLastBackslash(wchar_t * src){
	wchar_t 	* temp;

	temp = src;
	while(*temp++)
		;
	*temp--;
	*temp--;
	if(*temp == '\\')
		return TRUE;
	else
		return FALSE;
}

static void CALLBACK AutosaveTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime){
	ApplyAutosaveNotes();
}


/*-@@+@@------------------------------------------------------------------
 Procedure: ReplaceCharacters
 Created  : Thu May 10 14:07:22 2007
 Modified : Thu May 10 14:07:49 2007

 Synopsys : Replaces carriage return with aknowledge character, new line 
            with bell character and vice versa. It's need in order to write 
            single line in file instead of several lines.
 Input    : Destination and source strings
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static void ReplaceCharacters(wchar_t * dst, wchar_t * src){

	for(; *src; src++, dst++){
		switch(*src){
			case '\r':
				*dst = 6;		//from carriage return to aknowledge
				break;
			case 6:
				*dst = '\r';	//from aknowledge to carriage return
				break;
			case '\n':
				*dst = 7;		//from new line to bell
				break;
			case 7:
				*dst = '\n';	//from bell to new line	
				break;
			default:
				*dst = *src;
		}
	}
	*dst = *src;			//last null character
}

/*-@@+@@------------------------------------------------------------------
 Procedure: CleanPrefetch
 Created  : Mon Aug 20 11:27:22 2007
 Modified : Mon Aug 20 11:27:22 2007

 Synopsys : Cleans Prefetch folder on program exit
 Input    : lpcProgName - program executable file name
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
// static void CleanPrefetch(const wchar_t * lpcProgName){
	// wchar_t				szPath[MAX_PATH], szTemp[MAX_PATH], szCopy[MAX_PATH];
	// HANDLE 				handle = INVALID_HANDLE_VALUE;
	// BOOL				result = TRUE;
	// WIN32_FIND_DATAW 	fd;

	// GetSystemWindowsDirectoryW(szPath, MAX_PATH);
	// wcscat(szPath, L"\\Prefetch\\");
	// wcscpy(szCopy, szPath);
	// wcscat(szPath, L"*.*");
	// handle = FindFirstFileW(szPath, &fd);
	// if(handle == INVALID_HANDLE_VALUE)
		// return;
	// while(result){
		// if(_wcsistr(fd.cFileName, lpcProgName)){
			// wcscpy(szTemp, szCopy);
			// wcscat(szTemp, fd.cFileName);
			// DeleteFileW(szTemp);
		// }
		// result = FindNextFileW(handle, &fd);
	// }
	// FindClose(handle);
// }

static void ChangeDockingDefSkin(void){
	HWND		hCombo = GetDlgItem(m_Dialogs.hDocks, IDC_CBO_SKINS);
	wchar_t		szTemp[64];
	int			index = -1;

	wcscpy(szTemp, m_sTempSkinFile);
	szTemp[wcslen(szTemp) - 4] = '\0';
	index = SendMessageW(hCombo, CB_FINDSTRINGEXACT, -1, (LPARAM)szTemp);
	if(index != CB_ERR){
		SendMessageW(hCombo, CB_SETCURSEL, index, 0);
	}
	// wcscpy(m_TempDockingSettings.szCustSkin, szTemp);
	GetDockSkinProperties(m_Dialogs.hDocks, &m_TempDRTHandles, szTemp, FALSE);
}

static void ChangeDockingDefSize(int id, int val){
	// SetDlgItemInt(m_Dialogs.hDocks, id, val, FALSE);
}

//-----------	Skins stuff	-----------
static BOOL CALLBACK Skins_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_INITDIALOG, Skins_OnInitDialog);
	HANDLE_MSG (hwnd, WM_DRAWITEM, Skins_OnDrawItem);
	HANDLE_MSG (hwnd, WM_COMMAND, Skins_OnCommand);
	case WM_NOTIFY:{
		LPNMHDR lpnmh = (LPNMHDR)lParam;
		if(lpnmh->idFrom == IDC_LBL_MORE_SKINS && lpnmh->code == NM_CLICK){
			if(wcslen(g_DefBrowser) == 0)
				ShellExecuteW(hwnd, L"open", SKIN_PAGE, NULL, NULL, SW_SHOWDEFAULT);
			else{
				if((int)ShellExecuteW(hwnd, NULL, g_DefBrowser, SKIN_PAGE, NULL, SW_SHOWDEFAULT) <= 32){
					ShellExecuteW(hwnd, L"open", SKIN_PAGE, NULL, NULL, SW_SHOWDEFAULT);
				}
			}
		}
		return TRUE;
	}
	case CHM_DRAW_SN_PVW:
		DrawSimpleNotePreview((HWND)wParam, (DRAWITEMSTRUCT *)lParam, &m_rWindow, &m_rCaption, &m_TempAppearance, &m_TempRHandles);
		return TRUE;
	default: return FALSE;
	}
}

static void InitDlgSkins(HWND hwnd){
	//fill skins names
	FillSkins(hwnd, g_Appearance.szSkin, TRUE, TRUE);
	//set default skinless size
	SendDlgItemMessageW(hwnd, IDC_EDT_DEF_SIZE_W, EM_LIMITTEXT, 3, 0);
	SendDlgItemMessageW(hwnd, IDC_EDT_DEF_SIZE_H, EM_LIMITTEXT, 3, 0);
	SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_W, m_TempRHandles.szDef.cx, FALSE);
	SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_H, m_TempRHandles.szDef.cy, FALSE);
	SendDlgItemMessageW(hwnd, IDC_REDUCE_TO_CAPTION, BM_SETCHECK, (m_TempNoteSettings.rollOnDblClick ? BST_CHECKED : BST_UNCHECKED), 0);
	SendDlgItemMessageW(hwnd, IDC_CHK_RANDOM_COLOR, BM_SETCHECK, (IsBitOn(m_TempNoteSettings.reserved1, SB1_RANDCOLOR) ? BST_CHECKED : BST_UNCHECKED), 0);
	SendDlgItemMessageW(hwnd, IDC_CHK_INVERT_COLOR, BM_SETCHECK, (IsBitOn(m_TempNoteSettings.reserved1, SB1_INVERT_TEXT) ? BST_CHECKED : BST_UNCHECKED), 0);
	if(IsDlgButtonChecked(hwnd, IDC_CHK_RANDOM_COLOR) == BST_CHECKED)
		EnableWindow(GetDlgItem(hwnd, IDC_CHK_INVERT_COLOR), TRUE);
	else
		EnableWindow(GetDlgItem(hwnd, IDC_CHK_INVERT_COLOR), FALSE);
	//disable skinless properties when any skin is selected
	if(m_TempRHandles.hbSkin)
		EnableSkinlessProperties(FALSE);
	else
		EnableSkinlessProperties(TRUE);
}

static BOOL Skins_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	RECT		rc;

	GetWindowRect(GetDlgItem(hwnd, IDC_PLACEHOLDER_1), &rc);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
	CreateLinkLableW(hwnd, g_hInstance, L"More skins", rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, IDC_LBL_MORE_SKINS);
	return TRUE;
}

static void Skins_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	CHOOSEFONTW			cf;
	LOGFONTW			lfw;
	wchar_t 			szBuffer[256], szPath[MAX_PATH];
	BOOL				fChecked;

	switch(id){
		case IDC_CMD_FONT:	//choose font
			wcscpy(szPath, g_NotePaths.LangDir);
			wcscat(szPath, g_NotePaths.LangFile);
			GetPrivateProfileStringW(L"captions", L"font", L"Choose font", szBuffer, 256, szPath);
			memcpy(&lfw, &m_TempAppearance.lfFont, sizeof(lfw));
			ZeroMemory(&cf, sizeof(cf));
			cf.lStructSize = sizeof(cf);
			cf.Flags = CF_SCREENFONTS | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT | CF_ENABLEHOOK;
			cf.hwndOwner = hwnd;
			cf.lpLogFont = &lfw;
			cf.rgbColors = m_TempAppearance.crFont;
			cf.lpfnHook = CFHookProc;
			cf.lCustData = (int)szBuffer;
			if(ChooseFontW(&cf)){
				memcpy(&m_TempAppearance.lfFont, &lfw, sizeof(lfw));
				m_TempRHandles.hFont = CreateFontIndirectW(&m_TempAppearance.lfFont);
				m_TempAppearance.crFont = (cf.rgbColors == 0) ? 1 : ((cf.rgbColors == m_TempRHandles.crMask) ? m_TempRHandles.crMask + 1 : cf.rgbColors);
				RedrawWindow(GetDlgItem(hwnd, IDC_ST_BMP), NULL, NULL, RDW_INVALIDATE);
			}
			break;
		case IDC_CMD_SKNLESS_BCOLOR:
			if(codeNotify == BN_CLICKED){
				ChangeSkinlessColor(hwnd, &m_TempAppearance.crWindow, IDC_ST_BMP);
			}
			break;
		case IDC_CMD_SKNLESS_CFONT:
			if(codeNotify == BN_CLICKED){
				ChangeSkinlessCaptionFont(hwnd, &m_TempAppearance.lfCaption, &m_TempAppearance.crCaption, &m_TempRHandles, IDC_ST_BMP);
			}
			break;
		case IDC_LST_SKIN:	//show skin preview
			if(codeNotify == LBN_SELCHANGE){
				ChangeSkinPreview(hwnd, IDC_LST_SKIN, IDC_ST_BMP, &m_TempRHandles, m_sTempSkinFile, (g_RTHandles.hbSkin == m_TempRHandles.hbSkin) ? FALSE : TRUE);
				SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_GETTEXT, SendDlgItemMessageW(hwnd, IDC_LST_SKIN, LB_GETCURSEL, 0, 0), (LPARAM)m_TempAppearance.szSkin);
				//disable skinless properties when any skin is selected
				if(m_TempRHandles.hbSkin){
					EnableSkinlessProperties(FALSE);
					//change docking settings
					if(!m_TempDockingSettings.fCustSkin){
						ChangeDockingDefSkin();
					}
				}
				else
					EnableSkinlessProperties(TRUE);
			}
			break;
		case IDC_EDT_DEF_SIZE_W:
			if(codeNotify == EN_CHANGE){
				m_TempRHandles.szDef.cx = GetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_W, NULL, FALSE);
				if(!m_TempDockingSettings.fCustSize){
					ChangeDockingDefSize(IDC_EDT_DEF_SIZE_W, m_TempRHandles.szDef.cx);
				}
			}
			break;
		case IDC_EDT_DEF_SIZE_H:
			if(codeNotify == EN_CHANGE){
				m_TempRHandles.szDef.cy = GetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_H, NULL, FALSE);
				if(!m_TempDockingSettings.fCustSize){
					ChangeDockingDefSize(IDC_EDT_DEF_SIZE_H, m_TempRHandles.szDef.cy);
				}
			}
			break;
		case IDC_REDUCE_TO_CAPTION:
			if(codeNotify == BN_CLICKED){
				fChecked = SendDlgItemMessageW(hwnd, IDC_REDUCE_TO_CAPTION, BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				m_TempNoteSettings.rollOnDblClick = fChecked;	
			}
			break;
		case IDC_CHK_RANDOM_COLOR:
			if(codeNotify == BN_CLICKED){
				fChecked = SendDlgItemMessageW(hwnd, IDC_CHK_RANDOM_COLOR, BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				if(fChecked){
					BitOn(&m_TempNoteSettings.reserved1, SB1_RANDCOLOR);
					EnableWindow(GetDlgItem(hwnd, IDC_CHK_INVERT_COLOR), TRUE);
				}
				else{
					BitOff(&m_TempNoteSettings.reserved1, SB1_RANDCOLOR);	
					EnableWindow(GetDlgItem(hwnd, IDC_CHK_INVERT_COLOR), FALSE);
				}
			}
			break;
		case IDC_CHK_INVERT_COLOR:
			if(codeNotify == BN_CLICKED){
				fChecked = SendDlgItemMessageW(hwnd, IDC_CHK_INVERT_COLOR, BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				if(fChecked)
					BitOn(&m_TempNoteSettings.reserved1, SB1_INVERT_TEXT);
				else
					BitOff(&m_TempNoteSettings.reserved1, SB1_INVERT_TEXT);	
			}
			break;
	}
}

static void Skins_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if(lpDrawItem->CtlType == ODT_STATIC && lpDrawItem->CtlID == IDC_ST_BMP)
		//draw skin preview
		DrawSkinPreview(GetDlgItem(hwnd, IDC_ST_BMP), &m_TempRHandles, &m_TempAppearance, m_TempRHandles.crMask, lpDrawItem);
}

//-----------	Appearance stuff	-----------
static void SetTransValue(HWND hwnd, int value){
	wchar_t			szBuffer[32];

	_itow(value, szBuffer, 10);
	wcscat(szBuffer, L" %");
	SetDlgItemTextW(hwnd, IDC_TRANS_VALUE, szBuffer);
	m_TempNoteSettings.transValue = 255 - value * 2.55;
}

static BOOL CALLBACK Appearance_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_COMMAND, Appearance_OnCommand);
	HANDLE_MSG (hwnd, WM_CTLCOLORSTATIC, Appearance_OnCtlColorStatic);
	HANDLE_MSG (hwnd, WM_HSCROLL, Appearance_OnHScroll);

	default: return FALSE;
	}
}

static void InitDlgAppearance(HWND hwnd){
	HWND		hTrack;
	int			transVal;

	//set up track values
	hTrack = GetDlgItem(hwnd, IDC_TRANS_TRACK_BAR);
	SendMessageW(hTrack, TBM_SETRANGE, TRUE, MAKELONG(0, 100));
	SendMessageW(hTrack, TBM_SETTICFREQ, 10, 0);
	SendMessageW(hTrack, TBM_SETPAGESIZE, 0, 10);
	
	SendDlgItemMessageW(hwnd, IDC_CHK_TRANS, BM_SETCHECK, (m_TempNoteSettings.transAllow ? BST_CHECKED : BST_UNCHECKED), 0);
	EnableWindow(GetDlgItem(hwnd, IDC_GRP_TRANS), (m_TempNoteSettings.transAllow ? TRUE : FALSE));
	EnableWindow(GetDlgItem(hwnd, IDC_TRANS_LEFT_ST), (m_TempNoteSettings.transAllow ? TRUE : FALSE));
	EnableWindow(GetDlgItem(hwnd, IDC_TRANS_RIGHT_ST), (m_TempNoteSettings.transAllow ? TRUE : FALSE));
	EnableWindow(GetDlgItem(hwnd, IDC_TRANS_TRACK_BAR), (m_TempNoteSettings.transAllow ? TRUE : FALSE));
	EnableWindow(GetDlgItem(hwnd, IDC_TRANS_VALUE), (m_TempNoteSettings.transAllow ? TRUE : FALSE));
	// EnableWindow(GetDlgItem(hwnd, IDC_EDT_TRANS), (m_TempNoteSettings.transAllow ? TRUE : FALSE));
	EnableWindow(GetDlgItem(hwnd, IDC_TRANS_WARNING), (m_TempNoteSettings.transAllow ? TRUE : FALSE));
	
	if(m_TempNoteSettings.transValue)
		transVal = 100 - m_TempNoteSettings.transValue / 2.55;
	else
		transVal = 0;
	// SetDlgItemInt(hwnd, IDC_EDT_TRANS, transVal, FALSE);
	SetTransValue(hwnd, transVal);
	SendMessageW(hTrack, TBM_SETPOS, TRUE, transVal);
	SendDlgItemMessageW(hwnd, IDC_CHK_SCROLLBAR, BM_SETCHECK, (m_TempNoteSettings.showScrollbar ? BST_CHECKED : BST_UNCHECKED), 0);
	CheckDlgButton(hwnd, IDC_USE_CUST_FONTS, IsBitOn(m_TempNoteSettings.reserved1, SB1_CUST_FONTS) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(hwnd, IDC_HIDE_TOOLBAR, IsBitOn(m_TempNoteSettings.reserved1, SB1_HIDETOOLBAR) ? BST_CHECKED : BST_UNCHECKED);
}

static void Appearance_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	if(code == TB_THUMBPOSITION || code == TB_THUMBTRACK){
		SetTransValue(hwnd, pos);
	}
	else if(code == TB_LINEUP || code == TB_LINEDOWN || code == TB_PAGEUP || code == TB_PAGEDOWN || code == TB_BOTTOM || code == TB_TOP){
		SetTransValue(hwnd, SendDlgItemMessageW(hwnd, IDC_TRANS_TRACK_BAR, TBM_GETPOS, 0, 0));
	}
}

static void Appearance_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	BOOL				fChecked;

	switch(id){
	case IDC_CHK_TRANS:
		if(codeNotify == BN_CLICKED){
			fChecked = SendDlgItemMessageW(hwnd, IDC_CHK_TRANS, BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE : FALSE;
			EnableWindow(GetDlgItem(hwnd, IDC_GRP_TRANS), fChecked);
			EnableWindow(GetDlgItem(hwnd, IDC_TRANS_LEFT_ST), fChecked);
			EnableWindow(GetDlgItem(hwnd, IDC_TRANS_RIGHT_ST), fChecked);
			EnableWindow(GetDlgItem(hwnd, IDC_TRANS_TRACK_BAR), fChecked);
			EnableWindow(GetDlgItem(hwnd, IDC_TRANS_VALUE), fChecked);
			EnableWindow(GetDlgItem(hwnd, IDC_TRANS_WARNING), fChecked);
			m_TempNoteSettings.transAllow = fChecked;
		}
		break;
	case IDC_CHK_SCROLLBAR:
		fChecked = SendDlgItemMessageW(hwnd, IDC_CHK_SCROLLBAR, BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE : FALSE;
		m_TempNoteSettings.showScrollbar = fChecked;
		break;
	case IDC_HIDE_TOOLBAR:
		if(codeNotify == BN_CLICKED){
			if(IsDlgButtonChecked(hwnd, IDC_HIDE_TOOLBAR) == BST_CHECKED)
				BitOn(&m_TempNoteSettings.reserved1, SB1_HIDETOOLBAR);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_HIDETOOLBAR);	
		}
	case IDC_USE_CUST_FONTS:
		if(codeNotify == BN_CLICKED){
			if(IsDlgButtonChecked(hwnd, IDC_USE_CUST_FONTS) == BST_CHECKED)
				BitOn(&m_TempNoteSettings.reserved1, SB1_CUST_FONTS);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_CUST_FONTS);	
		}
	}
}

static HBRUSH Appearance_OnCtlColorStatic(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
	//draw "transparency warning" in red
	SetBkMode(hdc, TRANSPARENT);
	if(hwndChild == GetDlgItem(hwnd, IDC_TRANS_WARNING) || hwndChild == GetDlgItem(hwnd, IDC_ST_RELOAD_WARNING))
		SetTextColor(hdc, RGB(255, 0, 0));
	return GetSysColorBrush(COLOR_BTNFACE);
}

//-----------	Schedule stuff	-----------
static BOOL CALLBACK Schedule_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_COMMAND, Schedule_OnCommand);
	HANDLE_MSG (hwnd, WM_DRAWITEM, Schedule_OnDrawItem);

	default: return FALSE;
	}
}

static void InitDlgSchedule(HWND hwnd){
	int			index;
	wchar_t		szKey[12];

	//date/time formats
	SetDlgItemTextW(hwnd, IDC_EDT_DATE_FMTS, g_DTFormats.DateFormat);
	SetDlgItemTextW(hwnd, IDC_EDT_TIME_FMTS, g_DTFormats.TimeFormat);
	SetShowDateTime(hwnd);
	//sounds
	memcpy(&m_TempSound, &g_Sound, sizeof(SOUND_TYPE));
	memcpy(&m_PrevSound, &g_Sound, sizeof(SOUND_TYPE));
	SendDlgItemMessageW(hwnd, IDC_CHK_ALLOW_SOUND, BM_SETCHECK, (m_TempSound.allowSound ? BST_CHECKED : BST_UNCHECKED), 0);
	SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_ADDSTRING, 0, (LPARAM)DS_DEF_SOUND);
	LoadSounds(hwnd);
	
	if(m_TempSound.defSound > 0){
		EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_SOUND), TRUE);
		index = SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_FINDSTRINGEXACT, -1, (LPARAM)m_TempSound.custSound);
		if(index == LB_ERR){
			MessageBoxW(hwnd, m_sSound4, NULL, MB_OK);
			SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_SETCURSEL, 0, 0);
			EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_SOUND), FALSE);
			m_TempSound.defSound = 0;
			wcscpy(m_TempSound.custSound, DS_DEF_SOUND);
			memcpy(&g_Sound, &m_TempSound, sizeof(SOUND_TYPE));
			memcpy(&m_PrevSound, &m_TempSound, sizeof(SOUND_TYPE));
			//save settings (prevent further messages if Cancel will be clicked)
			if(m_TempSound.allowSound)
				WritePrivateProfileStringW(S_SOUND, DSK_ALLOW, __T("1"), m_sMainINI);
			else
				WritePrivateProfileStringW(S_SOUND, DSK_ALLOW, __T("0"), m_sMainINI);
			_itow(m_TempSound.defSound, szKey, 10);
			WritePrivateProfileStringW(S_SOUND, DSK_SOUND_DEF, szKey, m_sMainINI);
			WritePrivateProfileStringW(S_SOUND, DSK_SOUND_CUST, m_TempSound.custSound, m_sMainINI);
		} 
		else
			SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_SETCURSEL, index, 0);
	}
	else{
		SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_SETCURSEL, 0, 0);
		EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_SOUND), FALSE);
	}
	SendDlgItemMessageW(hwnd, IDC_VISUAL_NOTIFY, BM_SETCHECK, (m_TempNoteSettings.visualAlert ? BST_CHECKED : BST_UNCHECKED), 0);
	//tracking overdue
	if(IsBitOn(m_TempNoteSettings.reserved1, SB1_TRACK_OVERDUE))
		CheckDlgButton(hwnd, IDC_CHK_OVERDUE, BST_CHECKED);
	else
		CheckDlgButton(hwnd, IDC_CHK_OVERDUE, BST_UNCHECKED);
}

static void Schedule_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if(lpDrawItem->CtlType == ODT_LISTBOX && lpDrawItem->CtlID == IDC_LST_SOUND)
		DrawListItem(lpDrawItem);
}

static void Schedule_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	int					index;
	BOOL				fChecked;
	wchar_t				szFileName[MAX_PATH], szFileTitle[MAX_PATH];
	wchar_t 			szBuffer[256], szPath[MAX_PATH];

	switch(id){
		case IDC_EDT_DATE_FMTS:
			if(codeNotify == EN_CHANGE){
				GetDlgItemTextW(hwnd, IDC_EDT_DATE_FMTS, m_TempFormats.DateFormat, 128);
			}
			break;
		case IDC_EDT_TIME_FMTS:
			if(codeNotify == EN_CHANGE){
				GetDlgItemTextW(hwnd, IDC_EDT_TIME_FMTS, m_TempFormats.TimeFormat, 128);
			}
			break;
		case IDC_DATE_HELP:
			MessageBoxW(hwnd, g_Strings.DFormats, g_Strings.DFCaption, MB_OK);
			break;
		case IDC_TIME_HELP:
			MessageBoxW(hwnd, g_Strings.TFormats, g_Strings.TFCaption, MB_OK);
			break;
		case IDC_CHK_ALLOW_SOUND:
			if(codeNotify == BN_CLICKED){
				fChecked = SendDlgItemMessageW(hwnd, IDC_CHK_ALLOW_SOUND, BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				index = SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_GETCURSEL, 0, 0);
				EnableWindow(GetDlgItem(hwnd, IDC_LST_SOUND), fChecked);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_ADD_SOUND), fChecked);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_SOUND), (fChecked ? (index > 0 ? TRUE : FALSE) : FALSE));
				m_TempSound.allowSound = fChecked;
			}
			break;
		case IDC_CMD_ADD_SOUND:
			wcscpy(szPath, g_NotePaths.LangDir);
			wcscat(szPath, g_NotePaths.LangFile);
			GetPrivateProfileStringW(L"captions", L"sound", L"Choose sound", szBuffer, 256, szPath);
			if(ShowOpenFileDlg(hwnd, szFileName, szFileTitle, __T("Windows audio file (*.wav)\0*.wav\0\0"), szBuffer)){
				if(NotesDirExists(g_NotePaths.SoundDir)){
					if(FileExists(g_NotePaths.SoundDir, szFileTitle)){
						if(MessageBoxW(hwnd, m_sSound3, m_sSound2, MB_OKCANCEL | MB_ICONQUESTION) == IDOK){
							CopySoundFile(szFileName, szFileTitle);
						}
					}
					else{
						CopySoundFile(szFileName, szFileTitle);
						SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_ADDSTRING, 0, (LPARAM)szFileTitle);
					}
				}
				else{
					NotesDirCreate(g_NotePaths.SoundDir);
					CopySoundFile(szFileName, szFileTitle);
					SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_ADDSTRING, 0, (LPARAM)szFileTitle);
				}
			}
			break;
		case IDC_CMD_DEL_SOUND:
			index = SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_GETCURSEL, 0, 0);
			if(MessageBoxW(hwnd, m_sSound5, m_sSound2, MB_OKCANCEL | MB_ICONQUESTION) == IDOK){
				SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_GETTEXT, index, (LPARAM)szFileName);
				DeleteSoundFile(szFileName);
				SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_DELETESTRING, index, 0);
				index = SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_FINDSTRINGEXACT, -1, (LPARAM)m_PrevSound.custSound);
				memcpy(&m_TempSound, &m_PrevSound, sizeof(m_TempSound));
				if(index == LB_ERR){
					SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_SETCURSEL, 0, 0);
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_SOUND), FALSE);
				}
				else
					SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_SETCURSEL, index, 0);
			}
			break;
		case IDC_CMD_LISTEN:
			index = SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_GETCURSEL, 0, 0);
			SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_GETTEXT, index, (LPARAM)szFileName);
			PreviewSound(szFileName);			
			break;
		case IDC_LST_SOUND:
			if(codeNotify == LBN_SELCHANGE){
				index = SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_GETCURSEL, 0, 0);
				SendDlgItemMessageW(hwnd, IDC_LST_SOUND, LB_GETTEXT, index, (LPARAM)m_TempSound.custSound);
				m_TempSound.defSound = index;
				if(index > 0){
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_SOUND), TRUE);
				}
				else{
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_SOUND), FALSE);
				}
			}
			break;
		case IDC_VISUAL_NOTIFY:
			if(codeNotify == BN_CLICKED){
				fChecked = SendDlgItemMessageW(hwnd, IDC_VISUAL_NOTIFY, BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				m_TempNoteSettings.visualAlert = fChecked;	
			}
			break;
		case IDC_CHK_OVERDUE:
			if(codeNotify == BN_CLICKED){
				fChecked = SendDlgItemMessageW(hwnd, IDC_CHK_OVERDUE, BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				if(fChecked)
					BitOn(&m_TempNoteSettings.reserved1, SB1_TRACK_OVERDUE);
				else
					BitOff(&m_TempNoteSettings.reserved1, SB1_TRACK_OVERDUE);
			}
			break;
	}
}

//-----------	Behavior stuff	-----------
static BOOL CALLBACK Behavior_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_COMMAND, Behavior_OnCommand);
	HANDLE_MSG (hwnd, WM_DRAWITEM, Behavior_OnDrawItem);

	default: return FALSE;
	}
}

static void InitDlgBehavior(HWND hwnd){
	wchar_t		szKey[12];

	SendDlgItemMessageW(hwnd, IDC_ALWAYS_SAVE, BM_SETCHECK, (m_TempNoteSettings.saveOnExit ? BST_CHECKED : BST_UNCHECKED), 0);
	SendDlgItemMessageW(hwnd, IDC_ASK_B_SAVE, BM_SETCHECK, (m_TempNoteSettings.confirmSave ? BST_CHECKED : BST_UNCHECKED), 0);
	SendDlgItemMessageW(hwnd, IDC_CHK_DEL_CONFIRM, BM_SETCHECK, (m_TempNoteSettings.confirmDelete ? BST_CHECKED : BST_UNCHECKED), 0);
	SendDlgItemMessageW(hwnd, IDC_CHK_AUTOSAVE, BM_SETCHECK, (m_TempNoteSettings.autoSave ? BST_CHECKED : BST_UNCHECKED), 0);
	SendDlgItemMessageW(hwnd, IDC_CHK_NEW_ONTOP, BM_SETCHECK, (m_TempNoteSettings.newOnTop ? BST_CHECKED : BST_UNCHECKED), 0);
	EnableWindow(GetDlgItem(hwnd, IDC_EDT_AUTOSAVE), (m_TempNoteSettings.autoSave ? TRUE : FALSE));
	EnableWindow(GetDlgItem(hwnd, IDC_UPD_AUTOSAVE), (m_TempNoteSettings.autoSave ? TRUE : FALSE));
	EnableWindow(GetDlgItem(hwnd, IDC_ST_AUTOSAVE), (m_TempNoteSettings.autoSave ? TRUE : FALSE));
	_itow(m_TempNoteSettings.autoSValue, szKey, 10);
	SetWindowTextW(GetDlgItem(hwnd, IDC_EDT_AUTOSAVE), szKey);
	SendDlgItemMessageW(hwnd, IDC_EDT_AUTOSAVE, EM_LIMITTEXT, 2, 0);
	SendDlgItemMessageW(hwnd, IDC_UPD_AUTOSAVE, UDM_SETRANGE, 0, (LPARAM) MAKELONG(99, 1));
	CheckDlgButton(hwnd, IDC_CHK_REL_POSITION, IsBitOn(m_TempNoteSettings.reserved1, SB1_RELPOSITION) ? BST_CHECKED : BST_UNCHECKED);
	SendDlgItemMessageW(m_Dialogs.hBehavior, IDC_CBO_DBL_CLICK, CB_SETCURSEL, GetIndexOfDblClick(), 0);
	CheckDlgButton(hwnd, IDC_OPT_MAIN_MENU, BST_CHECKED);
	EnableWindow(GetDlgItem(hwnd, IDC_CBO_SINGLE_NOTE), FALSE);
}

static void Behavior_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if(lpDrawItem->CtlType == ODT_COMBOBOX && (lpDrawItem->CtlID == IDC_CBO_COMMANDS 
	|| lpDrawItem->CtlID == IDC_CBO_DBL_CLICK || lpDrawItem->CtlID == IDC_CBO_SINGLE_NOTE))
		//draw commands combo
		DrawComboItem(lpDrawItem);
}

static void Behavior_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	BOOL				fChecked;
	P_HK_TYPE			pHK;
	int					index;

	switch(id){
		case IDC_OPT_MAIN_MENU:
			if(codeNotify == BN_CLICKED){
				if(IsDlgButtonChecked(hwnd, IDC_OPT_MAIN_MENU) == BST_CHECKED){
					EnableWindow(GetDlgItem(hwnd, IDC_CBO_COMMANDS), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_CBO_SINGLE_NOTE), FALSE);
					index = SendDlgItemMessageW(hwnd, IDC_CBO_COMMANDS, CB_GETCURSEL, 0, 0);
					SetDlgItemTextW(hwnd, IDC_ST_HOTKEY, m_TempHotkeys[index].szKey);
					if(!wcslen(m_TempHotkeys[index].szKey))
						EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_HOT), FALSE);
					else
						EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_HOT), TRUE);

					pHK = GetHotKeyByID(SendDlgItemMessageW(hwnd, IDC_CBO_COMMANDS, CB_GETITEMDATA, SendDlgItemMessageW(hwnd, IDC_CBO_COMMANDS, CB_GETCURSEL, 0, 0), 0), m_TempHotkeys, NELEMS(m_TempHotkeys));
					if(pHK){
						SetDlgItemTextW(hwnd, IDC_ST_HOTKEY, pHK->szKey);
						if(wcslen(pHK->szKey))
							EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_HOT), TRUE);
						else
							EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_HOT), FALSE);
					}
					else
						SetDlgItemTextW(hwnd, IDC_ST_HOTKEY, NULL);
					}
			}
			break;
		case IDC_OPT_SINGLE_NOTE:
			if(codeNotify == BN_CLICKED){
				if(IsDlgButtonChecked(hwnd, IDC_OPT_SINGLE_NOTE) == BST_CHECKED){
					EnableWindow(GetDlgItem(hwnd, IDC_CBO_COMMANDS), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_CBO_SINGLE_NOTE), TRUE);
					index = SendDlgItemMessageW(hwnd, IDC_CBO_SINGLE_NOTE, CB_GETCURSEL, 0, 0);
					SetDlgItemTextW(hwnd, IDC_ST_HOTKEY, m_TempNoteHotkeys[index].szKey);
					if(!wcslen(m_TempNoteHotkeys[index].szKey))
						EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_HOT), FALSE);
					else
						EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_HOT), TRUE);

					pHK = GetHotKeyByID(SendDlgItemMessageW(hwnd, IDC_CBO_SINGLE_NOTE, CB_GETITEMDATA, SendDlgItemMessageW(hwnd, IDC_CBO_SINGLE_NOTE, CB_GETCURSEL, 0, 0), 0), m_TempNoteHotkeys, NoteMenuCount());
					if(pHK){
						SetDlgItemTextW(hwnd, IDC_ST_HOTKEY, pHK->szKey);
						if(wcslen(pHK->szKey))
							EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_HOT), TRUE);
						else
							EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_HOT), FALSE);
					}
					else
						SetDlgItemTextW(hwnd, IDC_ST_HOTKEY, NULL);
					}
			}
			break;
		case IDC_CHK_AUTOSAVE:
			if(codeNotify == BN_CLICKED){
				fChecked = SendDlgItemMessageW(hwnd, IDC_CHK_AUTOSAVE, BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE : FALSE;
				EnableWindow(GetDlgItem(hwnd, IDC_EDT_AUTOSAVE), fChecked);
				EnableWindow(GetDlgItem(hwnd, IDC_UPD_AUTOSAVE), fChecked);
				EnableWindow(GetDlgItem(hwnd, IDC_ST_AUTOSAVE), fChecked);
				m_TempNoteSettings.autoSave = fChecked;
			}
			break;
		case IDC_EDT_AUTOSAVE:
			if(codeNotify == EN_UPDATE){
				wchar_t		szValue[12];
				int			value = 1;
				GetWindowTextW(GetDlgItem(hwnd, IDC_EDT_AUTOSAVE), szValue, 12);
				if(wcslen(szValue) == 0 || _wtoi(szValue) == 0){
					wcscpy(szValue, L"1");
					SetWindowTextW(GetDlgItem(hwnd, IDC_EDT_AUTOSAVE), szValue);
				}
				value = _wtoi(szValue);
				m_TempNoteSettings.autoSValue = value;
			}
			break;
		case IDC_ALWAYS_SAVE:
			fChecked = SendDlgItemMessageW(hwnd, IDC_ALWAYS_SAVE, BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE : FALSE;
			m_TempNoteSettings.saveOnExit = fChecked;
			break;
		case IDC_ASK_B_SAVE:
			fChecked = SendDlgItemMessageW(hwnd, IDC_ASK_B_SAVE, BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE : FALSE;
			m_TempNoteSettings.confirmSave = fChecked;
			break;
		case IDC_CHK_DEL_CONFIRM:
			fChecked = SendDlgItemMessageW(hwnd, IDC_CHK_DEL_CONFIRM, BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE : FALSE;
			m_TempNoteSettings.confirmDelete = fChecked;
			break;
		case IDC_CHK_NEW_ONTOP:
			fChecked = SendDlgItemMessageW(hwnd, IDC_CHK_NEW_ONTOP, BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE : FALSE;
			m_TempNoteSettings.newOnTop = fChecked;
			break;
		case IDC_CBO_DBL_CLICK:
			if(codeNotify == CBN_SELCHANGE){
				int		index = SendDlgItemMessageW(hwnd, IDC_CBO_DBL_CLICK, CB_GETCURSEL, 0, 0);
				//erase previous settings
				BitOff(&m_TempNoteSettings.reserved2, SB2_NEW_NOTE);
				BitOff(&m_TempNoteSettings.reserved2, SB2_CONTROL_PANEL);
				BitOff(&m_TempNoteSettings.reserved2, SB2_PREFS);
				BitOff(&m_TempNoteSettings.reserved2, SB2_SEARCH_NOTES);
				//set new settings
				BitOn(&m_TempNoteSettings.reserved2, index);
			}
			break;
		case IDC_CBO_COMMANDS:
			//select menu command in combo
			if(codeNotify == CBN_SELCHANGE){
				pHK = GetHotKeyByID(SendDlgItemMessageW(hwnd, IDC_CBO_COMMANDS, CB_GETITEMDATA, SendDlgItemMessageW(hwnd, IDC_CBO_COMMANDS, CB_GETCURSEL, 0, 0), 0), m_TempHotkeys, NELEMS(m_TempHotkeys));
				if(pHK){
					SetDlgItemTextW(hwnd, IDC_ST_HOTKEY, pHK->szKey);
					if(wcslen(pHK->szKey))
						EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_HOT), TRUE);
					else
						EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_HOT), FALSE);
				}
				else
					SetDlgItemTextW(hwnd, IDC_ST_HOTKEY, NULL);
			}
			break;
		case IDC_CBO_SINGLE_NOTE:
			//select menu command in combo
			if(codeNotify == CBN_SELCHANGE){
				pHK = GetHotKeyByID(SendDlgItemMessageW(hwnd, IDC_CBO_SINGLE_NOTE, CB_GETITEMDATA, SendDlgItemMessageW(hwnd, IDC_CBO_SINGLE_NOTE, CB_GETCURSEL, 0, 0), 0), m_TempNoteHotkeys, NoteMenuCount());
				if(pHK){
					SetDlgItemTextW(hwnd, IDC_ST_HOTKEY, pHK->szKey);
					if(wcslen(pHK->szKey))
						EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_HOT), TRUE);
					else
						EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_HOT), FALSE);
				}
				else
					SetDlgItemTextW(hwnd, IDC_ST_HOTKEY, NULL);
			}
			break;
		case IDC_CMD_HOTKEY:
			//add or chanhe hot key
			if(IsDlgButtonChecked(hwnd, IDC_OPT_MAIN_MENU) == BST_CHECKED){
				pHK = GetHotKeyByID(SendDlgItemMessageW(hwnd, IDC_CBO_COMMANDS, CB_GETITEMDATA, SendDlgItemMessageW(hwnd, IDC_CBO_COMMANDS, CB_GETCURSEL, 0, 0), 0), m_TempHotkeys, NELEMS(m_TempHotkeys));
			}
			else{
				pHK = GetHotKeyByID(SendDlgItemMessageW(hwnd, IDC_CBO_SINGLE_NOTE, CB_GETITEMDATA, SendDlgItemMessageW(hwnd, IDC_CBO_SINGLE_NOTE, CB_GETCURSEL, 0, 0), 0), m_TempNoteHotkeys, NoteMenuCount());
			}
			memcpy(&m_HKCurrent, pHK, sizeof(m_HKCurrent));	
			if(DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_HOT_KEY), hwnd, Hot_DlgProc, 0) == IDOK){
				if(pHK->fsModifiers != m_HKCurrent.fsModifiers || pHK->vk != m_HKCurrent.vk){
					memcpy(pHK, &m_HKCurrent, sizeof(m_HKCurrent));
					SetDlgItemTextW(hwnd, IDC_ST_HOTKEY, pHK->szKey);
				}
			}
			if(wcslen(pHK->szKey))
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_HOT), TRUE);
			else
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_HOT), FALSE);
			ZeroMemory(&m_HKCurrent, sizeof(m_HKCurrent));
			break;
		case IDC_CMD_DEL_HOT:
			//delete hot key
			if(MessageBoxW(hwnd, m_sHK3, g_Strings.DCaption, MB_YESNO | MB_ICONQUESTION) == IDYES){
				if(IsDlgButtonChecked(hwnd, IDC_OPT_MAIN_MENU) == BST_CHECKED){
					pHK = GetHotKeyByID(SendDlgItemMessageW(hwnd, IDC_CBO_COMMANDS, CB_GETITEMDATA, SendDlgItemMessageW(hwnd, IDC_CBO_COMMANDS, CB_GETCURSEL, 0, 0), 0), m_TempHotkeys, NELEMS(m_TempHotkeys));
				}
				else{
					pHK = GetHotKeyByID(SendDlgItemMessageW(hwnd, IDC_CBO_SINGLE_NOTE, CB_GETITEMDATA, SendDlgItemMessageW(hwnd, IDC_CBO_SINGLE_NOTE, CB_GETCURSEL, 0, 0), 0), m_TempNoteHotkeys, NoteMenuCount());
				}
				pHK->fsModifiers = 0;
				pHK->identifier = 0;
				pHK->szKey[0] = '\0';
				pHK->vk = 0;
				SetDlgItemTextW(hwnd, IDC_ST_HOTKEY, pHK->szKey);
				EnableWindow(GetDlgItem(hwnd, IDC_CMD_DEL_HOT), FALSE);
			}
			break;
		case IDC_CHK_REL_POSITION:
			if(codeNotify == BN_CLICKED){
				if(IsDlgButtonChecked(hwnd, IDC_CHK_REL_POSITION) == BST_CHECKED)
					BitOn(&m_TempNoteSettings.reserved1, SB1_RELPOSITION);
				else
					BitOff(&m_TempNoteSettings.reserved1, SB1_RELPOSITION);
			}
			break;
	}
}

//-----------	Docks stuff	-----------
static BOOL CALLBACK Docks_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_COMMAND, Docks_OnCommand);
	HANDLE_MSG (hwnd, WM_HSCROLL, Docks_OnHScroll);
	HANDLE_MSG (hwnd, WM_DRAWITEM, Docks_OnDrawItem);
	default: return FALSE;
	}
}

static void Docks_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if(lpDrawItem->CtlType == ODT_STATIC){
		if(lpDrawItem->CtlID == IDC_ST_DOCK_SKIN){
			//draw skin preview
			DrawSkinPreviewDock(lpDrawItem->hwndItem, &m_TempDRTHandles, &m_TempDockingSettings, CLR_MASK, g_Appearance.crFont, lpDrawItem);
		}
		else if(lpDrawItem->CtlID == IDC_ST_DOCK_SKINLESS){
			DrawSimpleNotePreviewDock(lpDrawItem->hwndItem, lpDrawItem, &m_TempDockingSettings, g_Appearance.crFont, &m_TempDRTHandles);
		}
	}
}

static void Docks_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	// CHOOSEFONTW			cf;
	// LOGFONTW			lfw;
	// wchar_t 			szBuffer[256], szPath[MAX_PATH];

	switch(id){
		// case IDC_CMD_DOCK_FONT:	//choose font
			// if(codeNotify == BN_CLICKED){
				// wcscpy(szPath, g_NotePaths.LangDir);
				// wcscat(szPath, g_NotePaths.LangFile);
				// GetPrivateProfileStringW(L"captions", L"font", L"Choose font", szBuffer, 256, szPath);
				// memcpy(&lfw, &m_TempDockingSettings.lfFont, sizeof(lfw));
				// ZeroMemory(&cf, sizeof(cf));
				// cf.lStructSize = sizeof(cf);
				// cf.Flags = CF_SCREENFONTS | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT | CF_ENABLEHOOK;
				// cf.hwndOwner = hwnd;
				// cf.lpLogFont = &lfw;
				// cf.rgbColors = m_TempDockingSettings.crFont;
				// cf.lpfnHook = CFHookProc;
				// cf.lCustData = (int)szBuffer;
				// if(ChooseFontW(&cf)){
					// memcpy(&m_TempDockingSettings.lfFont, &lfw, sizeof(lfw));
					// m_TempDRTHandles.hFont = CreateFontIndirectW(&m_TempDockingSettings.lfFont);
					// m_TempDockingSettings.crFont = (cf.rgbColors == 0) ? 1 : ((cf.rgbColors == m_TempDRTHandles.crMask) ? m_TempDRTHandles.crMask + 1 : cf.rgbColors);
					// RedrawWindow(GetDlgItem(hwnd, IDC_ST_DOCK_SKIN), NULL, NULL, RDW_INVALIDATE);
					// RedrawWindow(GetDlgItem(hwnd, IDC_ST_DOCK_SKINLESS), NULL, NULL, RDW_INVALIDATE);
				// }
			// }
			// break;
		case IDC_CMD_SKNLESS_BCOLOR:
			if(codeNotify == BN_CLICKED){
				ChangeSkinlessColor(hwnd, &m_TempDockingSettings.crWindow, IDC_ST_DOCK_SKINLESS);
			}
			break;
		case IDC_CMD_SKNLESS_CFONT:
			if(codeNotify == BN_CLICKED){
				ChangeSkinlessCaptionFont(hwnd, &m_TempDockingSettings.lfCaption, &m_TempDockingSettings.crCaption, &m_TempDRTHandles, IDC_ST_DOCK_SKINLESS);
			}
			break;
		case IDC_EDT_DEF_SIZE_W:
			if(codeNotify == EN_UPDATE && m_TempDockingSettings.fCustSize){
				m_TempDockingSettings.custSize.cx = GetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_W, FALSE, FALSE);
			}
			break;
		case IDC_EDT_DEF_SIZE_H:
			if(codeNotify == EN_UPDATE && m_TempDockingSettings.fCustSize){
				m_TempDockingSettings.custSize.cy = GetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_H, FALSE, FALSE);
			}
			break;
		case IDC_OPT_DOCK_DEF_SKIN:
			if(codeNotify == BN_CLICKED){
				if(IsDlgButtonChecked(hwnd, IDC_OPT_DOCK_DEF_SKIN)){
					EnableWindow(GetDlgItem(hwnd, IDC_CBO_SKINS), FALSE);
					m_TempDockingSettings.fCustSkin = FALSE;
					if(m_TempRHandles.hbSkin)
						wcscpy(m_TempDockingSettings.szCustSkin, m_TempAppearance.szSkin);
					else
						*m_TempDockingSettings.szCustSkin = '\0';
					GetDockSkinProperties(hwnd, &m_TempDRTHandles, m_TempDockingSettings.szCustSkin, FALSE);
					RedrawWindow(GetDlgItem(hwnd, IDC_ST_DOCK_SKIN), NULL, NULL, RDW_INVALIDATE);
				}
			}
			break;
		case IDC_OPT_DOCK_RND_SKIN:
			if(codeNotify == BN_CLICKED){
				if(IsDlgButtonChecked(hwnd, IDC_OPT_DOCK_RND_SKIN)){
					EnableWindow(GetDlgItem(hwnd, IDC_CBO_SKINS), TRUE);
					m_TempDockingSettings.fCustSkin = TRUE;
					wchar_t		szTemp[128];

					SendDlgItemMessageW(hwnd, IDC_CBO_SKINS, CB_GETLBTEXT, SendDlgItemMessageW(hwnd, IDC_CBO_SKINS, CB_GETCURSEL, 0, 0), (LPARAM)szTemp);
					wcscpy(m_TempDockingSettings.szCustSkin, szTemp);
					GetDockSkinProperties(hwnd, &m_TempDRTHandles, m_TempDockingSettings.szCustSkin, FALSE);
					RedrawWindow(GetDlgItem(hwnd, IDC_ST_DOCK_SKIN), NULL, NULL, RDW_INVALIDATE);
				}
			}
			break;
		case IDC_INV_DOCK_ORDER:
			if(codeNotify == BN_CLICKED){
				if(SendDlgItemMessageW(hwnd, IDC_INV_DOCK_ORDER, BM_GETCHECK, 0, 0) == BST_CHECKED){
					m_TempDockingSettings.fInvOrder = TRUE;
				}
				else{
					m_TempDockingSettings.fInvOrder = FALSE;
				}
			}
			break;
		// case IDC_OPT_DOCK_DEF_SIZE:
			// if(codeNotify == BN_CLICKED){
				// if(IsDlgButtonChecked(hwnd, IDC_OPT_DOCK_DEF_SIZE)){
					// EnableWindow(GetDlgItem(hwnd, IDC_ST_DEF_SIZE_W), FALSE);
					// EnableWindow(GetDlgItem(hwnd, IDC_ST_DEF_SIZE_H), FALSE);
					// EnableWindow(GetDlgItem(hwnd, IDC_EDT_DEF_SIZE_W), FALSE);
					// EnableWindow(GetDlgItem(hwnd, IDC_EDT_DEF_SIZE_H), FALSE);
					// m_TempDockingSettings.fCustSize = FALSE;
				// }
			// }
			// break;
		case IDC_OPT_DOCK_RND_SIZE:
			if(codeNotify == BN_CLICKED){
				if(IsDlgButtonChecked(hwnd, IDC_OPT_DOCK_RND_SIZE) == BST_CHECKED){
					EnableWindow(GetDlgItem(hwnd, IDC_ST_DEF_SIZE_W), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_ST_DEF_SIZE_H), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_EDT_DEF_SIZE_W), TRUE);
					EnableWindow(GetDlgItem(hwnd, IDC_EDT_DEF_SIZE_H), TRUE);
					m_TempDockingSettings.fCustSize = TRUE;
				}
				else{
					EnableWindow(GetDlgItem(hwnd, IDC_ST_DEF_SIZE_W), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_ST_DEF_SIZE_H), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_EDT_DEF_SIZE_W), FALSE);
					EnableWindow(GetDlgItem(hwnd, IDC_EDT_DEF_SIZE_H), FALSE);
					m_TempDockingSettings.fCustSize = FALSE;
				}
			}
			break;
		case IDC_CHK_DOCK_C:
			if(codeNotify == BN_CLICKED){
				if(IsDlgButtonChecked(hwnd, IDC_CHK_DOCK_C) == BST_CHECKED){
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_SKNLESS_BCOLOR), TRUE);
					m_TempDockingSettings.fCustColor = TRUE;
				}
				else{
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_SKNLESS_BCOLOR), FALSE);
					m_TempDockingSettings.fCustColor = FALSE;
				}
			}
			break;
		case IDC_CHK_DOCK_F:
			if(codeNotify == BN_CLICKED){
				if(IsDlgButtonChecked(hwnd, IDC_CHK_DOCK_F) == BST_CHECKED){
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_SKNLESS_CFONT), TRUE);
					m_TempDockingSettings.fCustCaption = TRUE;
				}
				else{
					EnableWindow(GetDlgItem(hwnd, IDC_CMD_SKNLESS_CFONT), FALSE);
					m_TempDockingSettings.fCustCaption = FALSE;
				}
			}
			break;
		// case IDC_CHK_DOCK_CUST_F:
			// if(codeNotify == BN_CLICKED){
				// if(IsDlgButtonChecked(hwnd, IDC_CHK_DOCK_CUST_F) == BST_CHECKED){
					// EnableWindow(GetDlgItem(hwnd, IDC_CMD_DOCK_FONT), TRUE);
					// m_TempDockingSettings.fCustFont = TRUE;
				// }
				// else{
					// EnableWindow(GetDlgItem(hwnd, IDC_CMD_DOCK_FONT), FALSE);
					// m_TempDockingSettings.fCustFont = FALSE;
				// }
			// }
			// break;
		case IDC_CBO_SKINS:
			if(codeNotify == CBN_SELENDOK){
				wchar_t		szTemp[128];

				SendDlgItemMessageW(hwnd, IDC_CBO_SKINS, CB_GETLBTEXT, SendDlgItemMessageW(hwnd, IDC_CBO_SKINS, CB_GETCURSEL, 0, 0), (LPARAM)szTemp);
				wcscpy(m_TempDockingSettings.szCustSkin, szTemp);
				GetDockSkinProperties(hwnd, &m_TempDRTHandles, m_TempDockingSettings.szCustSkin, FALSE);
				RedrawWindow(GetDlgItem(hwnd, IDC_ST_DOCK_SKIN), NULL, NULL, RDW_INVALIDATE);
			}
	}
}

static void Docks_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	if(code == TB_THUMBPOSITION || code == TB_THUMBTRACK){
		m_TempDockingSettings.dist = pos;
		// SetDockDistance(hwnd, pos);
	}
	else if(code == TB_LINEUP || code == TB_LINEDOWN || code == TB_PAGEUP || code == TB_PAGEDOWN || code == TB_BOTTOM || code == TB_TOP){
		// SetDockDistance(hwnd, );
		m_TempDockingSettings.dist = SendDlgItemMessageW(hwnd, IDC_DOCK_SLIDER, TBM_GETPOS, 0, 0);
	}
}

// static void SetDockDistance(HWND hwnd, int val){
	// wchar_t		szText[64], szVal[32];

	// _itow(val, szVal, 10);
	// wcscpy(szText, szVal);	
	// wcscat(szText, L" px");
	// SetDlgItemTextW(hwnd, IDC_DOCK_ST_SLIDER_VAL, szText);
	// m_TempDockingSettings.dist = val;
// }

static void InitDlgDocks(HWND hwnd){
	HWND		hTrack;
	wchar_t		szSkin[64];

	CheckDlgButton(hwnd, IDC_INV_DOCK_ORDER, m_TempDockingSettings.fInvOrder ? BST_CHECKED : BST_UNCHECKED);
	hTrack = GetDlgItem(hwnd, IDC_DOCK_SLIDER);
	SendMessageW(hTrack, TBM_SETRANGE, TRUE, MAKELONG(0, 5));
	SendMessageW(hTrack, TBM_SETTICFREQ, 1, 0);
	SendMessageW(hTrack, TBM_SETPAGESIZE, 0, 1);
	SendMessageW(hTrack, TBM_SETPOS, TRUE, m_TempDockingSettings.dist);
	// SetDockDistance(hwnd, m_TempDockingSettings.dist);
	
	SendDlgItemMessageW(hwnd, IDC_EDT_DEF_SIZE_W, EM_LIMITTEXT, 3, 0);
	SendDlgItemMessageW(hwnd, IDC_EDT_DEF_SIZE_H, EM_LIMITTEXT, 3, 0);
	if(m_TempDockingSettings.fCustSkin){
		CheckDlgButton(hwnd, IDC_OPT_DOCK_RND_SKIN, BST_CHECKED);
		FillComboSkins(hwnd, IDC_CBO_SKINS, m_TempDockingSettings.szCustSkin);
		GetDockSkinProperties(hwnd, &m_TempDRTHandles, m_TempDockingSettings.szCustSkin, FALSE);
	}
	else{
		CheckDlgButton(hwnd, IDC_OPT_DOCK_DEF_SKIN, BST_CHECKED);
		if(wcslen(g_DockSettings.szCustSkin) > 0){
			wcscpy(szSkin, g_DockSettings.szCustSkin);
		}
		else{
			wcscpy(szSkin, g_Appearance.szSkin);
			szSkin[wcslen(szSkin) - 4] = '\0';
		}
		FillComboSkins(hwnd, IDC_CBO_SKINS, szSkin);
		if(!m_TempRHandles.hbSkin){
			SendDlgItemMessageW(hwnd, IDC_CBO_SKINS, CB_SETCURSEL, 0, 0);
			// SendDlgItemMessageW(hwnd, IDC_CBO_SKINS, CB_GETLBTEXT, (WPARAM)SendDlgItemMessageW(hwnd, IDC_CBO_SKINS, CB_GETCURSEL, 0, 0), (LPARAM)szSkin);
		}
		GetDockSkinProperties(hwnd, &m_TempDRTHandles, szSkin, FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_CBO_SKINS), FALSE);
	}
	if(m_TempDockingSettings.fCustSize){
		CheckDlgButton(hwnd, IDC_OPT_DOCK_RND_SIZE, BST_CHECKED);
		SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_W, m_TempDockingSettings.custSize.cx, FALSE);
		SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_H, m_TempDockingSettings.custSize.cy, FALSE);
	}
	else{
		CheckDlgButton(hwnd, IDC_OPT_DOCK_RND_SIZE, BST_UNCHECKED);
		if(m_TempDockingSettings.custSize.cx !=0 && m_TempDockingSettings.custSize.cy != 0){
			SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_W, m_TempDockingSettings.custSize.cx, FALSE);
			SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_H, m_TempDockingSettings.custSize.cy, FALSE);
		}
		else{
			if(m_TempRHandles.hbSkin){
				SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_W, DEF_FLAT_WIDTH, FALSE);
				SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_H, DEF_FLAT_HEIGHT, FALSE);
			}
			else{
				SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_W, m_TempRHandles.szDef.cx, FALSE);
				SetDlgItemInt(hwnd, IDC_EDT_DEF_SIZE_H, m_TempRHandles.szDef.cy, FALSE);
			}
		}
		EnableWindow(GetDlgItem(hwnd, IDC_ST_DEF_SIZE_W), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_ST_DEF_SIZE_H), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_EDT_DEF_SIZE_W), FALSE);
		EnableWindow(GetDlgItem(hwnd, IDC_EDT_DEF_SIZE_H), FALSE);
	}
	if(m_TempDockingSettings.fCustColor){
		CheckDlgButton(hwnd, IDC_CHK_DOCK_C, BST_CHECKED);
		EnableWindow(GetDlgItem(hwnd, IDC_CMD_SKNLESS_BCOLOR), TRUE);
	}
	else{
		CheckDlgButton(hwnd, IDC_CHK_DOCK_C, BST_UNCHECKED);
		EnableWindow(GetDlgItem(hwnd, IDC_CMD_SKNLESS_BCOLOR), FALSE);
	}
	if(m_TempDockingSettings.fCustCaption){
		CheckDlgButton(hwnd, IDC_CHK_DOCK_F, BST_CHECKED);
		EnableWindow(GetDlgItem(hwnd, IDC_CMD_SKNLESS_CFONT), TRUE);
	}
	else{
		CheckDlgButton(hwnd, IDC_CHK_DOCK_F, BST_UNCHECKED);
		EnableWindow(GetDlgItem(hwnd, IDC_CMD_SKNLESS_CFONT), FALSE);
	}
	// if(m_TempDockingSettings.fCustFont){
		// CheckDlgButton(hwnd, IDC_CHK_DOCK_CUST_F, BST_CHECKED);
		// EnableWindow(GetDlgItem(hwnd, IDC_CMD_DOCK_FONT), TRUE);
	// }
	// else{
		// CheckDlgButton(hwnd, IDC_CHK_DOCK_CUST_F, BST_UNCHECKED);
		// EnableWindow(GetDlgItem(hwnd, IDC_CMD_DOCK_FONT), FALSE);
	// }
}

static void FillComboSkins(HWND hwnd, int id, const wchar_t * lpSkinCurrent){
	WIN32_FIND_DATAW 	fd;
	wchar_t				filespec[MAX_PATH];
	wchar_t				szSkin[256];
	HANDLE 				handle = INVALID_HANDLE_VALUE;
	BOOL 				result = TRUE;
	LRESULT 			added;

	wcscpy(filespec, g_NotePaths.SkinDir);
	wcscat(filespec, __T("*.skn"));
	handle = FindFirstFileW(filespec, &fd);
	if(handle != INVALID_HANDLE_VALUE){
		while(result){
			wcscpy(szSkin, fd.cFileName);
			szSkin[wcslen(szSkin) - 4] = '\0';
			added = SendDlgItemMessageW(hwnd, id, CB_ADDSTRING, 0, (LPARAM)szSkin);
			if(wcscmp(szSkin, lpSkinCurrent) == 0){
				SendDlgItemMessageW(hwnd, id, CB_SETCURSEL, added, 0);
			}
			result = FindNextFileW(handle, &fd);
		}
		FindClose(handle);
	}
}

//-----------	Misc stuff	-----------
static BOOL CALLBACK Misc_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_COMMAND, Misc_OnCommand);

	default: return FALSE;
	}
}

static void InitDlgMisc(HWND hwnd){
	SendDlgItemMessageW(hwnd, IDC_CHK_NEW_V, BM_SETCHECK, (m_TempNoteSettings.checkOnStart ? BST_CHECKED : BST_UNCHECKED), 0);
	// SendDlgItemMessageW(hwnd, IDC_CHK_SHOW, BM_SETCHECK, (m_TempNoteSettings.showOnStart ? BST_CHECKED : BST_UNCHECKED), 0);
	SendDlgItemMessageW(hwnd, IDC_CHK_AUTORUN, BM_SETCHECK, (m_TempNoteSettings.onStartup ? BST_CHECKED : BST_UNCHECKED), 0);
	SetDlgItemTextW(hwnd, IDC_EDT_DEF_BROWSER, m_sTempdefBrowser);
	CheckDlgButton(hwnd, IDC_CHK_SHOW_CP, IsBitOn(m_TempNoteSettings.reserved1, SB1_SHOW_CP) ? BST_CHECKED : BST_UNCHECKED);
}

static void Misc_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	BOOL				fChecked;
	wchar_t				szFileName[MAX_PATH], szFileTitle[MAX_PATH];
	wchar_t 			szBuffer[256], szPath[MAX_PATH];

	switch(id){
	case IDC_CMD_PREV_VERSION:{
		if(codeNotify == BN_CLICKED){
			wcscpy(szPath, g_NotePaths.LangDir);
			wcscat(szPath, g_NotePaths.LangFile);
			GetPrivateProfileStringW(L"captions", L"import", L"Import notes from previous versions", szBuffer, 256, szPath);
			if(ShowOpenFileDlg(hwnd, szFileName, szFileTitle, OLD_V_OPEN_FILE_FILTER, szBuffer)){
				GetPrevious(szFileName);
			}
		}
		break;
	}
	case IDC_CMD_NEW_V:
		if(codeNotify == BN_CLICKED){
			m_CheckingFromButton = TRUE;
			StartUpdateProcess();
		}
		break;
	case IDC_CHK_NEW_V:
		fChecked = SendDlgItemMessageW(hwnd, IDC_CHK_NEW_V, BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE : FALSE;
		m_TempNoteSettings.checkOnStart = fChecked;
		break;
	// case IDC_CHK_SHOW:
		// fChecked = SendDlgItemMessageW(hwnd, IDC_CHK_SHOW, BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE : FALSE;
		// m_TempNoteSettings.showOnStart = fChecked;
		// break;
	case IDC_CHK_AUTORUN:
		fChecked = SendDlgItemMessageW(hwnd, IDC_CHK_AUTORUN, BM_GETCHECK, 0, 0) == BST_CHECKED ? TRUE : FALSE;
		m_TempNoteSettings.onStartup = fChecked;
		break;
	case IDC_EDT_DEF_BROWSER:
		if(codeNotify == EN_UPDATE){
			GetDlgItemTextW(hwnd, IDC_EDT_DEF_BROWSER, m_sTempdefBrowser, MAX_PATH);
		}
		break;
	case IDC_CMD_DEF_BROWSER:
		if(codeNotify == BN_CLICKED){
			wchar_t		szPath[MAX_PATH], szBuffer[256], szFileName[MAX_PATH], szFileTitle[128];
			wcscpy(szPath, g_NotePaths.LangDir);
			wcscat(szPath, g_NotePaths.LangFile);
			GetPrivateProfileStringW(L"captions", L"browser", L"Choose preferred browser", szBuffer, 256, szPath);
			if(ShowOpenFileDlg(hwnd, szFileName, szFileTitle, __T("Executable files (*.exe)\0*.exe\0\0"), szBuffer)){
				SetDlgItemTextW(hwnd, IDC_EDT_DEF_BROWSER, szFileName);
			}
		}
		break;
	case IDC_CHK_SHOW_CP:
		if(codeNotify == BN_CLICKED){
			if(IsDlgButtonChecked(hwnd, IDC_CHK_SHOW_CP) == BST_CHECKED)
				BitOn(&m_TempNoteSettings.reserved1, SB1_SHOW_CP);
			else
				BitOff(&m_TempNoteSettings.reserved1, SB1_SHOW_CP);
		}
		break;
	}
}

static int GetIndexOfDblClick(void){
	if(IsBitOn(g_NoteSettings.reserved2, SB2_NEW_NOTE))
		return 0;
	else if(IsBitOn(g_NoteSettings.reserved2, SB2_CONTROL_PANEL))
		return 1;
	else if(IsBitOn(g_NoteSettings.reserved2, SB2_PREFS))
		return 2;
	else if(IsBitOn(g_NoteSettings.reserved2, SB2_SEARCH_NOTES))
		return 3;
	else
		return 0;
}

static HWND NoteFromClipboard(void){
	HGLOBAL			hglb;
	wchar_t			*lptstr;
	HWND			hNote = NULL;

	if(!OpenClipboard(g_hMain))
		return NULL;
	hglb = GetClipboardData(CF_UNICODETEXT); 
    if(hglb){
		lptstr = (wchar_t *)GlobalLock(hglb);
		if(lptstr){
			hNote = CreateNewNote();
			if(hNote){
				SendMessageW((HWND)GetPropW(hNote, PH_EDIT), EM_REPLACESEL, FALSE, (LPARAM)lptstr);
			}
			GlobalUnlock(hglb);
		}
	}
	CloseClipboard();
	return hNote;
}

static HWND CreateNewNote(void){
	HWND		hNote = NULL;
	PMEMNOTE	pNote;

	pNote = AddMemNote();
	if(pNote){
		LoadNoteProperties(pNote, &g_RTHandles, NULL, FALSE, NULL);
		hNote = CreateNote(pNote, g_hInstance, g_hMain, FALSE, NULL);
	}
	if(!hNote){
		MessageBoxW(g_hMain, m_sNoWindowMessage, NULL, MB_OK);
		return NULL;
	}
	// if(g_hCPDialog)
		// SendMessageW(g_hCPDialog, CHM_RELOAD, 0, 0);
	return hNote;
}

static void GetVersionNumber(void){

	char szPath[MAX_PATH], szBuffer[MAX_PATH];
	char FORMAT_STRING[] = "\\StringFileInfo\\%04x%04x\\%s";
	DWORD dwSize, dwBytes = 0;
	HGLOBAL hMem;
	UINT cbLang;
	LPVOID lpt;
	UINT cbBuffSize;
	WORD * langInfo;

	m_Version[0] = '\0';
	GetModuleFileName(g_hInstance, szPath, MAX_PATH);
	dwBytes = GetFileVersionInfoSize(szPath, &dwSize);
	if(dwBytes){
		hMem = GlobalAlloc(GMEM_ZEROINIT | GMEM_FIXED, dwBytes);
		if(GetFileVersionInfo(szPath, 0, dwBytes, hMem)){
			if(VerQueryValue(hMem, "\\VarFileInfo\\Translation", (LPVOID*)&langInfo, &cbLang)){
				wsprintf(szBuffer, FORMAT_STRING, langInfo[0], langInfo[1], "FileVersion");
				if(VerQueryValue(hMem, szBuffer, &lpt, &cbBuffSize)){
					strcpy(m_Version, lpt);
				}
			}
		}
	}
}

static void StartUpdateProcess(void){
	HANDLE		hThread;
	DWORD		dwID;

	hThread = CreateThread(NULL, 0, CheckForUpdateFunc, g_hMain, 0, &dwID);
	if(hThread){
		CloseHandle(hThread);
	}
}

static void ShowNewVersionBaloon(wchar_t * szNewVersion){

	wchar_t		szBuffer[256];

	wcscpy(szBuffer, g_Strings.NewVersion1);
	wcscat(szBuffer, L" - ");
	wcscat(szBuffer, szNewVersion);
	wcscat(szBuffer, L".");
	if(m_CheckingFromButton){
		wcscat(szBuffer, L"\n");
		wcscat(szBuffer, g_Strings.NewVersion2);
		if(MessageBoxW(m_Dialogs.hMisc, szBuffer, g_Strings.CheckUpdate, MB_OKCANCEL | MB_ICONINFORMATION) == IDOK){
			if(wcslen(g_DefBrowser) == 0)
				ShellExecuteW(g_hMain, L"open", DOWNLOAD_PAGE, NULL, NULL, SW_SHOWDEFAULT);
			else{
				if((int)ShellExecuteW(g_hMain, NULL, g_DefBrowser, DOWNLOAD_PAGE, NULL, SW_SHOWDEFAULT) <= 32){
					ShellExecuteW(g_hMain, L"open", DOWNLOAD_PAGE, NULL, NULL, SW_SHOWDEFAULT);
				}
			}
		}
	}
	else{
		m_TrackBaloonClick = TRUE;
		m_nData.uFlags = NIF_INFO;
		m_nData.uTimeout = 15000;
		m_nData.dwInfoFlags = NIIF_INFO;
		wcscpy(m_nData.szInfoTitle, g_Strings.CheckUpdate);
		wcscpy(m_nData.szInfo, szBuffer);
		Shell_NotifyIconW(NIM_MODIFY, &m_nData);
	}
}

static BOOL IsNewDLLVersion(char * szLib){
	DLLVERSIONINFO		dvi;
	HMODULE				hLib;
	FARPROC				hProc;
	BOOL				bReturn = FALSE;

	dvi.cbSize = sizeof(dvi);
	hLib = LoadLibrary(szLib);
	if(hLib){
		hProc = GetProcAddress(hLib, "DllGetVersion");
		if(hProc){
			__asm{
				lea eax, dvi
				push eax
				call hProc
			}
			if(dvi.dwMajorVersion > 4)
				bReturn = TRUE;
			else
				bReturn = FALSE;
		}
		else{
			bReturn = FALSE;
		}
		FreeLibrary(hLib);
	}
	return bReturn;
}

static void UnregisterLogonMessage(HWND hwnd){
	HMODULE						hLib;
	UNREGSESSIONMESSAGES		hProc;

	hLib = LoadLibrary("Wtsapi32.dll");
	if(hLib){
		hProc = (UNREGSESSIONMESSAGES)GetProcAddress(hLib, "WTSUnRegisterSessionNotification");
		if(hProc){
			(hProc)(hwnd);
		}
		FreeLibrary(hLib);
	}
}

static void RegisterLogonMessage(HWND hwnd){
	HMODULE						hLib;
	REGSESSIONMESSAGES			hProc;
	BOOL						result;

	hLib = LoadLibrary("Wtsapi32.dll");
	if(hLib){
		hProc = (REGSESSIONMESSAGES)GetProcAddress(hLib, "WTSRegisterSessionNotification");
		if(hProc){
			result = (BOOL)(hProc)(hwnd, NOTIFY_FOR_ALL_SESSIONS);
			if(result){
				m_TrackLogon = TRUE;
			}
			else{
				m_TrackLogon = FALSE;
			}
		}
		else{
			m_TrackLogon = FALSE;
		}
		FreeLibrary(hLib);
	}
}

static int WinVer(void){
	OSVERSIONINFOW	osv;
	HMODULE			hLib;
	FARPROC			hProc;
	int				result = 4;

	osv.dwOSVersionInfoSize = sizeof(osv);
	GetVersionExW(&osv);
	if(osv.dwMajorVersion > 5 ){
		result = 6;
	}
	else if(osv.dwMajorVersion == 5 && osv.dwMinorVersion >=1){
		result = 6;
	}
	else if(osv.dwMajorVersion == 5){
		result = 5;
	}
	if(result > 4){
		hLib = LoadLibrary("Wtsapi32.dll");
		if(hLib){
			hProc = GetProcAddress(hLib, "WTSRegisterSessionNotification");
			if(hProc){
				m_TrackLogon = TRUE;
			}
			else{
				m_TrackLogon = FALSE;
			}
			FreeLibrary(hLib);
		}
	}
	return result;
}

static void RearrangeDockWindowsOnstart(dock_type dockType){
	PDOCKHEADER			pHeader;
	PDOCKITEM			pTemp;
	int					index = 0;
	P_NOTE_DATA			pD;
	HWND				hTray = NULL;

	pHeader = DHeader(dockType);
	if(pHeader->count > 0){
		SortDockList(pHeader);
		pTemp = pHeader->pNext;
		while(pTemp){
			pTemp->index = index++;
			pD = NoteData(pTemp->hwnd);
			SetDockIndex(&pD->dockData, pTemp->index);
			SaveNewDockData(pTemp->hwnd, pD->dockData);
			hTray = MoveDockWindow(pTemp->hwnd, dockType, pTemp->index);
			if(g_RTHandles.hbSkin){
				if(g_DockSettings.fCustSkin){
					SendMessageW(pTemp->hwnd,CHM_APPLY_NEW_SKIN,(WPARAM)&g_DRTHandles, 0);
				}
			}
			else{
				if(g_DockSettings.fCustColor || g_DockSettings.fCustCaption){
					ApplyDockColors(MEMNOTE(pTemp->hwnd));
				}
				RedrawWindow(pTemp->hwnd, NULL, NULL, RDW_INVALIDATE);
				ShowNoteMarks(pTemp->hwnd);
			}

			if(!pD->onTop){
				SendMessageW(pTemp->hwnd, WM_COMMAND, MAKEWPARAM(IDM_ON_TOP, 1), 0);
			}
			pTemp = pTemp->pNext;
		}
		if(hTray)
			SetWindowPos(hTray, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	}
}

static void GetDockSkinProperties(HWND hwnd, P_NOTE_RTHANDLES pH, wchar_t * lpSkin, BOOL fDeletePrev){
	wchar_t			szTemp[256];

	// DeleteFile("C:\\abc\\PortablePellesC\\PellesC\\Projects\\PNotes 4.0\\test.ini");
	wcscpy(szTemp, lpSkin);
	wcscat(szTemp, L".skn");
	// WritePrivateProfileString("test", "1", "1", "C:\\abc\\PortablePellesC\\PellesC\\Projects\\PNotes 4.0\\test.ini");
	pH->hbSkin = GetSkinById(hwnd, IDS_SKIN_BODY, szTemp);
	// WritePrivateProfileString("test", "2", "1", "C:\\abc\\PortablePellesC\\PellesC\\Projects\\PNotes 4.0\\test.ini");
	GetSkinProperties(hwnd, pH, szTemp, fDeletePrev);
} 

static void BuildFavoritesMenu(void){

	HMENU					hFav;
	int						count, id = 0;
	wchar_t					*pw, *pws, szSection[10 * 1024];
	MItem					mit;
	P_FAV_MENU_STRUCT		pFavs, pTemp;

	ZeroMemory(&mit, sizeof(mit));
	mit.xPos = -1;
	mit.yPos = -1;
	mit.xCheck = -1;
	mit.yCheck = -1;

	hFav = GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_FAVORITES));
	count = GetMenuItemCount(hFav);

	for(int i = count - 1; i >= 0; i--){
		FreeSingleMenu(hFav, i);
		DeleteMenu(hFav, i, MF_BYPOSITION);
	}
	GetPrivateProfileStringW(S_FAVORITES, NULL, NULL, szSection, 10 * 1024, g_NotePaths.INIFile);
	pw = szSection;

	count = 0;
	if(!*pw){
		return;
	}
	pws = pw;
	while(*pw || *(pw - 1)){
		if(*pw && *pw != 31888){
			count++;
		}
		while(*pw++)
			;
	}
	pw = pws;
	pFavs = calloc(count, sizeof(FAV_MENU_STRUCT));
	if(pFavs){
		pTemp = pFavs;
		while(*pw || *(pw - 1)){
			if(*pw && *pw != 31888){
				pTemp->id = ++id + FAVORITES_ADDITION;
				GetPrivateProfileStringW(S_FAVORITES, pw, NULL, pTemp->pName, 128, g_NotePaths.INIFile);
				wcscpy(pTemp->pReserved, pw);
			}
			pTemp++;
			while(*pw++)
				;
		}
		pTemp = pFavs;
		if(count > 1){
			qsort(pTemp, count, sizeof(FAV_MENU_STRUCT), FavMenusCompare);
		}
		for(int i = 0; i < count; i++){
			mit.id = pTemp->id;
			wcscpy(mit.szText, pTemp->pName);
			wcscpy(mit.szReserved, pTemp->pReserved);
			AppendMenuW(hFav, MF_STRING, mit.id, mit.szText);
			SetMenuItemProperties(&mit, hFav, i);
			pTemp++;
		}
		free(pFavs);
	}
}


static void LoadWeekdays(wchar_t * lpLang){
	wchar_t			szBuffer[128];
	int				i, j;

	GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK, szBuffer, 128);
	switch(_wtoi(szBuffer)){
		case 0:
			for(i = 0, j = 0; i <= 6; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			break;
		case 1:
			for(i = 1, j = 0; i <= 6; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			SetWeekday(0, 6, lpLang);
			break;
		case 2:
			for(i = 2, j = 0; i <= 6; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			for(i = 0, j = 5; i <= 1; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			break;
		case 3:
			for(i = 3, j = 0; i <= 6; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			for(i = 0, j = 4; i <= 2; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			break;
		case 4:
			for(i = 4, j = 0; i <= 6; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			for(i = 0, j = 3; i <= 3; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			break;
		case 5:
			for(i = 5, j = 0; i <= 6; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			for(i = 0, j = 2; i <= 4; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			break;
		case 6:
			SetWeekday(6, 0, lpLang);
			for(i = 0, j = 1; i <= 5; i++, j++){
				SetWeekday(i, j, lpLang);
			}
			break;
	}
}

static void SetWeekday(int day, int index, const wchar_t * lpFile){

	wchar_t 		szId[12];

	if(g_Weekdays[index] == NULL)
		g_Weekdays[index] = calloc(24, sizeof(wchar_t));
	_itow(day, szId, 10);
	GetPrivateProfileStringW(S_WEEKDAYS, szId, NULL, g_Weekdays[index], 24, lpFile);
}

static void FreeRepParts(void){
	for(int i = 0; i < NELEMS(g_RepParts); i++){
		if(g_RepParts[i])
			free(g_RepParts[i]);
	}
}

static void FreeWeekdays(void){
	for(int i = 0; i < NELEMS(g_Weekdays); i++){
		if(g_Weekdays[i])
			free(g_Weekdays[i]);
	}
}

static void FreeFavoritesMenu(void){

	HMENU			hFav;

	hFav = GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_FAVORITES));
	FreeMenus(hFav);
}

static void FindFavorite(wchar_t * id){
	PMEMNOTE	pNote = MemoryNotes();

	while(pNote){
		if(wcscmp(pNote->pFlags->id, id) == 0){
			if(DockType(pNote->pData->dockData) == DOCK_NONE){
				ShowNote(pNote);
				//if(!pNote->pData->visible){
					//CreateNote(pNote, g_hInstance, g_hMain, FALSE, NULL);
					//SaveVisibility(pNote->hwnd, TRUE);
				//}
				//SendMessageW(pNote->hwnd, WM_COMMAND, MAKEWPARAM(IDM_SHOW_CENTER, 0), 0);	
			}
			return;
		}
		pNote = pNote->next;
	}
}

static void ShowFavorite(int id){
	HMENU			hFav;
	MENUITEMINFOW	mi;
	PMItem			pmi;

	hFav = GetSubMenu(m_hPopUp, GetMenuPosition(m_hPopUp, IDM_FAVORITES));
	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_DATA;
	GetMenuItemInfoW(hFav, id, FALSE, &mi);
	if(mi.dwItemData){
		pmi = (PMItem)mi.dwItemData;
		FindFavorite(pmi->szReserved);
	}
}
