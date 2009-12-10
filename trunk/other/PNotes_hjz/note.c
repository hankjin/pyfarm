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

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "notes.h"
#include "skin.h"
#include "skinlessdrawing.h"
#include "send.h"
#include "print.h"
#include "lunar.h"

typedef struct {
	HWND	hAppearance;
	HWND	hSchedule;
}NDLG_WNDS;

typedef struct {
	HWND	hOnce;
	HWND	hEveryDay;
	HWND	hRepeat;
	HWND	hWeekly;
	HWND hYearly;
	HWND	hAfter;
}DNS_WNDS;
/** Constants ********************************************************/
#define	MARKS_COUNT			5
/** Enums ********************************************************/
typedef enum {MARK_SCHEDULE, MARK_CHANGED, MARK_PROTECTED, MARK_PRIORITY, MARK_COMPLETED} note_marks;
typedef enum {NDF, CC_TEXT, CC_HIGHLIGHT} cmd_colors;
typedef enum {FD_BOLD, FD_ITALIC, FD_UNDERLINE, FD_STRIKETHROUGH} font_decoration;
/** Prototypes ********************************************************/
static void DoAlarm(PMEMNOTE pNote);
static int CheckDockPositionForAlarm(HWND hwnd);
static BOOL CALLBACK PropEnumProc(HWND hwnd, LPCWSTR lpszString, HANDLE hData);
static void FormatNewNoteName(wchar_t * lpID);
static void FormatNewNoteID(wchar_t * lpID);
static LRESULT CALLBACK Note_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Note_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void Note_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem);
static void Note_OnKillFocus(HWND hwnd, HWND hwndNewFocus);
static void Note_OnRButtonUp(HWND hwnd, int x, int y, UINT flags);
static void Note_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
static void Note_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
static void Note_OnSetFocus(HWND hwnd, HWND hwndOldFocus);
static void Note_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Note_OnClose(HWND hwnd);
static BOOL Note_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
static void Note_OnPaint(HWND hwnd);
static void Note_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
static void Note_OnSize(HWND hwnd, UINT state, int cx, int cy);
static BOOL Note_OnWindowPosChanging(HWND hwnd, LPWINDOWPOS lpwpos);
static HBRUSH Note_OnCtlColorEdit(HWND hwnd, HDC hdc, HWND hwndChild, int type);
static void Note_OnDropFiles(HWND hwnd, HDROP hdrop);
static void Note_OnLButtonDblClk(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
static void Note_OnMouseLeave(HWND hwnd);
static void Note_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu);
static void OnResizeNote(HWND hwnd, LPRECT lpRc);
static LRESULT CALLBACK Edit_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Edit_OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
static void Adjust_OnClose(HWND hwnd);
static BOOL Adjust_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Adjust_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void AddTabs(HWND hwnd);
static BOOL IsAfterMatch(P_SCHEDULE_TYPE pS, LPSYSTEMTIME lpst);
static BOOL IsLeapYear(int year);
static void FillScheduleTypes(HWND hwnd, wchar_t * lpFile);
static void FillDaysOfWeek(wchar_t * lpLang);
static BOOL ScheduleChanged(PMEMNOTE pNote);
static void SetSizes(HWND hwnd, int cx, int cy);
static BOOL SaveAs_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void SaveAs_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void SaveAs_OnClose(HWND hwnd);
static void SetTooltip(HWND hOwner, HWND hTooltip, P_NOTE_DATA pD, P_NOTE_RTHANDLES pH, BOOL bUpdate);
static void ColorChanged(HWND hNote, int id);
static void SetNoteCaption(HWND hwnd);
static LRESULT CALLBACK Grip_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Grip_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
static void PaintGrip(HWND hwnd, HDC hdcPS, COLORREF crWindow);
static void PaintSkinnableWindow(HWND hwnd, HDC hdcPS, P_NOTE_RTHANDLES pH);
static void PaintSimpleWindow(HWND hwnd, HDC hdcPS, P_NOTE_RTHANDLES pH);
static void MeasureSkinlessNoteRectangles(P_NOTE_RTHANDLES pH, int cx, int cy);
static void RestoreStandardView(HWND hNote);
static void ClearOnAdjustClose(PMEMNOTE pNote);
static void SetREFontCommon(HWND hEdit, LPLOGFONTW lf);
static void SetREFontColorCommon(HWND hEdit, COLORREF crText);
static void SetREHighlight(HWND hNote, COLORREF crColor);
static void SetREFontFace(HWND hNote, P_FONT_TYPE pft);
static void SetREFontSize(HWND hNote, int size);
static void SetREFontColor(HWND hNote, COLORREF crColor);
static void SetREFontDecoration(HWND hNote, font_decoration fd);
static void SetREAlignment(HWND hNote, int alignment);
static void SetREBullets(HWND hNote);
static DWORD OutStreamCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);
static LRESULT CALLBACK Command_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Command_OnPaint(HWND hwnd);
static void Command_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
static void Tbr_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void Tbr_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem);
static void Tbr_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void Tbr_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu);
static void SetNoteCommandsBar(HWND hwnd, P_NOTE_RTHANDLES pH);
static void SetCommandsTooltips(HWND hwnd);
static void RecreateCommandTooltip(HWND hwnd);
static LRESULT CALLBACK Tbr_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void DeleteAutoSavedFile(wchar_t * id);
static int GetTemporarySaveName(HWND hNote, wchar_t * lpName, int nMax);
static void SetCurrentFontFace(HWND hEdit);
static int GetCurrentFontSize(HWND hEdit);
static BOOL ApplyNoteChanges(HWND hwnd);
static BOOL CALLBACK NApp_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void NApp_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void NApp_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL NApp_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static BOOL CALLBACK NSched_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void NSched_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void NSched_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void InitAppearancePart(HWND hwnd);
static void InitSchedulePart(HWND hwnd);
static void AddGroupsMenuItems(HWND hwnd, HMENU hMenu);
static void RemoveBell(HWND hwnd);
static void rseed(int seed);
static int rnd(int base);
static COLORREF MakeRandomColor(void);
static BOOL CALLBACK Dns_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Dns_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL Dns_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void SetDockType(int * dockData, dock_type dockType);
static void DockNote(HWND hwnd, dock_type dockType, P_NOTE_DATA pD);
static void UndockNote(PMEMNOTE pNote);
static void WriteNotePlacement(HWND hwnd, BOOL fSave);
static void SaveNewBColor(PMEMNOTE pNote, COLORREF crWindow);
static void SaveNewCFont(PMEMNOTE pNote, LPLOGFONTW lplf);
static void SaveNewCColor(PMEMNOTE pNote, COLORREF crCaption);
static int ArrowRequired(HWND hwnd, dock_type dockType, int dockData);
static void ApplyNewSkin(HWND hNote, P_NOTE_RTHANDLES pH);
static void GetPesterPart(LPSYSTEMTIME lps);
static void SetPesterPart(LPSYSTEMTIME lps);
static void ShowHideNoteToolbar(HWND hNote);
static void ShiftNotes(int pos);
static SIZE GetScreenMetrics(void);
static void SetWeekdayText(int id, int day, const wchar_t * lpFile);
static short SelectedWeekdays(void);
static void PasteContentsOfFile(HWND hwnd);
static void PasteLinkToFile(HWND hwnd);
static LRESULT CALLBACK Marks_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void Marks_OnPaint(HWND hwnd);

/** Module variables ********************************************************/
static BOOL				m_fFullDrag;
static NOTE_RTHANDLES	m_TNHandles;
static NOTE_APPEARANCE	m_TNAppearance;
static int				m_DataRes1;
static RECT				m_rWindow, m_rCaption;
static BOOL				m_JustCreated = TRUE;
static cmd_colors		m_CurrColorsChoice = NDF;
static BOOL				m_Reload = FALSE;
static NDLG_WNDS		m_Dlgs;
static DNS_WNDS			m_Dns;
static BOOL				m_DblClickSize = FALSE;
static HMENU			m_hTempMenu = NULL, m_hMenuTColor, m_hMenuTHighlight;
static int 				rnd_seed = 12345678;
static wchar_t			m_DropFile[MAX_PATH * 2];

/** Popup menus ********************************************************/
static MItem			m_NMenus[] = {{IDM_SAVE, 6, 0, -1, -1, MT_REGULARITEM, FALSE, L"Save", L"Ctrl+S"}, 
									{IDM_SAVE_AS, 10, 0, -1, -1, MT_REGULARITEM, FALSE, L"Save As", L""}, 
									{IDM_PRINT_NOTE, 80, 0, -1, -1, MT_REGULARITEM, FALSE, L"Print", L"Ctrl+P"}, 
									{IDM_ADJUST_APPEARANCE, 83, 0, -1, -1, MT_REGULARITEM, FALSE, L"Adjust Appearance", L""}, 
									{IDM_ADJUST_SCHEDULE, 84, 0, -1, -1, MT_REGULARITEM, FALSE, L"Adjust Schedule", L""}, 
									{IDM_HIDE, 16, 0, -1, -1, MT_REGULARITEM, FALSE, L"Hide", L""}, 
									{IDM_DEL, 9, 0, -1, -1, MT_REGULARITEM, FALSE, L"Delete", L""},
									{IDM_DOCK, 48, 0, -1, -1, MT_REGULARITEM, FALSE, L"Dock", L""},
									{IDM_DOCK_NONE, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"None", L""},
									{IDM_DOCK_LEFT, 44, 0, -1, -1, MT_REGULARITEM, FALSE, L"Left", L""},
									{IDM_DOCK_TOP, 45, 0, -1, -1, MT_REGULARITEM, FALSE, L"Top", L""},
									{IDM_DOCK_RIGHT, 46, 0, -1, -1, MT_REGULARITEM, FALSE, L"Right", L""},
									{IDM_DOCK_BOTTOM, 47, 0, -1, -1, MT_REGULARITEM, FALSE, L"Bottom", L""},
									{IDM_EMAIL, 25, 0, -1, -1, MT_REGULARITEM, FALSE, L"Send", L""},
									{IDM_ATTACHMENT, 70, 0, -1, -1, MT_REGULARITEM, FALSE, L"Send As Attachment", L""},
									{IDM_NOTE_GROUPS, 35, 0, -1, -1, MT_REGULARITEM, FALSE, L"Groups", L""},
									{IDM_ADD_TO_FAVORITES, 67, 0, -1, -1, MT_REGULARITEM, FALSE, L"Add To Favorites", L""},
									{IDM_ON_TOP, -1, 0, 22, 0, MT_REGULARITEM, FALSE, L"On Top", L""},
									{IDM_HIGH_PRIORITY, -1, 0, 22, 0, MT_REGULARITEM, FALSE, L"Toggle High Priority", L""}, 
									{IDM_TOGGLE_PROTECTION, -1, 0, 22, 0, MT_REGULARITEM, FALSE, L"Toggle Protection Mode", L""},
									{IDM_MARK_AS_COMPLETED, -1, 0, 22, 0, MT_REGULARITEM, FALSE, L"Mark As Completed", L""},
									{IDM_ROLL_UNROLL, 89, 0, -1, -1, MT_REGULARITEM, FALSE, L"Roll/Unroll", L""}};
static MItem			m_EMenus[] = {{IDM_UNDO, 40, 0, -1, -1, MT_REGULARITEM, FALSE, L"Undo", L"Ctrl+Z"}, 
									{IDM_REDO, 41, 0, -1, -1, MT_REGULARITEM, FALSE, L"Redo", L"Ctrl+Y"},
									{IDM_CUT, 36, 0, -1, -1, MT_REGULARITEM, FALSE, L"Cut", L"Ctrl+X"},
									{IDM_COPY, 37, 0, -1, -1, MT_REGULARITEM, FALSE, L"Copy", L"Ctrl+C"},
									{IDM_PASTE, 38, 0, -1, -1, MT_REGULARITEM, FALSE, L"Paste", L"Ctrl+V"},
									{IDM_FORMAT_NOTE_TEXT, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Format", L""},								
									{IDM_FORMAT_FONT_FAMILY, 55, 0, -1, -1, MT_REGULARITEM, FALSE, L"Font", L""},
									{IDM_FORMAT_FONT_SIZE, 56, 0, -1, -1, MT_REGULARITEM, FALSE, L"Font Size", L""},
									{IDM_FORMAT_FONT_COLOR, 57, 0, -1, -1, MT_REGULARITEM, FALSE, L"Font Color", L""},
									{IDM_FORMAT_FONT_BOLD, 58, 0, -1, -1, MT_REGULARITEM, FALSE, L"Bold", L""},
									{IDM_FORMAT_FONT_ITALIC, 59, 0, -1, -1, MT_REGULARITEM, FALSE, L"Italic", L""},
									{IDM_FORMAT_FONT_UNDER, 60, 0, -1, -1, MT_REGULARITEM, FALSE, L"Underline", L""},
									{IDM_FORMAT_FONT_STRIKE, 61, 0, -1, -1, MT_REGULARITEM, FALSE, L"Strikethrough", L""},
									{IDM_FORMAT_FONT_HILIT, 62, 0, -1, -1, MT_REGULARITEM, FALSE, L"Highlight", L""},
									{IDM_FORMAT_ALIGN_LEFT, 63, 0, -1, -1, MT_REGULARITEM, FALSE, L"Align Left", L""},
									{IDM_FORMAT_ALIGN_CENTER, 64, 0, -1, -1, MT_REGULARITEM, FALSE, L"Center", L""},
									{IDM_FORMAT_ALIGN_RIGHT, 65, 0, -1, -1, MT_REGULARITEM, FALSE, L"Align Right", L""},
									{IDM_FORMAT_BULLETS, 66, 0, -1, -1, MT_REGULARITEM, FALSE, L"Bullets", L""},
									{IDM_SEARCH_NOTE, 43, 0, -1, -1, MT_REGULARITEM, FALSE, L"Find", L"Ctrl+F"},
									{IDM_SEARCH_NOTE_NEXT, 53, 0, -1, -1, MT_REGULARITEM, FALSE, L"Find Next", L"F3"},
									{IDM_REPLACE_NOTE, 54, 0, -1, -1, MT_REGULARITEM, FALSE, L"Replace", L"Ctrl+H"},
									{IDM_SELECT_ALL, 39, 0, -1, -1, MT_REGULARITEM, FALSE, L"Select All", L"Ctrl+A"}};
static MItem			m_DMenus[] = {{IDM_DROP_SHORTCUT, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Paste File As Link", L""}, 
									{IDM_DROP_CONTENT, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Paste Contents Of File", L""}};

BOOL RegisterNoteClass(HINSTANCE hInstance){

	WNDCLASSEXW		wcx;

	ZeroMemory(&wcx, sizeof(wcx));
	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcx.lpfnWndProc = (WNDPROC)Note_WndProc;
	wcx.hInstance = hInstance;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.cbWndExtra = 16;
	wcx.lpszClassName = NWC_SKINNABLE_CLASS;
	//register skinnable class
	if(!RegisterClassExW(&wcx))
		return FALSE;
	//register grip class
	wcx.cbWndExtra = 0;
	wcx.lpfnWndProc = (WNDPROC)Grip_WndProc;
	wcx.lpszClassName = NWC_GRIP_CLASS;
	wcx.hCursor = LoadCursor(NULL, IDC_SIZENWSE);
	if(!RegisterClassExW(&wcx))
		return FALSE;
	//register command button class
	wcx.lpfnWndProc = (WNDPROC)Command_WndProc;
	wcx.lpszClassName = NWC_TBR_BUTTON_CLASS;
	wcx.hCursor = LoadCursor(NULL, IDC_HAND);
	if(!RegisterClassExW(&wcx))
		return FALSE;
	//register toolbar class
	wcx.lpfnWndProc = (WNDPROC)Tbr_WndProc;
	wcx.lpszClassName = NWC_TOOLBAR_CLASS;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	if(!RegisterClassExW(&wcx))
		return FALSE;
	//register marks class
	wcx.lpfnWndProc = (WNDPROC)Marks_WndProc;
	wcx.lpszClassName = NWC_NOTE_MARK_CLASS;
	if(!RegisterClassExW(&wcx))
		return FALSE;
	return TRUE;
}

void LoadNoteProperties(PMEMNOTE pNote, P_NOTE_RTHANDLES pHandles, wchar_t * lpID, BOOL loadNew, wchar_t * lpFile){
	BOOL				res, useSkin = FALSE, fOld = FALSE, fSetPrivate;
	wchar_t				szID[28];
	RECT				rc;
	NOTE_APPEARANCE		tmpA;
	COLORREF			textColor;

	SetRectEmpty(&rc);

	if(pHandles->hbSkin)
		useSkin = TRUE;
	memcpy(pNote->pRTHandles, pHandles, sizeof(NOTE_RTHANDLES));
	//set mask color for skinless note in order to prevent becoming selected text (black) transparent
	if(!useSkin)
		pNote->pRTHandles->crMask = CLR_MASK;
	//set note initially saved
	pNote->pFlags->saved = TRUE;
	//copy common appearance
	memcpy(pNote->pAppearance, &g_Appearance, sizeof(NOTE_APPEARANCE));
	if(lpID != NULL && !loadNew){				//in case of loading existing note from program DB
		pNote->pFlags->fromDB = TRUE;
		//get note data
		//for version 4.0
		if(!GetPrivateProfileStructW(lpID, S_DATA, pNote->pData, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
			//for version 3.5
			GetPrivateProfileStructW(lpID, S_POSITION, &rc, sizeof(rc), g_NotePaths.DataFile);
			pNote->pData->rcp = rc;
			//set note initally visible
			pNote->pData->visible = TRUE;
			if(!GetPrivateProfileStructW(lpID, S_DATA, pNote->pData, sizeof(NOTE_DATA) - sizeof(RECT) - sizeof(BOOL) - sizeof(BOOL) - sizeof(BOOL) - 2 * sizeof(int), g_NotePaths.DataFile)){
				wcscpy(pNote->pData->szName, lpID);
				GetLocalTime(&pNote->pData->stChanged);
			}
			//save note new data
			WritePrivateProfileStructW(lpID, S_DATA, pNote->pData, sizeof(NOTE_DATA), g_NotePaths.DataFile);
		}
		//save prevOnTop
		pNote->pData->prevOnTop = pNote->pData->onTop;
		//get scheduling (if any)
		//version 5.0
		if(!GetPrivateProfileStructW(lpID, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE), g_NotePaths.DataFile)){
			//version 4.7
			if(!GetPrivateProfileStructW(lpID, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE) - sizeof(SYSTEMTIME), g_NotePaths.DataFile)){
				//check for version previous to 4.5
				GetPrivateProfileStructW(lpID, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE) - sizeof(SYSTEMTIME) * 2, g_NotePaths.DataFile);
			}
		}
		//reset changed flag
		pNote->pFlags->maskChanged = 0;
		//get individual appearance
		ZeroMemory(&tmpA, sizeof(tmpA));
		res = GetPrivateProfileStructW(lpID, S_APPEARANCE, &tmpA, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
		if(!res){
			//try to get structure from previous version
			res = GetPrivateProfileStructW(lpID, S_APPEARANCE, &tmpA, 
				sizeof(NOTE_APPEARANCE) - sizeof(LOGFONTW) - sizeof(COLORREF) -
				sizeof(COLORREF) - sizeof(int), g_NotePaths.DataFile);
			fOld = TRUE;
		}
		if(res){
			//check skin
			if(*tmpA.szSkin){
				fSetPrivate = FALSE;
				if((tmpA.nPrivate & F_SKIN) == F_SKIN){
					fSetPrivate = TRUE;
				}
				else{
					if(fOld){
						if(wcscmp(tmpA.szSkin, pNote->pAppearance->szSkin) != 0){
							fSetPrivate = TRUE;
						}
					}
				}
				if(fSetPrivate){
					//save private skin
					wcscpy(pNote->pAppearance->szSkin, tmpA.szSkin);
					pNote->pAppearance->nPrivate |= F_SKIN;
				}
			}
			//check note's font
			if(tmpA.fFontSet){
				pNote->pAppearance->fFontSet = TRUE;
				pNote->pAppearance->nPrivate |= F_RTFONT;
			}
			//check skinless note caption's font
			if(*tmpA.lfCaption.lfFaceName && (tmpA.nPrivate & F_C_FONT) == F_C_FONT){
					//save private caption font
					memcpy(&pNote->pAppearance->lfCaption, &tmpA.lfCaption, sizeof(tmpA.lfCaption));
					pNote->pRTHandles->hFCaption = CreateFontIndirectW(&pNote->pAppearance->lfCaption);
					if(!fOld)
						pNote->pAppearance->nPrivate |= F_C_FONT;
			}
			//check skinless note caption's font color
			if(tmpA.crCaption && (tmpA.nPrivate & F_C_COLOR) == F_C_COLOR){
				//save private caption font color
				pNote->pAppearance->crCaption = tmpA.crCaption;
				if(!fOld)
					pNote->pAppearance->nPrivate |= F_C_COLOR;
			}
			//check skinless note background color
			if(tmpA.crWindow && (tmpA.nPrivate & F_B_COLOR) == F_B_COLOR){
				//save private background color
				pNote->pAppearance->crWindow = tmpA.crWindow;
				if(!fOld)
					pNote->pAppearance->nPrivate |= F_B_COLOR;
			}
		}
		//set note id
		wcscpy(pNote->pFlags->id, lpID);
		//check schedule
		if(pNote->pSchedule->scType != 0 && pNote->pData->idGroup != GROUP_RECYCLE){
			if(pNote->pSchedule->scType == SCH_REP)	//set repeat count according to timer interval
				pNote->pRTHandles->nRepeat = SetRepeatCount(&pNote->pSchedule->scDate);
			pNote->pRTHandles->idTimer = SetTimer(NULL, 0, NOTE_TIMER_INTERVAL, NoteTimerProc);
		}
		//docking
		if(DockType(pNote->pData->dockData) == DOCK_NONE || pNote->pData->idGroup == GROUP_RECYCLE){
			//set dock index to -777 for notes from previous version
			if(DockIndex(pNote->pData->dockData) != -777)
				SetDockIndex(&pNote->pData->dockData, -777);
		}
	}
	else if(lpID != NULL && loadNew){		//load note which is not from program DB
		//store note's id
		wcscpy(pNote->pFlags->id, lpID);
		//store temporary note's name
		wcscpy(pNote->pData->szName, lpID);
		//set dock index to -777
		SetDockIndex(&pNote->pData->dockData, -777);
		//check for random color setting
		if(IsBitOn(g_NoteSettings.reserved1, SB1_RANDCOLOR)){		
			pNote->pAppearance->crWindow = MakeRandomColor();
			pNote->pAppearance->nPrivate |= F_B_COLOR;
			pNote->pFlags->maskChanged |= F_B_COLOR;
		}
	}
	else{			//new note
		//set dock index to -777
		SetDockIndex(&pNote->pData->dockData, -777);
		FormatNewNoteID(szID);
		//set note id
		wcscpy(pNote->pFlags->id, szID);
		FormatNewNoteName(szID);
		wcscpy(pNote->pData->szName, szID);
		//check for random color setting
		if(IsBitOn(g_NoteSettings.reserved1, SB1_RANDCOLOR)){		
			pNote->pAppearance->crWindow = MakeRandomColor();
			if(IsBitOn(g_NoteSettings.reserved1, SB1_INVERT_TEXT)){	
				textColor = ~pNote->pAppearance->crWindow;
				textColor <<= 8;
				textColor >>= 8;
				pNote->pAppearance->crFont = textColor;
				pNote->pAppearance->nPrivate |= F_RTFONT;
				pNote->pFlags->maskChanged |= F_RTFONT;
			}
			pNote->pAppearance->nPrivate |= F_B_COLOR;
			pNote->pFlags->maskChanged |= F_B_COLOR;
		}
		//new note is always on-top
		if(g_NoteSettings.newOnTop){
			pNote->pData->onTop = TRUE;
			pNote->pData->prevOnTop = TRUE;
		}
	}
	//set visibility
	if(pNote->pData->idGroup > GROUP_RECYCLE){
		if(lpID == NULL || loadNew){
			//new note is always visible
			pNote->pData->visible = TRUE;
		}
	}
	else{
		pNote->pData->visible = FALSE;
	}
}

HWND CreateNote(PMEMNOTE pNote, HINSTANCE hInstance, HWND hwndParent, BOOL loadNew, wchar_t * lpFile){

	HWND				hwnd = NULL, hEdit, hStatic, hGrip, hToolbar, hSysToolbar, *phMarks, *phTemp;
	int					style = WS_POPUP;
	BITMAP				bm;
	P_NCMD_BAR			pC, pDH;
	SIZE				sz;
	RECT				rc, rcEdit;
	BOOL				useSkin = FALSE;
	CHARFORMAT2			chr;
	NOTE_REL_POSITION	nrp;
	int					left, top, width, height;

	SetRectEmpty(&rc);
	SetRectEmpty(&rcEdit);
	ZeroMemory(&nrp, sizeof(nrp));

	if(pNote->pRTHandles->hbSkin){
		useSkin = TRUE;
		//get skin dimensions
		GetObject(pNote->pRTHandles->hbSkin, sizeof(bm), &bm);
		//create note window with skin dimensions
		hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_ACCEPTFILES, NWC_SKINNABLE_CLASS, g_NotePaths.ProgName, style, 0, 0, bm.bmWidth, bm.bmHeight, hwndParent, NULL, hInstance, NULL);
	}
	else{
		//create skinless note
		hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_ACCEPTFILES, NWC_SKINNABLE_CLASS, g_NotePaths.ProgName, style, 0, 0, pNote->pRTHandles->szDef.cx, pNote->pRTHandles->szDef.cy, hwndParent, NULL, hInstance, NULL);
	}
	if(hwnd == NULL){
		return NULL;
	}
	m_JustCreated = TRUE;
	pNote->hwnd = hwnd;
	SetWindowLongPtrW(hwnd, OFFSET_MNOTE, (LONG_PTR)pNote);
	//create "toolbar buttons"
	hToolbar = CreateWindowExW(0, NWC_TOOLBAR_CLASS, NULL, WS_CHILD, 0, 0, 240, 240, hwnd, NULL, hInstance, NULL);
	SetPropW(hwnd, PH_COMMANDS, hToolbar);
	pC = calloc(CMD_TBR_BUTTONS, sizeof(NCMD_BAR));
	if(pC){
		SetWindowLongPtrW(hwnd, OFFSET_COMMANDS, (LONG_PTR)pC);
		for(int i = CMD_FONT; i < CMD_TBR_BUTTONS; i++){
			pC->hCmd = CreateWindowExW(0, NWC_TBR_BUTTON_CLASS, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 24, 24, hToolbar, (HMENU)(5000 + i), hInstance, NULL);
			pC++;
		}
	}
	//create delete/hide toolbar and buttons
	hSysToolbar = CreateWindowExW(0, NWC_TOOLBAR_CLASS, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 240, 240, hwnd, NULL, hInstance, NULL);
	SetPropW(hwnd, PH_DELHIDE, hSysToolbar);
	pDH = calloc(2, sizeof(NCMD_BAR));
	if(pDH){
		SetWindowLongPtrW(hwnd, OFFSET_DH, (LONG_PTR)pDH);
		for(int i = 0; i < CMD_SYS_BUTTONS; i++){
			pDH->hCmd = CreateWindowExW(0, NWC_TBR_BUTTON_CLASS, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 24, 24, hSysToolbar, (HMENU)(IDM_HIDE + i), hInstance, NULL);
			pDH++;
		}
	}
	//create marks buttons array
	phMarks = calloc(MARKS_COUNT, sizeof(HWND));
	if(phMarks){
		phTemp = phMarks;
		SetWindowLongPtrW(hwnd, OFFSET_SYS, (LONG_PTR)phMarks);
		for(int i = 0; i < MARKS_COUNT; i++){
			*phTemp = CreateWindowExW(0, NWC_NOTE_MARK_CLASS, NULL, WS_CHILD, 0, 0, 24, 24, hwnd, NULL, hInstance, NULL);
			SetWindowLongPtr(*phTemp, GWLP_USERDATA, (LONG_PTR)i);
			phTemp++;
		}
	}

	if(!useSkin){
		//create grip window for skinless note
		hGrip = CreateWindowExW(0, NWC_GRIP_CLASS, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, hInstance, NULL);
		SetPropW(hwnd, PH_GRIP, hGrip);
		if(g_NoteSettings.showScrollbar)
			hEdit = CreateWindowExW(0, RICHEDIT_CLASSW, NULL, E_STYLE_WITH_SCROLL, 0, 0, 0, 0, hwnd, (HMENU)IDC_NOTE_EDIT, hInstance, NULL);
		else
			hEdit = CreateWindowExW(0, RICHEDIT_CLASSW, NULL, E_STYLE_WITHOUT_SCROLL, 0, 0, 0, 0, hwnd, (HMENU)IDC_NOTE_EDIT, hInstance, NULL);
	}
	else{
		//create static window for rich edit transparency simulation
		hStatic = CreateWindowExW(0, __T("Static"), NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_REALSIZEIMAGE, 0, 0, 10, 10, hwnd, NULL, hInstance, NULL);
		SetPropW(hwnd, PH_STATIC, hStatic);
		if(g_NoteSettings.showScrollbar)
			hEdit = CreateWindowExW(WS_EX_TRANSPARENT, RICHEDIT_CLASSW, NULL, E_STYLE_WITH_SCROLL, 0, 0, 0, 0, hwnd, (HMENU)IDC_NOTE_EDIT, hInstance, NULL);
		else
			hEdit = CreateWindowExW(WS_EX_TRANSPARENT, RICHEDIT_CLASSW, NULL, E_STYLE_WITHOUT_SCROLL, 0, 0, 0, 0, hwnd, (HMENU)IDC_NOTE_EDIT, hInstance, NULL);
	}
	SetPropW(hwnd, PH_EDIT, hEdit);
	RichEdit_AutoURLDetect(hEdit, TRUE);
	//create tooltip
	pNote->pRTHandles->hTooltip = CreateWindowExW(WS_EX_TOPMOST, __T("Tooltips_class32"), NULL, TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	

	if(pNote->pFlags->fromDB){				//in case of loading existing note from program DB
		//load RTF file contents
		ReadRTFFile(pNote->pFlags->id, hEdit, NULL);

		GetPrivateProfileStructW(pNote->pFlags->id, IK_RELPOSITION, &nrp, sizeof(nrp), g_NotePaths.DataFile);

		rc = pNote->pData->rcp;
		
		//prepare needed handles
		if(useSkin && (pNote->pAppearance->nPrivate & F_SKIN) == F_SKIN){
			//change window size if it has a skin different from global settings
			GetSkinProperties(hwnd, pNote->pRTHandles, pNote->pAppearance->szSkin, FALSE);
			//get skin dimensions
			GetObject(pNote->pRTHandles->hbSkin, sizeof(bm), &bm);
		}

		//check note's font
		if(!pNote->pAppearance->fFontSet){
			//set richtext font to common
			SetREFontCommon(hEdit, &g_Appearance.lfFont);
			SetREFontColorCommon(hEdit, g_Appearance.crFont);
		}

		//mesure rectangles for skinless note
		if(!useSkin){
			CreateSkinlessToolbarBitmap(hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbCommand);
			CreateSkinlessSysBitmap(hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbDelHide);
			MeasureSkinlessNoteRectangles(pNote->pRTHandles, rc.right - rc.left, rc.bottom - rc.top);
		}
			
		//prepare edit box rectangle
		CopyRect(&rcEdit, &pNote->pRTHandles->rcEdit);
		
		//position window
		if(useSkin){
			width = bm.bmWidth;
			height = bm.bmHeight;
		}
		else{
			width = rc.right - rc.left;
			height = rc.bottom - rc.top;
		}
		if(IsBitOn(g_NoteSettings.reserved1, SB1_RELPOSITION)){
			sz = GetScreenMetrics();
			double		lf = nrp.left * (double)sz.cx;
			double		tp = nrp.top * (double)sz.cy;
			left = floor(lf);// - width;
			top = floor(tp);// - height;
			//prevent leakage of notes
			if(left + width > sz.cx)
				left = sz.cx - width - 1;
			else if(left < 0)
				left = 1;

			if(top + height > sz.cy)
				top = sz.cy - height - 1;
			else if(top < 0)
				top = 1;
		}
		else{
			left = rc.left;
			top = rc.top;
		}
		MoveWindow(hwnd, left, top, width, height, TRUE);
		if(g_NoteSettings.rollOnDblClick){
			if(!useSkin && DockType(pNote->pData->dockData) == DOCK_NONE){
				if(pNote->pData->rolled && pNote->pData->idGroup != GROUP_RECYCLE){
					m_DblClickSize = TRUE;
					MoveWindow(hwnd, left, top, width, pNote->pRTHandles->rcCaption.bottom - pNote->pRTHandles->rcCaption.top, TRUE);
					pNote->pRTHandles->rolled = TRUE;
					m_DblClickSize = FALSE;
				}
			}
		}

		//check for OnTop
		if(pNote->pData->onTop){
			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		//check schedule
		if(pNote->pSchedule->scType != 0 && pNote->pData->idGroup != GROUP_RECYCLE){
			pNote->isAlarm = TRUE;
		}		
		//set tooltip
		SetTooltip(hwnd, pNote->pRTHandles->hTooltip, pNote->pData, pNote->pRTHandles, FALSE);
		//docking
		if(DockType(pNote->pData->dockData) != DOCK_NONE && pNote->pData->idGroup != GROUP_RECYCLE){
			AddDockItem(DHeader(DockType(pNote->pData->dockData)), hwnd, DockIndex(pNote->pData->dockData));
		}		
	}
	else if(!pNote->pFlags->fromDB && loadNew){		//load note which is not from program DB
		//load RTF file contents
		ReadRTFFile(pNote->pFlags->id, hEdit, lpFile);
		//prepare edit box rectangle
		CopyRect(&rcEdit, &pNote->pRTHandles->rcEdit);
		//measure skinless note rectangles
		if(!useSkin){
			CreateSkinlessToolbarBitmap(hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbCommand);
			CreateSkinlessSysBitmap(hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbDelHide);
			MeasureSkinlessNoteRectangles(pNote->pRTHandles, pNote->pRTHandles->szDef.cx, pNote->pRTHandles->szDef.cy);
		}
		SetTooltip(hwnd, pNote->pRTHandles->hTooltip, pNote->pData, pNote->pRTHandles, FALSE);
		SaveNote(pNote);
	}
	else{			//new note
		//prepare edit box rectangle
		CopyRect(&rcEdit, &pNote->pRTHandles->rcEdit);
		//measure skinless note rectangles
		if(!useSkin){
			CreateSkinlessToolbarBitmap(hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbCommand);
			CreateSkinlessSysBitmap(hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbDelHide);
			MeasureSkinlessNoteRectangles(pNote->pRTHandles, pNote->pRTHandles->szDef.cx, pNote->pRTHandles->szDef.cy);
		}
		SetTooltip(hwnd, pNote->pRTHandles->hTooltip, pNote->pData, pNote->pRTHandles, FALSE);
		//set richtext font to common
		SetREFontCommon(hEdit, &pNote->pAppearance->lfFont);
		SetREFontColorCommon(hEdit, pNote->pAppearance->crFont);
		if(pNote->pData->onTop){
			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}

	if(!useSkin){
		//set richtext backcolor
		RichEdit_SetBkgndColor(hEdit, 0, pNote->pAppearance->crWindow);
		//position and size edit box
		MoveWindow(hEdit, 1, pNote->pRTHandles->rcCaption.bottom + 1, (pNote->pRTHandles->rcCaption.right - pNote->pRTHandles->rcCaption.left - 2), pNote->pRTHandles->rcGrip.top - (pNote->pRTHandles->rcCaption.bottom - pNote->pRTHandles->rcCaption.top) - 1, TRUE);
		//position and size grip box
		MoveWindow(hGrip, pNote->pRTHandles->rcGrip.left, pNote->pRTHandles->rcGrip.top, pNote->pRTHandles->rcGrip.right - pNote->pRTHandles->rcGrip.left, pNote->pRTHandles->rcGrip.bottom - pNote->pRTHandles->rcGrip.top, TRUE);
	}
	else{
		//position and size static window
		MoveWindow(hStatic, rcEdit.left, rcEdit.top, rcEdit.right - rcEdit.left, rcEdit.bottom - rcEdit.top, TRUE);
		SendMessageW(hStatic, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)pNote->pRTHandles->hbBack);
		//position and size edit box
		MoveWindow(hEdit, rcEdit.left, rcEdit.top, rcEdit.right - rcEdit.left, rcEdit.bottom - rcEdit.top, TRUE);
	}

	if(useSkin){
		SetLayeredWindowAttributes(hwnd, pNote->pRTHandles->crMask, 255, LWA_COLORKEY);
	}
	else{
		SetLayeredWindowAttributes(hwnd, CLR_MASK, 255, LWA_COLORKEY);
		SetRect(&pNote->pRTHandles->rcSize, left, top, left + width, top + height);
	}

	SetNoteCommandsBar(hwnd, pNote->pRTHandles);

	//initialize rich edit properties
	RichEdit_EmptyUndoBuffer(hEdit);
	RichEdit_SetEventMask(hEdit, ENM_CHANGE | ENM_DROPFILES | ENM_LINK | ENM_KEYEVENTS | ENM_MOUSEEVENTS | ENM_PROTECTED);
	//set protection on rich edit in order to receive EN_PROTECTED notification
	ZeroMemory(&chr, sizeof(chr));
	chr.cbSize = sizeof(chr);
	chr.dwMask = CFM_PROTECTED;
	chr.dwEffects = CFE_PROTECTED;
	SendMessageW(hEdit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&chr);
	RichEdit_SetModify(hEdit, FALSE);
	SetWindowLongPtrW(hEdit, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(hEdit, GWLP_WNDPROC, (LONG_PTR)Edit_WndProc));

	SetCommandsTooltips(hwnd);

	//pNote->pData->visible = TRUE;

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	
	SetNoteCaption(hwnd);
	if(DockType(pNote->pData->dockData) == DOCK_NONE){ //not docked and from DB
		if(pNote->pFlags->fromDB)
			WriteNotePlacement(hwnd, TRUE);
		else
			WriteNotePlacement(hwnd, FALSE);
	}
	m_JustCreated = FALSE;
	SetForegroundWindow(hwnd);
	SetFocus(hEdit);
	ShowHideNoteToolbar(hwnd);
	ShowNoteMarks(hwnd);
	return hwnd;
}

void AddColorsMenu(void){
	MENUITEMINFOW	mi;
	HMENU			hFormat, hSize;
	wchar_t 		szAutomatic[256], szNoFill[256], szFile[MAX_PATH], szKey[12];

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);
	_itow(ID_FC_CAPTION, szKey, 10);
	GetPrivateProfileStringW(S_COMMANDS, szKey, L"Automatic", szAutomatic, 256, szFile);
	_itow(ID_FH_CAPTION, szKey, 10);
	GetPrivateProfileStringW(S_COMMANDS, szKey, L"No Fill", szNoFill, 256, szFile);
	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_SUBMENU | MIIM_ID;
	GetMenuItemInfoW(g_hEditPopUp, GetMenuPosition(g_hEditPopUp, IDM_FORMAT_NOTE_TEXT), TRUE, &mi);
	hFormat = mi.hSubMenu;
	GetMenuItemInfoW(hFormat, GetMenuPosition(hFormat, IDM_FORMAT_FONT_COLOR), TRUE, &mi);
	m_hMenuTColor = mi.hSubMenu;
	CreateColorMenu(m_hMenuTColor, &g_FormatColorID, IDI_FORMAT_COLOR_START, szAutomatic);
	GetMenuItemInfoW(hFormat, GetMenuPosition(hFormat, IDM_FORMAT_FONT_HILIT), TRUE, &mi);
	m_hMenuTHighlight = mi.hSubMenu;
	CreateColorMenu(m_hMenuTHighlight, &g_FormatHighlightID, IDI_FORMAT_HIGHLIGHT_START, szNoFill);
	GetMenuItemInfoW(hFormat, GetMenuPosition(hFormat, IDM_FORMAT_FONT_SIZE), TRUE, &mi);
	hSize = mi.hSubMenu;
	CreateSizeMenu(&hSize, FALSE);

	// if(g_PMEdit){
		// free(g_PMEdit);
	// }
	// g_PMECount = 0;
	// g_PMEdit = (PMItem)calloc(NELEMS(m_EMenus), NELEMS(m_EMenus) * sizeof(MItem));
	// memcpy(g_PMEdit, m_EMenus, NELEMS(m_EMenus) * sizeof(MItem));
	// count = GetMenuItemCount(hFColors);
	// for(int k = 0; k < 2; k++){
		// for(int i = 0; i< count; i++){
			// //add new PMItem
			// g_PMECount++;
			// number = NELEMS(m_EMenus);
			// number += g_PMECount;
			// size = number * sizeof(MItem);
			// g_PMEdit = realloc(g_PMEdit, size);
			// if(g_PMEdit){
				// PMItem	pmi = g_PMEdit;
				// for(int j = 0; j < number - 1; j++)
					// pmi++;
				// if(k == 0)
					// pmi->id = GetMenuItemID(hFColors, i);
				// else
					// pmi->id = GetMenuItemID(hHighlight, i);
				// pmi->xPos = -1;
				// pmi->yPos = -1;
				// pmi->xCheck = 0;
				// pmi->yCheck = 0;
				// if(i > 0){
					// pmi->type = MT_COLORITEM;
					// wcscpy(pmi->szText, L"");
				// }
				// else{
					// pmi->type = MT_REGULARITEM;
					// if(k == 0)
						// wcscpy(pmi->szText, szAutomatic);
					// else
						// wcscpy(pmi->szText, szNoFill);
				// }
			// }
		// }
	// }
	// count = GetMenuItemCount(hSize);
	// for(int i = 0; i< count; i++){
		// //add new PMItem
		// g_PMECount++;
		// number = NELEMS(m_EMenus);
		// number += g_PMECount;
		// size = number * sizeof(MItem);
		// g_PMEdit = realloc(g_PMEdit, size);
		// if(g_PMEdit){
			// PMItem	pmi = g_PMEdit;
			// for(int j = 0; j < number - 1; j++)
				// pmi++;
			// pmi->id = GetMenuItemID(hSize, i);
			// pmi->xPos = -1;
			// pmi->yPos = -1;
			// pmi->xCheck = 0;
			// pmi->yCheck = 0;
			// pmi->type = MT_REGULARITEM;
			// _itow(pmi->id - IDI_SIZE_START, szKey, 10);
			// wcscpy(pmi->szText, szKey);
		// }
	// }
	// SetMenuOwnerDraw(hFColors);
	// SetMenuOwnerDraw(hHighlight);
}

void SetScheduleForPrevious(HWND hwnd, wchar_t * lpID, wchar_t * lpFile){
	PMEMNOTE			pNote = MEMNOTE(hwnd);

	if(pNote){
		//get scheduling (if any)
		if(!GetPrivateProfileStructW(lpID, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE), lpFile)){
			//check for version previous to 4.5
			GetPrivateProfileStructW(lpID, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE) - sizeof(SYSTEMTIME), lpFile);
		}
		if(pNote->pSchedule->scType != 0){
			pNote->isAlarm = TRUE;
			if(pNote->pSchedule->scType == SCH_REP)	//set repeat count according to timer interval
				pNote->pRTHandles->nRepeat = SetRepeatCount(&pNote->pSchedule->scDate);
			pNote->pRTHandles->idTimer = SetTimer(NULL, 0, NOTE_TIMER_INTERVAL, NoteTimerProc);
		}
	}
}

static void SetREFontSize(HWND hNote, int size){
	CHARFORMAT2W		chf;
	HDC					hdc;
	HWND				hEdit;
	PMEMNOTE			pNote = MEMNOTE(hNote);


	hEdit = (HWND)GetPropW(hNote, PH_EDIT);
	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	chf.dwMask = CFM_SIZE;
	hdc = GetDC(hEdit);
	size = -MulDiv(size, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	chf.yHeight = 20 * -(size * 72) / GetDeviceCaps(hdc, LOGPIXELSY);
	ReleaseDC(hEdit, hdc);
	RichEdit_SetCharFormat(hEdit, SCF_SELECTION, &chf);
	pNote->pFlags->maskChanged |= F_RTFONT;
	pNote->pAppearance->fFontSet = TRUE;
}

static void SetREFontFace(HWND hNote, P_FONT_TYPE pft){
	CHARFORMAT2W		chf;
	PMEMNOTE			pNote = MEMNOTE(hNote);

	if(wcslen(pft->szFace)){
		ZeroMemory(&chf, sizeof(chf));
		chf.cbSize = sizeof(chf);
		chf.dwMask = CFM_FACE | CFM_CHARSET;
		wcscpy(chf.szFaceName, pft->szFace);
		chf.bCharSet = (BYTE)g_SelectedFont.nCharSet;
		RichEdit_SetCharFormat((HWND)GetPropW(hNote, PH_EDIT), SCF_SELECTION, &chf);
		pNote->pFlags->maskChanged |= F_RTFONT;
		pNote->pAppearance->fFontSet = TRUE;
	}
}

static void SetREHighlight(HWND hNote, COLORREF crColor){
	CHARFORMAT2W		chf;
	PMEMNOTE			pNote = MEMNOTE(hNote);
	HWND				hEdit;

	//prevent transparency
	if(crColor == pNote->pRTHandles->crMask)
		crColor--;
	hEdit = (HWND)GetPropW(hNote, PH_EDIT);
	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	if(crColor == -1){
		chf.dwMask = CFE_AUTOBACKCOLOR;
		chf.dwEffects = CFE_AUTOBACKCOLOR;
	}
	else{
		chf.dwMask = CFM_BACKCOLOR;
		chf.crBackColor = crColor;
	}
	RichEdit_SetCharFormat(hEdit, SCF_SELECTION, &chf);
	pNote->pFlags->maskChanged |= F_RTFONT;
	pNote->pAppearance->fFontSet = TRUE;
}

static void SetREFontColor(HWND hNote, COLORREF crColor){
	CHARFORMAT2W		chf;
	PMEMNOTE			pNote = MEMNOTE(hNote);
	HWND				hEdit;
	COLORREF			crDef;


	//prevent transparency
	if(crColor == pNote->pRTHandles->crMask)
		crColor--;
	hEdit = (HWND)GetPropW(hNote, PH_EDIT);
	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	chf.dwMask = CFM_COLOR;
	RichEdit_GetCharFormat(hEdit, SCF_DEFAULT, &chf);
	crDef = chf.crTextColor;
	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	chf.dwMask = CFM_COLOR;
	if(crColor == -1){
		if((pNote->pAppearance->nPrivate & F_RTFONT) == F_RTFONT)
			chf.crTextColor = crDef;
		else
			chf.crTextColor = g_Appearance.crFont;
	}
	else{
		chf.crTextColor = crColor;
	}
	RichEdit_SetCharFormat(hEdit, SCF_SELECTION, &chf);
	pNote->pFlags->maskChanged |= F_RTFONT;
	pNote->pAppearance->fFontSet = TRUE;
}

static void SetREFontDecoration(HWND hNote, font_decoration fd){
	CHARFORMAT2W		chf;
	PMEMNOTE			pNote = MEMNOTE(hNote);
	HWND				hEdit;
	int					state;
	BOOL				set;

	hEdit = (HWND)GetPropW(hNote, PH_EDIT);

	//get character format (the first selected character) and save desired format
	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	switch(fd){
	case FD_BOLD:
		chf.dwMask = CFM_BOLD;
		state = CFE_BOLD;
		break;
	case FD_ITALIC:
		chf.dwMask = CFM_ITALIC;
		state = CFE_ITALIC;
		break;
	case FD_UNDERLINE:
		chf.dwMask = CFM_UNDERLINE;
		state = CFE_UNDERLINE;
		break;
	case FD_STRIKETHROUGH:
		chf.dwMask = CFM_STRIKEOUT;
		state = CFE_STRIKEOUT;
		break;
	}
	RichEdit_GetCharFormat(hEdit, SCF_SELECTION, &chf);
	
	if((chf.dwEffects & state) == state)
		//remove format
		set = FALSE;
	else
		//set format
		set = TRUE;

	//set character format
	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	switch(fd){
	case FD_BOLD:
		chf.dwMask = CFM_BOLD;
		break;
	case FD_ITALIC:
		chf.dwMask = CFM_ITALIC;
		break;
	case FD_UNDERLINE:
		chf.dwMask = CFM_UNDERLINE;
		break;
	case FD_STRIKETHROUGH:
		chf.dwMask = CFM_STRIKEOUT;
		break;
	}
	if(set)
		//set format, otherwise the field stays 0 - remove format
		chf.dwEffects = state;
	RichEdit_SetCharFormat(hEdit, SCF_SELECTION, &chf);

	pNote->pFlags->maskChanged |= F_RTFONT;
	pNote->pAppearance->fFontSet = TRUE;
}

static void SetREAlignment(HWND hNote, int alignment){
	PARAFORMAT2			pfm;
	PMEMNOTE			pNote = MEMNOTE(hNote);
	HWND				hEdit;

	hEdit = (HWND)GetPropW(hNote, PH_EDIT);
	ZeroMemory(&pfm, sizeof(pfm));
	pfm.cbSize = sizeof(pfm);
	pfm.dwMask = PFM_ALIGNMENT;
	pfm.wAlignment = alignment;
	RichEdit_SetParaFormat(hEdit, &pfm);
	pNote->pFlags->maskChanged |= F_RTFONT;
	pNote->pAppearance->fFontSet = TRUE;
}

static void SetREBullets(HWND hNote){
	PARAFORMAT2			pfm;
	PMEMNOTE			pNote = MEMNOTE(hNote);
	HWND				hEdit;
	BOOL				isSet;

	hEdit = (HWND)GetPropW(hNote, PH_EDIT);
	ZeroMemory(&pfm, sizeof(pfm));
	pfm.cbSize = sizeof(pfm);
	pfm.dwMask = PFM_NUMBERING;
	RichEdit_GetParaFormat(hEdit, &pfm);
	if((pfm.wNumbering & PFN_BULLET) == PFN_BULLET)
		isSet = TRUE;
	else
		isSet = FALSE;
	ZeroMemory(&pfm, sizeof(pfm));
	pfm.cbSize = sizeof(pfm);
	pfm.dwMask = PFM_NUMBERING;
	if(!isSet)
		pfm.wNumbering = PFN_BULLET;
	RichEdit_SetParaFormat(hEdit, &pfm);
	pNote->pFlags->maskChanged |= F_RTFONT;
	pNote->pAppearance->fFontSet = TRUE;
}

static void SetREFontColorCommon(HWND hEdit, COLORREF crText){
	CHARFORMAT2W		chf;
	
	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	chf.dwMask = CFM_COLOR;
	chf.crTextColor = crText;
	RichEdit_SetCharFormat(hEdit, SCF_ALL, &chf);
}

static void SetREFontCommon(HWND hEdit, LPLOGFONTW lf){
	CHARFORMAT2W		chf;
	HDC					hdc;

	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	chf.dwMask = CFM_BOLD | CFM_FACE | CFM_ITALIC | CFM_STRIKEOUT | CFM_UNDERLINE | CFM_CHARSET | CFM_SIZE;
	if(lf->lfWeight >= FW_BOLD)
		chf.dwEffects |= CFE_BOLD;
	if(lf->lfItalic)
		chf.dwEffects |= CFE_ITALIC;
	if(lf->lfStrikeOut)
		chf.dwEffects |= CFE_STRIKEOUT;
	if(lf->lfUnderline)
		chf.dwEffects |= CFE_UNDERLINE;
	hdc = GetDC(hEdit);
	chf.yHeight = 20 * -(lf->lfHeight * 72) / GetDeviceCaps(hdc, LOGPIXELSY);
	ReleaseDC(hEdit, hdc);
	chf.bCharSet = lf->lfCharSet;
	chf.bPitchAndFamily = lf->lfPitchAndFamily;
	wcscpy(chf.szFaceName, lf->lfFaceName);
	RichEdit_SetCharFormat(hEdit, SCF_ALL, &chf);
}

void SetNoteMIImageIndex(int id, int index){
	//set menu item index
	SetMIImageIndex(m_NMenus, NELEMS(m_NMenus), id, index);
}

void SetNoteMIText(int id, wchar_t * lpText){
	//set menu item text
	SetMIText(m_NMenus, NELEMS(m_NMenus), id, lpText);
}

void SetEditMIText(int id, wchar_t * lpText){
	//set menu item text
	SetMIText(m_EMenus, NELEMS(m_EMenus), id, lpText);
}

int SetRepeatCount(LPSYSTEMTIME lst){
	return ((lst->wDay * 24 * 3600 + lst->wHour * 3600 + lst->wMinute * 60 + lst->wSecond) * (1000 / NOTE_TIMER_INTERVAL));
}

static void ColorChanged(HWND hNote, int id){
	PMItem			pmi;
	COLORREF		color;
	MENUITEMINFOW	mi;

	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_DATA;
	if(m_CurrColorsChoice == CC_TEXT)
		GetMenuItemInfoW(m_hMenuTColor, id, FALSE, &mi);
	else
		GetMenuItemInfoW(m_hMenuTHighlight, id, FALSE, &mi);

	if(id == IDI_FORMAT_COLOR_START || id == IDI_FORMAT_HIGHLIGHT_START){
		if(m_CurrColorsChoice == CC_TEXT)
			SetREFontColor(hNote, -1);
		else if(m_CurrColorsChoice == CC_HIGHLIGHT)
			SetREHighlight(hNote, -1);
	}
	else{
		pmi = (PMItem)mi.dwItemData;
		if(pmi){
			color = _wtoi(pmi->szText);
			if(m_CurrColorsChoice == CC_TEXT)
				SetREFontColor(hNote, color);
			else if(m_CurrColorsChoice == CC_HIGHLIGHT)
				SetREHighlight(hNote, color);
		}
	}
	m_CurrColorsChoice = NDF;
	SetFocus(hNote);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SetTooltip
 Created  : Sun May 27 15:19:09 2007
 Modified : Sun May 27 15:24:02 2007

 Synopsys : Creates or updates note tooltip
 Input    : hOwner - note window handle
            hTooltip - tooltip handle
            pD - pointer to NOTE_DATA
            pH - pointer to NOTE_RTHANDLES
            bUpdate - whether tooltip should be created or updated
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void SetTooltip(HWND hOwner, HWND hTooltip, P_NOTE_DATA pD, P_NOTE_RTHANDLES pH, BOOL bUpdate){
	
	wchar_t			szTooltip[256], szTemp[64];
	TOOLINFOW		ti;
	//int				style;

	ZeroMemory(&ti, sizeof(ti));
	ti.cbSize = sizeof(ti);
	ti.hwnd = hOwner;
	ti.uFlags = TTF_SUBCLASS;
	wcscpy(szTooltip, pD->szName);
	wcscat(szTooltip, __T(" - "));
	if(pD->stChanged.wDay != 0){
		//add last saved time if note has been saved
		ConstructDateTimeString(&pD->stChanged, szTemp, FALSE);
		wcscat(szTooltip, szTemp);
	}
	else{
		//note has not been saved yet
		wcscat(szTooltip, __T("Not saved"));
	}
	//set tooltip text
	ti.lpszText = szTooltip;	
	//set tooltip rectangle
	CopyRect(&ti.rect, &pH->rcTooltip);
	if(bUpdate){
		//update tooltip
		SendMessageW(hTooltip, TTM_NEWTOOLRECTW, 0, (LPARAM)&ti);
		SendMessageW(hTooltip, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
	}
	else{
		//add tooltip
		SendMessageW(hTooltip, TTM_ADDTOOLW, 0, (LPARAM)&ti);
	}
	//style = GetWindowLongPtrW(hTooltip, GWL_EXSTYLE);
	//style |= WS_EX_TOPMOST;
	//SetWindowLongPtrW(hTooltip, GWL_EXSTYLE, style);
}

void CALLBACK NoteTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime){

	SYSTEMTIME			st;
	PMEMNOTE			pNote;

	GetLocalTime(&st);
	pNote = MemNoteByTimer(idEvent);
	if (!pNote){
		return;
	}
	
	if(pNote->pSchedule->scType == SCH_ED){
		if((st.wHour < pNote->pSchedule->scDate.wHour)
			|| (st.wHour == pNote->pSchedule->scDate.wHour && st.wMinute < pNote->pSchedule->scDate.wMinute)){
			//continue checking
			pNote->isAlarm = TRUE;
			return;
		}
		if(pNote->pSchedule->scDate.wHour == st.wHour && pNote->pSchedule->scDate.wMinute == st.wMinute){
			if(pNote->isAlarm){
				//stop checking and alarm
				pNote->isAlarm = FALSE;
				DoAlarm(pNote);
			}
		}
	}
	else if(pNote->pSchedule->scType == SCH_ONCE){
		if((st.wYear < pNote->pSchedule->scDate.wYear) || (st.wYear == pNote->pSchedule->scDate.wYear && st.wMonth < pNote->pSchedule->scDate.wMonth)
			|| (st.wYear == pNote->pSchedule->scDate.wYear && st.wMonth == pNote->pSchedule->scDate.wMonth && st.wDay < pNote->pSchedule->scDate.wDay)
			|| (st.wYear == pNote->pSchedule->scDate.wYear && st.wMonth == pNote->pSchedule->scDate.wMonth && st.wDay == pNote->pSchedule->scDate.wDay && st.wHour < pNote->pSchedule->scDate.wHour)
			|| (st.wYear == pNote->pSchedule->scDate.wYear && st.wMonth == pNote->pSchedule->scDate.wMonth && st.wDay == pNote->pSchedule->scDate.wDay && st.wHour == pNote->pSchedule->scDate.wHour && st.wMinute < pNote->pSchedule->scDate.wMinute)){
			//continue checking
			pNote->isAlarm = TRUE;
			return;
		}
		if(pNote->pSchedule->scDate.wYear == st.wYear && pNote->pSchedule->scDate.wMonth == st.wMonth 
			&& pNote->pSchedule->scDate.wDay == st.wDay && pNote->pSchedule->scDate.wHour == st.wHour 
			&& pNote->pSchedule->scDate.wMinute == st.wMinute){

			if(pNote->isAlarm){
				//stop checking and alarm
				pNote->isAlarm = FALSE;
				DoAlarm(pNote);
			}
		}
	}
	else if(pNote->pSchedule->scType == SCH_REP){
		if(pNote->pRTHandles->nRepeat != 0){
			pNote->pRTHandles->nRepeat--;
			if(pNote->pRTHandles->nRepeat == 0){
				DoAlarm(pNote);
				pNote->pRTHandles->nRepeat = SetRepeatCount(&pNote->pSchedule->scDate);
			}
		}
	}
	else if(pNote->pSchedule->scType == SCH_PER_WD){
		int dof = RealDayOfWeek(st.wDayOfWeek);
		if(!IsBitOn(pNote->pSchedule->scDate.wDayOfWeek, dof + 1)){
		// if(pS->scDate.wDayOfWeek != st.wDayOfWeek){
			//continue checking
			pNote->isAlarm = TRUE;
			return;
		}
		else{
			if(pNote->pSchedule->scDate.wHour == st.wHour && pNote->pSchedule->scDate.wMinute == st.wMinute){
				if(pNote->isAlarm){
					//stop checking and alarm
					pNote->isAlarm = FALSE;
					DoAlarm(pNote);
				}
			}
			else{
				//continue checking
				pNote->isAlarm = TRUE;
				return;
			}
		}
	}
	else if(pNote->pSchedule->scType == SCH_PER_YR){
		if(pNote->isAlarm){
			if(pNote->pSchedule->scDate.wDay == st.wDay//the same day
				&&pNote->pSchedule->scDate.wMonth == st.wMonth
				&&pNote->pSchedule->scDate.wYear == st.wYear
				&&pNote->pSchedule->scDate.wHour == st.wHour 
				&& pNote->pSchedule->scDate.wMinute == st.wMinute){//the same time
				DoAlarm(pNote);
				SetNextYear(&pNote->pSchedule->scStart, &pNote->pSchedule->scDate, pNote->pSchedule->reserved);
			}
		}
	}
	else if(pNote->pSchedule->scType == SCH_AFTER){
		if(pNote->isAlarm){
			if(IsAfterMatch(pNote->pSchedule, &st)){
				//stop checking and alarm
				pNote->isAlarm = FALSE;
				DoAlarm(pNote);
			}
			else{
				//continue checking
				pNote->isAlarm = TRUE;
				return;
			}
		}
	}
}

static BOOL IsLeapYear(int year){
	if(((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)))
		return TRUE;
	return FALSE;
}

static BOOL IsAfterMatch(P_SCHEDULE_TYPE pS, LPSYSTEMTIME lpst){
	TIMEUNION 				ft1;
	TIMEUNION 				ft2;
	unsigned long long		res1, res2;

	//return false if starting date is in the future
	if(lpst->wYear < pS->scStart.wYear)
		return FALSE;
	if(lpst->wYear == pS->scStart.wYear && lpst->wMonth < pS->scStart.wMonth)
		return FALSE;
	if(lpst->wYear == pS->scStart.wYear && lpst->wMonth == pS->scStart.wMonth && lpst->wDay < pS->scStart.wDay)
		return FALSE;
	if(lpst->wYear == pS->scStart.wYear && lpst->wMonth == pS->scStart.wMonth && lpst->wDay == pS->scStart.wDay && lpst->wHour < pS->scStart.wHour)
		return FALSE;
	if(lpst->wYear == pS->scStart.wYear && lpst->wMonth == pS->scStart.wMonth && lpst->wDay == pS->scStart.wDay && lpst->wHour == pS->scStart.wHour && lpst->wMinute < pS->scStart.wMinute)
		return FALSE;
	if(lpst->wYear == pS->scStart.wYear && lpst->wMonth == pS->scStart.wMonth && lpst->wDay == pS->scStart.wDay && lpst->wHour == pS->scStart.wHour && lpst->wMinute == pS->scStart.wMinute && lpst->wSecond < pS->scStart.wSecond)
		return FALSE;
	SystemTimeToFileTime(lpst, &ft1.fileTime);
	SystemTimeToFileTime(&pS->scStart, &ft2.fileTime);
	res1 = (ft1.ul.QuadPart - ft2.ul.QuadPart) / 10000000;
	res2 = pS->scDate.wSecond;
	res2 += pS->scDate.wMinute * 60;
	res2 += pS->scDate.wHour * 60 * 60;
	res2 += pS->scDate.wDay * 60 * 60 * 24;
	res2 += pS->scDate.wDayOfWeek * 60 * 60 * 24 * 7;
	for(int i = 0, j = pS->scStart.wMonth, y = pS->scStart.wYear; i < pS->scDate.wMonth; i++){
		switch(j){
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			res2 += 31 * 60 * 60 * 24;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			res2 += 30 * 60 * 60 * 24;
			break;
		case 2:
			if(IsLeapYear(y))
				res2 += 29 * 60 * 60 * 24;
			else
				res2 += 28 * 60 * 60 * 24;
			break;
		}
		if(j < 12)
			j++;
		else{
			j = 1;
			y++;
		}
	}
	for(int i = 0, j = pS->scStart.wYear; i < pS->scDate.wYear; i++, j++){
		if(IsLeapYear(j) || IsLeapYear(j + 1))
			res2 += 366 * 60 * 60 * 24;
		else
			res2 += 365 * 60 * 60 * 24;
	}
	if(res1 == res2)
		return TRUE;
	return FALSE;
}

static void ShiftNotes(int pos){
	PDOCKITEM		pItem;
	PDOCKHEADER 	pHeader;
	int				offset, dockType;

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
}

static int CheckDockPositionForAlarm(HWND hwnd){
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	int					result;

	result = ArrowRequired(hwnd, DockType(pNote->pData->dockData), pNote->pData->dockData);
	if(result == -1){
		return 0;
	}
	else{
		ShiftNotes(result);
		return 1;
	}
}

static void DoAlarm(PMEMNOTE pNote){
	wchar_t				szPath[MAX_PATH];
	RECT				rc;
	HWND				hBell;
	HWND 				hNote;
	SYSTEMTIME			st;

	if(DockType(pNote->pData->dockData) == DOCK_NONE){
		CenterRestorePlacementNote(pNote, IDM_SHOW_CENTER);
		
	}
	else{
		if(pNote->pData->visible){
			while(CheckDockPositionForAlarm(hNote) == 1){
				;
			}
		}
	}
	hNote = pNote->hwnd;
	if(g_NoteSettings.visualAlert){
		hBell = (HWND)GetPropW(hNote, PH_BELL);
		if(hBell && IsWindow(hBell)){
			DestroyWindow(hBell);
		}
		CopyRect(&rc, &pNote->pRTHandles->rcTooltip);
		MapWindowPoints(hNote, HWND_DESKTOP, (LPPOINT)&rc, 2);
		if(!pNote->pRTHandles->hbSkin)
			OffsetRect(&rc, -16, -16);
		SetPropW(hNote, PH_BELL, (HANDLE)CreateBell(hNote, g_hInstance, rc.left, rc.top, pNote->pData->onTop));
	}
	if(g_Sound.allowSound){
		if(g_Sound.defSound == 0)
			PlaySoundW(__T("Notify"), 0, SND_ALIAS | SND_ASYNC);
		else{
			wcscpy(szPath, g_NotePaths.SoundDir);
			wcscat(szPath, g_Sound.custSound);
			PlaySoundW(szPath, 0, SND_FILENAME | SND_ASYNC);
		}
	}
	if(pNote->pSchedule->scType == SCH_ONCE || pNote->pSchedule->scType == SCH_AFTER){
		//stop timer and remove scheduling because alarm should fire only once
		ZeroMemory(pNote->pSchedule, sizeof(SCHEDULE_TYPE));
		KillTimer(NULL, pNote->pRTHandles->idTimer);
		WritePrivateProfileStructW(pNote->pFlags->id, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE), g_NotePaths.DataFile);
		RedrawWindow(hNote, NULL, NULL, RDW_INVALIDATE);
		ShowNoteMarks(hNote);
		if(g_hCPDialog){
			SendMessageW(g_hCPDialog, CHM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
		}
	}
	else{
		//save last alarm time
		GetLocalTime(&st);
		memcpy(&pNote->pSchedule->scLastRun, &st, sizeof(SYSTEMTIME));
		WritePrivateProfileStructW(pNote->pFlags->id, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE), g_NotePaths.DataFile);
	}
}

static BOOL CALLBACK PropEnumProc(HWND hwnd, LPCWSTR lpszString, HANDLE hData){
	if((DWORD)(VOID *)hData != 0xffffffff){
		RemovePropW(hwnd, lpszString);
	}
	return TRUE;
}

static void FormatNewNoteName(wchar_t * lpID){
	
	SYSTEMTIME		st;

	GetLocalTime(&st);
	ConstructDateTimeString(&st, lpID, TRUE);
}

static void FormatNewNoteID(wchar_t * lpID){
	SYSTEMTIME		st;
	wchar_t			buff[8];

	GetLocalTime(&st);
	_itow(st.wYear, buff, 10);
	wcscpy(lpID, buff);
	_itow(st.wMonth, buff, 10);
	wcscat(lpID, buff);
	_itow(st.wDay, buff, 10);
	wcscat(lpID, buff);
	_itow(st.wHour, buff, 10);
	wcscat(lpID, buff);
	_itow(st.wMinute, buff, 10);
	wcscat(lpID, buff);
	_itow(st.wSecond, buff, 10);
	wcscat(lpID, buff);
	_itow(st.wMilliseconds, buff, 10);
	wcscat(lpID, buff);
}

void AutosaveNote(PMEMNOTE pNote){
	HWND				hwnd = pNote->hwnd;
	wchar_t				szPath[MAX_PATH], szDBFile[MAX_PATH];
	HANDLE				hFile;
	EDITSTREAM			esm;
	RECT				rcNote;
	NOTE_REL_POSITION	nrp;
	SIZE				sz;

	if(!NotesDirExists(g_NotePaths.DataDir))
		NotesDirCreate(g_NotePaths.DataDir);
	wcscpy(szDBFile, g_NotePaths.DataDir);
	wcscat(szDBFile, AUTOSAVE_INDEX_FILE);
	if(!PathFileExistsW(szDBFile)){
		//create file with "Hidden" attribute
		hFile = CreateFileW(szDBFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			return;
		CloseHandle(hFile);
	}
	
	if(pNote->pData->visible){
		if(!pNote->pFlags->fromDB){
			GetTemporarySaveName(hwnd, pNote->pData->szName, 128);
		}
		//save window position and size
		if(DockType(pNote->pData->dockData) == DOCK_NONE){
			GetWindowRect(hwnd, &rcNote);
			//save current absolute position
			if(pNote->pRTHandles->rolled)
				rcNote.bottom = rcNote.top + (pNote->pRTHandles->rcSize.bottom - pNote->pRTHandles->rcSize.top);
			CopyRect(&pNote->pData->rcp, &rcNote);
			CopyRect(&pNote->pData->rcp, &rcNote);
			//save rolled status
			pNote->pData->rolled = pNote->pRTHandles->rolled;
			//save current relational position
			sz = GetScreenMetrics();
			nrp.left = (double)rcNote.left / (double)sz.cx;
			nrp.top = (double)rcNote.top / (double)sz.cy;
			nrp.width = rcNote.right - rcNote.left;
			nrp.height = rcNote.bottom - rcNote.top;
			WritePrivateProfileStructW(pNote->pFlags->id, IK_RELPOSITION, &nrp, sizeof(nrp), szDBFile);
		}
		//save note
		wcscpy(szPath, g_NotePaths.DataDir);
		wcscat(szPath, L"~");
		wcscat(szPath, pNote->pFlags->id);
		wcscat(szPath, L".no~");
		hFile = CreateFileW(szPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
		if(hFile != INVALID_HANDLE_VALUE){
			esm.dwCookie = (DWORD)hFile;
			esm.dwError = 0;
			esm.pfnCallback = OutStreamCallback;
			RichEdit_StreamOut((HWND)GetPropW(hwnd, PH_EDIT), SF_RTF | SF_UNICODE, &esm);
			CloseHandle(hFile);
		}
	}
	if((pNote->pFlags->maskChanged & F_SKIN) == F_SKIN 
	|| (pNote->pFlags->maskChanged & F_RTFONT) == F_RTFONT 
	|| (pNote->pFlags->maskChanged & F_C_COLOR) == F_C_COLOR
	|| (pNote->pFlags->maskChanged & F_C_FONT) == F_C_FONT
	|| (pNote->pFlags->maskChanged & F_B_COLOR) == F_B_COLOR){
		if((pNote->pFlags->maskChanged & F_SKIN) == F_SKIN)
			pNote->pAppearance->nPrivate |= F_SKIN;
		if((pNote->pFlags->maskChanged & F_RTFONT) == F_RTFONT)
			pNote->pAppearance->nPrivate |= F_RTFONT;
		if((pNote->pFlags->maskChanged & F_C_COLOR) == F_C_COLOR)
			pNote->pAppearance->nPrivate |= F_C_COLOR;
		if((pNote->pFlags->maskChanged & F_C_FONT) == F_C_FONT)
			pNote->pAppearance->nPrivate |= F_C_FONT;
		if((pNote->pFlags->maskChanged & F_B_COLOR) == F_B_COLOR)
			pNote->pAppearance->nPrivate |= F_B_COLOR;
		WritePrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, pNote->pAppearance, sizeof(NOTE_APPEARANCE), szDBFile);
	}
	if((pNote->pFlags->maskChanged & F_SCHEDULE) == F_SCHEDULE)
		WritePrivateProfileStructW(pNote->pFlags->id, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE), szDBFile);
	GetLocalTime(&pNote->pData->stChanged);
	WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, pNote->pData, sizeof(NOTE_DATA), szDBFile);
}

void SaveNoteAs(PMEMNOTE pNote){
	wchar_t			szBuffer[256];
	int				result = 0;
	BOOL			in_favorites = FALSE;

	//check whether note was in favorites
	GetPrivateProfileStringW(S_FAVORITES, pNote->pFlags->id, NULL, szBuffer, 256, g_NotePaths.INIFile);
	if(wcslen(szBuffer) > 0){
		//in such case delete its name from favorites
		in_favorites = TRUE;
		WritePrivateProfileStringW(S_FAVORITES, pNote->pFlags->id, NULL, g_NotePaths.INIFile);
	}
	if(pNote->pData->visible)
		result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_SAVE_AS), pNote->hwnd, SaveAs_DlgProc, (LPARAM)pNote);
	else
		result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_SAVE_AS), g_hMain, SaveAs_DlgProc, (LPARAM)pNote);
	if(result == IDOK){
		SaveNote(pNote);
		if(in_favorites){
			//if note was previously in favorites - save it in favorites with new name
			WritePrivateProfileStringW(S_FAVORITES, pNote->pFlags->id, pNote->pData->szName, g_NotePaths.INIFile);
		}
	}
	//make note opaque
	if(pNote->pData->visible){
		if(g_NoteSettings.transAllow){
			if(GetFocus() == (HWND)GetPropW(pNote->hwnd, PH_EDIT)){
				SetLayeredWindowAttributes(pNote->hwnd, pNote->pRTHandles->crMask, 255, LWA_COLORKEY);
				pNote->pFlags->transparent = FALSE;
			}
		}
	}
}

void SaveNote(PMEMNOTE pNote){
	
	wchar_t				szPath[MAX_PATH];
	HWND				hwnd;
	HANDLE				hFile;
	EDITSTREAM			esm;

	hwnd = pNote->hwnd;
	if(pNote->pData->visible){
		//save window position and size
		if(DockType(pNote->pData->dockData) == DOCK_NONE){
			WriteNotePlacement(hwnd, TRUE);
		}
		//save text
		if((pNote->pFlags->maskChanged & F_TEXT) == F_TEXT){	
			if(!NotesDirExists(g_NotePaths.DataDir))
				NotesDirCreate(g_NotePaths.DataDir);
			wcscpy(szPath, g_NotePaths.DataDir);
			wcscat(szPath, pNote->pFlags->id);
			wcscat(szPath, NOTE_EXTENTION);
			hFile = CreateFileW(szPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile != INVALID_HANDLE_VALUE){
				esm.dwCookie = (DWORD)hFile;
				esm.dwError = 0;
				esm.pfnCallback = OutStreamCallback;
				RichEdit_StreamOut((HWND)GetPropW(hwnd, PH_EDIT), SF_RTF | SF_UNICODE, &esm);
				CloseHandle(hFile);
				DeleteAutoSavedFile(pNote->pFlags->id);
			}
		}
	}
	if((pNote->pFlags->maskChanged & F_SKIN) == F_SKIN 
	|| (pNote->pFlags->maskChanged & F_RTFONT) == F_RTFONT 
	|| (pNote->pFlags->maskChanged & F_C_COLOR) == F_C_COLOR
	|| (pNote->pFlags->maskChanged & F_C_FONT) == F_C_FONT
	|| (pNote->pFlags->maskChanged & F_B_COLOR) == F_B_COLOR){
		if((pNote->pFlags->maskChanged & F_SKIN) == F_SKIN)
			pNote->pAppearance->nPrivate |= F_SKIN;
		if((pNote->pFlags->maskChanged & F_RTFONT) == F_RTFONT)
			pNote->pAppearance->nPrivate |= F_RTFONT;
		if((pNote->pFlags->maskChanged & F_C_COLOR) == F_C_COLOR)
			pNote->pAppearance->nPrivate |= F_C_COLOR;
		if((pNote->pFlags->maskChanged & F_C_FONT) == F_C_FONT)
			pNote->pAppearance->nPrivate |= F_C_FONT;
		if((pNote->pFlags->maskChanged & F_B_COLOR) == F_B_COLOR)
			pNote->pAppearance->nPrivate |= F_B_COLOR;
		WritePrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, pNote->pAppearance, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
	}
	if((pNote->pFlags->maskChanged & F_SCHEDULE) == F_SCHEDULE)
		WritePrivateProfileStructW(pNote->pFlags->id, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE), g_NotePaths.DataFile);

	GetLocalTime(&pNote->pData->stChanged);
	WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, pNote->pData, sizeof(NOTE_DATA), g_NotePaths.DataFile);

	pNote->pFlags->saved = TRUE;
	pNote->pFlags->fromDB = TRUE;

	if(pNote->pData->visible){
		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
		ShowNoteMarks(hwnd);
		SetTooltip(hwnd, pNote->pRTHandles->hTooltip, pNote->pData, pNote->pRTHandles, TRUE);
	}
	
	if(g_hCPDialog){
		SendMessageW(g_hCPDialog, CHM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
		//SendMessageW(g_hCPDialog, CHM_CTRL_UPD, 0, (LPARAM)pNote);
	}
}

BOOL CALLBACK SaveAs_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_INITDIALOG, SaveAs_OnInitDialog);
	HANDLE_MSG (hwnd, WM_COMMAND, SaveAs_OnCommand);
	HANDLE_MSG (hwnd, WM_CLOSE, SaveAs_OnClose);

	default: return FALSE;
	}
}

static BOOL SaveAs_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	PMEMNOTE		pNote = (PMEMNOTE)lParam;
	wchar_t 		szBuffer[256], szFile[MAX_PATH];

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);

	//set dialog and controls text
	GetPrivateProfileStringW(S_OPTIONS, __T("1002"), __T("Save note as..."), szBuffer, 256, szFile);
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDOK, szFile, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, szFile, DS_CANCEL);
	SetDlgCtlText(hwnd, IDC_ST_SAVE_AS, szFile, __T("Note name"));
	SendDlgItemMessageW(hwnd, IDC_EDT_SAVE_AS, EM_SETLIMITTEXT, 127, 0);
	//save memory note handle
	SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)lParam);
	if(pNote->pFlags->fromDB || pNote->pFlags->saved)
		wcscpy(szBuffer, pNote->pData->szName);
	else
		if(GetTemporarySaveName(pNote->hwnd, szBuffer, 128) == 0)
			wcscpy(szBuffer, pNote->pData->szName);
	SetDlgItemTextW(hwnd, IDC_EDT_SAVE_AS, szBuffer);
	return FALSE;
}

static void SaveAs_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL);
			break;
		case IDOK:{
			PMEMNOTE	pNote = (PMEMNOTE)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
			GetDlgItemTextW(hwnd, IDC_EDT_SAVE_AS, pNote->pData->szName, 128);
			if(pNote->pData->visible){
				SetNoteCaption(pNote->hwnd);
			}
			EndDialog(hwnd, IDOK);
			break;
		}
		case IDC_EDT_SAVE_AS:
			if(codeNotify == EN_CHANGE){
				if(GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_SAVE_AS)))
					EnableWindow(GetDlgItem(hwnd ,IDOK), TRUE);
				else
					EnableWindow(GetDlgItem(hwnd ,IDOK), FALSE);
			}
			break;
	}
}

static void SaveAs_OnClose(HWND hwnd)
{
	EndDialog(hwnd, IDCANCEL);
}

static LRESULT CALLBACK Note_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	PMEMNOTE			pNote;

	switch(msg){
		HANDLE_MSG (hwnd, WM_CREATE, Note_OnCreate);
		HANDLE_MSG (hwnd, WM_PAINT, Note_OnPaint);
		HANDLE_MSG (hwnd, WM_MOUSEMOVE, Note_OnMouseMove);
		HANDLE_MSG (hwnd, WM_CLOSE, Note_OnClose);
		HANDLE_MSG (hwnd, WM_COMMAND, Note_OnCommand);
		HANDLE_MSG (hwnd, WM_SETFOCUS, Note_OnSetFocus);
		HANDLE_MSG (hwnd, WM_LBUTTONDOWN, Note_OnLButtonDown);
		HANDLE_MSG (hwnd, WM_RBUTTONUP, Note_OnRButtonUp);
		HANDLE_MSG (hwnd, WM_KILLFOCUS, Note_OnKillFocus);
		HANDLE_MSG (hwnd, WM_DRAWITEM, Note_OnDrawItem);
		HANDLE_MSG (hwnd, WM_MEASUREITEM, Note_OnMeasureItem);
		HANDLE_MSG (hwnd, WM_SIZE, Note_OnSize);
		HANDLE_MSG (hwnd, WM_DROPFILES, Note_OnDropFiles);
		HANDLE_MSG (hwnd, WM_WINDOWPOSCHANGING, Note_OnWindowPosChanging);
		HANDLE_MSG (hwnd, WM_RBUTTONDOWN, Note_OnRButtonDown);
		HANDLE_MSG (hwnd, WM_LBUTTONDBLCLK, Note_OnLButtonDblClk);
		HANDLE_MSG (hwnd, WM_MOUSELEAVE, Note_OnMouseLeave);
		HANDLE_MSG (hwnd, WM_INITMENUPOPUP, Note_OnInitMenuPopup);
		case WM_SYSCOMMAND:
			//hide note window instead of closing when Alt+F4 is pressed
			if((wParam & SC_CLOSE) == SC_CLOSE){
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_HIDE, 0), 0);
				return FALSE;
			}
			return DefWindowProcW (hwnd, msg, wParam, lParam);
		case CHM_RESIZE:
			OnResizeNote(hwnd, (LPRECT)wParam);
			return FALSE;
		case CHM_SAVE:
			SaveNote(MEMNOTE(hwnd));
			return FALSE;
		case CHM_SAVE_ON_SHUTDOWN:{
			wchar_t szBuffer[128];
			pNote = MEMNOTE(hwnd);
			if(GetTemporarySaveName(hwnd, szBuffer, 128) != 0)
				wcscpy(pNote->pData->szName, szBuffer);
			SaveNote(MEMNOTE(hwnd));
			return FALSE;
		}
		//case CHM_AUTOSAVE:
			//AutosaveNote(hwnd);
			//return FALSE;
		case CHM_TT_UPDATE:{
			P_NOTE_RTHANDLES	pH;
			P_NOTE_DATA			pD;
			if(g_RTHandles.hbSkin){
				if(g_DockSettings.fCustSkin){
					pH = &g_DRTHandles;
				}
				else{
					pH = NoteRTHandles(hwnd);
				}
			}
			else{
				pH = NoteRTHandles(hwnd);
			}
			pD = NoteData(hwnd);
			SetTooltip(hwnd, pH->hTooltip, pD, pH, TRUE);
			return FALSE;
		}
		case CHM_CHANGE_FONT:
			SetREFontCommon((HWND)GetPropW(hwnd, PH_EDIT), (LPLOGFONTW)wParam);
			return FALSE;
		case CHM_CHANGE_FONT_COLOR:
			SetREFontColorCommon((HWND)GetPropW(hwnd, PH_EDIT), (COLORREF)wParam);
			return FALSE;
		case CHM_NEW_LANGUAGE:
			SetCommandsTooltips(hwnd);
			return FALSE;
		case CHM_MOVE_DOCK_NOTE:{
			P_NOTE_DATA pD = NoteData(hwnd);
			if(DockType(pD->dockData) != DOCK_NONE){
				HWND hTray = MoveDockWindow(hwnd, DockType(pD->dockData), DockIndex(pD->dockData));
				if(hTray)
					SetWindowPos(hTray, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
			}
			return FALSE;
		}
		case CHM_NEW_BGCOLOR:
			pNote = MEMNOTE(hwnd);
			RichEdit_SetBkgndColor(GetPropW(hwnd, PH_EDIT), 0, pNote->pAppearance->crWindow);
			CreateSkinlessToolbarBitmap(hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbCommand);
			CreateSkinlessSysBitmap(hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbDelHide);
			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);	
			ShowNoteMarks(hwnd);
			return FALSE;
		case CHM_APPLY_NEW_SKIN:
			ApplyNewSkin(hwnd, (P_NOTE_RTHANDLES)wParam);
			return FALSE;
		case WM_NOTIFY:{
			LPNMHDR pnmh = (LPNMHDR)lParam;
			if(pnmh->code == EN_MSGFILTER){
				MSGFILTER * lpmf = (MSGFILTER *)lParam;
				if(lpmf->msg == WM_CHAR || lpmf->msg == WM_KEYDOWN){
					pNote = MEMNOTE(hwnd);
					if(pNote->pFlags->transparent){
						//draw window opaque while typing
						SetLayeredWindowAttributes(hwnd, pNote->pRTHandles->crMask, 255, LWA_COLORKEY);
						pNote->pFlags->transparent = FALSE;
					}
					if(lpmf->msg == WM_KEYDOWN){ 
 						//if((GetKeyState(VK_CONTROL) & 0x8000) == 0x8000){
							//if((GetKeyState(VK_F) & 0x8000) == 0x8000)
								//if((GetKeyState(VK_MENU) & 0x8000) == 0x8000)
									//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_ADD_TO_FAVORITES, 0), 0);
								//else
									//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_SEARCH_NOTE, 0), 0);
							//else if((GetKeyState(VK_H) & 0x8000) == 0x8000){
								//if((GetKeyState(VK_MENU) & 0x8000) == 0x8000)
									//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_HIDE, 0), 0);
								//else
									//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_REPLACE_NOTE, 0), 0);
							//}
							//else if((GetKeyState(VK_S) & 0x8000) == 0x8000){
								//if((GetKeyState(VK_SHIFT) & 0x8000) == 0x8000)
									//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_SAVE_AS, 0), 0);
								//else
									//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_SAVE, 0), 0);
							//}
							//else if((GetKeyState(VK_P) & 0x8000) == 0x8000){
								//if((GetKeyState(VK_MENU) & 0x8000) == 0x8000)
									//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_HIGH_PRIORITY, 0), 0);
								//else
									//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_PRINT_NOTE, 0), 0);
							//}
							//else if(((GetKeyState(VK_MENU) & 0x8000) == 0x8000) && ((GetKeyState(VK_X) & 0x8000) == 0x8000))
								//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_ADJUST, 0), 0);
							//else if(((GetKeyState(VK_MENU) & 0x8000) == 0x8000) && ((GetKeyState(VK_T) & 0x8000) == 0x8000))
								//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_ON_TOP, 0), 0);
							//else if(((GetKeyState(VK_MENU) & 0x8000) == 0x8000) && ((GetKeyState(VK_F6) & 0x8000) == 0x8000))
								//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_EMAIL, 0), 0);
							//else if(((GetKeyState(VK_MENU) & 0x8000) == 0x8000) && ((GetKeyState(VK_F7) & 0x8000) == 0x8000))
								//PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_ATTACHMENT, 0), 0);	
						//}
						//else if((GetKeyState(VK_F3) & 0x8000) == 0x8000)
							//SharedFindProc((HWND)GetPropW(hwnd, PH_EDIT));
						//if((GetKeyState(VK_F3) & 0x8000) == 0x8000)
							//SharedFindProc((HWND)GetPropW(hwnd, PH_EDIT));
					}
				}
			}
			else if(pnmh->code == EN_LINK){
				ENLINK * lpel = (ENLINK *)lParam;
				if(lpel->msg == WM_LBUTTONUP){
					wchar_t 	szBuffer[256];
					TEXTRANGEW	tr;
					tr.chrg.cpMax = lpel->chrg.cpMax;
					tr.chrg.cpMin = lpel->chrg.cpMin;
					tr.lpstrText = szBuffer;
					SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), EM_GETTEXTRANGE, 0, (LPARAM)&tr);
					
					if(_wcsistr(szBuffer, L"http:") == szBuffer || _wcsistr(szBuffer, L"ftp:") == szBuffer || _wcsistr(szBuffer, L"https:") == szBuffer || _wcsistr(szBuffer, L"gopher:") == szBuffer || _wcsistr(szBuffer, L"www.") == szBuffer){
						if(wcslen(g_DefBrowser) == 0)
							ShellExecuteW(hwnd, L"open", szBuffer, NULL, NULL, SW_SHOWDEFAULT);
						else{
							if((int)ShellExecuteW(hwnd, NULL, g_DefBrowser, szBuffer, NULL, SW_SHOWDEFAULT) <= 32){
								ShellExecuteW(hwnd, L"open", szBuffer, NULL, NULL, SW_SHOWDEFAULT);
							}
						}
					}
					else{
						ShellExecuteW(hwnd, L"open", szBuffer, NULL, NULL, SW_SHOWDEFAULT);
					}
				}
			}
			else if(pnmh->code == EN_PROTECTED){
				pNote = MEMNOTE(hwnd);
				if(IsBitOn(pNote->pData->res1, NB_PROTECTED)){
					return TRUE;
				}
				else{
					return FALSE;
				}
			}
			return FALSE;
		}
		default: 
			return DefWindowProcW (hwnd, msg, wParam, lParam);
	}
}

static void Note_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu)
{
	PrepareMenuGradientColors(GetSysColor(COLOR_BTNFACE));
	if(g_NoteSettings.rollOnDblClick && !MEMNOTE(hwnd)->pRTHandles->hbSkin){
		EnableMenuItem(g_hNotePopUp, IDM_ROLL_UNROLL, MF_BYCOMMAND | MF_ENABLED);
	}
	else{
		EnableMenuItem(g_hNotePopUp, IDM_ROLL_UNROLL, MF_BYCOMMAND | MF_GRAYED);
	}
}

static void Note_OnMouseLeave(HWND hwnd){
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	POINT				pt;
	RECT				rc;

	GetCursorPos(&pt);
	GetWindowRect(hwnd, &rc);
	pNote->pFlags->mouseIn = FALSE;
	if(!PtInRect(&rc, pt)){
		dock_type dockType = DockType(pNote->pData->dockData);
		switch(dockType){
		case DOCK_LEFT:
			ShowWindow(g_DArrows.hlu, SW_HIDE);
			ShowWindow(g_DArrows.hld, SW_HIDE);
			break;
		case DOCK_TOP:
			ShowWindow(g_DArrows.htl, SW_HIDE);
			ShowWindow(g_DArrows.htr, SW_HIDE);
			break;
		case DOCK_RIGHT:
			ShowWindow(g_DArrows.hru, SW_HIDE);
			ShowWindow(g_DArrows.hrd, SW_HIDE);
			break;
		case DOCK_BOTTOM:
			ShowWindow(g_DArrows.hbl, SW_HIDE);
			ShowWindow(g_DArrows.hbr, SW_HIDE);
			break;
		}
	}
}

static void Note_OnLButtonDblClk(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	POINT				pt;
	
	pt.x = x;
	pt.y = y;

	if(PtInRect(&pNote->pRTHandles->rcCaption, pt)){
		if(pNote->pRTHandles->rolled){
			UnrollNote(pNote);
		}
		else{
			RollNote(pNote);
		}
	}

}

static void PasteLinkToFile(HWND hwnd){
	int				len;
	wchar_t			*pFile, *pPrefix = L"<file:", *pPostfix = L">";
	
	len = wcslen(m_DropFile) + wcslen(pPrefix) + wcslen(pPostfix);
	pFile = (wchar_t *)calloc(len + 1, sizeof(wchar_t));
	if(pFile){
		wcscpy(pFile, pPrefix);
		wcscat(pFile, m_DropFile);
		wcscat(pFile, pPostfix);
		SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), EM_REPLACESEL, TRUE, (LPARAM)pFile);
		free(pFile);
	}
}

static void PasteContentsOfFile(HWND hwnd){
	PMEMNOTE		pNote = MEMNOTE(hwnd);
	wchar_t			szMessage[256];
	BYTE			bytes[5];
	int				flags = 0, result = IDYES;
	ULONG			read;
	HANDLE			hFile;

	if(!pNote->pFlags->saved){
		ConstructSaveMessage(pNote->pData->szName, szMessage);
		result = MessageBoxW(hwnd, szMessage, g_Strings.SCaption, MB_YESNOCANCEL | MB_ICONQUESTION);
		if(result == IDYES)
			SaveNote(MEMNOTE(hwnd));
		else if(result == IDCANCEL)
			return;
	}
	hFile = CreateFileW(m_DropFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile != INVALID_HANDLE_VALUE){
		//check which file is it
		ReadFile(hFile, bytes, 5, &read, NULL);
		if(bytes[2] == 'r' && bytes[3] == 't' && bytes[4] == 'f'){
			flags = SF_RTF;
		}
		if((bytes[0] == 0x0ff && bytes[1] == 0x0fe) || (bytes[0] == 0x0fe && bytes[1] == 0x0ff)){
			//unicode or unicode BE
			flags |= SF_TEXT | SF_UNICODE;
			SetFilePointer(hFile, -3, 0, FILE_CURRENT);
		}
		else if(bytes[0] == 0x0ef && bytes[1] == 0x0bb && bytes[2] == 0x0bf){
			//utf8
			flags |= SF_TEXT;
		}
		else{
			flags |= SF_TEXT;
			SetFilePointer(hFile, -5, 0, FILE_CURRENT);
		}
		EDITSTREAM			esm;
		esm.dwCookie = (DWORD)hFile;
		esm.dwError = 0;
		esm.pfnCallback = InStreamCallback;
		RichEdit_StreamIn((HWND)GetPropW(hwnd, PH_EDIT), flags, &esm);
		CloseHandle(hFile);
	}		
}

static void Note_OnDropFiles(HWND hwnd, HDROP hdrop)
{
	int 			count = DragQueryFileW(hdrop, 0xffffffff, NULL, 0);

	if(count > 1){
		MessageBoxW(hwnd, g_Strings.TooManyFiles, PROG_NAME, MB_OK | MB_ICONERROR);
	}
	else{
		*m_DropFile = '\0';
		DragQueryFileW(hdrop, 0, m_DropFile, MAX_PATH);
		ShowPopUp(hwnd, g_hDropPopUp);
	}
}

static BOOL Note_OnWindowPosChanging(HWND hwnd, LPWINDOWPOS lpwpos)
{
	P_NOTE_RTHANDLES	pH = NoteRTHandles(hwnd);

	if(pH){
		if(!pH->hbSkin){
			if(lpwpos->cx < DEF_SIZE_MIN){
				lpwpos->cx = DEF_SIZE_MIN;
			}
			if(m_DblClickSize){
				if(lpwpos->cy < (pH->rcCaption.bottom - pH->rcCaption.top)){
					lpwpos->cy = pH->rcCaption.bottom - pH->rcCaption.top;
				}
			}
			else{
				if(!pH->rolled){
					if(lpwpos->cy < (pH->rcCaption.bottom - pH->rcCaption.top) + (pH->rcGrip.bottom - pH->rcGrip.top)){
						lpwpos->cy = (pH->rcCaption.bottom - pH->rcCaption.top) + (pH->rcGrip.bottom - pH->rcGrip.top);
					}
				}
				else{
					if(lpwpos->cy < (pH->rcCaption.bottom - pH->rcCaption.top)){
						lpwpos->cy = pH->rcCaption.bottom - pH->rcCaption.top;
					}
				}
			}
		}
	}
	return FALSE;
}

static void Note_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	HWND				hEdit, hGrip;

	if(pNote){
		if(pNote->pRTHandles && pNote->pRTHandles->hbSkin){
			if(DockType(pNote->pData->dockData) != DOCK_NONE){
				memcpy(pNote->pRTHandles, &g_DRTHandles, sizeof(NOTE_RTHANDLES));
			}
		}

		if(pNote->pRTHandles && !pNote->pRTHandles->hbSkin){
			if(!m_DblClickSize){
				//save note size if the resize event was not raised from double click
				GetWindowRect(hwnd, &pNote->pRTHandles->rcSize);
				MeasureSkinlessNoteRectangles(pNote->pRTHandles, cx, cy);
				hEdit = (HWND)GetPropW(hwnd, PH_EDIT);
				hGrip = (HWND)GetPropW(hwnd, PH_GRIP);
				MoveWindow(hEdit, 1, pNote->pRTHandles->rcCaption.bottom + 1, cx - 2, pNote->pRTHandles->rcGrip.top - (pNote->pRTHandles->rcCaption.bottom - pNote->pRTHandles->rcCaption.top) - 1, TRUE);
				MoveWindow(hGrip, pNote->pRTHandles->rcGrip.left, pNote->pRTHandles->rcGrip.top, pNote->pRTHandles->rcGrip.right - pNote->pRTHandles->rcGrip.left, pNote->pRTHandles->rcGrip.bottom - pNote->pRTHandles->rcGrip.top, TRUE);
				SetNoteCommandsBar(hwnd, pNote->pRTHandles);
				pNote->pRTHandles->rolled = FALSE;
			}
			else{
				;
			}
			m_DblClickSize = FALSE;
		}
	}
}

static void Note_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if(lpDrawItem->CtlType == ODT_MENU)
		DrawMItem(lpDrawItem, g_hBmpMenuNormal, g_hBmpMenuGray, CLR_MASK);
}

static void Note_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem)
{
	MeasureMItem(g_hMenuFont, lpMeasureItem);
}

static void Note_OnKillFocus(HWND hwnd, HWND hwndNewFocus)
{
	PMEMNOTE	pNote = MEMNOTE(hwnd);

	if(pNote){
		if(hwndNewFocus != (HWND)GetPropW(hwnd, PH_EDIT)){
			if(g_NoteSettings.transAllow){
				SetLayeredWindowAttributes(hwnd, pNote->pRTHandles->crMask, g_NoteSettings.transValue, LWA_COLORKEY | LWA_ALPHA);
				pNote->pFlags->transparent = TRUE;
			}
		}
	}
}

static void Note_OnRButtonUp(HWND hwnd, int x, int y, UINT flags)
{
	PMEMNOTE		pNote = MEMNOTE(hwnd);
	HMENU			hMenu = g_hNotePopUp;
	dock_type		dockType;

	m_hTempMenu = hMenu;
	
	dockType = DockType(pNote->pData->dockData);
	if(pNote->pFlags->maskChanged && !pNote->pFlags->saved)
		EnableMenuItem(hMenu, IDM_SAVE, MF_BYCOMMAND | MF_ENABLED);
	else
		EnableMenuItem(hMenu, IDM_SAVE, MF_BYCOMMAND | MF_GRAYED);

	if(g_hAdjust == NULL){
		EnableMenuItem(hMenu, IDM_ADJUST_APPEARANCE, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hMenu, IDM_ADJUST_SCHEDULE, MF_BYCOMMAND | MF_ENABLED);
	}
	else{
		EnableMenuItem(hMenu, IDM_ADJUST_APPEARANCE, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenu, IDM_ADJUST_SCHEDULE, MF_BYCOMMAND | MF_GRAYED);
	}

	PrepareDockMenu(MEMNOTE(hwnd), hMenu, GetMenuPosition(hMenu, IDM_DOCK));
	AddGroupsMenuItems(hwnd, hMenu);

	if(pNote->pData->onTop || dockType != DOCK_NONE)
		CheckMenuItem(hMenu, IDM_ON_TOP, MF_CHECKED | MF_BYCOMMAND);
	else
		CheckMenuItem(hMenu, IDM_ON_TOP, MF_UNCHECKED | MF_BYCOMMAND);

	if(IsBitOn(pNote->pData->res1, NB_HIGH_PRIORITY)){
		CheckMenuItem(hMenu, IDM_HIGH_PRIORITY, MF_CHECKED | MF_BYCOMMAND);
	}
	else{
		CheckMenuItem(hMenu, IDM_HIGH_PRIORITY, MF_UNCHECKED | MF_BYCOMMAND);
	}
	if(IsBitOn(pNote->pData->res1, NB_PROTECTED)){
		CheckMenuItem(hMenu, IDM_TOGGLE_PROTECTION, MF_CHECKED | MF_BYCOMMAND);
	}
	else{
		CheckMenuItem(hMenu, IDM_TOGGLE_PROTECTION, MF_UNCHECKED | MF_BYCOMMAND);
	}
	if(IsBitOn(pNote->pData->res1, NB_COMPLETED)){
		CheckMenuItem(hMenu, IDM_MARK_AS_COMPLETED, MF_CHECKED | MF_BYCOMMAND);
	}
	else{
		CheckMenuItem(hMenu, IDM_MARK_AS_COMPLETED, MF_UNCHECKED | MF_BYCOMMAND);
	}
	if(dockType != DOCK_NONE){
		EnableMenuItem(hMenu, IDM_ON_TOP, MF_BYCOMMAND | MF_GRAYED);
	}
	else{
		EnableMenuItem(hMenu, IDM_ON_TOP, MF_BYCOMMAND | MF_ENABLED);
	}

	ShowPopUp(hwnd, hMenu);
}

static void SetDockType(int * dockData, dock_type dockType){
	short		th = HIWORD(*dockData);

	*dockData = MAKELONG(dockType, th);
}

static void UndockNote(PMEMNOTE pNote){

	dock_type			dockType;
	PDOCKITEM			pRemain;
	P_NOTE_DATA			ptd;
	HWND				hTray = NULL;
	BOOL				rolled;

	rolled = pNote->pSavedHandles->rolled;

	dockType = DockType(pNote->pData->dockData);
	pRemain = NextDockItem(DHeader(dockType), pNote->hwnd);
	SetDockType(&pNote->pData->dockData, DOCK_NONE);
	SetDockIndex(&pNote->pData->dockData, -777);
	SaveNewDockData(pNote->hwnd, pNote->pData->dockData);
	RemoveDockItem(DHeader(dockType), pNote->hwnd);
	
	//restore saved run-time handles
	memcpy(pNote->pRTHandles, pNote->pSavedHandles, sizeof(NOTE_RTHANDLES));

	MoveWindow(pNote->hwnd, pNote->pData->rcp.left, pNote->pData->rcp.top, pNote->pData->rcp.right - pNote->pData->rcp.left, pNote->pData->rcp.bottom - pNote->pData->rcp.top, TRUE);

	if(!pNote->pData->prevOnTop){
		SetWindowPos(pNote->hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	
	if(g_RTHandles.hbSkin){
		 //if(g_DockSettings.fCustSkin){
			ApplyNewSkin(pNote->hwnd, pNote->pRTHandles);
		 //}
	}
	else{
		if(g_DockSettings.fCustColor){
			CreateSkinlessToolbarBitmap(pNote->hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbCommand);
			CreateSkinlessSysBitmap(pNote->hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbDelHide);
			RichEdit_SetBkgndColor(GetPropW(pNote->hwnd, PH_EDIT), 0, pNote->pAppearance->crWindow);
		}
		RedrawWindow(pNote->hwnd, NULL, NULL, RDW_INVALIDATE);
	}

	while(pRemain){
		ptd = NoteData(pRemain->hwnd);
		// itmp = DockIndex(ptd->dockData);
		// itmp--;
		SetDockIndex(&ptd->dockData, pRemain->index);
		hTray = MoveDockWindow(pRemain->hwnd, dockType, pRemain->index);
		pRemain = pRemain->pNext;
	}

	if(rolled){
		pNote->pRTHandles->rolled = FALSE;
		SendMessageW(pNote->hwnd, WM_LBUTTONDBLCLK, MK_LBUTTON, MAKELPARAM(pNote->pRTHandles->rcCaption.left + 1, pNote->pRTHandles->rcCaption.top + 1));
	}
	
	ShowNoteMarks(pNote->hwnd);

	if(hTray)
		SetWindowPos(hTray, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
}

static void DockNote(HWND hwnd, dock_type dockType, P_NOTE_DATA pD){
	dock_type			dtPrev;
	PDOCKITEM			pRemain, pTemp;
	HWND				hTray = NULL;
	BOOL				rolled;
	PMEMNOTE			pNote = MEMNOTE(hwnd);

	rolled = pNote->pRTHandles->rolled;

	dtPrev = DockType(pD->dockData);

	if(dtPrev != DOCK_NONE){
		pRemain = NextDockItem(DHeader(dtPrev), hwnd);
	}
	else{
		//save run-time handles
		memcpy(pNote->pSavedHandles, pNote->pRTHandles, sizeof(NOTE_RTHANDLES));
	}

	SetDockType(&pD->dockData, dockType);
	pTemp = AddDockItem(DHeader(dockType), hwnd, -1);
	SetDockIndex(&pD->dockData, pTemp->index);
	SaveNewDockData(hwnd, pD->dockData);
	hTray = MoveDockWindow(hwnd, dockType, pTemp->index);
	if(hTray)
		SetWindowPos(hTray, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	if(g_RTHandles.hbSkin){
		if(g_DockSettings.fCustSkin){
			ApplyNewSkin(hwnd, &g_DRTHandles);
		}
	}
	else{
		if(g_DockSettings.fCustColor || g_DockSettings.fCustCaption){
			ApplyDockColors(MEMNOTE(hwnd));
		}
		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
	}

	//if(!pD->onTop){
		SetWindowPos(pNote->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	//}

	if(dtPrev != DOCK_NONE){
		P_NOTE_DATA		ptd;
		RemoveDockItem(DHeader(dtPrev), hwnd);
		while(pRemain){
			ptd = NoteData(pRemain->hwnd);
			SetDockIndex(&ptd->dockData, pRemain->index);
			hTray = MoveDockWindow(pRemain->hwnd, dtPrev, pRemain->index);
			pRemain = pRemain->pNext;
		}
		if(hTray)
			SetWindowPos(hTray, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	}

	ShowNoteMarks(hwnd);
	pNote->pSavedHandles->rolled = rolled;
}

static void AddGroupsMenuItems(HWND hwnd, HMENU hMenu){
	HMENU					hGroups;
	P_PNGROUP				ppg;
	MENUITEMINFOW			mi;
	int						count, pos = 0, groupsCount;
	PMEMNOTE				pNote = MEMNOTE(hwnd);
	wchar_t					szBuffer[256], szFile[MAX_PATH], szAcc[128];
	MItem					mit, *pmi;
	P_GROUP_MENU_STRUCT		pGroups, pTemp;

	ZeroMemory(&mit, sizeof(mit));
	mit.xPos = -1;
	mit.yPos = -1;
	mit.xCheck = 22;

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);
	hGroups = GetSubMenu(hMenu, GetMenuPosition(hMenu, IDM_NOTE_GROUPS));
	count = GetMenuItemCount(hGroups);
	FreeMenus(hGroups);
	for(int i = count -1; i >= 0; i--){
		DeleteMenu(hGroups, i, MF_BYPOSITION);
	}

	ZeroMemory(&mi, sizeof(mi));
	mi.cbSize = sizeof(mi);
	mi.fMask = MIIM_DATA | MIIM_ID;
	mi.wID = IDM_ADD_TO_FAVORITES;
	GetMenuItemInfoW(g_hNotePopUp, IDM_ADD_TO_FAVORITES, FALSE, &mi);
	pmi = (PMItem)mi.dwItemData;
	if(pmi){
		GetMIReserved(NoteMenu(), NoteMenuCount(), IDM_ADD_TO_FAVORITES, szAcc);
		GetPrivateProfileStringW(S_FAVORITES, pNote->pFlags->id, NULL, szBuffer, 256, g_NotePaths.INIFile);
		if(wcslen(szBuffer) > 0){
			SetMenuTextWithAccelerator(IDM_REMOVE_FAVORITES, S_MENU, szFile, L"Remove From Favorites", pmi->szText, szAcc);
			pmi->xPos = 69;
		}
		else{
			SetMenuTextWithAccelerator(IDM_ADD_TO_FAVORITES, S_MENU, szFile, L"Add To Favorites", pmi->szText, szAcc);
			pmi->xPos = 68;
		}
	}
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
				//move next
				ppg = ppg->next;
			}
			pTemp = pGroups;
			if(groupsCount > 1){
				qsort(pTemp, groupsCount, sizeof(GROUP_MENU_STRUCT), GroupMenusCompare);
			}
			for(int i = 0; i < groupsCount; i++){
				mit.id = pTemp->id + GROUP_ADDITION;
				wcscpy(mit.szText, pTemp->pName);
				if(pTemp->id == pNote->pData->idGroup){
					AppendMenuW(hGroups, MF_STRING | MF_CHECKED, mit.id, mit.szText);
				}
				else{
					AppendMenuW(hGroups, MF_STRING , mit.id, mit.szText);
				}
				SetMenuItemProperties(&mit, hGroups, pos++);
				pTemp++;
			}
			free(pGroups);
		}
	}
}

static void Note_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	RemoveBell(hwnd);
}

static void Note_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	POINT				pt;
	
	RemoveBell(hwnd);
	pt.x = x;
	pt.y = y;
	if(pNote->pRTHandles->hbSkin){
		pNote->pFlags->inDrag = TRUE;
		SystemParametersInfoW(SPI_GETDRAGFULLWINDOWS, 0, &m_fFullDrag, 0);
	}
	else{
		if(PtInRect(&pNote->pRTHandles->rcCaption, pt)){
			pNote->pFlags->inDrag = TRUE;
			SystemParametersInfoW(SPI_GETDRAGFULLWINDOWS, 0, &m_fFullDrag, 0);
		}
		else
			pNote->pFlags->inDrag = FALSE;
	}
}

static void Note_OnSetFocus(HWND hwnd, HWND hwndOldFocus)
{
	SetFocus((HWND)GetPropW(hwnd, PH_EDIT));
}

static void Note_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	PMEMNOTE			pNote;
	int					result = 0;
	BOOL				delete = FALSE, changed = FALSE;
	ADJUST_STRUCT		as;

	switch(id){
		case IDM_ROLL_UNROLL:
			pNote = MEMNOTE(hwnd);
			if(pNote->pRTHandles->rolled){
				UnrollNote(pNote);
			}
			else{
				RollNote(pNote);
			}
			break;
		case IDM_DROP_SHORTCUT:
			PasteLinkToFile(hwnd);
			break;
		case IDM_DROP_CONTENT:
			PasteContentsOfFile(hwnd);
			break;
		case IDM_ADJUST_APPEARANCE:
		case IDM_ADJUST_SCHEDULE:
			pNote = MEMNOTE(hwnd);
			as.pNote = pNote;
			if(id == IDM_ADJUST_APPEARANCE)
				as.type = 0;
			else
				as.type = 1;
			m_Reload = FALSE;
			DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_ADJUST), hwnd, Adjust_DlgProc, (LPARAM)&as);
			if(!m_Reload && !g_IsClosing){
				//make note opaque
				SetLayeredWindowAttributes(hwnd, pNote->pRTHandles->crMask, 255, LWA_COLORKEY);
				pNote->pFlags->transparent = FALSE;
			}
			g_hAdjust = NULL;
			break;
		case IDM_SAVE:
			pNote = MEMNOTE(hwnd);
			SavePNote(pNote);
			//make note opaque
			if(g_NoteSettings.transAllow){
				if(GetFocus() == (HWND)GetPropW(hwnd, PH_EDIT)){
					SetLayeredWindowAttributes(hwnd, pNote->pRTHandles->crMask, 255, LWA_COLORKEY);
					pNote->pFlags->transparent = FALSE;
				}
			}
			break;
		case IDM_ATTACHMENT:{
			pNote = MEMNOTE(hwnd);
			if(!pNote->pFlags->saved){
				if(MessageBoxW(hwnd, g_Strings.SaveBeforeSend, g_Strings.SCaption, MB_YESNO | MB_ICONQUESTION) == IDNO){
					break;
				}
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDM_SAVE, 0), 0);
			}
			SendNoteAsAttachment(pNote);
			break;
		}
		case IDM_SAVE_AS:
			SaveNoteAs(MEMNOTE(hwnd));
			break;
		case IDM_HIDE:
			HideNote(MEMNOTE(hwnd));
			break;
		case IDM_DEL:{
			BOOL		delComplete = FALSE;
			wchar_t		szMessage[256];
			int			iGroup;

			pNote = MEMNOTE(hwnd);
			
			iGroup = pNote->pData->idGroup;
			WritePrivateProfileStringW(S_FAVORITES, pNote->pFlags->id, NULL, g_NotePaths.INIFile);

			if((GetKeyState(VK_LSHIFT) & 0x8000) == 0x8000){
				delComplete = TRUE;
				wcscpy(szMessage, g_Strings.DMessageComplete);
			}
			else{
				wcscpy(szMessage, g_Strings.DMessageBin);
			}
			if(codeNotify == CHN_DEL_WITH_GROUP){
				//do not ask question when delete with group
				delete = TRUE;
			}
			else{
				if(g_NoteSettings.confirmDelete){
					result = MessageBoxW(hwnd, szMessage, g_Strings.DCaption, MB_ICONQUESTION | MB_YESNO);
					if (result == IDYES){
						delete = TRUE;
					}
				}
				else
					delete = TRUE;
			}

			if(delete){
				//remove from favorites
				WritePrivateProfileStringW(S_FAVORITES, pNote->pFlags->id, NULL, g_NotePaths.INIFile);
				if(g_hCPDialog){
					if(codeNotify != CHN_DEL_WITH_GROUP){
						//refresh control panel only when delete note (not group in whole)
						SendMessageW(g_hCPDialog, CHM_CTRL_DEL, 0, (LPARAM)pNote);
					}
					SendMessageW(g_hCPDialog, CHM_UPDATE_CTRL_STATUS, 0, 0);
				}
				if(delComplete){
					DeleteNoteCompletely(pNote);
				}
				else{
					if(codeNotify == CHN_DEL_WITH_GROUP)
						DeleteToBin(pNote, 0);
					else
						DeleteToBin(pNote, -1);
				}
				if(g_hCPDialog){
					if(codeNotify != CHN_DEL_WITH_GROUP){
						//refresh control panel only when delete note (not group in whole)
						SendMessageW(g_hCPDialog, CHM_RELOAD, 0, 0);
					}
				}
			}
			if(g_hCPDialog){
				SendMessageW(g_hCPDialog, CHM_CTRL_GROUP_UPDATE, iGroup, GROUP_RECYCLE);
			}
			break;
		}
		case IDM_EMAIL:
			SendNoteByEmail(MEMNOTE(hwnd));
			break;
		case IDM_HIGH_PRIORITY:
			ToggleNotePriority(MEMNOTE(hwnd));
			break;
		case IDM_TOGGLE_PROTECTION:
			ToggleNoteProtection(MEMNOTE(hwnd));
			break;
		case IDM_MARK_AS_COMPLETED:
			ToggleNoteCompleted(MEMNOTE(hwnd));
			break;
		case IDM_PRINT_NOTE:
			pNote = MEMNOTE(hwnd);
			PrintEdit((HWND)GetPropW(pNote->hwnd, PH_EDIT), pNote->pData->szName);
			break;
		case IDM_ON_TOP:
			pNote = MEMNOTE(hwnd);
			
			if(codeNotify == 0){
				result = GetMenuState(g_hNotePopUp, IDM_ON_TOP, MF_BYCOMMAND);
				if((result & MF_CHECKED) != MF_CHECKED){
					CheckMenuItem(g_hNotePopUp, IDM_ON_TOP, MF_CHECKED | MF_BYCOMMAND);
				}
				else{
					CheckMenuItem(g_hNotePopUp, IDM_ON_TOP, MF_UNCHECKED | MF_BYCOMMAND);
				}
				changed = TRUE;
				ToggleNoteOnTop(pNote);
			}
			else{
				CheckMenuItem(g_hNotePopUp, IDM_ON_TOP, MF_CHECKED | MF_BYCOMMAND);
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}

			if(changed && codeNotify == 0){
				RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
				ShowNoteMarks(hwnd);
				if(g_hCPDialog)
					SendMessageW(g_hCPDialog, CHM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
			}
			break;
		case IDC_NOTE_EDIT:
			if(codeNotify == EN_CHANGE && !m_JustCreated){
				pNote = MEMNOTE(hwnd);		
				pNote->pFlags->maskChanged |= F_TEXT;
				if(pNote->pFlags->saved){
					pNote->pFlags->saved = FALSE;
					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
					ShowNoteMarks(hwnd);
					if(g_hCPDialog){
						SendMessageW(g_hCPDialog, CHM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
					}
				}
			}
			break;
		case IDM_UNDO:
			SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), EM_UNDO, 0, 0);
			break;
		case IDM_REDO:
			SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), EM_REDO, 0, 0);
			break;
		case IDM_CUT:
			SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), WM_CUT, 0, 0);
			break;
		case IDM_COPY:
			SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), WM_COPY, 0, 0);
			break;
		case IDM_PASTE:
			SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), WM_PASTE, 0, 0);
			break;
		case IDM_SELECT_ALL:{
			CHARRANGE	chr;
			chr.cpMin = 0;
			chr.cpMax = -1;
			SendMessageW((HWND)GetPropW(hwnd, PH_EDIT), EM_EXSETSEL, 0, (LPARAM)&chr);
			break;
		}
		case IDM_SEARCH_NOTE:
		case IDM_REPLACE_NOTE:{
			NFIND nf;
			nf.hNote = hwnd;
			nf.iFindReplace = id;
			if(g_hFindDialog == NULL)
				DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_FIND_SINGLE), g_hMain, Find_DlgProc, (LPARAM)&nf);
			else
				SendMessageW(g_hFindDialog, CHM_NEW_FIND_PARENT, 0, (LPARAM)&nf);
			break;
		}
		case IDM_SEARCH_NOTE_NEXT:
			if(SharedFindProc((HWND)GetPropW(hwnd, PH_EDIT)) == -1){
				wchar_t		szBuffer[256], szKey[16], szFile[MAX_PATH];
				wcscpy(szFile, g_NotePaths.LangDir);
				wcscat(szFile, g_NotePaths.LangFile);
				_itow(DLG_FIND_SINGLE, szKey, 10);
				GetPrivateProfileStringW(S_OPTIONS, szKey, L"Find", szBuffer, 256, szFile);
				MessageBoxW(hwnd, g_Strings.NoMatchesFound, szBuffer, MB_OK | MB_ICONINFORMATION);
			}
			break;
		case IDM_DOCK_LEFT:
			pNote = MEMNOTE(hwnd);
			DockNote(hwnd, DOCK_LEFT, pNote->pData);
			break;
		case IDM_DOCK_TOP:
			pNote = MEMNOTE(hwnd);
			DockNote(hwnd, DOCK_TOP, pNote->pData);
			break;
		case IDM_DOCK_RIGHT:
			pNote = MEMNOTE(hwnd);
			DockNote(hwnd, DOCK_RIGHT, pNote->pData);
			break;
		case IDM_DOCK_BOTTOM:
			pNote = MEMNOTE(hwnd);
			DockNote(hwnd, DOCK_BOTTOM, pNote->pData);
			break;
		case IDM_DOCK_NONE:
			UndockNote(MEMNOTE(hwnd));
			break;
		case IDM_ADD_TO_FAVORITES:
			AddToFavorites(MEMNOTE(hwnd));
			break;
		case IDM_FORMAT_FONT_FAMILY:{
			P_NCMD_BAR			pC;
			CBCREATION 			cbr;

			pC = (P_NCMD_BAR)GetWindowLongPtrW(hwnd, OFFSET_COMMANDS);
			cbr.hParent = g_hMain;
			cbr.hButton = pC->hCmd;
			cbr.hEdit = (HWND)GetPropW(hwnd, PH_EDIT);
			SetCurrentFontFace((HWND)GetPropW(hwnd, PH_EDIT));
			CreateComboDialog(&cbr, g_hInstance);
			break;
		}
		case IDM_FORMAT_FONT_BOLD:
		case IDM_FORMAT_FONT_ITALIC:
		case IDM_FORMAT_FONT_UNDER:
		case IDM_FORMAT_FONT_STRIKE:
		case IDM_FORMAT_FONT_HILIT:
		case IDM_FORMAT_ALIGN_LEFT:
		case IDM_FORMAT_ALIGN_CENTER:
		case IDM_FORMAT_ALIGN_RIGHT:
		case IDM_FORMAT_BULLETS:
			
			SendMessageW((HWND)GetPropW(hwnd, PH_COMMANDS), WM_COMMAND, MAKEWPARAM(id, 0), 0);
			break;
		default:
			if(codeNotify == 0 && hwndCtl == 0){	
				if(id >= IDI_FORMAT_COLOR_START && id <= IDI_FORMAT_COLOR_END){
					m_CurrColorsChoice = CC_TEXT;
					ColorChanged(hwnd, id);
				}
				else if(id >= IDI_FORMAT_HIGHLIGHT_START && id <= IDI_FORMAT_HIGHLIGHT_END){
					m_CurrColorsChoice = CC_HIGHLIGHT;
					ColorChanged(hwnd, id);
				}
				else if(id > IDI_SIZE_START && id <= IDI_SIZE_START + MAX_FONT_SIZE){
					//size menu
					SetREFontSize(hwnd, id - IDI_SIZE_START);
					SetFocus(hwnd);
					//if(id - IDI_SIZE_START != GetCFSize()){
						//SetREFontSize(hwnd, id - IDI_SIZE_START);
						//SetFocus(hwnd);
					//}
				}
				else{	//from groups menu
					HMENU			hGroups;
					NOTE_DATA		data;
					int				iCurrent, iNew;

					pNote = MEMNOTE(hwnd);
					hGroups = GetSubMenu(g_hNotePopUp, GetMenuPosition(g_hNotePopUp, IDM_NOTE_GROUPS));	
					if((GetMenuState(hGroups, id, MF_BYCOMMAND) & MFS_CHECKED) != MFS_CHECKED){
						iCurrent = pNote->pData->idGroup;
						iNew = id - GROUP_ADDITION;

						pNote->pData->idGroup = id - GROUP_ADDITION;
						if(pNote->pFlags->fromDB){
							if(GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
								data.idGroup = pNote->pData->idGroup;
								WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
							}
						}
						if(g_hCPDialog){
							//SendMessageW(g_hCPDialog, CHM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
							SendMessageW(g_hCPDialog, CHM_CTRL_GROUP_UPDATE, iCurrent, iNew);
						}
					}
				}
			}
			else{
				DefWindowProcW(hwnd, WM_COMMAND, MAKEWPARAM(codeNotify, id), (LPARAM)hwndCtl);
			}
	}
}

static void Note_OnClose(HWND hwnd)
{
	PMEMNOTE		pNote = MEMNOTE(hwnd);

	if(pNote){
		//delete commands bitmap for skinless note
		if(pNote->pRTHandles && !pNote->pRTHandles->hbSkin){
			DeleteObject(pNote->pRTHandles->hbCommand);
			DeleteObject(pNote->pRTHandles->hbDelHide);
		}
	}
	EnumPropsW(hwnd, PropEnumProc);
	free((P_NCMD_BAR)GetWindowLongPtrW(hwnd, OFFSET_COMMANDS));
	free((P_NCMD_BAR)GetWindowLongPtrW(hwnd, OFFSET_DH));
	free((HWND *)GetWindowLongPtrW(hwnd, OFFSET_SYS));
	DestroyWindow(hwnd);
}

static BOOL Note_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	CenterWindow(hwnd, FALSE);
	return TRUE;
}

static void Note_OnPaint(HWND hwnd)
{
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	PAINTSTRUCT			ps;

	if(pNote){
		BeginPaint(hwnd, &ps);
		if(pNote->pRTHandles->hbSkin){
			if(DockType(pNote->pData->dockData) == DOCK_NONE){
				PaintSkinnableWindow(hwnd, ps.hdc, pNote->pRTHandles);
			}
			else{
				if(g_DockSettings.fCustSkin){
					PaintSkinnableWindow(hwnd, ps.hdc, &g_DRTHandles);
				}
				else{
					if((pNote->pAppearance->nPrivate & F_SKIN) != F_SKIN){
						PaintSkinnableWindow(hwnd, ps.hdc, pNote->pRTHandles);
					}
					else{
						PaintSkinnableWindow(hwnd, ps.hdc, &g_DRTHandles);
					}
				}
			}
		}
		else
			PaintSimpleWindow(hwnd, ps.hdc, pNote->pRTHandles);
		EndPaint(hwnd, &ps);
	}
}

static void Note_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	POINT				pt;
	BOOL				dragIt = FALSE;

	if(keyFlags == MK_LBUTTON){
		pt.x = x;
		pt.y = y;
		if(pNote->pRTHandles->hbSkin){
			if(pNote->pFlags->inDrag){
				dragIt = TRUE;
			}
		}
		else{
			if(PtInRect(&pNote->pRTHandles->rcCaption, pt) && pNote->pFlags->inDrag){
				dragIt = TRUE;
			}
		}
		//drag full window
		if(dragIt && DockType(pNote->pData->dockData) == DOCK_NONE){
			ReleaseCapture();
			SystemParametersInfoW(SPI_SETDRAGFULLWINDOWS, TRUE, 0, 0);
			SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
			SystemParametersInfoW(SPI_SETDRAGFULLWINDOWS, m_fFullDrag, 0, 0);
			if(pNote->pFlags->fromDB)
				WriteNotePlacement(hwnd, TRUE);
			else
				WriteNotePlacement(hwnd, FALSE);
		}
	}
	else if(keyFlags == 0 && !pNote->pFlags->mouseIn){
		if(DockType(pNote->pData->dockData) != DOCK_NONE){
			int result = ArrowRequired(hwnd, DockType(pNote->pData->dockData), pNote->pData->dockData);
			if(result != -1){
				HWND hTemp;
				switch(result){
				case DA_LEFT_UP:
					hTemp = g_DArrows.hlu;
					break;
				case DA_LEFT_DOWN:
					hTemp = g_DArrows.hld;
					break;
				case DA_TOP_LEFT:
					hTemp = g_DArrows.htl;
					break;
				case DA_TOP_RIGHT:
					hTemp = g_DArrows.htr;
					break;
				case DA_RIGHT_UP:
					hTemp = g_DArrows.hru;
					break;
				case DA_RIGHT_DOWN:
					hTemp = g_DArrows.hrd;
					break;
				case DA_BOTTOM_LEFT:
					hTemp = g_DArrows.hbl;
					break;
				case DA_BOTTOM_RIGHT:
					hTemp = g_DArrows.hbr;
					break;
				}
				// ShowWindow(hTemp, SW_SHOWNORMAL);
				SetWindowPos(hTemp, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
				pNote->pFlags->mouseIn = TRUE;
				TRACKMOUSEEVENT	tm;
				tm.cbSize = sizeof(tm);
				tm.hwndTrack = hwnd;
				tm.dwHoverTime = 0;
				tm.dwFlags = TME_LEAVE;
				TrackMouseEvent(&tm);
			}
		}
	}
}

void UnrollNote(PMEMNOTE pNote){
	RECT		rc;

	if(g_NoteSettings.rollOnDblClick){
		if(!pNote->pRTHandles->hbSkin && DockType(pNote->pData->dockData) == DOCK_NONE && pNote->pData->visible){
			if(pNote->pRTHandles->rolled){
				GetWindowRect(pNote->hwnd, &rc);
				MoveWindow(pNote->hwnd, rc.left, rc.top, rc.right - rc.left, pNote->pRTHandles->rcSize.bottom - pNote->pRTHandles->rcSize.top, TRUE);
				pNote->pRTHandles->rolled = FALSE;
				pNote->pData->rolled = FALSE;
			}
		}
	}
}

void RollNote(PMEMNOTE pNote){
	RECT		rc;

	if(g_NoteSettings.rollOnDblClick){
		if(!pNote->pRTHandles->hbSkin && DockType(pNote->pData->dockData) == DOCK_NONE && pNote->pData->visible){
			if(!pNote->pRTHandles->rolled){
				//if(fromNote)
					GetWindowRect(pNote->hwnd, &rc);
				//else
					//CopyRect(&rc, &pNote->pData->rcp);
				m_DblClickSize = TRUE;
				CopyRect(&pNote->pRTHandles->rcSize, &rc);
				CopyRect(&pNote->pData->rcp, &rc);
				MoveWindow(pNote->hwnd, rc.left, rc.top, rc.right - rc.left, pNote->pRTHandles->rcCaption.bottom - pNote->pRTHandles->rcCaption.top, TRUE);
				pNote->pRTHandles->rolled = TRUE;
				pNote->pData->rolled = TRUE;
				m_DblClickSize = FALSE;
			}
		}
	}
	
}

static int ArrowRequired(HWND hwnd, dock_type dockType, int dockData){
	RECT			rcDesktop, rc, rcTray, rcReal;
	int				mcount, index;
	short 			count = DHeader(dockType)->count;
	HWND			hTray;

	GetWindowRect(hwnd, &rc);
	GetClientRect(GetDesktopWindow(), &rcDesktop);
	mcount = GetSystemMetrics(SM_CMONITORS);
	if(mcount > 1){
		rcDesktop.right = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		rcDesktop.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	}
	hTray = FindWindow("Shell_TrayWnd", NULL);
	GetWindowRect(hTray, &rcTray);
	SubtractRect(&rcReal, &rcDesktop, &rcTray);

	index = DockIndex(dockData);
	switch(dockType){
	case DOCK_LEFT:
		if(rc.top < rcReal.top){
			return DA_LEFT_UP;
		}
		else if(rc.bottom > rcReal.bottom){
			return DA_LEFT_DOWN;
		}
		else if(index < count - 1 && rc.bottom == rcReal.bottom){
			return DA_LEFT_DOWN;
		}
		else if(index > 0 && rc.top == rcReal.top){
			return DA_LEFT_UP;
		}
		break;
	case DOCK_TOP:
		if(rc.left < rcReal.left){
			return DA_TOP_LEFT;
		}
		else if(rc.right > rcReal.right){
			return DA_TOP_RIGHT;
		}
		else if(index < count - 1 && rc.right == rcReal.right){
			return DA_TOP_RIGHT;
		}
		else if(index > 0 && rc.left == rcReal.left){
			return DA_TOP_LEFT;
		}
		break;
	case DOCK_RIGHT:
		if(rc.top < rcReal.top){
			return DA_RIGHT_UP;
		}
		else if(rc.bottom > rcReal.bottom){
			return DA_RIGHT_DOWN;
		}
		else if(index < count - 1 && rc.bottom == rcReal.bottom){
			return DA_RIGHT_DOWN;
		}
		else if(index > 0 && rc.top == rcReal.top){
			return DA_RIGHT_UP;
		}
		break;
	case DOCK_BOTTOM:
		if(rc.left < rcReal.left){
			return DA_BOTTOM_LEFT;
		}
		else if(rc.right > rcReal.right){
			return DA_BOTTOM_LEFT;
		}
		else if(index < count - 1 && rc.right == rcReal.right){
			return DA_BOTTOM_RIGHT;
		}
		else if(index > 0 && rc.left == rcReal.left){
			return DA_BOTTOM_LEFT;
		}
		break;
	}
	
	return -1;
}

static void OnResizeNote(HWND hwnd, LPRECT lpRc){

	RECT				rc;
	PMEMNOTE			pNote = MEMNOTE(hwnd);

	if(pNote->pRTHandles && pNote->pRTHandles->hbSkin){
		if(DockType(pNote->pData->dockData) != DOCK_NONE){
			if(g_DockSettings.fCustSkin){
				memcpy(pNote->pRTHandles, &g_DRTHandles, sizeof(NOTE_RTHANDLES));
			}
		}
	}

	if((pNote->pFlags->maskChanged & F_SKIN) == F_SKIN)
		CopyRect(&rc, &pNote->pRTHandles->rcEdit);
	else
		CopyRect(&rc, lpRc);
	//resize edit box when skin is changed
	MoveWindow((HWND)GetPropW(hwnd, PH_EDIT), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
	//move commands bar
	if(pNote->pRTHandles && pNote->pRTHandles->hbSkin)
		SetNoteCommandsBar(hwnd, pNote->pRTHandles);
}

static void SetNoteCommandsBar(HWND hwnd, P_NOTE_RTHANDLES pH){
	P_NCMD_BAR			pC, pDH;
	int					width, height;
	HRGN				rBase, rMark;
	HWND				hToolbar, *phMarks;
	BITMAP				bm;

	hToolbar = (HWND)GetPropW(hwnd, PH_COMMANDS);
	if(pH->hbSkin){
		rBase = RegionFromSkin(pH->hbCommand, pH->crMask);
		SetWindowRgn(hToolbar, rBase, TRUE);
	}
	if(!pH->vertTbr){
		height = pH->rcCommands.bottom - pH->rcCommands.top;
		width = (pH->rcCommands.right - pH->rcCommands.left) / CMD_TBR_BUTTONS;
	}
	else{
		height = (pH->rcCommands.bottom - pH->rcCommands.top) / CMD_TBR_BUTTONS;
		width = pH->rcCommands.right - pH->rcCommands.left;
	}
	if(IsBitOn(g_NoteSettings.reserved1, SB1_HIDETOOLBAR))
		MoveWindow(hToolbar, pH->rcCommands.left, pH->rcCommands.top, pH->rcCommands.right - pH->rcCommands.left, pH->rcCommands.bottom - pH->rcCommands.top, FALSE);
	else
		MoveWindow(hToolbar, pH->rcCommands.left, pH->rcCommands.top, pH->rcCommands.right - pH->rcCommands.left, pH->rcCommands.bottom - pH->rcCommands.top, TRUE);
	pC = (P_NCMD_BAR)GetWindowLongPtrW(hwnd, OFFSET_COMMANDS);
	for(int i = CMD_FONT; i < CMD_TBR_BUTTONS; i++){
		HWND hw = pC->hCmd;
		if(!pH->vertTbr)
			MoveWindow(hw, width * i, 0, width, height, TRUE);		
		else
			MoveWindow(hw, 0, height * i, width, height, TRUE);		
		pC++;
	}
	//set delete/hide toolbar
	hToolbar = (HWND)GetPropW(hwnd, PH_DELHIDE);
	if(pH->hbSkin){
		SetWindowRgn(hToolbar, RegionFromSkin(pH->hbDelHide, pH->crMask), TRUE);
	}
	height = pH->rcDH.bottom - pH->rcDH.top;
	width = (pH->rcDH.right - pH->rcDH.left) / 2;
	MoveWindow(hToolbar, pH->rcDH.left, pH->rcDH.top, pH->rcDH.right - pH->rcDH.left, height, TRUE);
	pDH = (P_NCMD_BAR)GetWindowLongPtrW(hwnd, OFFSET_DH);
	for(int i = 0; i < CMD_SYS_BUTTONS; i++){
		MoveWindow(pDH->hCmd, width * i, 0, width, height, TRUE);	
		pDH++;
	}
	//set note marks buttons
	phMarks = (HWND *)GetWindowLongPtrW(hwnd, OFFSET_SYS);
	if(phMarks && pH->hbSys){
		HDC		hdc, hdcDest, hdcSrc;
		HBITMAP	hbmp, hold, ho;
		GetObject(pH->hbSys, sizeof(bm), &bm);
		
		hdc = GetDC(hwnd);
		hdcDest = CreateCompatibleDC(hdc);
		hdcSrc = CreateCompatibleDC(hdc);
		ho = SelectObject(hdcSrc, pH->hbSys);
		for(int i = 0; i < pH->marks; i++){
			hbmp = CreateCompatibleBitmap(hdc, bm.bmWidth / pH->marks, bm.bmHeight);
			hold = SelectObject(hdcDest, hbmp);
			BitBlt(hdcDest, 0, 0, bm.bmWidth / pH->marks, bm.bmHeight, hdcSrc, i * (bm.bmWidth / pH->marks), 0, SRCCOPY);
			SelectObject(hdcDest, hold);
			rMark = RegionFromSkin(hbmp, pH->crMask);
			SetWindowPos(*phMarks, 0, 0, 0, bm.bmWidth / pH->marks, bm.bmHeight, SWP_NOMOVE | SWP_NOZORDER);
			SetWindowRgn(*phMarks++, rMark, TRUE);
			DeleteObject(hbmp);
		}
		SelectObject(hdcSrc, ho);
		DeleteDC(hdcDest);
		DeleteDC(hdcSrc);
		ReleaseDC(hwnd, hdc);
	}
}

static LRESULT CALLBACK Grip_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

	switch(msg){
	HANDLE_MSG (hwnd, WM_MOUSEMOVE, Grip_OnMouseMove);

	case WM_PAINT:
	{
		PAINTSTRUCT			ps;
		PMEMNOTE			pNote = MEMNOTE(GetParent(hwnd));

		BeginPaint(hwnd, &ps);
		if(DockType(pNote->pData->dockData) != DOCK_NONE && g_DockSettings.fCustColor)
			PaintGrip(hwnd, ps.hdc, g_DockSettings.crWindow);
		else
			PaintGrip(hwnd, ps.hdc, pNote->pAppearance->crWindow);
		EndPaint(hwnd, &ps);
		return FALSE;
	}
	default:
		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
}

static void Grip_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	PMEMNOTE			pNote = MEMNOTE(GetParent(hwnd));

	if(keyFlags == MK_LBUTTON && DockType(pNote->pData->dockData) == DOCK_NONE){
		ReleaseCapture();
		SendMessageW(GetParent(hwnd), WM_NCLBUTTONDOWN, HTBOTTOMRIGHT, 0);
		if(pNote->pFlags->fromDB)
			WriteNotePlacement(GetParent(hwnd), TRUE);
		else
			WriteNotePlacement(GetParent(hwnd), FALSE);
	}
}

static void ShowHideNoteToolbar(HWND hNote){
	P_NCMD_BAR			pC;

	pC = (P_NCMD_BAR)GetWindowLongPtrW(hNote, OFFSET_COMMANDS);
	if(pC){
		if(!IsBitOn(g_NoteSettings.reserved1, SB1_HIDETOOLBAR))
			ShowWindow((HWND)GetPropW(hNote, PH_COMMANDS), SW_SHOW);
	}
}

static LRESULT CALLBACK Edit_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PMEMNOTE			pNote;
	P_NCMD_BAR			pC;

	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_RBUTTONUP, Edit_OnRButtonUp);
		case WM_KEYDOWN:
			if((GetKeyState(VK_CONTROL) & 0x8000) == 0x8000){
				//HWND		hToolbar = (HWND)GetPropW(GetParent(hwnd), PH_COMMANDS);
				switch(wParam){
					//case VK_B:
						//PostMessageW(hToolbar, WM_COMMAND, MAKEWPARAM(CMD_BOLD + 5000, 0), 0);
						//return FALSE;
					//case VK_I:
						//PostMessageW(hToolbar, WM_COMMAND, MAKEWPARAM(CMD_ITALIC + 5000, 0), 0);
						//return FALSE;
					case VK_P:
						PostMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_PRINT_NOTE, 0), 0);
						return FALSE;
					case VK_S:
						PostMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_SAVE, 0), 0);
						return FALSE;
					case VK_F:
						PostMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_SEARCH_NOTE, 0), 0);
						return FALSE;
					case VK_H:
						PostMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_REPLACE_NOTE, 0), 0);
						return FALSE;
					default:
						return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
				}
			}
			else{
				if((GetKeyState(VK_SHIFT) & 0x8000) != 0x8000 && (GetKeyState(VK_MENU) & 0x8000) != 0x8000){
					if(wParam == VK_F3){
						PostMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(IDM_SEARCH_NOTE_NEXT, 0), 0);
						return FALSE;
					}
				}
			}
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
		case WM_KILLFOCUS:
			if((HWND)wParam != GetParent(hwnd)){
				if(g_NoteSettings.transAllow){
					//return transparency
					pNote = MEMNOTE(GetParent(hwnd));
					SetLayeredWindowAttributes(GetParent(hwnd), pNote->pRTHandles->crMask, g_NoteSettings.transValue, LWA_COLORKEY | LWA_ALPHA);
					pNote->pFlags->transparent = TRUE;
				}
				pC = (P_NCMD_BAR)GetWindowLongPtrW(GetParent(hwnd), OFFSET_COMMANDS);
				if(pC){
					if(!g_hComboDialog || (g_hComboDialog && (HWND)wParam != g_hComboDialog)){
						if(!IsBitOn(g_NoteSettings.reserved1, SB1_HIDETOOLBAR)){
							//hide toolbar and update toolbar rectangle on note
							RECT	rc;
							GetWindowRect((HWND)GetPropW(GetParent(hwnd), PH_COMMANDS), &rc);
							ShowWindow((HWND)GetPropW(GetParent(hwnd), PH_COMMANDS), SW_HIDE);
							MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&rc, 2);
							RedrawWindow(GetParent(hwnd), &rc, NULL, RDW_INVALIDATE);
						}
					}
				}
			}
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
		case WM_SETFOCUS:
			RemoveBell(GetParent(hwnd));
			pNote = MEMNOTE(GetParent(hwnd));
			if(pNote->pFlags->transparent){
				//draw window opaque 
				SetLayeredWindowAttributes(GetParent(hwnd), pNote->pRTHandles->crMask, 255, LWA_COLORKEY);
				pNote->pFlags->transparent = FALSE;
			}
			ShowHideNoteToolbar(GetParent(hwnd));
			// pC = (P_NCMD_BAR)GetWindowLongPtrW(GetParent(hwnd), OFFSET_COMMANDS);
			// if(pC){
				// if(!IsBitOn(g_NoteSettings.reserved1, SB_HIDETOOLBAR))
					// ShowWindow((HWND)GetPropW(GetParent(hwnd), PH_COMMANDS), SW_SHOW);
			// }
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
		case WM_VSCROLL:
		case WM_MOUSEWHEEL:
			pNote = MEMNOTE(GetParent(hwnd));
			if(pNote->pFlags->transparent){
				//draw window opaque 
				SetLayeredWindowAttributes(GetParent(hwnd), pNote->pRTHandles->crMask, 255, LWA_COLORKEY);
				pNote->pFlags->transparent = FALSE;
			}
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
		default:
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
	}
}

static void Edit_OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	HMENU			hMenu = g_hEditPopUp;
	CHARRANGE		chr;

	m_hTempMenu = hMenu;
	if(SendMessageW(hwnd, EM_CANUNDO, 0, 0))
		EnableMenuItem(hMenu, IDM_UNDO, MF_BYCOMMAND | MF_ENABLED);
	else
		EnableMenuItem(hMenu, IDM_UNDO, MF_BYCOMMAND | MF_GRAYED);

	if(SendMessageW(hwnd, EM_CANREDO, 0, 0))
		EnableMenuItem(hMenu, IDM_REDO, MF_BYCOMMAND | MF_ENABLED);
	else
		EnableMenuItem(hMenu, IDM_REDO, MF_BYCOMMAND | MF_GRAYED);

	ZeroMemory(&chr, sizeof(chr));
	SendMessageW(hwnd, EM_EXGETSEL, 0, (LPARAM)&chr);
	if(chr.cpMin == chr.cpMax){
		EnableMenuItem(hMenu, IDM_CUT, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenu, IDM_COPY, MF_BYCOMMAND | MF_GRAYED);
	}
	else{
		EnableMenuItem(hMenu, IDM_CUT, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(hMenu, IDM_COPY, MF_BYCOMMAND | MF_ENABLED);
	}

	if(SendMessageW(hwnd, EM_CANPASTE, CF_TEXT, 0) || SendMessageW(hwnd, EM_CANPASTE, CF_UNICODETEXT, 0) || SendMessageW(hwnd, EM_CANPASTE, CF_OEMTEXT, 0))
		EnableMenuItem(hMenu, IDM_PASTE, MF_BYCOMMAND | MF_ENABLED);
	else
		EnableMenuItem(hMenu, IDM_PASTE, MF_BYCOMMAND | MF_GRAYED);

	ShowPopUp(GetParent(hwnd), hMenu);
}

static void ApplyNewSkin(HWND hNote, P_NOTE_RTHANDLES pH){

	RECT				rc;
	BITMAP				bm;

	GetWindowRect(hNote, &rc);
	GetObjectW(pH->hbSkin, sizeof(bm), &bm);
	//resize controls on note window
	SendMessage(hNote, CHM_RESIZE, (WPARAM)&pH->rcEdit, 0);
	//resize note window
	MoveWindow(hNote, rc.left, rc.top, bm.bmWidth, bm.bmHeight, TRUE);
	//update tooltip
	SendMessageW(hNote, CHM_TT_UPDATE, 0, 0);
}

static BOOL ApplyNoteChanges(HWND hwnd){
	PMEMNOTE			pNote = (PMEMNOTE)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
	int					wChanged = 0;
	BOOL				fReload = FALSE;

	if((pNote->pFlags->maskChanged & F_RTFONT) == F_RTFONT)
		wChanged |= F_RTFONT;
	if((pNote->pFlags->maskChanged & F_SKIN) == F_SKIN)
		wChanged |= F_SKIN;
	if((pNote->pFlags->maskChanged & F_C_FONT) == F_C_FONT)
		wChanged |= F_C_FONT;
	if((pNote->pFlags->maskChanged & F_C_COLOR) == F_C_COLOR)
		wChanged |= F_C_COLOR;
	if((pNote->pFlags->maskChanged & F_B_COLOR) == F_B_COLOR)
		wChanged |= F_B_COLOR;
	if(ScheduleChanged(pNote))
		wChanged |= F_SCHEDULE;
	if(wChanged){
		//if appearance changed
		if((wChanged & F_SKIN) == F_SKIN 
		|| (wChanged & F_RTFONT) == F_RTFONT 
		|| (wChanged & F_C_FONT) == F_C_FONT
		|| (wChanged & F_C_COLOR) == F_C_COLOR
		|| (wChanged & F_B_COLOR) == F_B_COLOR){
			memcpy(pNote->pAppearance, &m_TNAppearance, sizeof(NOTE_APPEARANCE));
		}
		if((wChanged & F_SKIN) == F_SKIN){	//skin changed
			if(pNote->pRTHandles->crMask == m_TNHandles.crMask){
				if((pNote->pAppearance->nPrivate & F_SKIN) == F_SKIN)			
					//if previous skin was from separate bitmap
					GetSkinProperties(hwnd, pNote->pRTHandles, pNote->pAppearance->szSkin, TRUE);
				else
					GetSkinProperties(hwnd, pNote->pRTHandles, pNote->pAppearance->szSkin, FALSE);
				pNote->pAppearance->nPrivate |= F_SKIN;
				if(pNote->pData->visible){
					ApplyNewSkin(pNote->hwnd, pNote->pRTHandles);
				}
				if(pNote->pFlags->fromDB){
					WritePrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, pNote->pAppearance, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
				}
				else{
					pNote->pFlags->saved = FALSE;
				}
			}
			else{
				fReload = TRUE;
			}
		}
		
		if((wChanged & F_C_FONT) == F_C_FONT){
			if((pNote->pAppearance->nPrivate & F_C_FONT) == F_C_FONT)
				DeleteObject(pNote->pRTHandles->hFCaption);
			pNote->pRTHandles->hFCaption = CreateFontIndirectW(&pNote->pAppearance->lfCaption);
			pNote->pAppearance->nPrivate |= F_C_FONT;
			if(pNote->pFlags->fromDB){
				SaveNewCFont(pNote, &pNote->pAppearance->lfCaption);
			}
			else{
				pNote->pFlags->saved = FALSE;
			}
		}

		if((wChanged & F_C_COLOR) == F_C_COLOR){
			pNote->pAppearance->nPrivate |= F_C_COLOR;
			if(pNote->pFlags->fromDB){
				SaveNewCColor(pNote, pNote->pAppearance->crCaption);
			}
			else{
				pNote->pFlags->saved = FALSE;
			}
		}

		if((wChanged & F_B_COLOR) == F_B_COLOR){
			pNote->pAppearance->nPrivate |= F_B_COLOR;
			if(pNote->pFlags->fromDB){
				SaveNewBColor(pNote, pNote->pAppearance->crWindow);
			}
			else{
				pNote->pFlags->saved = FALSE;
			}
		}

		if((wChanged & F_SCHEDULE) == F_SCHEDULE){	//schedule changed
			if(pNote->pFlags->fromDB){
				//just save new schedule without prompting
				WritePrivateProfileStructW(pNote->pFlags->id, S_SCHEDULE, pNote->pSchedule, sizeof(SCHEDULE_TYPE), g_NotePaths.DataFile);
			}
			else{
				pNote->pFlags->maskChanged |= F_SCHEDULE;
				pNote->pFlags->saved = FALSE;
			}
		}

		//recreate toolbar for skinless window
		if((wChanged & F_C_FONT) == F_C_FONT || (wChanged & F_C_COLOR) == F_C_COLOR || (wChanged & F_B_COLOR) == F_B_COLOR) {
			if(pNote->pData->visible){
				CreateSkinlessToolbarBitmap(pNote->hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbCommand);
				CreateSkinlessSysBitmap(pNote->hwnd, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbDelHide);
			}
		}
		//set new richedit background color
		if((wChanged & F_B_COLOR) == F_B_COLOR){
			if(pNote->pData->visible){
				RichEdit_SetBkgndColor(GetPropW(pNote->hwnd, PH_EDIT), 0, pNote->pAppearance->crWindow);
			}
		}
		if(pNote->pData->visible){
			RedrawWindow(pNote->hwnd, NULL, NULL, RDW_INVALIDATE);
			ShowNoteMarks(pNote->hwnd);
		}
		if(g_hCPDialog){
			SendMessageW(g_hCPDialog, CHM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
		}
		if(g_hOverdueDialog){
			SendMessageW(g_hOverdueDialog, CHM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
		}
	}
	return fReload;
}

BOOL CALLBACK Adjust_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	NMHDR		* nm;

	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_CLOSE, Adjust_OnClose);
	HANDLE_MSG (hwnd, WM_INITDIALOG, Adjust_OnInitDialog);
	HANDLE_MSG (hwnd, WM_COMMAND, Adjust_OnCommand);
	case WM_NOTIFY:
		nm = (NMHDR *)lParam;
		if(nm->code == TCN_SELCHANGE){
			//show/hide controls on tabs selection
			switch(SendDlgItemMessageW(hwnd, IDC_TAB_ADJUST, TCM_GETCURSEL, 0, 0)){
				case 0:
					ShowWindow(m_Dlgs.hSchedule, SW_HIDE);
					ShowWindow(m_Dlgs.hAppearance, SW_SHOW);
					break;
				case 1:
					ShowWindow(m_Dlgs.hSchedule, SW_SHOW);
					ShowWindow(m_Dlgs.hAppearance, SW_HIDE);
					break;
			}
		}
		return TRUE;
	default: return FALSE;
	}
}

static void Adjust_OnClose(HWND hwnd)
{
	g_hAdjust = NULL;
	EndDialog(hwnd, 0);
}

static BOOL Adjust_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	P_ADJUST_STRUCT		pAS = (P_ADJUST_STRUCT)lParam;
	PMEMNOTE			pNote = pAS->pNote;
	RECT				rc;
	wchar_t				szCaption[256];

	//store handle of calling window
	SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)pNote);
	
	m_Dlgs.hAppearance = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_NOTE_APP), hwnd, NApp_DlgProc, 0); 
	m_Dlgs.hSchedule = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_NOTE_SCHEDULE), hwnd, NSched_DlgProc, 0); 

	//create schedule part dialogs
	m_Dns.hOnce = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DNS_ONCE), m_Dlgs.hSchedule, Dns_DlgProc, 0); 
	m_Dns.hEveryDay = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DNS_EVERYDAY), m_Dlgs.hSchedule, Dns_DlgProc, 0); 
	m_Dns.hRepeat = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DNS_REPEATEVERY), m_Dlgs.hSchedule, Dns_DlgProc, 0); 
	m_Dns.hWeekly = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DNS_WEEKLY), m_Dlgs.hSchedule, Dns_DlgProc, 0); 
	m_Dns.hYearly = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DNS_YEARLY),m_Dlgs.hSchedule, Dns_DlgProc, 0);
	m_Dns.hAfter = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DNS_AFTER), m_Dlgs.hSchedule, Dns_DlgProc, 0); 

	//store parameters
	memcpy(&m_TNAppearance, pNote->pAppearance, sizeof(NOTE_APPEARANCE));
	memcpy(&m_TNHandles, pNote->pRTHandles, sizeof(NOTE_RTHANDLES));
	m_DataRes1 = pNote->pData->res1;

	AddTabs(hwnd);
	GetClientRect(hwnd, &rc);
	SetSizes(hwnd, rc.right - rc.left, rc.bottom -rc.top);

	if(DockType(pNote->pData->dockData) != DOCK_NONE){
		ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_LST_SKIN), SW_HIDE);
		ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_ST_BMP), SW_HIDE);
		ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_CMD_SKNLESS_CFONT), SW_HIDE);
		ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_CMD_SKNLESS_BCOLOR), SW_HIDE);
		ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_ST_ADJUST_DISABLE), SW_SHOW);
	}
	else{
		if(m_TNHandles.hbSkin){
			ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_LST_SKIN), SW_SHOW);
			ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_CMD_SKNLESS_CFONT), SW_HIDE);
			ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_CMD_SKNLESS_BCOLOR), SW_HIDE);
		}
		else{
			ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_LST_SKIN), SW_HIDE);
			ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_CMD_SKNLESS_CFONT), SW_SHOW);
			ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_CMD_SKNLESS_BCOLOR), SW_SHOW);
		}
		ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_ST_BMP), SW_SHOW);
		ShowWindow(GetDlgItem(m_Dlgs.hAppearance, IDC_ST_ADJUST_DISABLE), SW_HIDE);
	}


	InitAppearancePart(m_Dlgs.hAppearance);
	InitSchedulePart(m_Dlgs.hSchedule);

	//enable "Standard view" button
	if((pNote->pAppearance->nPrivate & F_SKIN) == F_SKIN 
	|| (pNote->pAppearance->nPrivate & F_RTFONT) == F_RTFONT 
	|| (pNote->pAppearance->nPrivate & F_C_COLOR) == F_C_COLOR
	|| (pNote->pAppearance->nPrivate & F_C_FONT) == F_C_FONT
	|| (pNote->pAppearance->nPrivate & F_B_COLOR) == F_B_COLOR){
		if(DockType(pNote->pData->dockData) == DOCK_NONE){
			EnableWindow(GetDlgItem(hwnd, IDC_CMD_STD_VIEW), TRUE);
		}
		else{
			EnableWindow(GetDlgItem(hwnd, IDC_CMD_STD_VIEW), FALSE);
		}
	}
	else{
		EnableWindow(GetDlgItem(hwnd, IDC_CMD_STD_VIEW), FALSE);
	}

	GetWindowTextW(hwnd, szCaption, 128);
	
	wcscat(szCaption, L" - ");
	wcscat(szCaption, pNote->pData->szName);
	SetWindowTextW(hwnd, szCaption);

	if(pAS->type == 1){
		NMHDR		nm;

		nm.code = TCN_SELCHANGE;
		nm.idFrom = IDC_TAB_ADJUST;
		nm.hwndFrom = GetDlgItem(hwnd, IDC_TAB_ADJUST);
		SendDlgItemMessageW(hwnd, IDC_TAB_ADJUST, TCM_SETCURSEL, 1, 0);
		SendMessageW(hwnd, WM_NOTIFY, IDC_TAB_ADJUST, (LPARAM)&nm);
	}
	//prevent another window appearance
	g_hAdjust = hwnd;

	return FALSE;
}

static void Adjust_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	PMEMNOTE		pNote = (PMEMNOTE)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

	switch(id){
		case IDCANCEL:
			ClearOnAdjustClose(pNote);
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
		case IDOK:
			//store flags (because of not-track flag)
			if(pNote->pData->res1 != m_DataRes1){
				NOTE_DATA			data;
				GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
				data.res1 = m_DataRes1;
				WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
				pNote->pData->res1 = m_DataRes1;
			}
			
			m_Reload = ApplyNoteChanges(hwnd);
			if(m_Reload){
				//the note should be reloaded
				wchar_t			szID[28];
				wcscpy(szID, pNote->pFlags->id);
				SaveNote(pNote);
				if(pNote->pData->visible){
					SendMessageW(pNote->hwnd, WM_CLOSE, 0, 0);
					LoadNoteProperties(pNote, &g_RTHandles, szID, FALSE, NULL);
					CreateNote(pNote, g_hInstance, g_hMain, FALSE, NULL);
				}
				if(g_hCPDialog){
					SendMessageW(g_hCPDialog, CHM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
				}
				if(g_hOverdueDialog){
					SendMessageW(g_hOverdueDialog, CHM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
				}
			}
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
		case IDC_CMD_STD_VIEW:
			if(pNote->pData->visible){
				RestoreStandardView(pNote->hwnd);
			}
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;
	}
}

static void ClearOnAdjustClose(PMEMNOTE pNote){
	if(pNote->pRTHandles->hFCaption != m_TNHandles.hFCaption)
		DeleteObject(m_TNHandles.hFCaption);
	if(pNote->pRTHandles->hFont != m_TNHandles.hFont)
		DeleteObject(m_TNHandles.hFont);
	if(pNote->pRTHandles->hbSkin != m_TNHandles.hbSkin)
		DeleteObject(m_TNHandles.hbSkin);
	if(pNote->pRTHandles->hbPattern != m_TNHandles.hbPattern)
		DeleteObject(m_TNHandles.hbPattern);
	if(pNote->pRTHandles->hbDelHide != m_TNHandles.hbDelHide)
		DeleteObject(m_TNHandles.hbDelHide);
	if(pNote->pRTHandles->hbSys != m_TNHandles.hbSys)
		DeleteObject(m_TNHandles.hbSys);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: RestoreStandardView
 Created  : Sun Sep  9 12:36:00 2007
 Modified : Sun Sep  9 12:36:00 2007

 Synopsys : Restores standard view for note
 Input    : hNote - note handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void RestoreStandardView(HWND hNote){
	PMEMNOTE				pNote = MEMNOTE(hNote);
	P_NOTE_RTHANDLES		pMainHandles;
	RECT					rc;
	BITMAP					bm;
	HWND					hEdit;
	BOOL					fDeletePrevious = FALSE;

	hEdit = (HWND)GetPropW(hNote, PH_EDIT);
	//copy standard appearance
	if((pNote->pAppearance->nPrivate & F_SKIN) == F_SKIN)
		fDeletePrevious = TRUE;
	memcpy(pNote->pAppearance, &g_Appearance, sizeof(NOTE_APPEARANCE));
	//clear note's flags
	pNote->pFlags->maskChanged &= ~F_SKIN;
	pNote->pFlags->maskChanged &= ~F_RTFONT;
	pNote->pFlags->maskChanged &= ~F_C_FONT;
	pNote->pFlags->maskChanged &= ~F_C_COLOR;
	pNote->pFlags->maskChanged &= ~F_B_COLOR;
	//get standard run-time handles
	pMainHandles = (P_NOTE_RTHANDLES)SendMessageW(GetParent(hNote), CHM_GET_RT_HANDLES, 0, 0);
	//get standard font and skin
	pNote->pRTHandles->hFont = pMainHandles->hFont;
	pNote->pRTHandles->hbSkin = pMainHandles->hbSkin;
	//clear "saved" flag, if note's text has not been changed
	if((pNote->pFlags->maskChanged & F_TEXT) != F_TEXT)
		pNote->pFlags->saved = TRUE;
	if(!pNote->pRTHandles->hbSkin){
		//in case of skinless window - get caption font
		pNote->pRTHandles->hFCaption = pMainHandles->hFCaption;
		//prepare needed rectangles
		GetClientRect(hNote, &rc);
		DeleteObject(pNote->pRTHandles->hbCommand);
		CreateSkinlessToolbarBitmap(hNote, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbCommand);
		CreateSkinlessSysBitmap(hNote, pNote->pAppearance, pNote->pRTHandles, &pNote->pRTHandles->hbDelHide);
		MeasureSkinlessNoteRectangles(pNote->pRTHandles, rc.right - rc.left, rc.bottom - rc.top);
		RichEdit_SetBkgndColor(hEdit, FALSE, pNote->pAppearance->crWindow);
	}
	else{
		//get standard skin properties
		GetSkinProperties(hNote, pNote->pRTHandles, pNote->pAppearance->szSkin, fDeletePrevious);
		GetWindowRect(hNote, &rc);
		GetObjectW(pNote->pRTHandles->hbSkin, sizeof(bm), &bm);
		//resize controls on note window
		SendMessage(hNote, CHM_RESIZE, (WPARAM)&pNote->pRTHandles->rcEdit, 0);
		//resize note window
		MoveWindow(hNote, rc.left, rc.top, bm.bmWidth, bm.bmHeight, TRUE);
		//update tooltip
		SendMessageW(hNote, CHM_TT_UPDATE, 0, 0);
	}
	//clear private appearance
	pNote->pAppearance->nPrivate = 0;
	//remove appearance entry from data file
	WritePrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, NULL, 0, g_NotePaths.DataFile);
	SetREFontCommon(hEdit, &pNote->pAppearance->lfFont);
	SetREFontColorCommon(hEdit, pNote->pAppearance->crFont);

	//redraw note
	RedrawWindow(hNote, NULL, NULL, RDW_INVALIDATE);
	ShowNoteMarks(hNote);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ScheduleChanged
 Created  : Sun May 27 15:33:17 2007
 Modified : Sun May 27 15:33:17 2007

 Synopsys : Checks whether schedule of note has been changed
 Input    : hwnd - note handle
 Output   : TRUE if changed, otherwise - FALSE
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static BOOL ScheduleChanged(PMEMNOTE pNote){

	SYSTEMTIME			st, stSt;
	int					prevType, weekday;
	int					scType;
	BOOL				translated;
	int					temp;

	GetLocalTime(&st);
	scType = SendDlgItemMessageW(m_Dlgs.hSchedule, IDC_CBO_NOTE_SCHEDULE, CB_GETCURSEL, 0, 0);

	switch(scType){
		case SCH_NO:					//no schedule checked
			if(pNote->pSchedule->scType != 0){		//note was scheduled before
				ZeroMemory(pNote->pSchedule, sizeof(SCHEDULE_TYPE));//remove scheduling		
				if(pNote->pRTHandles->idTimer)			//stop timer
					KillTimer(NULL, pNote->pRTHandles->idTimer);
				return TRUE;
			}
			break;
		case SCH_ONCE:					//schedule once checked
			if(pNote->pSchedule->scType != SCH_ONCE){		//different schedule type before
				prevType = pNote->pSchedule->scType;
				pNote->pSchedule->scType = SCH_ONCE;
				//get selected time
				SendDlgItemMessageW(m_Dns.hOnce, IDC_DTP_ONCE, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				//copy time
				memcpy(&pNote->pSchedule->scDate, &st, sizeof(SYSTEMTIME));
				if(prevType == SCH_NO){		//no timer before - now set timer
					pNote->isAlarm = TRUE;
					pNote->pRTHandles->idTimer = SetTimer(NULL, 0, NOTE_TIMER_INTERVAL, NoteTimerProc);
				}
				return TRUE;
			}
			else{			//the previous scheduling was of the same type - must check times difference
				//get selected time
				SendDlgItemMessageW(m_Dns.hOnce, IDC_DTP_ONCE, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				if(pNote->pSchedule->scDate.wDay != st.wDay || pNote->pSchedule->scDate.wMonth != st.wMonth
					|| pNote->pSchedule->scDate.wYear != st.wYear || pNote->pSchedule->scDate.wHour != st.wHour 
					|| pNote->pSchedule->scDate.wMinute != st.wMinute){
					//copy time
					memcpy(&pNote->pSchedule->scDate, &st, sizeof(SYSTEMTIME));
					return TRUE;
				}
			}
			break;
		case SCH_ED:			//schedule every day checked
			if(pNote->pSchedule->scType != SCH_ED){		//different schedule type before
				prevType = pNote->pSchedule->scType;
				pNote->pSchedule->scType = SCH_ED;
				//get selected time
				SendDlgItemMessageW(m_Dns.hEveryDay, IDC_DTP_EV_DAY, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				//copy time
				memcpy(&pNote->pSchedule->scDate, &st, sizeof(SYSTEMTIME));
				if(prevType == SCH_NO){		//no timer before - now set timer
					pNote->isAlarm = TRUE;
					pNote->pRTHandles->idTimer = SetTimer(NULL, 0, NOTE_TIMER_INTERVAL, NoteTimerProc);
				}
				//need for further checking
				GetLocalTime(&pNote->pSchedule->scStart);
				return TRUE;
			}
			else{			//the previous scheduling was of the same type - must check times difference
				//get selected time
				SendDlgItemMessageW(m_Dns.hEveryDay, IDC_DTP_EV_DAY, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				if(pNote->pSchedule->scDate.wHour != st.wHour || pNote->pSchedule->scDate.wMinute != st.wMinute){
					//copy time
					memcpy(&pNote->pSchedule->scDate, &st, sizeof(SYSTEMTIME));
					//need for further checking
					GetLocalTime(&pNote->pSchedule->scStart);
					return TRUE;
				}
			}
			break;
		case SCH_REP:					//schedule repeat checked
			if(pNote->pSchedule->scType != SCH_REP){		//different schedule before
				prevType = pNote->pSchedule->scType;
				pNote->pSchedule->scType = SCH_REP;
				//get selected time
				GetPesterPart(&st);
				// SendDlgItemMessageW(hdlg, IDC_DT_REPEAT, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				//copy time
				memcpy(&pNote->pSchedule->scDate, &st, sizeof(SYSTEMTIME));
				pNote->pRTHandles->nRepeat = SetRepeatCount(&pNote->pSchedule->scDate);		//set repeat timer according to timer interval
				if(prevType == SCH_NO){		//no timer before - now set timer
					pNote->isAlarm = TRUE;
					pNote->pRTHandles->idTimer = SetTimer(NULL, 0, NOTE_TIMER_INTERVAL, NoteTimerProc);
				}
				//need for further checking
				GetLocalTime(&pNote->pSchedule->scLastRun);
				return TRUE;
			}
			else{			//the previous scheduling was of the same type - must check times difference
				//get selected time
				GetPesterPart(&st);
				// SendDlgItemMessageW(hdlg, IDC_DT_REPEAT, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				if(pNote->pSchedule->scDate.wHour != st.wHour || pNote->pSchedule->scDate.wMinute != st.wMinute || pNote->pSchedule->scDate.wSecond != st.wSecond){
					//copy time
					memcpy(&pNote->pSchedule->scDate, &st, sizeof(SYSTEMTIME));
					pNote->pRTHandles->nRepeat = SetRepeatCount(&pNote->pSchedule->scDate);		//set repeat timer according to timer interval
					//need for further checking
					GetLocalTime(&pNote->pSchedule->scLastRun);
					return TRUE;
				}
			}
			break;
		case SCH_PER_WD:		//weekly schedule checked
			//get weekday
			weekday = SelectedWeekdays();
			if(weekday == 0)
				return TRUE;
			// weekday = SendDlgItemMessageW(hdlg, IDC_CBO_EVERY_WD, CB_GETITEMDATA, SendDlgItemMessageW(hdlg, IDC_CBO_EVERY_WD, CB_GETCURSEL, 0, 0), 0);
			if(pNote->pSchedule->scType != SCH_PER_WD){		//different schedule type
				prevType = pNote->pSchedule->scType;
				pNote->pSchedule->scType = SCH_PER_WD;
				//get selected time
				SendDlgItemMessageW(m_Dns.hWeekly, IDC_DTP_TIME_EVERY_WD, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				//store weekday
				st.wDayOfWeek = weekday;
				//copy time
				memcpy(&pNote->pSchedule->scDate, &st, sizeof(SYSTEMTIME));
				if(prevType == SCH_NO){		//no timer before - now set timer
					pNote->isAlarm = TRUE;
					pNote->pRTHandles->idTimer = SetTimer(NULL, 0, NOTE_TIMER_INTERVAL, NoteTimerProc);
				}
				//need for further checking
				GetLocalTime(&pNote->pSchedule->scLastRun);
				pNote->pSchedule->scLastRun.wSecond = 0;
				pNote->pSchedule->scLastRun.wMilliseconds = 0;
				if(pNote->pSchedule->scLastRun.wMinute > 0){
					pNote->pSchedule->scLastRun.wMinute--;
				}
				else{
					if(pNote->pSchedule->scLastRun.wHour > 0)
						pNote->pSchedule->scLastRun.wHour--;
					else
						pNote->pSchedule->scLastRun.wHour = 23;
					pNote->pSchedule->scLastRun.wMinute = 59;
				}
				return TRUE;
			}
			else{
				//get selected time
				SendDlgItemMessageW(m_Dns.hWeekly, IDC_DTP_TIME_EVERY_WD, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				if(pNote->pSchedule->scDate.wDayOfWeek != weekday || pNote->pSchedule->scDate.wHour != st.wHour || pNote->pSchedule->scDate.wMinute != st.wMinute){
					st.wDayOfWeek = weekday;
					//copy time
					memcpy(&pNote->pSchedule->scDate, &st, sizeof(SYSTEMTIME));
					//need for further checking
					GetLocalTime(&pNote->pSchedule->scLastRun);
					pNote->pSchedule->scLastRun.wSecond = 0;
					pNote->pSchedule->scLastRun.wMilliseconds = 0;
					if(pNote->pSchedule->scLastRun.wMinute > 0){
						pNote->pSchedule->scLastRun.wMinute--;
					}
					else{
						if(pNote->pSchedule->scLastRun.wHour > 0)
							pNote->pSchedule->scLastRun.wHour--;
						else
							pNote->pSchedule->scLastRun.wHour = 23;
						pNote->pSchedule->scLastRun.wMinute = 59;
					}
					return TRUE;
				}
			}
			break;
		case SCH_PER_YR:		//yearly schedule checked
			temp = IsDlgButtonChecked(m_Dns.hYearly, IDC_RDO_LUNAR) ? SCH_YR_LUNAR : SCH_YR_SOLAR;
			if(pNote->pSchedule->scType != SCH_PER_YR){		//different schedule type
				prevType = pNote->pSchedule->scType;
				pNote->pSchedule->scType = SCH_PER_YR;
				//get selected time
				SendDlgItemMessageW(m_Dns.hYearly, IDC_DTP_YEAR, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				//copy time
				memcpy(&pNote->pSchedule->scStart, &st, sizeof(SYSTEMTIME));
				SetNextYear(&pNote->pSchedule->scStart, &pNote->pSchedule->scDate, temp);
				//get lunar or solar
				pNote->pSchedule->reserved = temp;
				if(prevType == SCH_NO){		//no timer before - now set timer
					pNote->isAlarm = TRUE;
					pNote->pRTHandles->idTimer = SetTimer(NULL, 0, NOTE_TIMER_INTERVAL, NoteTimerProc);
				}
				return TRUE;
			}
			else{
				SendDlgItemMessageW(m_Dns.hYearly, IDC_DTP_YEAR, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				if(pNote->pSchedule->scDate.wDay != st.wDay || pNote->pSchedule->scDate.wMonth != st.wMonth
					|| pNote->pSchedule->scDate.wYear != st.wYear || pNote->pSchedule->scDate.wHour != st.wHour 
					|| pNote->pSchedule->scDate.wMinute != st.wMinute
					|| pNote->pSchedule->reserved != temp){
					//copy time
					memcpy(&pNote->pSchedule->scStart, &st, sizeof(SYSTEMTIME));
					SetNextYear(&pNote->pSchedule->scStart, &pNote->pSchedule->scDate, temp);
					//set lunar or solar
					pNote->pSchedule->reserved = temp;
					return TRUE;
				}
			}
			break;
		case SCH_AFTER:				//'after' schedule checked
			if(pNote->pSchedule->scType != SCH_AFTER){	//different schedule type
				prevType = pNote->pSchedule->scType;
				pNote->pSchedule->scType = SCH_AFTER;
				//get selected time
				SendDlgItemMessageW(m_Dns.hAfter, IDC_DTP_AFTER, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				//copy start time
				memcpy(&pNote->pSchedule->scStart, &st, sizeof(SYSTEMTIME));				
				//store date parts - wDayOfWeek is used for weeks count
				pNote->pSchedule->scDate.wYear = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_YEARS, &translated, FALSE);
				pNote->pSchedule->scDate.wMonth = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_MONTHS, &translated, FALSE);
				pNote->pSchedule->scDate.wDayOfWeek = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_WEEKS, &translated, FALSE);
				pNote->pSchedule->scDate.wDay = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_DAYS, &translated, FALSE);
				pNote->pSchedule->scDate.wHour = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_HOURS, &translated, FALSE);
				pNote->pSchedule->scDate.wMinute = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_MINS, &translated, FALSE);
				pNote->pSchedule->scDate.wSecond = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_SECS, &translated, FALSE);
				if(prevType == SCH_NO){		//no timer before - now set timer
					pNote->isAlarm = TRUE;
					pNote->pRTHandles->idTimer = SetTimer(NULL, 0, NOTE_TIMER_INTERVAL, NoteTimerProc);
				}
				return TRUE;
			}
			else{
				//get selected time
				SendDlgItemMessageW(m_Dns.hAfter, IDC_DTP_AFTER, DTM_GETSYSTEMTIME, 0, (LPARAM)&st);
				//store date parts in temporary variable - wDayOfWeek is used for weeks count
				stSt.wYear = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_YEARS, &translated, FALSE);
				stSt.wMonth = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_MONTHS, &translated, FALSE);
				stSt.wDayOfWeek = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_WEEKS, &translated, FALSE);
				stSt.wDay = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_DAYS, &translated, FALSE);
				stSt.wHour = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_HOURS, &translated, FALSE);
				stSt.wMinute = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_MINS, &translated, FALSE);
				stSt.wSecond = GetDlgItemInt(m_Dns.hAfter, IDC_EDT_SECS, &translated, FALSE);
				if(pNote->pSchedule->scStart.wYear != st.wYear || pNote->pSchedule->scStart.wMonth != st.wMonth 
				|| pNote->pSchedule->scStart.wDayOfWeek != st.wDayOfWeek || pNote->pSchedule->scStart.wDay != st.wDay 
				|| pNote->pSchedule->scStart.wHour != st.wHour || pNote->pSchedule->scStart.wMinute != st.wMinute 
				|| pNote->pSchedule->scStart.wSecond != st.wSecond 
				|| pNote->pSchedule->scDate.wYear != stSt.wYear || pNote->pSchedule->scDate.wMonth != stSt.wMonth 
				|| pNote->pSchedule->scDate.wDayOfWeek != stSt.wDayOfWeek || pNote->pSchedule->scDate.wDay != stSt.wDay 
				|| pNote->pSchedule->scDate.wHour != stSt.wHour || pNote->pSchedule->scDate.wMinute != stSt.wMinute 
				|| pNote->pSchedule->scDate.wSecond != stSt.wSecond){
					//copy start time
					memcpy(&pNote->pSchedule->scStart, &st, sizeof(SYSTEMTIME));
					//copy dates parts
					memcpy(&pNote->pSchedule->scDate, &stSt, sizeof(SYSTEMTIME));
					return TRUE;
				}	
			}
			break;
	}
	return FALSE;
}

static void FillScheduleTypes(HWND hwnd, wchar_t * lpFile){
	wchar_t 		szBuffer[256], szId[12];
	
	_itow(IDC_OPT_NO_SCH, szId, 10);
	GetPrivateProfileStringW(S_OPTIONS, szId, L"No schedule", szBuffer, 256, lpFile);
	SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	_itow(IDC_OPT_ONCE, szId, 10);
	GetPrivateProfileStringW(S_OPTIONS, szId, L"Once at:", szBuffer, 256, lpFile);
	SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	_itow(IDC_OPT_EV_DAY, szId, 10);
	GetPrivateProfileStringW(S_OPTIONS, szId, L"Every day at:", szBuffer, 256, lpFile);
	SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	_itow(IDC_OPT_REPEAT, szId, 10);
	GetPrivateProfileStringW(S_OPTIONS, szId, L"Repeat every:", szBuffer, 256, lpFile);
	SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	_itow(IDC_OPT_EVERY_WD, szId, 10);
	GetPrivateProfileStringW(S_OPTIONS, szId, L"Weekly on:", szBuffer, 256, lpFile);
	SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	_itow(IDC_OPT_EVERY_YR,szId,10);
	GetPrivateProfileStringW(S_OPTIONS, szId, L"Yearly on:", szBuffer, 256, lpFile);
	SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
	_itow(IDC_OPT_AFTER, szId, 10);
	GetPrivateProfileStringW(S_OPTIONS, szId, L"After:", szBuffer, 256, lpFile);
	SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_ADDSTRING, 0, (LPARAM)szBuffer);
}

static void AddTabs(HWND hwnd){

	TCITEMW		tci;
	wchar_t 	szBuffer[256], szFile[MAX_PATH];

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);

	//set dialog and controls text
	GetPrivateProfileStringW(S_OPTIONS, __T("1003"), DS_CAP_SINGLE, szBuffer, 256, szFile);
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDOK, szFile, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, szFile, DS_CANCEL);
	SetDlgCtlText(hwnd, IDC_CMD_STD_VIEW, szFile, L"Standard view");
	//tabs
	ZeroMemory(&tci, sizeof(tci));
	tci.mask = TCIF_TEXT | TCIF_PARAM;
	tci.pszText = szBuffer;
	tci.cchTextMax = 256;
	GetPrivateProfileStringW(__T("tabs"), __T("0"), __T("Skin"), szBuffer, 256, szFile);
	SendDlgItemMessageW(hwnd, IDC_TAB_ADJUST, TCM_INSERTITEMW, 0, (LPARAM)&tci);
	GetPrivateProfileStringW(__T("tabs"), __T("2"), __T("Schedule"), szBuffer, 256, szFile);
	SendDlgItemMessageW(hwnd, IDC_TAB_ADJUST, TCM_INSERTITEMW, 1, (LPARAM)&tci);

	//appearance part
	SetDlgCtlText(m_Dlgs.hAppearance, IDC_ST_ADJUST_DISABLE, szFile, L"Settings are available for not docked notes only");

	//-----------	schedule part	-----------
	SetDlgCtlText(m_Dlgs.hSchedule, IDC_CHK_NO_TRACK, szFile, L"Do not track the note if it is overdue");
	//'repeat' dialog
	SetDlgCtlText(m_Dns.hRepeat, IDC_ST_DAYS, szFile, L"Days");
	SetDlgCtlText(m_Dns.hRepeat, IDC_ST_HOURS, szFile, L"Hours");
	SetDlgCtlText(m_Dns.hRepeat, IDC_ST_MINS, szFile, L"Minutes");
	SetDlgCtlText(m_Dns.hRepeat, IDC_ST_SECS, szFile, L"Seconds");
	//'after' dialog
	SetDlgCtlText(m_Dns.hAfter, IDC_ST_DAYS, szFile, L"Days");
	SetDlgCtlText(m_Dns.hAfter, IDC_ST_HOURS, szFile, L"Hours");
	SetDlgCtlText(m_Dns.hAfter, IDC_ST_MINS, szFile, L"Minutes");
	SetDlgCtlText(m_Dns.hAfter, IDC_ST_SECS, szFile, L"Seconds");
	SetDlgCtlText(m_Dns.hAfter, IDC_ST_YEARS, szFile, L"Years");
	SetDlgCtlText(m_Dns.hAfter, IDC_ST_MONTHS, szFile, L"Months");
	SetDlgCtlText(m_Dns.hAfter, IDC_ST_WEEKS, szFile, L"Weeks");
	SetDlgCtlText(m_Dns.hAfter, IDC_ST_STARTING, szFile, L"Starting from:");
	//'weekly' dialog
	SetDlgCtlText(m_Dns.hWeekly, IDC_ST_TIME_EVERY_WD, szFile, L"At:");
	//yearly's dialog
	CheckDlgButton(m_Dns.hYearly, IDC_RDO_LUNAR, BST_CHECKED);
	//format datetime pickers
	SendDlgItemMessageW(m_Dns.hOnce, IDC_DTP_ONCE, DTM_SETFORMATW, 0, (LPARAM)g_DTFormats.DateFormat);
	SendDlgItemMessageW(m_Dns.hEveryDay, IDC_DTP_EV_DAY, DTM_SETFORMATW, 0, (LPARAM)g_DTFormats.TimeFormat);
	SendDlgItemMessageW(m_Dns.hWeekly, IDC_DTP_TIME_EVERY_WD, DTM_SETFORMATW, 0, (LPARAM)g_DTFormats.TimeFormat);
	SendDlgItemMessageW(m_Dns.hYearly, IDC_DTP_YEAR, DTM_SETFORMATW, 0, (LPARAM)g_DTFormats.DateFormat);
	SendDlgItemMessageW(m_Dns.hAfter, IDC_DTP_AFTER, DTM_SETFORMATW, 0, (LPARAM)g_DTFormats.DateFormat);
	//prepare textboxes for 'repeat' dialog
	for(int i = IDC_EDT_DAYS; i <= IDC_EDT_SECS; i++)
		SendDlgItemMessageW(m_Dns.hRepeat, i, EM_LIMITTEXT, 2, 0);
	//prepare textboxes for 'after' dialog
	for(int i = IDC_EDT_DAYS; i <= IDC_EDT_SECS; i++)
		SendDlgItemMessageW(m_Dns.hAfter, i, EM_LIMITTEXT, 2, 0);
	for(int i = IDC_EDT_YEARS; i <= IDC_EDT_WEEKS; i++)
		SendDlgItemMessageW(m_Dns.hAfter, i, EM_LIMITTEXT, 2, 0);
	//prepare days of week
	FillDaysOfWeek(szFile);
	
	FillScheduleTypes(m_Dlgs.hSchedule, szFile);
}

static void FillDaysOfWeek(wchar_t * lpLang){
	wchar_t			szBuffer[128];

	GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK, szBuffer, 128);
	switch(_wtoi(szBuffer)){
		case 0:
			for(int i = IDC_CHK_WD_0, j = 0; i <= IDC_CHK_WD_6; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			break;
		case 1:
			for(int i = IDC_CHK_WD_0, j = 1; i <= IDC_CHK_WD_5; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			SetWeekdayText(IDC_CHK_WD_6, 0, lpLang);
			break;
		case 2:
			for(int i = IDC_CHK_WD_0, j = 2; i <= IDC_CHK_WD_4; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			for(int i = IDC_CHK_WD_5, j = 0; i <= IDC_CHK_WD_6; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			break;
		case 3:
			for(int i = IDC_CHK_WD_0, j = 3; i <= IDC_CHK_WD_3; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			for(int i = IDC_CHK_WD_4, j = 0; i <= IDC_CHK_WD_6; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			break;
		case 4:
			for(int i = IDC_CHK_WD_0, j = 4; i <= IDC_CHK_WD_2; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			for(int i = IDC_CHK_WD_3, j = 0; i <= IDC_CHK_WD_6; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			break;
		case 5:
			for(int i = IDC_CHK_WD_0, j = 5; i <= IDC_CHK_WD_1; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			for(int i = IDC_CHK_WD_2, j = 0; i <= IDC_CHK_WD_6; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			break;
		case 6:
			SetWeekdayText(IDC_CHK_WD_0, 6, lpLang);
			for(int i = IDC_CHK_WD_1, j = 0; i <= IDC_CHK_WD_6; i++, j++){
				SetWeekdayText(i, j, lpLang);
			}
			break;
	}
}

static short SelectedWeekdays(void){
	int			result = 0;

	if(IsDlgButtonChecked(m_Dns.hWeekly, IDC_CHK_WD_0))
		// return 1;
		BitOn(&result, 1);
	if(IsDlgButtonChecked(m_Dns.hWeekly, IDC_CHK_WD_1))
		// return 2;
		BitOn(&result, 2);
	if(IsDlgButtonChecked(m_Dns.hWeekly, IDC_CHK_WD_2))
		// return 3;
		BitOn(&result, 3);
	if(IsDlgButtonChecked(m_Dns.hWeekly, IDC_CHK_WD_3))
		// return 4;
		BitOn(&result, 4);
	if(IsDlgButtonChecked(m_Dns.hWeekly, IDC_CHK_WD_4))
		// return 5;
		BitOn(&result, 5);
	if(IsDlgButtonChecked(m_Dns.hWeekly, IDC_CHK_WD_5))
		// return 6;
		BitOn(&result, 6);
	if(IsDlgButtonChecked(m_Dns.hWeekly, IDC_CHK_WD_6))
		// return 7;
		BitOn(&result, 7);
	return (short)result;
}

static void SetWeekdayText(int id, int day, const wchar_t * lpFile){

	wchar_t 		szBuffer[24], szDefault[24], szId[12];

	_itow(day, szId, 10);
	GetDlgItemTextW(m_Dns.hWeekly, id, szDefault, 24);
	GetPrivateProfileStringW(S_WEEKDAYS, szId, szDefault, szBuffer, 24, lpFile);
	SetDlgItemTextW(m_Dns.hWeekly, id, szBuffer);
}

static void SetPesterPart(LPSYSTEMTIME lps){
	SetDlgItemInt(m_Dns.hRepeat, IDC_EDT_DAYS, lps->wDay, FALSE);
	SetDlgItemInt(m_Dns.hRepeat, IDC_EDT_HOURS, lps->wHour, FALSE);
	SetDlgItemInt(m_Dns.hRepeat, IDC_EDT_MINS, lps->wMinute, FALSE);
	SetDlgItemInt(m_Dns.hRepeat, IDC_EDT_SECS, lps->wSecond, FALSE);
}

static void GetPesterPart(LPSYSTEMTIME lps){
	lps->wDay = GetDlgItemInt(m_Dns.hRepeat, IDC_EDT_DAYS, NULL, FALSE);
	lps->wHour = GetDlgItemInt(m_Dns.hRepeat, IDC_EDT_HOURS, NULL, FALSE);
	lps->wMinute = GetDlgItemInt(m_Dns.hRepeat, IDC_EDT_MINS, NULL, FALSE);
	lps->wSecond = GetDlgItemInt(m_Dns.hRepeat, IDC_EDT_SECS, NULL, FALSE);
}

static void SetSizes(HWND hwnd, int cx, int cy){
	
	RECT		rc, rcd;

	//size and move controls on adjust dialog
	SetRect(&rc, 0, 0, cx, cy);
	MoveWindow(GetDlgItem(hwnd, IDC_TAB_ADJUST), 0, 0, cx, cy, TRUE);
	SendDlgItemMessageW(hwnd, IDC_TAB_ADJUST, TCM_ADJUSTRECT, FALSE, (LPARAM)&rc);
	MoveWindow(GetDlgItem(hwnd, IDC_ST_TAB), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
	GetWindowRect(GetDlgItem(hwnd, IDC_CMD_STD_VIEW), &rcd);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rcd, 2);
	MoveWindow(m_Dlgs.hSchedule, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top - (rcd.bottom - rcd.top + 2 * (rc.bottom - rcd.bottom)), TRUE);
	MoveWindow(m_Dlgs.hAppearance, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top - (rcd.bottom - rcd.top + 2 * (rc.bottom - rcd.bottom)), TRUE);
}

static void SetNoteCaption(HWND hwnd){
	P_NOTE_DATA			pD;
	wchar_t				szText[256];

	pD = NoteData(hwnd);
	wcscpy(szText, NOTE_CAPTION_FIRST_PART);
	wcscat(szText, pD->szName);
	wcscat(szText, L"]");
	SetWindowTextW(hwnd, szText);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: PaintGrip
 Created  : Sun Sep  9 13:51:36 2007
 Modified : Sun Sep  9 13:51:36 2007

 Synopsys : Paints "sizeable" grip
 Input    : hwnd - note's handle
            hdcPS - device context
            pA - note's appearance handles
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void PaintGrip(HWND hwnd, HDC hdcPS, COLORREF crWindow){
	RECT				rc;
	HPEN				hPen, hOldPen;
	POINT				pt;
	HBRUSH				hBrush;

	GetClientRect(hwnd, &rc);
	hBrush = CreateSolidBrush(crWindow);
	FillRect(hdcPS, &rc, hBrush);
	hPen = CreatePen(PS_SOLID, 2, GetSysColor(COLOR_BTNSHADOW));
	hOldPen = SelectObject(hdcPS, hPen);
	MoveToEx(hdcPS, rc.right - 5, rc.bottom + 1, &pt);
	LineTo(hdcPS, rc.right + 1, rc.bottom - 5);
	MoveToEx(hdcPS, rc.right - 10, rc.bottom + 1, &pt);
	LineTo(hdcPS, rc.right + 1, rc.bottom - 10);
	MoveToEx(hdcPS, rc.right - 15, rc.bottom + 1, &pt);
	LineTo(hdcPS, rc.right + 1, rc.bottom - 15);
	hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW));
	SelectObject(hdcPS, hPen);
	MoveToEx(hdcPS, rc.right - 6, rc.bottom + 1, &pt);
	LineTo(hdcPS, rc.right + 1, rc.bottom - 6);
	MoveToEx(hdcPS, rc.right - 11, rc.bottom + 1, &pt);
	LineTo(hdcPS, rc.right + 1, rc.bottom - 11);
	MoveToEx(hdcPS, rc.right - 16, rc.bottom + 1, &pt);
	LineTo(hdcPS, rc.right + 1, rc.bottom - 16);
	SelectObject(hdcPS, hOldPen);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: PaintSimpleWindow
 Created  : Sun Sep  9 13:44:08 2007
 Modified : Sun Sep  9 13:44:08 2007

 Synopsys : Paints skinless note
 Input    : hwnd - note's handle
            hdcPS - device context
            pH - note's run-time handles
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void PaintSimpleWindow(HWND hwnd, HDC hdcPS, P_NOTE_RTHANDLES pH){
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	RECT				rc, rc1, rc2;
	HPEN				hPen, hOldPen;
	HBRUSH				hBrush, hOldBrush, hTextBrush;
	HFONT				hFont, hOldFont;
	COLORREF			crWindow, crCaption;
	int					flag = 0;

	//get needed parameters
	if(DockType(pNote->pData->dockData) != DOCK_NONE && g_DockSettings.fCustColor)
		crWindow = g_DockSettings.crWindow;
	else
		crWindow = pNote->pAppearance->crWindow;
	if(DockType(pNote->pData->dockData) != DOCK_NONE && g_DockSettings.fCustCaption){
		crCaption = g_DockSettings.crCaption;
		hFont = g_DRTHandles.hFCaption;
	}
	else{
		crCaption = pNote->pAppearance->crCaption;
		hFont = pNote->pRTHandles->hFCaption;
	}
	//get note's dimensions
	GetClientRect(hwnd, &rc);
	//create solid brush with needed color
	hBrush = CreateSolidBrush(crWindow);
	//fill note's surface with brush
	FillRect(hdcPS, &rc, hBrush);
	//draw note's caption
	FillSkinlessNoteCaption(hdcPS, &pNote->pRTHandles->rcCaption, crWindow, &pNote->pRTHandles->rcGrip);
	//save original brush
	hOldBrush = SelectObject(hdcPS, hBrush);
	//draw caption text
	hTextBrush = CreateSolidBrush(crCaption);
	SetBkMode(hdcPS, TRANSPARENT);
	SetTextColor(hdcPS, crCaption);
	hOldFont = SelectObject(hdcPS, hFont);
	DrawTextW(hdcPS, pNote->pData->szName, -1, &pH->rcTextCaption, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
	//create solid pen with caption font color
	hPen = CreatePen(PS_SOLID, 1, crCaption);
	hOldPen = SelectObject(hdcPS, hPen);
	if(pNote->pSchedule->scType != 0){
		//draw "scheduled" sign
		CopyRect(&rc1, &pNote->pRTHandles->rcSchedule);
		Ellipse(hdcPS, rc1.left, rc1.top, rc1.right, rc1.bottom);
		MoveToEx(hdcPS, rc1.left + 5, rc1.top + 2, NULL);
		LineTo(hdcPS, rc1.left + 5, rc1.top + 6);
		LineTo(hdcPS, rc1.left + 7, rc1.top + 6);
		flag |= 1;
	}
	if(!pNote->pFlags->saved){
		//draw "changed" sign
		CopyRect(&rc1, &pNote->pRTHandles->rcNotSaved);
		if((flag & 1) == 0){
			OffsetRect(&rc1, -(SMALL_RECT + 2), 0);
		}
		CopyRect(&rc2, &rc1);

		rc1.left += 4;
		rc1.top += 1;
		FrameRect(hdcPS, &rc1, hTextBrush);
		MoveToEx(hdcPS, rc2.left, rc2.top, NULL);
		LineTo(hdcPS, rc2.left + 7, rc2.top + 7);
		flag |= 2;
	}
	if(IsBitOn(pNote->pData->res1, NB_PROTECTED)){
		//draw "protected" sign
		CopyRect(&rc1, &pNote->pRTHandles->rcProtected);
		if((flag & 1) == 1 && (flag & 2) == 2){
			//do nothing
		}
		else if((flag & 1) == 1 || (flag & 2) == 2){
			//shift rectangle left
			OffsetRect(&rc1, -(SMALL_RECT + 2), 0);
		}
		else{
			//shift rectangle left twice
			OffsetRect(&rc1, -(SMALL_RECT + 2) * 2, 0);
		}
		MoveToEx(hdcPS, rc1.left + 4, rc1.top, NULL);
		LineTo(hdcPS, rc1.left + 8, rc1.top);
		MoveToEx(hdcPS, rc1.left + 2, rc1.top + 2, NULL);
		LineTo(hdcPS, rc1.left + 5, rc1.top - 1);
		MoveToEx(hdcPS, rc1.left + 7, rc1.top, NULL);
		LineTo(hdcPS, rc1.left + 10, rc1.top + 3);
		MoveToEx(hdcPS, rc1.left + 2, rc1.top + 2, NULL);
		LineTo(hdcPS, rc1.left + 2, rc1.top + 6);
		MoveToEx(hdcPS, rc1.left + 9, rc1.top + 2, NULL);
		LineTo(hdcPS, rc1.left + 9, rc1.top + 6);
		MoveToEx(hdcPS, rc1.left + 5, rc1.top + 2, NULL);
		LineTo(hdcPS, rc1.left + 7, rc1.top + 2);
		MoveToEx(hdcPS, rc1.left + 4, rc1.top + 3, NULL);
		LineTo(hdcPS, rc1.left + 4, rc1.top + 6);
		MoveToEx(hdcPS, rc1.left + 7, rc1.top + 3, NULL);
		LineTo(hdcPS, rc1.left + 7, rc1.top + 6);
		MoveToEx(hdcPS, rc1.left + 1, rc1.top + 5, NULL);
		LineTo(hdcPS, rc1.left + 3, rc1.top + 5);
		MoveToEx(hdcPS, rc1.left + 5, rc1.top + 5, NULL);
		LineTo(hdcPS, rc1.left + 8, rc1.top + 5);
		MoveToEx(hdcPS, rc1.left + 9, rc1.top + 5, NULL);
		LineTo(hdcPS, rc1.left + 11, rc1.top + 5);
		MoveToEx(hdcPS, rc1.left + 0, rc1.top + 6, NULL);
		LineTo(hdcPS, rc1.left + 0, rc1.top + 11);
		MoveToEx(hdcPS, rc1.left + 11, rc1.top + 6, NULL);
		LineTo(hdcPS, rc1.left + 11, rc1.top + 11);
		MoveToEx(hdcPS, rc1.left + 1, rc1.top + 11, NULL);
		LineTo(hdcPS, rc1.left + 11, rc1.top + 11);
		MoveToEx(hdcPS, rc1.left + 4, rc1.top + 8, NULL);
		LineTo(hdcPS, rc1.left + 8, rc1.top + 8);
		MoveToEx(hdcPS, rc1.left + 5, rc1.top + 9, NULL);
		LineTo(hdcPS, rc1.left + 7, rc1.top + 9);
		//CopyRect(&rc2, &rc1);
		//rc1.top += 3;
		//FrameRect(hdcPS, &rc1, hTextBrush);
		//rc1.left += 3;
		//rc1.right -= 3;
		//rc1.top -= 3;
		//rc1.bottom -= 8;
		//FrameRect(hdcPS, &rc1, hTextBrush);
		//MoveToEx(hdcPS, rc2.left + 4, rc2.top + 5, NULL);
		//LineTo(hdcPS, rc2.left + 8, rc2.top + 9);
		//MoveToEx(hdcPS, rc2.left + 4, rc2.top + 8, NULL);
		//LineTo(hdcPS, rc2.left + 8, rc2.top + 4);
		flag |= 4;
	}
	if(IsBitOn(pNote->pData->res1, NB_HIGH_PRIORITY)){
		//draw priority sign
		CopyRect(&rc1, &pNote->pRTHandles->rcPriority);
		if((flag & 1) == 1 && (flag & 2) == 2 && (flag & 4) == 4){
			//do nothing - draw on its original position
		}
		else if(((flag & 1) == 1 && (flag & 2) == 2) 
		|| ((flag & 1) == 1 && (flag & 4) == 4) 
		|| ((flag & 2) == 2 && (flag & 4) == 4)){
			//offset one rectangle left
			OffsetRect(&rc1, -(SMALL_RECT + 2), 0);
		}
		else if((flag & 1) == 1 || (flag & 2) == 2 || (flag & 4) == 4){
			//offset two rectangles left
			OffsetRect(&rc1, -(SMALL_RECT + 2) * 2, 0);
		}
		else{
			//offset three rectangles left
			OffsetRect(&rc1, -(SMALL_RECT + 2) * 3, 0);
		}
		MoveToEx(hdcPS, rc1.left + 1, rc1.top + 1, NULL);
		LineTo(hdcPS, rc1.left + 7, rc1.top + 12);
		MoveToEx(hdcPS, rc1.left + 4, rc1.top + 7, NULL);
		LineTo(hdcPS, rc1.left + 12, rc1.top + 3);
		MoveToEx(hdcPS, rc1.left + 1, rc1.top + 1, NULL);
		LineTo(hdcPS, rc1.left + 12, rc1.top + 2);
		flag |= 8;
	}
	if(IsBitOn(pNote->pData->res1, NB_COMPLETED)){
		//draw completed sign
		CopyRect(&rc1, &pNote->pRTHandles->rcCompleted);
		if((flag & 1) == 1 && (flag & 2) == 2 && (flag & 4) == 4 && (flag & 8) == 8){
			//do nothing - draw on its original position
		}
		else if(((flag & 1) == 1 && (flag & 2) == 2 && (flag & 4) == 4) 
		|| ((flag & 1) == 1 && (flag & 2) == 2 && (flag & 8) == 8) 
		|| ((flag & 1) == 1 && (flag & 4) == 4 && (flag & 8) == 8) 
		|| ((flag & 2) == 2 && (flag & 4) == 4 && (flag & 8) == 8)){
			//offset one rectangle left
			OffsetRect(&rc1, -(SMALL_RECT + 2), 0);
		}
		else if(((flag & 1) == 1 && (flag & 2) == 2) 
		|| ((flag & 1) == 1 && (flag & 4) == 4) 
		|| ((flag & 1) == 1 && (flag & 8) == 8) 
		|| ((flag & 2) == 2 && (flag & 4) == 4)
		|| ((flag & 2) == 2 && (flag & 8) == 8)
		|| ((flag & 4) == 4 && (flag & 8) == 8)){
			//offset two rectangles left
			OffsetRect(&rc1, -(SMALL_RECT + 2) * 2, 0);
		}
		else if((flag & 1) == 1 || (flag & 2) == 2 || (flag & 4) == 4 || (flag & 8) == 8){
			//offset three rectangles left
			OffsetRect(&rc1, -(SMALL_RECT + 2) * 3, 0);
		}
		else{
			//offset four rectangles left
			OffsetRect(&rc1, -(SMALL_RECT + 2) * 4, 0);
		}
		MoveToEx(hdcPS, rc1.left + 1, rc1.top + 6, NULL);
		LineTo(hdcPS, rc1.left + 7, rc1.top + 12);
		MoveToEx(hdcPS, rc1.left + 6, rc1.top + 11, NULL);
		LineTo(hdcPS, rc1.left + 12, rc1.top);
	}
	//frame note
	FrameSkinlessNote(hdcPS, &rc, crWindow);
	//restore DC
	SelectObject(hdcPS, hOldBrush);
	SelectObject(hdcPS, hOldFont);
	SelectObject(hdcPS, hOldPen);
	DeleteObject(hPen);
	DeleteObject(hBrush);
	DeleteObject(hTextBrush);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: PaintSkinnableWindow
 Created  : Sun Sep  9 13:39:04 2007
 Modified : Sun Sep  9 13:39:04 2007

 Synopsys : Paints skinnable note
 Input    : hwnd - note handle
            hdcPS - device context
            pH - note's run-time handles
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void PaintSkinnableWindow(HWND hwnd, HDC hdcPS, P_NOTE_RTHANDLES pH){
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	HDC					hdc, hdcSave;
	HBITMAP				hOriginal, hBmSave, hBmSaveOld;
	RECT				rc;

	//get note's dimensions
	GetClientRect(hwnd, &rc);
	//save original bitmap
	hOriginal = pNote->pRTHandles->hbSkin;
	//prepare compatible DC
	hdc = CreateCompatibleDC(hdcPS);
	//set back mode to transparent
	SetBkMode(hdc, TRANSPARENT);
	//select original bitmap in temporary DC
	SelectObject(hdc, hOriginal);
	//create DC to store original bitmap
	hdcSave = CreateCompatibleDC(hdcPS);
	hBmSave = CreateCompatibleBitmap(hdcPS, rc.right - rc.left, rc.bottom - rc.top);
	hBmSaveOld = SelectObject(hdcSave, hBmSave);
	//Save the original bitmap, because it will be overwritten
	BitBlt(hdcSave, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdc, 0, 0, SRCCOPY);
	//copy prepared bitmap onto destination
	BitBlt(hdcPS, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdc, 0, 0, SRCCOPY);
	//Place the original bitmap back into the bitmap sent here
	BitBlt(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdcSave, 0, 0, SRCCOPY);
	//clean up
	DeleteObject(SelectObject(hdcSave, hBmSaveOld));
	DeleteDC(hdcSave);
	DeleteDC(hdc);
}

void ShowNoteMarks(HWND hwnd){
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	HWND				*phMarks;
	int					flag = 0;

	phMarks = (HWND *)GetWindowLongPtrW(hwnd, OFFSET_SYS);
	if(pNote->pRTHandles->hbSkin){
		if(pNote->pSchedule->scType != 0){
			flag |= 1;
			SetWindowPos(phMarks[MARK_SCHEDULE], HWND_TOP, pNote->pRTHandles->rcSchedule.left, pNote->pRTHandles->rcSchedule.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
		}
		else{
			ShowWindow(phMarks[MARK_SCHEDULE], SW_HIDE);
		}
		if(!pNote->pFlags->saved){
			if((flag & 1) == 1){
				//schedule mark is visible - show changed mark at its original location
				SetWindowPos(phMarks[MARK_CHANGED], HWND_TOP, pNote->pRTHandles->rcNotSaved.left, pNote->pRTHandles->rcNotSaved.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
			}
			else{
				//schedule mark is not visible - show changed mark at schedule mark location
				SetWindowPos(phMarks[MARK_CHANGED], HWND_TOP, pNote->pRTHandles->rcSchedule.left, pNote->pRTHandles->rcSchedule.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
			}
			flag |= 2;
		}
		else{
			ShowWindow(phMarks[MARK_CHANGED], SW_HIDE);
		}
		if(pNote->pRTHandles->marks > 2){
			//new skins
			if(IsBitOn(pNote->pData->res1, NB_PROTECTED)){
				if((flag & 1) == 1){
					//schedule mark is visible
					if((flag & 2) == 2){
						//changed flag is visible - show protected mark at its original location
						SetWindowPos(phMarks[MARK_PROTECTED], HWND_TOP, pNote->pRTHandles->rcProtected.left, pNote->pRTHandles->rcProtected.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
					}
					else{
						//only schedule flag is visible - show protected mark at changed flag location
						SetWindowPos(phMarks[MARK_PROTECTED], HWND_TOP, pNote->pRTHandles->rcNotSaved.left, pNote->pRTHandles->rcNotSaved.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
					}
				}
				else{
					//schedule flag is not visible
					if((flag & 2) == 2){
						//changed flag is visible - show protected mark at changed flag location
						SetWindowPos(phMarks[MARK_PROTECTED], HWND_TOP, pNote->pRTHandles->rcNotSaved.left, pNote->pRTHandles->rcNotSaved.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
					}
					else{
						//only schedule flag is visible - show protected mark at schedule flag location
						SetWindowPos(phMarks[MARK_PROTECTED], HWND_TOP, pNote->pRTHandles->rcSchedule.left, pNote->pRTHandles->rcSchedule.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
					}
				}
				flag |= 4;
			}
			else{
				ShowWindow(phMarks[MARK_PROTECTED], SW_HIDE);
			}
			if(pNote->pRTHandles->marks > 3){
				//priority mark
				if(IsBitOn(pNote->pData->res1, NB_HIGH_PRIORITY)){
					if((flag & 1) == 1 && (flag & 2) == 2 && (flag & 4) == 4){
						//show mark on its original location
						SetWindowPos(phMarks[MARK_PRIORITY], HWND_TOP, pNote->pRTHandles->rcPriority.left, pNote->pRTHandles->rcPriority.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
					}
					else if(((flag & 1) == 1 && (flag & 2) == 2)
					|| ((flag & 1) == 1 && (flag & 4) == 4)
					|| ((flag & 2) == 2 && (flag & 4) == 4)){
						//shift mark one step left
						SetWindowPos(phMarks[MARK_PRIORITY], HWND_TOP, pNote->pRTHandles->rcProtected.left, pNote->pRTHandles->rcProtected.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
					}
					else if(flag == 1 || flag == 2 || flag == 4){
						//shift mark two steps left
						SetWindowPos(phMarks[MARK_PRIORITY], HWND_TOP, pNote->pRTHandles->rcNotSaved.left, pNote->pRTHandles->rcNotSaved.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
					}
					else{
						//shift mark three steps left
						SetWindowPos(phMarks[MARK_PRIORITY], HWND_TOP, pNote->pRTHandles->rcSchedule.left, pNote->pRTHandles->rcSchedule.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
					}
					flag |= 8;
				}
				else{
					ShowWindow(phMarks[MARK_PRIORITY], SW_HIDE);
				}
				//completed mark
				if(IsBitOn(pNote->pData->res1, NB_COMPLETED)){
					if((flag & 1) == 1 && (flag & 2) == 2 && (flag & 4) == 4 && (flag & 8) == 8){
						//show mark on its original location
						SetWindowPos(phMarks[MARK_COMPLETED], HWND_TOP, pNote->pRTHandles->rcCompleted.left, pNote->pRTHandles->rcCompleted.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
					}
					else if(((flag & 1) == 1 && (flag & 2) == 2 && (flag & 4) == 4)
					|| ((flag & 1) == 1 && (flag & 2) == 2 && (flag & 8) == 8)
					|| ((flag & 1) == 1 && (flag & 4) == 4 && (flag & 8) == 8)
					|| ((flag & 2) == 2 && (flag & 4) == 4 && (flag & 8) == 8)){
						//shift mark one step left
						SetWindowPos(phMarks[MARK_COMPLETED], HWND_TOP, pNote->pRTHandles->rcPriority.left, pNote->pRTHandles->rcPriority.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
					}
					else if(((flag & 1) == 1 && (flag & 2) == 2)
					|| ((flag & 1) == 1 && (flag & 4) == 4)
					|| ((flag & 2) == 2 && (flag & 4) == 4)
					|| ((flag & 1) == 1 && (flag & 8) == 8)
					|| ((flag & 2) == 2 && (flag & 8) == 8)
					|| ((flag & 4) == 4 && (flag & 8) == 8)){
						//shift mark two steps left
						SetWindowPos(phMarks[MARK_COMPLETED], HWND_TOP, pNote->pRTHandles->rcProtected.left, pNote->pRTHandles->rcProtected.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
					}
					else if(flag == 1 || flag == 2 || flag == 4 || flag == 8){
						//shift mark three steps left
						SetWindowPos(phMarks[MARK_COMPLETED], HWND_TOP, pNote->pRTHandles->rcNotSaved.left, pNote->pRTHandles->rcNotSaved.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
					}
					else{
						//shift mark four steps left
						SetWindowPos(phMarks[MARK_COMPLETED], HWND_TOP, pNote->pRTHandles->rcSchedule.left, pNote->pRTHandles->rcSchedule.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
					}
					flag |= 16;
				}
				else{
					ShowWindow(phMarks[MARK_COMPLETED], SW_HIDE);
				}
			}
		}
		else{
			ShowWindow(phMarks[MARK_PROTECTED], SW_HIDE);
			ShowWindow(phMarks[MARK_PRIORITY], SW_HIDE);
			ShowWindow(phMarks[MARK_COMPLETED], SW_HIDE);
		}
	}
	else{
		if(IsWindowVisible(phMarks[MARK_SCHEDULE]))
			ShowWindow(phMarks[MARK_SCHEDULE], SW_HIDE);
		if(IsWindowVisible(phMarks[MARK_CHANGED]))
			ShowWindow(phMarks[MARK_CHANGED], SW_HIDE);
		if(IsWindowVisible(phMarks[MARK_PROTECTED]))
			ShowWindow(phMarks[MARK_PROTECTED], SW_HIDE);
		if(IsWindowVisible(phMarks[MARK_PRIORITY]))
			ShowWindow(phMarks[MARK_PRIORITY], SW_HIDE);
		if(IsWindowVisible(phMarks[MARK_COMPLETED]))
			ShowWindow(phMarks[MARK_COMPLETED], SW_HIDE);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: MeasureSkinlessNoteRectangles
 Created  : Sun Sep  9 13:36:39 2007
 Modified : Sun Sep  9 13:36:39 2007

 Synopsys : Measures rectangles for skinless note
 Input    : pH - note's run-time handles
            cx - note's width
            cy - note's height
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void MeasureSkinlessNoteRectangles(P_NOTE_RTHANDLES pH, int cx, int cy){
	int					width, height, top, bottom;
	BITMAP				bm;

	//get width of grip rectangle
	width = GetSystemMetrics(SM_CXVSCROLL);
	//get caption height
	height = GetSystemMetrics(SM_CYSMCAPTION);
	//set all needed rectangles
	SetRect(&pH->rcGrip, cx - width, cy - width, cx, cy);
	if(pH->hbDelHide){
		SetRect(&pH->rcDH, cx - SMALL_RECT * 2 - 2, 2, cx - 2, SMALL_RECT + 2);
	}
	//cy - SMALL_RECT - 2
	top = cy - width + (width - SMALL_RECT) / 2;
	bottom = top + SMALL_RECT;
	SetRect(&pH->rcCaption, 0, 0, cx, height);
	SetRect(&pH->rcEdit, 1, pH->rcCaption.bottom + 1, cx - 2, pH->rcGrip.top - 1);
	SetRect(&pH->rcTextCaption, 2, 0, pH->rcDH.left, height);
	SetRect(&pH->rcSchedule, 2, top, SMALL_RECT + 2, bottom);
	SetRect(&pH->rcNotSaved, pH->rcSchedule.right + 2, top, pH->rcSchedule.right + SMALL_RECT + 2, bottom);
	SetRect(&pH->rcProtected, pH->rcNotSaved.right + 2, top, pH->rcNotSaved.right + SMALL_RECT + 2, bottom);
	SetRect(&pH->rcPriority, pH->rcProtected.right + 2, top, pH->rcProtected.right + SMALL_RECT + 2, bottom);
	SetRect(&pH->rcCompleted, pH->rcPriority.right + 2, top, pH->rcPriority.right + SMALL_RECT + 2, bottom);
	CopyRect(&pH->rcTooltip, &pH->rcTextCaption);
	if(pH->hbCommand){
		int diff;
		GetObject(pH->hbCommand, sizeof(bm), &bm);
		diff = ((pH->rcGrip.bottom - pH->rcGrip.top) - SMALL_RECT) / 2;
		SetRect(&pH->rcCommands, pH->rcProtected.right + 24 + 8, pH->rcGrip.top + diff, pH->rcProtected.right + 24 + 8 + bm.bmWidth, pH->rcGrip.top + diff + SMALL_RECT);
	}
}

static DWORD OutStreamCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb){
	WriteFile((HANDLE)dwCookie, pbBuff, cb, (PULONG)pcb, NULL);
	return 0;
}

static LRESULT CALLBACK Marks_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		HANDLE_MSG (hwnd, WM_PAINT, Marks_OnPaint);

		default:
			return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
}

static void Marks_OnPaint(HWND hwnd)
{
	PMEMNOTE			pNote = MEMNOTE(GetParent(hwnd));
	P_NOTE_RTHANDLES	pH;
	PAINTSTRUCT			ps;
	int					index;
	RECT				rc;

	if(pNote && pNote->pRTHandles){
		BeginPaint(hwnd, &ps);
		if(DockType(pNote->pData->dockData) == DOCK_NONE){
			pH = pNote->pRTHandles;
		}
		else{
			if(g_DockSettings.fCustSkin){
				pH = &g_DRTHandles;
			}
			else{
				if((pNote->pAppearance->nPrivate & F_SKIN) != F_SKIN){
					pH = pNote->pRTHandles;
				}
				else{
					pH = &g_DRTHandles;
				}
			}
		}
		GetWindowRect(hwnd, &rc);
		index = (int)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
		DrawTransparentBitmap(pH->hbSys, ps.hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, (rc.right - rc.left) * index, 0, CLR_MASK);
		EndPaint(hwnd, &ps);
	}
}

static LRESULT CALLBACK Tbr_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

	switch(msg){
	HANDLE_MSG (hwnd, WM_DRAWITEM, Tbr_OnDrawItem);
	HANDLE_MSG (hwnd, WM_MEASUREITEM, Tbr_OnMeasureItem);
	HANDLE_MSG (hwnd, WM_COMMAND, Tbr_OnCommand);
	HANDLE_MSG (hwnd, WM_INITMENUPOPUP, Tbr_OnInitMenuPopup);

	case CHM_NEW_FONT_FACE:
		SetREFontFace(GetParent(hwnd), &g_SelectedFont);
		SetFocus(GetParent(hwnd));
		return FALSE;
	default:
		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
}

static void Tbr_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu)
{
	PrepareMenuGradientColors(GetSysColor(COLOR_BTNFACE));
}

static void Tbr_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	RemoveBell(GetParent(hwnd));
	if(id > IDI_SIZE_START && id <= IDI_SIZE_START + MAX_FONT_SIZE){
		SetREFontSize(GetParent(hwnd), id - IDI_SIZE_START);
		RemoveSizeMenu();
		SetFocus(GetParent(hwnd));
		//if(id - IDI_SIZE_START != GetCFSize()){
			//SetREFontSize(GetParent(hwnd), id - IDI_SIZE_START);
			//RemoveSizeMenu();
			//SetFocus(GetParent(hwnd));
		//}
	}
	else if(id >= IDI_FORMAT_COLOR_START && id <= IDI_FORMAT_COLOR_END){
		m_CurrColorsChoice = CC_TEXT;
		ColorChanged(GetParent(hwnd), id);
	}
	else if(id >= IDI_FORMAT_HIGHLIGHT_START && id <= IDI_FORMAT_HIGHLIGHT_END){
		m_CurrColorsChoice = CC_HIGHLIGHT;
		ColorChanged(GetParent(hwnd), id);
	}
	else{
		switch(id){
		case CMD_BOLD + 5000:
			SetREFontDecoration(GetParent(hwnd), FD_BOLD);
			SetFocus(GetParent(hwnd));
			break;
		case CMD_ITALIC + 5000:
			SetREFontDecoration(GetParent(hwnd), FD_ITALIC);
			SetFocus(GetParent(hwnd));
			break;
		case CMD_UNDERLINE + 5000:
			SetREFontDecoration(GetParent(hwnd), FD_UNDERLINE);
			SetFocus(GetParent(hwnd));
			break;
		case CMD_STRIKETHROUGH + 5000:
			SetREFontDecoration(GetParent(hwnd), FD_STRIKETHROUGH);
			SetFocus(GetParent(hwnd));
			break;
		case CMD_A_LEFT + 5000:
			SetREAlignment(GetParent(hwnd), PFA_LEFT);
			SetFocus(GetParent(hwnd));
			break;
		case CMD_A_CENTER + 5000:
			SetREAlignment(GetParent(hwnd), PFA_CENTER);
			SetFocus(GetParent(hwnd));
			break;
		case CMD_A_RIGHT + 5000:
			SetREAlignment(GetParent(hwnd), PFA_RIGHT);
			SetFocus(GetParent(hwnd));
			break;
		case CMD_BULLETS + 5000:
			SetREBullets(GetParent(hwnd));
			SetFocus(GetParent(hwnd));
			break;
		}
	}
}

static void Tbr_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	DrawMItem(lpDrawItem, g_hBmpMenuNormal, g_hBmpMenuGray, CLR_MASK);
}

static void Tbr_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem)
{
	MeasureMItem(g_hMenuFont, lpMeasureItem);
}

static LRESULT CALLBACK Command_WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){

	switch(msg){
	HANDLE_MSG (hwnd, WM_PAINT, Command_OnPaint);
	HANDLE_MSG (hwnd, WM_LBUTTONUP, Command_OnLButtonUp);

	case WM_RBUTTONUP:
	case WM_MBUTTONUP:{
		RecreateCommandTooltip(hwnd);
		return FALSE;
	}
	default:
		return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
}

static void Command_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	POINT		pt;
	int			id;
	BOOL		delete = FALSE;

	GetCursorPos(&pt);
	id = GetDlgCtrlID(hwnd);
	switch(id){
	case CMD_FONT + 5000:{
		CBCREATION cbr;
		cbr.hParent = g_hMain;
		cbr.hButton = hwnd;
		cbr.hEdit = (HWND)GetPropW(cbr.hParent, PH_EDIT);
		SetCurrentFontFace((HWND)GetPropW(GetParent(GetParent(hwnd)), PH_EDIT));
		CreateComboDialog(&cbr, g_hInstance);
		break;
	}
	case CMD_COLOR + 5000:
	case CMD_HIGHLIGHT + 5000:{
		if(id == CMD_COLOR + 5000){
			m_CurrColorsChoice = CC_TEXT;
			TrackPopupMenu(m_hMenuTColor, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, GetParent(hwnd), NULL);
		}
		else{
			m_CurrColorsChoice = CC_HIGHLIGHT;
			TrackPopupMenu(m_hMenuTHighlight, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, GetParent(hwnd), NULL);
		}
		break;
	}
	case CMD_SIZE + 5000:{
		MENUITEMINFOW mi;
		int size = GetCurrentFontSize((HWND)GetPropW(GetParent(GetParent(hwnd)), PH_EDIT));
		SetCFSize(size);
		CreateSizeMenu(&g_hSizePopUp, TRUE);
		ZeroMemory(&mi, sizeof(mi));
		mi.cbSize = sizeof(mi);
		mi.fMask = MIIM_STATE;
		mi.fState = MFS_HILITE;
		SetMenuItemInfoW(g_hSizePopUp, IDI_SIZE_START + size, FALSE, &mi);
		TrackPopupMenu(g_hSizePopUp, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, GetParent(hwnd), NULL);
		break;
	}
	case CMD_BOLD + 5000:
	case CMD_ITALIC + 5000:
	case CMD_UNDERLINE + 5000:
	case CMD_STRIKETHROUGH + 5000:
	case CMD_A_LEFT + 5000:
	case CMD_A_CENTER + 5000:
	case CMD_A_RIGHT + 5000:
	case CMD_BULLETS + 5000:
		//forward processing to toolbar window
		SendMessageW(GetParent(hwnd), WM_COMMAND, MAKEWPARAM(id, 0), (LPARAM)hwnd);
		break;
	case IDM_DEL:
		delete = TRUE;
		SendMessageW(GetParent(GetParent(hwnd)), WM_COMMAND, MAKEWPARAM(IDM_DEL, 0), 0);
		break;
	case IDM_HIDE:
		SendMessageW(GetParent(GetParent(hwnd)), WM_COMMAND, MAKEWPARAM(IDM_HIDE, 0), 0);
		break;
	}
	if(!delete)
		RecreateCommandTooltip(hwnd);
}

static void Command_OnPaint(HWND hwnd)
{
	PMEMNOTE			pNote = MEMNOTE(GetParent(GetParent(hwnd)));
	PAINTSTRUCT			ps;
	RECT				rc;
	int					id;

	BeginPaint(hwnd, &ps);
	id = GetDlgCtrlID(hwnd);
	if(pNote->pRTHandles){
		//check for docked note
		if(DockType(pNote->pData->dockData) != DOCK_NONE){
			if(g_DockSettings.fCustSkin){
				if(g_RTHandles.hbSkin){
					memcpy(pNote->pRTHandles, &g_DRTHandles, sizeof(NOTE_RTHANDLES));
				}
			}
		}
		GetClientRect(hwnd, &rc);
		switch(id){
		case IDM_DEL:
		case IDM_HIDE:
			if(pNote->pRTHandles->hbDelHide){
				DrawTransparentBitmap(pNote->pRTHandles->hbDelHide, ps.hdc, 0, 0, (rc.right - rc.left), (rc.bottom - rc.top), (id - IDM_HIDE) * (rc.right - rc.left), 0, pNote->pRTHandles->crMask);
			}
			break;
		default:
			if(pNote->pRTHandles->hbCommand){
				if(!pNote->pRTHandles->vertTbr)
					DrawTransparentBitmap(pNote->pRTHandles->hbCommand, ps.hdc, 0, 0, (rc.right - rc.left), (rc.bottom - rc.top), (id - 5000) * (rc.right - rc.left), 0, pNote->pRTHandles->crMask);
				else
					DrawTransparentBitmap(pNote->pRTHandles->hbCommand, ps.hdc, 0, 0, (rc.right - rc.left), (rc.bottom - rc.top), 0, (id - 5000) * (rc.bottom - rc.top), pNote->pRTHandles->crMask);
			}
		}
	}
	EndPaint(hwnd, &ps);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SetCommandsTooltips
 Created  : Tue Jan  8 12:06:50 2008
 Modified : Tue Jan  8 12:06:50 2008

 Synopsys : Creates toolbar tooltips
 Input    : hwnd - note window handle
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void SetCommandsTooltips(HWND hwnd){

	wchar_t 		szBuffer[256], szFile[MAX_PATH], szKey[12];
	P_NCMD_BAR		pC, pDH;
	TOOLINFOW		ti;

	ZeroMemory(&ti, sizeof(ti));
	ti.cbSize = sizeof(ti);
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);
	pC = (P_NCMD_BAR)GetWindowLongPtrW(hwnd, OFFSET_COMMANDS);
	for(int i = CMD_FONT; i < CMD_TBR_BUTTONS; i++){
		_itow(5000 + i, szKey, 10);
		GetPrivateProfileStringW(S_COMMANDS, szKey, NULL, szBuffer, 256, szFile);
		ti.uId = (UINT)pC->hCmd;
		ti.hwnd = pC->hCmd;
		if(pC->hTtp != NULL){
			SendMessageW(pC->hTtp, TTM_DELTOOLW, 0, (LPARAM)&ti);
			DestroyWindow(pC->hTtp);
		}
		ti.lpszText = szBuffer;
		pC->hTtp = CreateWindowExW(WS_EX_TOPMOST, __T("Tooltips_class32"), NULL, TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, g_hInstance, NULL);
		SendMessageW(pC->hTtp, TTM_ADDTOOLW, 0, (LPARAM)&ti);
		pC++;
	}
	pDH = (P_NCMD_BAR)GetWindowLongPtrW(hwnd, OFFSET_DH);
	for(int i = 0; i < CMD_SYS_BUTTONS; i++){
		_itow(IDM_HIDE + i, szKey, 10);
		GetPrivateProfileStringW(S_MENU, szKey, NULL, szBuffer, 256, szFile);
		ti.uId = (UINT)pDH->hCmd;
		if(pDH->hTtp != NULL){
			SendMessageW(pDH->hTtp, TTM_DELTOOLW, 0, (LPARAM)&ti);
			DestroyWindow(pDH->hTtp);
		}
		ti.lpszText = szBuffer;
		pDH->hTtp = CreateWindowExW(WS_EX_TOPMOST, __T("Tooltips_class32"), NULL, TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, g_hInstance, NULL);
		SendMessageW(pDH->hTtp, TTM_ADDTOOLW, 0, (LPARAM)&ti);
		pDH++;
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: RecreateCommandTooltip
 Created  : Tue Jan  8 12:05:48 2008
 Modified : Tue Jan  8 12:05:48 2008

 Synopsys : Recreates toolbar tooltips
 Input    : hwnd - note window handle
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void RecreateCommandTooltip(HWND hwnd){

	wchar_t 		szBuffer[256], szFile[MAX_PATH], szKey[12], szSection[64];
	P_NCMD_BAR		pCmd;
	TOOLINFOW		ti;
	int				id;

	if(MEMNOTE(GetParent(GetParent(hwnd)))){
		ZeroMemory(&ti, sizeof(ti));
		ti.cbSize = sizeof(ti);
		ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;

		wcscpy(szFile, g_NotePaths.LangDir);
		wcscat(szFile, g_NotePaths.LangFile);
		id = GetDlgCtrlID(hwnd);
		switch(id){
		case IDM_HIDE:
		case IDM_DEL:
			pCmd = (P_NCMD_BAR)GetWindowLongPtrW(GetParent(GetParent(hwnd)), OFFSET_DH);
			pCmd += (id - IDM_HIDE);
			wcscpy(szSection, S_MENU);
			break;
		default:
			pCmd = (P_NCMD_BAR)GetWindowLongPtrW(GetParent(GetParent(hwnd)), OFFSET_COMMANDS);
			pCmd += (id - 5000);
			wcscpy(szSection, S_COMMANDS);
		}
		_itow(id, szKey, 10);
		GetPrivateProfileStringW(szSection, szKey, NULL, szBuffer, 256, szFile);
		ti.uId = (UINT)hwnd;
		ti.hwnd = hwnd;
		if(pCmd->hTtp != NULL){
			SendMessageW(pCmd->hTtp, TTM_DELTOOLW, 0, (LPARAM)&ti);
			DestroyWindow(pCmd->hTtp);
		}
		ti.lpszText = szBuffer;
		pCmd->hTtp = CreateWindowExW(WS_EX_TOPMOST, __T("Tooltips_class32"), NULL, TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, g_hInstance, NULL);
		SendMessageW(pCmd->hTtp, TTM_ADDTOOLW, 0, (LPARAM)&ti);
	}
}

void DeleteRTFile(wchar_t * id){
	wchar_t		szPath[MAX_PATH];

	if(NotesDirExists(g_NotePaths.DataDir)){
		wcscpy(szPath, g_NotePaths.DataDir);
		wcscat(szPath, id);
		wcscat(szPath, NOTE_EXTENTION);
		DeleteFileW(szPath);
		DeleteAutoSavedFile(id);
	}
}

static void DeleteAutoSavedFile(wchar_t * id){
	wchar_t		szPath[MAX_PATH];

	if(NotesDirExists(g_NotePaths.DataDir)){
		wcscpy(szPath, g_NotePaths.DataDir);
		wcscat(szPath, L"~");
		wcscat(szPath, id);
		wcscat(szPath, L".no~");
		DeleteFileW(szPath);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetTemporarySaveName
 Created  : Tue Jan  8 12:04:33 2008
 Modified : Tue Jan  8 12:04:33 2008

 Synopsys : Creates temporary name when note is saving for the first time 
            (the first nMax characters from note text)
 Input    : hNote - note window handle
            lpName - buffer for note name
            nMax - max count of characters to get from note text
 Output   : Temporary name length
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static int GetTemporarySaveName(HWND hNote, wchar_t * lpName, int nMax){

	HWND		hEdit;
	wchar_t		szBuffer[nMax];
	int			count = 0;

	hEdit = (HWND)GetPropW(hNote, PH_EDIT);
	count = GetWindowTextLengthW(hEdit);
	if(count > nMax)
		count = GetWindowTextW(hEdit, szBuffer, nMax);
	else
		count = GetWindowTextW(hEdit, szBuffer, count + 1);
	if(count > 0){
		wcscpy(lpName, szBuffer);
		while(*lpName++){
			if(*lpName == '\r' || *lpName == '\n'){
				*lpName = '\0';
				break;
			}
		}
	}
	return count;
}

static void SetCurrentFontFace(HWND hEdit){

	CHARFORMAT2W		chf;

	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	chf.dwMask = CFM_FACE;
	RichEdit_GetCharFormat(hEdit, SCF_SELECTION, &chf);
	wcscpy(g_CurrentFontFace, chf.szFaceName);

}

static int GetCurrentFontSize(HWND hEdit){

	CHARFORMAT2W		chf;
	int					size;
	HDC					hdc;

	ZeroMemory(&chf, sizeof(chf));
	chf.cbSize = sizeof(chf);
	chf.dwMask = CFM_SIZE;
	RichEdit_GetCharFormat(hEdit, SCF_SELECTION, &chf);
	hdc = GetDC(hEdit);
	size = -(chf.yHeight * GetDeviceCaps(hdc, LOGPIXELSY)) / (20 * 72);
	size = -MulDiv(size, 72, GetDeviceCaps(hdc, LOGPIXELSY));
	ReleaseDC(hEdit, hdc);
	return size;
}

static void InitAppearancePart(HWND hwnd){
	//fill skins
	if(m_TNHandles.hbSkin){
		FillSkins(hwnd, m_TNAppearance.szSkin, TRUE, FALSE);
	}
	else{
		FillSkins(hwnd, m_TNAppearance.szSkin, FALSE, TRUE);
		RedrawWindow(GetDlgItem(hwnd, IDC_ST_BMP), NULL, NULL, RDW_INVALIDATE);
	}
}

static void InitSchedulePart(HWND hwnd){

	PMEMNOTE			pNote = (PMEMNOTE)GetWindowLongPtrW(GetParent(hwnd), GWLP_USERDATA);
	P_SCHEDULE_TYPE		pS;

	//schedule part
	if(IsBitOn(pNote->pData->res1, NB_NOT_TRACK))
		CheckDlgButton(hwnd, IDC_CHK_NO_TRACK, BST_CHECKED);
	else
		CheckDlgButton(hwnd, IDC_CHK_NO_TRACK, BST_UNCHECKED);
	pS = pNote->pSchedule;
	SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_SETCURSEL, pS->scType, 0);
	SendMessageW(m_Dlgs.hSchedule, WM_COMMAND, MAKEWPARAM(IDC_CBO_NOTE_SCHEDULE, CBN_SELENDOK), (LPARAM)GetDlgItem(m_Dlgs.hSchedule, IDC_CBO_NOTE_SCHEDULE));
	if(pS->scType == SCH_ED){
		SendDlgItemMessageW(m_Dns.hEveryDay, IDC_DTP_EV_DAY, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&pS->scDate); 
	}
	else if(pS->scType == SCH_ONCE){
		SendDlgItemMessageW(m_Dns.hOnce, IDC_DTP_ONCE, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&pS->scDate); 
	}
	else if(pS->scType == SCH_REP){
		SetPesterPart(&pS->scDate);
	}
	else if(pS->scType == SCH_PER_WD){
		for(int i = 0; i <= 6; i++){
			if(IsBitOn(pS->scDate.wDayOfWeek, i + 1)){
				CheckDlgButton(m_Dns.hWeekly, IDC_CHK_WD_0 + i, BST_CHECKED);
			}
			else{
				CheckDlgButton(m_Dns.hWeekly, IDC_CHK_WD_0 + i, BST_UNCHECKED);
			}
		}
		SendDlgItemMessageW(m_Dns.hWeekly, IDC_DTP_TIME_EVERY_WD, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&pS->scDate); 
	}
	else if(pS->scType == SCH_PER_YR){
		SendDlgItemMessageW(m_Dns.hYearly, IDC_DTP_YEAR, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&pS->scStart); 		
		if(pS->reserved == SCH_YR_LUNAR){
			CheckDlgButton(m_Dns.hYearly, IDC_RDO_LUNAR, BST_CHECKED);
			CheckDlgButton(m_Dns.hYearly, IDC_RDO_SOLAR, BST_UNCHECKED);
		}
		else{
			CheckDlgButton(m_Dns.hYearly, IDC_RDO_SOLAR, BST_CHECKED);
			CheckDlgButton(m_Dns.hYearly, IDC_RDO_LUNAR, BST_UNCHECKED);
		}
	}
	else if(pS->scType == SCH_AFTER){
		SendDlgItemMessageW(m_Dns.hAfter, IDC_DTP_AFTER, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&pS->scStart); 
		SetDlgItemInt(m_Dns.hAfter, IDC_EDT_YEARS, pS->scDate.wYear, FALSE);
		SetDlgItemInt(m_Dns.hAfter, IDC_EDT_MONTHS, pS->scDate.wMonth, FALSE);
		SetDlgItemInt(m_Dns.hAfter, IDC_EDT_WEEKS, pS->scDate.wDayOfWeek, FALSE);
		SetDlgItemInt(m_Dns.hAfter, IDC_EDT_DAYS, pS->scDate.wDay, FALSE);
		SetDlgItemInt(m_Dns.hAfter, IDC_EDT_HOURS, pS->scDate.wHour, FALSE);
		SetDlgItemInt(m_Dns.hAfter, IDC_EDT_MINS, pS->scDate.wMinute, FALSE);
		SetDlgItemInt(m_Dns.hAfter, IDC_EDT_SECS, pS->scDate.wSecond, FALSE);
	}
	//'repeat' dialog
	SendDlgItemMessageW(m_Dns.hRepeat, IDC_UPD_DAYS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 31, (short) 0));
	SendDlgItemMessageW(m_Dns.hRepeat, IDC_UPD_HOURS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 24, (short) 0));
	SendDlgItemMessageW(m_Dns.hRepeat, IDC_UPD_MINS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 60, (short) 0));
	SendDlgItemMessageW(m_Dns.hRepeat, IDC_UPD_SECS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 60, (short) 0));
	//'after' dialog
	SendDlgItemMessageW(m_Dns.hAfter, IDC_UPD_YEARS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 99, (short) 0));
	SendDlgItemMessageW(m_Dns.hAfter, IDC_UPD_MONTHS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 12, (short) 0));
	SendDlgItemMessageW(m_Dns.hAfter, IDC_UPD_WEEKS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 48, (short) 0));
	SendDlgItemMessageW(m_Dns.hAfter, IDC_UPD_DAYS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 31, (short) 0));
	SendDlgItemMessageW(m_Dns.hAfter, IDC_UPD_HOURS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 24, (short) 0));
	SendDlgItemMessageW(m_Dns.hAfter, IDC_UPD_MINS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 60, (short) 0));
	SendDlgItemMessageW(m_Dns.hAfter, IDC_UPD_SECS, UDM_SETRANGE, 0, (LPARAM) MAKELONG((short) 60, (short) 0));
}

static BOOL CALLBACK NApp_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_DRAWITEM, NApp_OnDrawItem);
	HANDLE_MSG (hwnd, WM_COMMAND, NApp_OnCommand);
	HANDLE_MSG (hwnd, WM_INITDIALOG, NApp_OnInitDialog);

	// HANDLE_MSG (hwnd, WM_CTLCOLORSTATIC, NApp_OnCtlColorStatic);
	case CHM_DRAW_SN_PVW:
		DrawSimpleNotePreview((HWND)wParam, (DRAWITEMSTRUCT *)lParam, &m_rWindow, &m_rCaption, &m_TNAppearance, &m_TNHandles);
		return TRUE;
	default: return FALSE;
	}
}

static BOOL NApp_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	wchar_t		szFile[MAX_PATH];
	RECT		rc1, rc2;

	GetWindowRect(GetDlgItem(hwnd, IDC_ST_BMP), &rc2);
	MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc2, 2);
	if(!g_RTHandles.hbSkin){
		GetClientRect(hwnd, &rc1);
		ShowWindow(GetDlgItem(hwnd, IDC_LST_SKIN), SW_HIDE);
		wcscpy(szFile, g_NotePaths.LangDir);
		wcscat(szFile, g_NotePaths.LangFile);
		SetDlgCtlText(hwnd, IDC_CMD_SKNLESS_BCOLOR, szFile, __T("Background color"));
		SetDlgCtlText(hwnd, IDC_CMD_SKNLESS_CFONT, szFile, __T("Caption font"));
		ShowWindow(GetDlgItem(hwnd, IDC_CMD_SKNLESS_BCOLOR), SW_SHOW);
		ShowWindow(GetDlgItem(hwnd, IDC_CMD_SKNLESS_CFONT), SW_SHOW);
		MoveWindow(GetDlgItem(hwnd, IDC_ST_BMP), ((rc1.right - rc1.left) - (rc2.right - rc2.left)) / 2, rc2.top, rc2.right - rc2.left, rc2.bottom - rc2.top, TRUE);
	}
	else{
		GetWindowRect(GetDlgItem(hwnd, IDC_PLACEHOLDER), &rc1);
		MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc1, 2);
		MoveWindow(GetDlgItem(hwnd, IDC_ST_BMP), rc1.left, rc2.top, rc2.right - rc2.left, rc2.bottom - rc2.top, TRUE);
	}
	return TRUE;
}

static void NApp_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	PMEMNOTE	pNote = (PMEMNOTE)GetWindowLongPtrW(GetParent(hwnd), GWLP_USERDATA);

	switch(id){
		case IDC_CMD_SKNLESS_BCOLOR:
			if(codeNotify == BN_CLICKED){
				if(ChangeSkinlessColor(hwnd, &m_TNAppearance.crWindow, IDC_ST_BMP)){
					pNote->pFlags->maskChanged |= F_B_COLOR;
				}
			}
			break;
		case IDC_CMD_SKNLESS_CFONT:{
				int res = ChangeSkinlessCaptionFont(hwnd, &m_TNAppearance.lfCaption, &m_TNAppearance.crCaption, &m_TNHandles, IDC_ST_BMP);
				if((res & F_C_FONT) == F_C_FONT)
					pNote->pFlags->maskChanged |= F_C_FONT;
				if((res & F_C_COLOR) == F_C_COLOR)
					pNote->pFlags->maskChanged |= F_C_COLOR;
			}
			break;
		// case IDC_ST_BMP:
			// if(codeNotify == STN_DBLCLK){
				// if(!m_TNHandles.hbSkin){
					// GetCursorPos(&pt);
					// MapWindowPoints(HWND_DESKTOP, hwndCtl, &pt, 1);
					// if(PtInRect(&m_rWindow, pt)){
						// if(ChangeSkinlessColor(hwnd, &m_TNAppearance)){
							// pF->maskChanged |= F_B_COLOR;
						// }
					// }
					// else if(PtInRect(&m_rCaption, pt)){
						// int res = ChangeSkinlessCaptionFont(hwnd, &m_TNAppearance, &m_TNHandles);
						// if((res & F_C_FONT) == F_C_FONT)
							// pF->maskChanged |= F_C_FONT;
						// if((res & F_C_COLOR) == F_C_COLOR)
							// pF->maskChanged |= F_C_COLOR;
					// }
				// }
			// }
			// break;
		case IDC_LST_SKIN:	//show skin preview
			if(codeNotify == LBN_SELCHANGE){
				ChangeSkinPreview(hwnd, IDC_LST_SKIN, IDC_ST_BMP, &m_TNHandles, m_TNAppearance.szSkin, (pNote->pRTHandles->hbSkin == m_TNHandles.hbSkin) ? FALSE : TRUE);
				pNote->pFlags->maskChanged |= F_SKIN;
			}
			break;
	}
}

static void NApp_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if(lpDrawItem->CtlType == ODT_STATIC && lpDrawItem->CtlID == IDC_ST_BMP)
		//draw skin preview
		DrawSkinPreview(GetDlgItem(hwnd, IDC_ST_BMP), &m_TNHandles, &m_TNAppearance, m_TNHandles.crMask, lpDrawItem);
}

static BOOL CALLBACK NSched_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_COMMAND, NSched_OnCommand);
	HANDLE_MSG (hwnd, WM_DRAWITEM, NSched_OnDrawItem);

	default: return FALSE;
	}
}

static void NSched_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if(lpDrawItem->CtlType == ODT_COMBOBOX)
		DrawComboItem(lpDrawItem);
}

static void NSched_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDC_CBO_NOTE_SCHEDULE:
			if(codeNotify == CBN_SELENDOK){
				HWND		*pDlg = (HWND *)&m_Dns;
				int			count = SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_GETCOUNT, 0, 0);
				int			selected = SendDlgItemMessageW(hwnd, IDC_CBO_NOTE_SCHEDULE, CB_GETCURSEL, 0, 0);

				for(int i = 1; i < count; i++){
					if(i == selected)
						ShowWindow(*pDlg, SW_SHOW);
					else
						ShowWindow(*pDlg, SW_HIDE);
					pDlg++;
				}
			}
			break;
		case IDC_CHK_NO_TRACK:
			if(codeNotify == BN_CLICKED){
				if(IsDlgButtonChecked(hwnd, IDC_CHK_NO_TRACK) == BST_CHECKED)
					BitOn(&m_DataRes1, NB_NOT_TRACK);
				else
					BitOff(&m_DataRes1, NB_NOT_TRACK);
			}
			break;
	}
}

static void RemoveBell(HWND hwnd){
	HWND				hBell;

	if(g_NoteSettings.visualAlert){
		//destoy bell window, if any
		hBell = (HWND)GetPropW(hwnd, PH_BELL);
		if(hBell && IsWindow(hBell)){
			//destroy bell window only if it exists
			DestroyWindow(hBell);
		}
	}
}

static void SaveNewCFont(PMEMNOTE pNote, LPLOGFONTW lplf){
	NOTE_APPEARANCE		app;

	GetPrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, &app, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
	memcpy(&app.lfCaption, lplf, sizeof(LOGFONTW));
	app.fFontSet = TRUE;
	app.nPrivate |= F_C_FONT;
	WritePrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, &app, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
}

static void SaveNewCColor(PMEMNOTE pNote, COLORREF crCaption){
	NOTE_APPEARANCE	app;

	GetPrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, &app, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
	app.crCaption = crCaption;
	app.fFontSet = TRUE;
	app.nPrivate |= F_C_COLOR;
	WritePrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, &app, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
}

static void SaveNewBColor(PMEMNOTE pNote, COLORREF crWindow){
	NOTE_APPEARANCE	app;

	GetPrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, &app, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
	app.crWindow = crWindow;
	app.nPrivate |= F_B_COLOR;
	WritePrivateProfileStructW(pNote->pFlags->id, S_APPEARANCE, &app, sizeof(NOTE_APPEARANCE), g_NotePaths.DataFile);
}

void SaveVisibility(HWND hwnd, BOOL visible){
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	NOTE_DATA			data;

	if(pNote->pData){
		pNote->pData->visible = visible;
		//save only the visibility data
		if(pNote->pFlags->fromDB){
			//version 4.0
			if(GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
				data.visible = visible;
				WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
			}
			else{
				//version 3.5
				if(GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA) - sizeof(RECT) - sizeof(BOOL), g_NotePaths.DataFile)){
					data.visible = visible;
					WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
				}
			}
		}		
	}
}

static void WriteNotePlacement(HWND hwnd, BOOL fSave){
	PMEMNOTE			pNote = MEMNOTE(hwnd);
	RECT				rcNote;
	NOTE_DATA			data;
	NOTE_REL_POSITION	nrp;
	SIZE				sz;

	GetWindowRect(hwnd, &rcNote);
	if(pNote->pData){
		if(DockType(pNote->pData->dockData) == DOCK_NONE){
			//save current absolute position
			if(pNote->pRTHandles->rolled)
				rcNote.bottom = rcNote.top + (pNote->pRTHandles->rcSize.bottom - pNote->pRTHandles->rcSize.top);
			CopyRect(&pNote->pData->rcp, &rcNote);
			if(fSave){
				GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
				CopyRect(&data.rcp, &rcNote);
				//save rolled status
				if(pNote->pRTHandles->rolled != data.rolled){
					data.rolled = pNote->pRTHandles->rolled;
				}
				WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
				//save current relational position
				sz = GetScreenMetrics();
				nrp.left = (double)rcNote.left / (double)sz.cx;
				nrp.top = (double)rcNote.top / (double)sz.cy;
				nrp.width = rcNote.right - rcNote.left;
				nrp.height = rcNote.bottom - rcNote.top;
				WritePrivateProfileStructW(pNote->pFlags->id, IK_RELPOSITION, &nrp, sizeof(nrp), g_NotePaths.DataFile);
			}
		}
	}
}

static SIZE GetScreenMetrics(void){
	SIZE		sz;
	RECT		rcDesktop;

	if(GetSystemMetrics(SM_CMONITORS) == 1){
		GetClientRect(GetDesktopWindow(), &rcDesktop);
		sz.cx = rcDesktop.right - rcDesktop.left;
		sz.cy = rcDesktop.bottom - rcDesktop.top;
	}
	else{
		sz.cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		sz.cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	}
	return sz;
}

void AddToFavorites(PMEMNOTE pNote){
	wchar_t			szBuffer[256];

	GetPrivateProfileStringW(S_FAVORITES, pNote->pFlags->id, NULL, szBuffer, 256, g_NotePaths.INIFile);
	if(wcslen(szBuffer) == 0)
		WritePrivateProfileStringW(S_FAVORITES, pNote->pFlags->id, pNote->pData->szName, g_NotePaths.INIFile);
	else
		WritePrivateProfileStringW(S_FAVORITES, pNote->pFlags->id, NULL, g_NotePaths.INIFile);
}

PMItem NoteMenu(void){
	return m_NMenus;
}

PMItem EditMenu(void){
	return m_EMenus;
}

PMItem DropMenu(void){
	return m_DMenus;
}

int NoteMenuCount(void){
	return NELEMS(m_NMenus);
}

int EditMenuCount(void){
	return NELEMS(m_EMenus);
}

int DropMenuCount(void){
	return NELEMS(m_DMenus);
}

static BOOL CALLBACK Dns_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_COMMAND, Dns_OnCommand);
	HANDLE_MSG (hwnd, WM_INITDIALOG, Dns_OnInitDialog);

	default: return FALSE;
	}
}

static BOOL Dns_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	RECT		rc1, rc2, rc3, rc4;

	GetClientRect(GetParent(hwnd), &rc1);
	GetClientRect(hwnd, &rc2);
	GetWindowRect(GetDlgItem(GetParent(hwnd), IDC_CBO_NOTE_SCHEDULE), &rc3);
	MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&rc3, 2);
	GetWindowRect(GetDlgItem(GetParent(hwnd), IDC_CHK_NO_TRACK), &rc4);
	MapWindowPoints(HWND_DESKTOP, GetParent(hwnd), (LPPOINT)&rc4, 2);
	MoveWindow(hwnd, ((rc1.right - rc1.left) - (rc2.right - rc2.left)) / 2, rc3.bottom, rc2.right - rc2.left, rc2.bottom - rc2.top - (rc4.bottom - rc4.top) + 8, TRUE);
	return FALSE;
}

static void Dns_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
		case IDC_EDT_DAYS:
		case IDC_EDT_HOURS:
		case IDC_EDT_MINS:
		case IDC_EDT_SECS:
			if(hwnd == m_Dns.hRepeat){
				if(codeNotify == EN_UPDATE){
					BOOL	translated;
					//prevent empty control
					if(GetDlgItemInt(hwnd, id, &translated, FALSE) == 0 && translated == FALSE)
						SetDlgItemInt(hwnd, id, 0, FALSE);
				}
			}
			break;
	}
}

void DeleteNoteCompletely(PMEMNOTE pNote){
	WritePrivateProfileSectionW(pNote->pFlags->id, NULL, g_NotePaths.DataFile);
	DeleteRTFile(pNote->pFlags->id);
	if(pNote->pData->visible){
		if(DockType(pNote->pData->dockData) != DOCK_NONE){
			SendMessageW(pNote->hwnd, WM_COMMAND, MAKEWPARAM(IDM_DOCK_NONE, 0), 0);
		}
		SendMessageW(pNote->hwnd, WM_CLOSE, 0, 0);
	}
	DeleteMemNote(pNote);
}

void CenterRestorePlacementNote(PMEMNOTE pNote, int operation){
	if(pNote->pData->idGroup > GROUP_RECYCLE){
		if(pNote->pData->visible){
			if(DockType(pNote->pData->dockData) != DOCK_NONE){
				SendMessageW(pNote->hwnd, WM_COMMAND, MAKEWPARAM(IDM_DOCK_NONE, 0), 0);
			}
		}
		else{
			CreateNote(pNote, g_hInstance, g_hMain, FALSE, NULL);
			SaveVisibility(pNote->hwnd, TRUE);
		}
		if(operation == IDM_SHOW_CENTER)
			CenterWindow(pNote->hwnd, FALSE);
		else
			CenterWindow(pNote->hwnd, TRUE);
		ShowWindow(pNote->hwnd, SW_SHOW);
		SetWindowPos(pNote->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		if(!pNote->pData->onTop){
			SetWindowPos(pNote->hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		}
		if(g_hCPDialog){
			SendMessageW(g_hCPDialog, CHM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
		}
	}
}

static COLORREF MakeRandomColor(void){
	BYTE	r, g, b;

	rseed((unsigned)time(NULL));
	r = rnd(255);
	g = rnd(255);
	b = rnd(255);
	return RGB(r, g, b);
}

static int rnd(int base){
	__asm{
		mov eax, rnd_seed
		test eax, 80000000h
    	jz  _next
    	add eax, 7fffffffh
		_next:
		xor edx, edx
	    mov ecx, 127773
	    div ecx
	    mov ecx, eax
	    mov eax, 16807
	    mul edx
	    mov edx, ecx
	    mov ecx, eax
	    mov eax, 2836
	    mul edx
	    sub ecx, eax
	    xor edx, edx
	    mov eax, ecx
	    mov rnd_seed, ecx
	    div base

	    mov eax, edx
	}
}

static void rseed(int seed){
	rnd_seed = 12345678;

	rnd_seed = seed;
}
