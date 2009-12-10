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

#include <olectl.h>

#include "notes.h"
#include "splitcontainer.h"
#include "print.h"

/** Constants ********************************************************/
#define	PROP_GRP					L"_ngroup"
#define OCR_NORMAL  				32512
#define OCR_NO  					32648
static enum group_visibility {VISIBLE_ONLY = 2, HIDDEN_ONLY = 4, BOTH_VH = 6};
static enum columns {COL_NAME, COL_PRIORITY, COL_COMPLETED, COL_PROTECTED, COL_GROUP, COL_DATE, COL_SCHEDULE};
static enum selected_event {SEL_TREE_ROOT, SEL_TREE_ORD, SEL_TREE_REC, SEL_TREE_FAV, 
	SEL_LIST_SELECTED_ON_ROOT_SINGLE, SEL_LIST_SELECTED_ON_ROOT_MULTIPLE, SEL_LIST_DESELECTED_ON_ROOT, 
	SEL_LIST_ORD_SELECTED_SINGLE, SEL_LIST_ORD_SELECTED_MULTIPLE, SEL_LIST_ORD_DESELECTED, 
	SEL_LIST_REC_SELECTED_SINGLE, SEL_LIST_REC_SELECTED_MULTIPLE, SEL_LIST_REC_DESELECTED, 
	SEL_LIST_FAV_SELECTED_SINGLE, SEL_LIST_FAV_SELECTED_MULTIPLE, SEL_LIST_FAV_DESELECTED, 
	SEL_LIST_AFTER_COMMAND_SINGLE, SEL_LIST_AFTER_COMMAND_MULTIPLE};

// typedef struct _COLUMNS_ORDER{
	// int 	first;
	// int 	second;
	// int 	third;
	// int 	fourth;
// }COLUMNS_ORDER, *P_COLUMNS_ORDER;

typedef struct _CPENUM{
	int		group;
	int		result;
	int		reserved;
	int		total;
}CPENUM, *P_CPENUM;

typedef struct _DRAGSTRUCT{
	BOOL		fStarted;
	HWND		hCurrent;
	HIMAGELIST	hIml;
	BOOL		fDropAllowed;
	int			idGroup;
	HTREEITEM	target;
	HTREEITEM	source;
}DRAGSTRUCT, *P_DRAGSTRUCT;

/** List View columns sorting ********************************************************/
typedef struct _LVSORT_TYPE {
	int		iSortName;
	int		iSortPriority;
	int		iSortCompleted;
	int		iSortProtected;
	int		iSortDate;
	int		iSortSchedule;
	int		iSortGroup;
}LVSORT_TYPE;

typedef struct _LVSORT_MEMBER {
	int		column;
	int		order;
}LVSORT_MEMBER, *P_LVSORT_MEMBER;

/** Prototypes ********************************************************/
static BOOL CALLBACK Control_DlgProc (HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static void Control_OnClose(HWND hwnd);
static void Control_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL Control_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void Control_OnSize(HWND hwnd, UINT state, int cx, int cy);
static void Control_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void Control_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem);
static void Control_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
static void Control_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
static void Control_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu);
static void PrepareList(HWND hDlg, HWND hList);
static void UpdateListView(void);
static void UpdateControlStatus(void);
static void ApplyControlPanelLanguage(HWND hwnd);
static void ReplaceItem(HWND hList, PMEMNOTE pNote);
static void DeleteItem(HWND hList, LPARAM lParam);
static void InsertItem(HWND hList, PMEMNOTE pNote, int index);
static void EnableCommands(int flag);
static void SendNotesMessage(HWND hwnd, int id);
static void EnableMenus(UINT fEnabled, BOOL bMultiSelect);
static int CALLBACK DateCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static int CALLBACK NameCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static int CALLBACK PriorityCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static int CALLBACK CompletedCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static int CALLBACK ProtectedCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
static void ChangeListViewView(HWND hwnd, int view);
static void PrepareTree(HWND hwnd, HWND hTree);
static void EnableTreeMenus(HTREEITEM hi);
static void EnableTreeMenusForRoot(void);
static void DisableTreeToolbar(void);
static BOOL CALLBACK NewGroup_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void NewGroup_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL NewGroup_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void NewGroup_OnDestroy(HWND hwnd);
static BOOL CALLBACK Bitmaps_DlgProc (HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static void Bitmaps_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static BOOL Bitmaps_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static HTREEITEM InsertGroup(HTREEITEM hItem, P_PNGROUP ppg, BOOL fSetParent, BOOL fSave);
static void ShowAllGroups(int iParent, HTREEITEM hItem);
static BOOL CALLBACK TreeView_Proc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void TreeView_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
static void TreeLButtonDown(HWND hwnd, int x, int y, UINT keyFlags);
static BOOL CALLBACK Tree_Proc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void ClearDragStructure(void);
static BOOL IsTreeItemSuitable(void);
static void FindItemById(HTREEITEM hItem, int id);
static int GetSelectedTVItemId(void);
static void DeleteTreeGroup(HTREEITEM hItem, BOOL fGetSibling);
static int GetTVItemId(HTREEITEM hItem);
static void ShowTreePopUp(void);
static void ShowRecyclePopUp(void);
static HTREEITEM GetDropHighLightedItem(int x, int y);
static LRESULT CALLBACK List_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static BOOL IsItemFromChildBranch(HTREEITEM hDragged, HTREEITEM hHighlighted);
static HTREEITEM InsertTreeviewItems(void);
static void DefineFavoriteStatus(int index);
static BOOL DeleteItems(void);
static BOOL EmptyBin(void);
static BOOL RestoreFromBin(int group);
static BOOL RestoreAllFromBin(void);
static void InsertNotesIntoList(P_CPENUM pcp);
static void CheckGroupVisibility(P_CPENUM pcp);
static void HideGroup(HTREEITEM hItem);
static void DeleteNotesGroup(int group);
static void ShowGroup(HTREEITEM hItem);
static void RollUnrollItems(void);
static void HideItems(void);
static void ShowItems(void);
static void SendItemByEmail(void);
static void SendItemAsAttachment(void);
static void SaveItem(void);
static void SaveItemAs(void);
static void AddItemsToFavorites(void);
static void ToggleItemsPriority(void);
static void ToggleItemsProtection(void);
static void ToggleItemsCompleted(void);
static void RestoreCenterItem(int operation);
static void AdjustItem(int type);
static void PrintItem(void);
static PMEMNOTE SelectedNote(void);
static LONG_PTR * SelectedPItems(INT_PTR * pCount);
static void UpdateNoteItem(PMEMNOTE pNote);
static void SetItemGroupName(PMEMNOTE pNote, int index);
static void SetItemChangeDate(PMEMNOTE pNote, int index);
static void SetItemPriority(PMEMNOTE pNote, int index);
static void SetItemCompleted(PMEMNOTE pNote, int index);
static void SetItemProtected(PMEMNOTE pNote, int index);
static void SetItemScheduleDescription(PMEMNOTE pNote, int index);
static void ShowButtonDropDown(int buttonId, int menuId);
static void UpdateGroupText(int id, HTREEITEM hItem);
static void ShowHidePreview(void);
static BOOL CALLBACK DTree_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static BOOL DTree_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
static void DTree_OnSize(HWND hwnd, UINT state, int cx, int cy);
static void DTree_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
static void DTree_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
static void DTree_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem);
static void DTree_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu);
static void SetPreviewBackgroundColor(BOOL clear, COLORREF color);
static void ShowNotePreview(int selCount);
static BOOL ChoosePreviewColor(void);
static void ReselectAfterCommand(void);

/** Module variables ********************************************************/
static HIMAGELIST			m_hImlSmall, m_hImlLarge, m_hImlTbrNormal, m_hImlTbrGray, m_hImlTreeNormal, m_hImlTreeGray;
static HIMAGELIST			m_hImlDragSmall = 0, m_hImlDragLarge = 0;
static HWND					m_hSplitter, m_hHorSplitter, m_hTree, m_hList, m_hDTree, m_hCtrlPanel;
static HWND					m_hTbrCtrl, m_hTbrTree, m_hStbControl, m_hEdit;
static HMENU				m_hCtrlMenu = 0, m_hCtrlPUMenu, m_hTreeMenu = 0, m_hTreePUMenu, m_hRecycleMenu = 0, m_hRecyclePUMenu, m_hMenuCtrlTemp;
static LVSORT_TYPE			m_LVSort = {LVS_SORTASCENDING, LVS_SORTASCENDING};
static int					m_View = LVS_REPORT;
static DRAGSTRUCT			m_DragStruct;
static HCURSOR				m_CurNo, m_CurArrow;
static int					m_OffVert, m_OffHor, m_xMouse, m_yMouse, m_Up, m_Down;
static HTREEITEM			m_tItem = NULL, m_tSelected = NULL, m_tHighlighted = NULL;
// static COLUMNS_ORDER		m_cOrder = {0, 1, 2, 3};
static int					m_GroupWidth;
static LVSORT_MEMBER		m_lvsm = {0, LVS_SORTASCENDING};
static BOOL					m_TreeDragReady = FALSE;
static int					m_PvwColor;
static BOOL					m_PvwUseColor;

/** Toolbar buttons ********************************************************/
#if defined(_WIN64)
static TBBUTTON 			m_TBBtnArray[] = {{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,0,0,0,0},
							{0,IDM_NEW,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{71,IDM_LOAD_NOTE,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{76,IDM_NOTE_FROM_CLIPBOARD,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,0,0,0,0},
							{6,IDM_SAVE,0,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{10,IDM_SAVE_AS,0,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{80,IDM_PRINT_NOTE,0,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{7,IDM_ADJUST,0,TBSTYLE_DROPDOWN,0,0,0,0,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,0,0,0,0},
							{27,IDM_SAVE_ALL,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,0,0,0,0},
							{48,IDM_DOCK_ALL,TBSTATE_ENABLED,TBSTYLE_DROPDOWN,0,0,0,0,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,0,0,0,0},
							{79,IDM_VISIBILITY,0,TBSTYLE_DROPDOWN,0,0,0,0,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,0,0,0,0},
							{17,IDM_SHOW_CENTER,0,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,0,0,0,0},
							{42,IDM_RESTORE_PLACEMENT,0,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,0,0,0,0},
							{9,IDM_DEL,0,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{25,IDM_EMAIL,0,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{70,IDM_ATTACHMENT,0,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,0,0,0,0},
							{85,IDM_NOTE_MARKS,0,TBSTYLE_DROPDOWN,0,0,0,0,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,0,0,0,0},
							{68,IDM_ADD_TO_FAVORITES,0,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,0,0,0,0},
							{73,IDM_EMPTY_BIN,0,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{74,IDM_RESTORE_NOTE,0,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,0,0,0,0},
							{82,IDM_PREVIEW,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{87,IDM_PVW_COLOR_SET,TBSTATE_ENABLED,TBSTYLE_DROPDOWN,0,0,0,0,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,0,0,0,0},
							{78,IDM_V_VIEW,TBSTATE_ENABLED,TBSTYLE_DROPDOWN,0,0,0,0,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,0,0,0,0},
							{1,IDM_OPTIONS,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,0,0,0,0},
							{43,IDM_SEARCH_SUBMENU,TBSTATE_ENABLED,TBSTYLE_DROPDOWN,0,0,0,0,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,0,0,0,0},
							{5,IDM_HELP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0}
							};
static TBBUTTON				m_TreeButtons[] = {
							{28,IDM_NEW_GROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{31,IDM_NEW_SUBGROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{29,IDM_EDIT_GROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{30,IDM_DELETE_GROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0,0,0,0,0},
							{33,IDM_SHOW_GROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0},
							{32,IDM_HIDE_GROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0,0,0,0,0}
							};
#else
static TBBUTTON 			m_TBBtnArray[] = {{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{0,IDM_NEW,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{71,IDM_LOAD_NOTE,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{76,IDM_NOTE_FROM_CLIPBOARD,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{6,IDM_SAVE,0,TBSTYLE_BUTTON,0,0,0,0},
							{10,IDM_SAVE_AS,0,TBSTYLE_BUTTON,0,0,0,0},
							{80,IDM_PRINT_NOTE,0,TBSTYLE_BUTTON,0,0,0,0},
							{7,IDM_ADJUST,0,TBSTYLE_DROPDOWN,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{27,IDM_SAVE_ALL,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{48,IDM_DOCK_ALL,TBSTATE_ENABLED,TBSTYLE_DROPDOWN,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{79,IDM_VISIBILITY,0,TBSTYLE_DROPDOWN,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{17,IDM_SHOW_CENTER,0,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{42,IDM_RESTORE_PLACEMENT,0,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{9,IDM_DEL,0,TBSTYLE_BUTTON,0,0,0,0},
							{25,IDM_EMAIL,0,TBSTYLE_BUTTON,0,0,0,0},
							{70,IDM_ATTACHMENT,0,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{85,IDM_NOTE_MARKS,0,TBSTYLE_DROPDOWN,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{68,IDM_ADD_TO_FAVORITES,0,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{73,IDM_EMPTY_BIN,0,TBSTYLE_BUTTON,0,0,0,0},
							{74,IDM_RESTORE_NOTE,0,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{82,IDM_PREVIEW,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{87,IDM_PVW_COLOR_SET,TBSTATE_ENABLED,TBSTYLE_DROPDOWN,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{78,IDM_V_VIEW,TBSTATE_ENABLED,TBSTYLE_DROPDOWN,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{1,IDM_OPTIONS,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{43,IDM_SEARCH_SUBMENU,TBSTATE_ENABLED,TBSTYLE_DROPDOWN,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{5,IDM_HELP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0}
							};
static TBBUTTON				m_TreeButtons[] = {
							{28,IDM_NEW_GROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{31,IDM_NEW_SUBGROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{29,IDM_EDIT_GROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{30,IDM_DELETE_GROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,0,0},
							{33,IDM_SHOW_GROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0},
							{32,IDM_HIDE_GROUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,0,0}
							};
#endif

/** Menu items ********************************************************/
static MItem				m_CtrlMenus[] = {{IDM_NEW, 0, 0, -1, -1, MT_REGULARITEM, FALSE, L"New Note", L""}, 
							{IDM_LOAD_NOTE, 71, 0, -1, -1, MT_REGULARITEM, FALSE, L"Load Note", L""},
							{IDM_NOTE_FROM_CLIPBOARD, 76, 0, -1, -1, MT_REGULARITEM, FALSE, L"New Note From Clipboard", L""}, 
							{IDM_SAVE, 6, 0, -1, -1, MT_REGULARITEM, FALSE, L"Save", L""}, 
							{IDM_SAVE_AS, 10, 0, -1, -1, MT_REGULARITEM, FALSE, L"Save As...", L""}, 
							{IDM_PRINT_NOTE, 80, 0, -1, -1, MT_REGULARITEM, FALSE, L"Print", L""}, 
							{IDM_SAVE_ALL, 27, 0, -1, -1, MT_REGULARITEM, FALSE, L"Save All", L""}, 
							{IDM_DOCK_ALL, 48, 0, -1, -1, MT_REGULARITEM, FALSE, L"Docking (all notes)", L""}, 
							{IDM_DOCK_NONE_ALL, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"None", L""},
							{IDM_DOCK_LEFT_ALL, 49, 0, -1, -1, MT_REGULARITEM, FALSE, L"Left", L""},
							{IDM_DOCK_TOP_ALL, 50, 0, -1, -1, MT_REGULARITEM, FALSE, L"Top", L""},
							{IDM_DOCK_RIGHT_ALL, 51, 0, -1, -1, MT_REGULARITEM, FALSE, L"Right", L""},
							{IDM_DOCK_BOTTOM_ALL, 52, 0, -1, -1, MT_REGULARITEM, FALSE, L"Bottom", L""},
							{IDM_VISIBILITY, 79, 0, -1, -1, MT_REGULARITEM, FALSE, L"Visibility", L""}, 
							{IDM_SHOW, 8, 0, -1, -1, MT_REGULARITEM, FALSE, L"Show", L""}, 
							{IDM_SHOW_ALL, 3, 0, -1, -1, MT_REGULARITEM, FALSE, L"Show All", L""}, 
							{IDM_ADJUST, 7, 0, -1, -1, MT_REGULARITEM, FALSE, L"Adjust", L""}, 
							{IDM_ADJUST_APPEARANCE, 83, 0, -1, -1, MT_REGULARITEM, FALSE, L"Adjust Appearance", L""}, 
							{IDM_ADJUST_SCHEDULE, 84, 0, -1, -1, MT_REGULARITEM, FALSE, L"Adjust Schedule", L""}, 
							{IDM_HIDE, 16, 0, -1, -1, MT_REGULARITEM, FALSE, L"Hide", L""},
							{IDM_HIDE_ALL, 2, 0, -1, -1, MT_REGULARITEM, FALSE, L"Hide All", L""},
							{IDM_ALL_TO_FRONT, 77, 0, -1, -1, MT_REGULARITEM, FALSE, L"Bring All To Front", L""},
							{IDM_SHOW_CENTER, 17, 0, -1, -1, MT_REGULARITEM, FALSE, L"Centralize", L""},
							{IDM_RESTORE_PLACEMENT, 42, 0, -1, -1, MT_REGULARITEM, FALSE, L"Restore Default Placement", L""},
							{IDM_DEL, 9, 0, -1, -1, MT_REGULARITEM, FALSE, L"Delete", L""}, 
							{IDM_DOCK, 48, 0, -1, -1, MT_REGULARITEM, FALSE, L"Dock", L""},
							{IDM_DOCK_NONE, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"None", L""},
							{IDM_DOCK_LEFT, 44, 0, -1, -1, MT_REGULARITEM, FALSE, L"Left", L""},
							{IDM_DOCK_TOP, 45, 0, -1, -1, MT_REGULARITEM, FALSE, L"Top", L""},
							{IDM_DOCK_RIGHT, 46, 0, -1, -1, MT_REGULARITEM, FALSE, L"Right", L""},
							{IDM_DOCK_BOTTOM, 47, 0, -1, -1, MT_REGULARITEM, FALSE, L"Bottom", L""},
							{IDM_EMAIL, 25, 0, -1, -1, MT_REGULARITEM, FALSE, L"Send", L""},
							{IDM_ATTACHMENT, 70, 0, -1, -1, MT_REGULARITEM, FALSE, L"Send As Attachment", L""},
							{IDM_NOTE_MARKS, 85, 0, -1, -1, MT_REGULARITEM, FALSE, L"Switches", L""},
							{IDM_HIGH_PRIORITY, 75, 0, -1, -1, MT_REGULARITEM, FALSE, L"Toggle High Priority", L""},
							{IDM_TOGGLE_PROTECTION, 81, 0, -1, 0, MT_REGULARITEM, FALSE, L"Toggle Protection Mode", L""},
							{IDM_MARK_AS_COMPLETED, 86, 0, -1, -1, MT_REGULARITEM, FALSE, L"Mark As Completed", L""},
							{IDM_ROLL_UNROLL, 89, 0, -1, -1, MT_REGULARITEM, FALSE, L"Roll/Unroll", L""},
							{IDM_ADD_TO_FAVORITES, 68, 0, -1, -1, MT_REGULARITEM, FALSE, L"Add To Favorites", L""},
							{IDM_EMPTY_BIN, 73, 0, -1, -1, MT_REGULARITEM, FALSE, L"Empty Recycle Bin", L""},
							{IDM_RESTORE_NOTE, 74, 0, -1, -1, MT_REGULARITEM, FALSE, L"Restore Note", L""},
							{IDM_PREVIEW, -1, 0, 22, 0, MT_REGULARITEM, FALSE, L"Preview", L""}, 
							{IDM_PVW_COLOR_SET, 87, 0, -1, -1, MT_REGULARITEM, FALSE, L"Preview Window Background Settings", L""},
							{IDM_PVW_USE_COLOR, -1, 0, 22, 0, MT_REGULARITEM, FALSE, L"Use Specified Color", L""}, 
							{IDM_PVW_COLOR_VALUE, 88, 0, -1, -1, MT_REGULARITEM, FALSE, L"Choose Color", L""}, 
							{IDM_V_VIEW, 78, 0, -1, -1, MT_REGULARITEM, FALSE, L"Views", L""}, 
							{IDM_V_ICONS, 12, 0, 18, 0, MT_REGULARITEM, FALSE, L"Icons", L""}, 
							{IDM_V_SMALL, 13, 0, 19, 0, MT_REGULARITEM, FALSE, L"Small Icons", L""}, 
							{IDM_V_LIST, 14, 0, 20, 0, MT_REGULARITEM, FALSE, L"List", L""}, 
							{IDM_V_REPORT, 15, 0, 21, 0, MT_REGULARITEM, FALSE, L"Details", L""}, 
							{IDM_OPTIONS, 1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Options", L""},
							{IDM_SEARCH_SUBMENU, 43, 0, -1, -1, MT_REGULARITEM, FALSE, L"Search", L""},
							{IDM_SEARCH_IN_NOTES, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Search In Notes", L""},
							{IDM_SEARCH_TITLE, -1, 0, -1, -1, MT_REGULARITEM, FALSE, L"Search By Title", L""},
							{IDM_HELP, 5, 0, -1, -1, MT_REGULARITEM, FALSE, L"Help", L""}};
static MItem				m_TreeMenu[] = {{IDM_NEW_GROUP, 28, 0, -1, -1, MT_REGULARITEM, FALSE, L"New Group", L""}, 
							{IDM_NEW_SUBGROUP, 31, 0, -1, -1, MT_REGULARITEM, FALSE, L"New Subgroup", L""},
							{IDM_EDIT_GROUP, 29, 0, -1, -1, MT_REGULARITEM, FALSE, L"Modify Group", L""}, 
							{IDM_DELETE_GROUP, 30, 0, -1, -1, MT_REGULARITEM, FALSE, L"Delete Group", L""},
							// {IDM_NOTE_TO_GROUP, 34, 0, -1, -1, MT_REGULARITEM, FALSE, L"Add note to selected group"},
							{IDM_SHOW_GROUP, 33, 0, -1, -1, MT_REGULARITEM, FALSE, L"Show Group", L""}, 
							{IDM_HIDE_GROUP, 32, 0, -1, -1, MT_REGULARITEM, FALSE, L"Hide Group", L""}};
static MItem				m_RecycleMenu[] = {{IDM_EMPTY_BIN, 73, 0, -1, -1, MT_REGULARITEM, FALSE, L"Empty Recycle Bin", L""},
							{IDM_RESTORE_ALL_BIN, 74, 0, -1, -1, MT_REGULARITEM, FALSE, L"Restore All Notes", L""}};

void ShowControlPanel(void){
	//show dialog
	CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_CONTROL), NULL, Control_DlgProc, 0);
}

static BOOL CALLBACK Tree_Proc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg){
		HANDLE_MSG (hwnd, WM_MOUSEMOVE, TreeView_OnMouseMove);
		case WM_KEYDOWN:
			if(wParam == VK_DELETE){
				int 	tvid = GetTVItemId(m_tSelected);
				if(tvid > GROUP_ROOT && tvid != GROUP_RECYCLE){
					SendMessageW(m_hCtrlPanel, WM_COMMAND, MAKEWPARAM(IDM_DELETE_GROUP, 0), (LPARAM)m_hTbrTree);
				}	
				return FALSE;	
			}
			else{
				return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
			}
		case WM_LBUTTONDOWN:
			CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
			TreeLButtonDown(hwnd, LOWORD(lParam), HIWORD(lParam), wParam);
			return FALSE;
		default:
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, msg, wParam, lParam);
	}
}

static void TreeLButtonDown(HWND hwnd, int x, int y, UINT keyFlags)
{
	TVHITTESTINFO	tvh;

	if(!m_TreeDragReady && keyFlags == MK_LBUTTON){
		tvh.hItem = NULL;
		tvh.pt.x = x;
		tvh.pt.y = y;
		tvh.flags = TVHT_ONITEMICON | TVHT_ONITEMLABEL;
		TreeView_HitTest(m_hTree, &tvh);
		if(tvh.hItem && GetTVItemId(tvh.hItem) != -1){
			m_TreeDragReady = TRUE;
		}
	}
}

static void TreeView_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	TVHITTESTINFO	tvh;
	RECT			rc;

	if(m_TreeDragReady && keyFlags == MK_LBUTTON){
		tvh.hItem = NULL;
		tvh.pt.x = x;
		tvh.pt.y = y;
		tvh.flags = TVHT_ONITEMICON | TVHT_ONITEMLABEL;
		TreeView_HitTest(m_hTree, &tvh);
		if(tvh.hItem){
			ClearDragStructure();
			TreeView_GetItemRect(m_hTree, tvh.hItem, &rc, TRUE);
			m_xMouse = x - rc.left;
			m_yMouse = y - rc.top;
			m_DragStruct.source = tvh.hItem;
			m_DragStruct.fStarted = TRUE;
			m_DragStruct.hCurrent = m_hTree;
			m_DragStruct.idGroup = GetTVItemId(tvh.hItem);
			
			m_DragStruct.hIml = TreeView_CreateDragImage(m_hTree, tvh.hItem);
			ImageList_BeginDrag(m_DragStruct.hIml, 0, 0, 0);
			ImageList_DragEnter(m_hCtrlPanel, x, y);
			SetCapture(m_hCtrlPanel);
		}
	}
}

static void ClearDragStructure(void){
	ImageList_EndDrag();
	if(m_DragStruct.hIml)
		ImageList_Destroy(m_DragStruct.hIml);
	ZeroMemory(&m_DragStruct, sizeof(m_DragStruct));
}

static BOOL IsTreeItemSuitable(void){
	HTREEITEM		hItem;
	TVITEMW		tvi;

	hItem = TreeView_GetSelection(m_hTree);
	ZeroMemory(&tvi, sizeof(tvi));
	tvi.mask = TVIF_HANDLE | TVIF_PARAM;
	tvi.hItem = hItem;
	SendMessageW(m_hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
	if(tvi.lParam != -1)
		return TRUE;
	else
		return FALSE;
}

static LRESULT CALLBACK List_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	LPNMHEADER		lpnm;

	switch(uMsg){
		case WM_KEYDOWN:
			if(wParam == VK_DELETE){
				SendMessageW(m_hCtrlPanel, WM_COMMAND, MAKEWPARAM(IDM_DEL, 0), (LPARAM)m_hTbrCtrl);
				return FALSE;
			}
			else{
				return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
			}
		case WM_NOTIFY:
			lpnm = (LPNMHEADER)lParam;
			switch(lpnm->hdr.code){
				case HDN_DIVIDERDBLCLICK:
				case HDN_DIVIDERDBLCLICKW:
				case HDN_BEGINTRACK:
				case HDN_BEGINTRACKW:
					if(GetTVItemId(m_tSelected) == -1){
						if(lpnm->iItem == COL_PRIORITY || lpnm->iItem == COL_COMPLETED || lpnm->iItem == COL_PROTECTED)
							return TRUE;
						else
							return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
					}
					else{
						if(lpnm->iItem == COL_GROUP || lpnm->iItem == COL_PRIORITY || lpnm->iItem == COL_COMPLETED || lpnm->iItem == COL_PROTECTED)
							return TRUE;
						else
							return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
					}	
				default:
					return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
			}
		default:
			return CallWindowProcW((WNDPROC)GetWindowLongPtrW(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
	}
}

static void ReselectAfterCommand(void){
	int			count = ListView_GetSelectedCount(m_hList);

	if(count == 1){
		EnableCommands(SEL_LIST_AFTER_COMMAND_SINGLE);
	}
	else if(count > 1){
		EnableCommands(SEL_LIST_AFTER_COMMAND_MULTIPLE);
	}
}

static BOOL CALLBACK Control_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LVITEMW			lvi;
    LPNMHDR			lpnm;
	NMLISTVIEW		* nml;
	NMTREEVIEWW		* nmt;
	TOOLTIPTEXTW 	* ttp;
	wchar_t			szTooltip[128];
	int				tvid, group, count;
	PMEMNOTE		pNote;

	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_CLOSE, Control_OnClose);
	HANDLE_MSG (hwnd, WM_COMMAND, Control_OnCommand);
	HANDLE_MSG (hwnd, WM_INITDIALOG, Control_OnInitDialog);
	HANDLE_MSG (hwnd, WM_SIZE, Control_OnSize);
	HANDLE_MSG (hwnd, WM_DRAWITEM, Control_OnDrawItem);
	HANDLE_MSG (hwnd, WM_MEASUREITEM, Control_OnMeasureItem);
	HANDLE_MSG (hwnd, WM_MOUSEMOVE, Control_OnMouseMove);
	HANDLE_MSG (hwnd, WM_LBUTTONUP, Control_OnLButtonUp);
	HANDLE_MSG (hwnd, WM_INITMENUPOPUP, Control_OnInitMenuPopup);

	case CHM_CTRL_ITEM_UPDATE:
		UpdateNoteItem((PMEMNOTE)lParam);
		return TRUE;
	case CHM_UPDATE_CTRL_STATUS:
		UpdateControlStatus();
		return TRUE;
	case CHM_CTRL_INS:{
		//insert only if appropriate group is currently open
		int				id = GetSelectedTVItemId();

		pNote = (PMEMNOTE)lParam;
		
		if(pNote->pData->idGroup == id || id == -1){
			InsertItem(m_hList, pNote, SendMessageW(m_hList, LVM_GETITEMCOUNT, 0, 0));
		}
		return TRUE;
	}
	case CHM_CTRL_UPD_LANG:
		ApplyControlPanelLanguage(hwnd);
		return TRUE;
	case CHM_RELOAD:
		UpdateListView();
		return TRUE;
	case CHM_CTRL_GROUP_UPDATE:
		m_tItem = NULL;
		FindItemById(TreeView_GetRoot(m_hTree), wParam);
		UpdateGroupText(wParam, m_tItem);
		m_tItem = NULL;
		FindItemById(TreeView_GetRoot(m_hTree), lParam);
		UpdateGroupText(lParam, m_tItem);
		UpdateListView();
		return TRUE;
	case CHM_CTRL_UPD:
		ReplaceItem(m_hList, (PMEMNOTE)lParam);
		return TRUE;
	case CHM_CTRL_DEL:
		DeleteItem(m_hList, lParam);
		return TRUE;
	case WM_NOTIFY:
    	lpnm = (LPNMHDR)lParam;
		switch(lpnm->code){
			case TBN_DROPDOWN:{
				LPNMTOOLBARW	lpnmtbr = (LPNMTOOLBARW)lParam;
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(lpnmtbr->iItem, 0), 0);
				return TBDDRET_TREATPRESSED;
			}
			case TVN_SELCHANGEDW:
				SetPreviewBackgroundColor(TRUE, 0);
				SetWindowTextW(m_hEdit, NULL);
				nmt = (NMTREEVIEWW *)lParam;
				m_tSelected = nmt->itemNew.hItem;
				UpdateListView();
				tvid = GetTVItemId(m_tSelected);
				if(tvid > GROUP_ROOT){
					EnableCommands(SEL_TREE_ORD);
					EnableTreeMenus(m_tSelected);
				}
				else if(tvid == GROUP_ROOT){
					EnableCommands(SEL_TREE_ROOT);
					EnableTreeMenusForRoot();
				}
				else if(tvid == GROUP_RECYCLE){
					EnableCommands(SEL_TREE_REC);
					DisableTreeToolbar();
				}
				return TRUE;
			case LVN_COLUMNCLICK:
				m_TreeDragReady = FALSE;
				//sort list view by columns
				nml = (NMLISTVIEW *)lParam;
				switch(nml->iSubItem){
					case COL_NAME:
						m_lvsm.column = COL_NAME;
						if(m_LVSort.iSortName == LVS_SORTASCENDING)
							m_LVSort.iSortName = LVS_SORTDESCENDING;
						else
							m_LVSort.iSortName = LVS_SORTASCENDING;
						SendMessageW(m_hList, LVM_SORTITEMSEX, (WPARAM)m_LVSort.iSortName, (LPARAM)NameCompareFunc);
						m_lvsm.order = m_LVSort.iSortName;
						break;
					case COL_PRIORITY:
						m_lvsm.column = COL_PRIORITY;
						if(m_LVSort.iSortPriority == LVS_SORTASCENDING)
							m_LVSort.iSortPriority = LVS_SORTDESCENDING;
						else
							m_LVSort.iSortPriority = LVS_SORTASCENDING;
						SendMessageW(m_hList, LVM_SORTITEMS, (WPARAM)m_LVSort.iSortPriority, (LPARAM)PriorityCompareFunc);
						m_lvsm.order = m_LVSort.iSortPriority;
						break;
					case COL_COMPLETED:
						m_lvsm.column = COL_COMPLETED;
						if(m_LVSort.iSortCompleted == LVS_SORTASCENDING)
							m_LVSort.iSortCompleted = LVS_SORTDESCENDING;
						else
							m_LVSort.iSortCompleted = LVS_SORTASCENDING;
						SendMessageW(m_hList, LVM_SORTITEMS, (WPARAM)m_LVSort.iSortCompleted, (LPARAM)CompletedCompareFunc);
						m_lvsm.order = m_LVSort.iSortCompleted;
						break;
					case COL_PROTECTED:
						m_lvsm.column = COL_PROTECTED;
						if(m_LVSort.iSortProtected == LVS_SORTASCENDING)
							m_LVSort.iSortProtected = LVS_SORTDESCENDING;
						else
							m_LVSort.iSortProtected = LVS_SORTASCENDING;
						SendMessageW(m_hList, LVM_SORTITEMS, (WPARAM)m_LVSort.iSortProtected, (LPARAM)ProtectedCompareFunc);
						m_lvsm.order = m_LVSort.iSortProtected;
						break;
					case COL_GROUP:
						m_lvsm.column = COL_GROUP;
						if(m_LVSort.iSortGroup == LVS_SORTASCENDING)
							m_LVSort.iSortGroup = LVS_SORTDESCENDING;
						else
							m_LVSort.iSortGroup = LVS_SORTASCENDING;
						SendMessageW(m_hList, LVM_SORTITEMSEX, (WPARAM)m_LVSort.iSortGroup, (LPARAM)NameCompareFunc);
						m_lvsm.order = m_LVSort.iSortGroup;
						break;
					case COL_DATE:
						m_lvsm.column = COL_DATE;
						if(m_LVSort.iSortDate == LVS_SORTASCENDING)
							m_LVSort.iSortDate = LVS_SORTDESCENDING;
						else
							m_LVSort.iSortDate = LVS_SORTASCENDING;
						SendMessageW(m_hList, LVM_SORTITEMS, (WPARAM)m_LVSort.iSortDate, (LPARAM)DateCompareFunc);
						m_lvsm.order = m_LVSort.iSortDate;
						break;
					case COL_SCHEDULE:
						m_lvsm.column = COL_SCHEDULE;
						if(m_LVSort.iSortSchedule == LVS_SORTASCENDING)
							m_LVSort.iSortSchedule = LVS_SORTDESCENDING;
						else
							m_LVSort.iSortSchedule = LVS_SORTASCENDING;
						SendMessageW(m_hList, LVM_SORTITEMSEX, (WPARAM)m_LVSort.iSortSchedule, (LPARAM)NameCompareFunc);
						m_lvsm.order = m_LVSort.iSortSchedule;
						break;
				}
				break;
			case LVN_ITEMCHANGED:
				m_TreeDragReady = FALSE;
				//enable or disable toolbar buttons and menu items depending on list view items selection state
				nml = (NMLISTVIEW *)lParam;
				ZeroMemory(&lvi, sizeof(lvi));
				lvi.iItem = nml->iItem;
				lvi.mask = LVIF_PARAM;
				SendMessageW(m_hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
				pNote = (PMEMNOTE)lvi.lParam;
				group = pNote->pData->idGroup;
				count = ListView_GetSelectedCount(m_hList);
				tvid = GetTVItemId(m_tSelected);
				if(nml->uNewState == 3 || nml->uNewState == 2){
					ShowNotePreview(count);
					if(tvid == GROUP_ROOT){
						if(count == 1){
							DefineFavoriteStatus(nml->iItem);
							EnableCommands(SEL_LIST_SELECTED_ON_ROOT_SINGLE);
						}
						else{
							EnableCommands(SEL_LIST_SELECTED_ON_ROOT_MULTIPLE);
						}
						
					}
					else{
						if(count > 1){
							//multiple items selected
							if(group > GROUP_RECYCLE){
								EnableCommands(SEL_LIST_ORD_SELECTED_MULTIPLE);
							}
							else if(group == GROUP_RECYCLE){
								EnableCommands(SEL_LIST_REC_SELECTED_MULTIPLE);
							}
						}
						else{
							//single item selected
							if(group > GROUP_RECYCLE){
								DefineFavoriteStatus(nml->iItem);
								EnableCommands(SEL_LIST_ORD_SELECTED_SINGLE);
							}
							else if(group == GROUP_RECYCLE){
								EnableCommands(SEL_LIST_REC_SELECTED_SINGLE);
							}
						}
					}
				}
				else{
					SetPreviewBackgroundColor(TRUE, 0);
					SetWindowTextW(m_hEdit, NULL);
					if(tvid == GROUP_ROOT){
						if(count == 1){
							DefineFavoriteStatus(nml->iItem);
							EnableCommands(SEL_LIST_SELECTED_ON_ROOT_SINGLE);
						}
						else if(count == 0){
							EnableCommands(SEL_LIST_DESELECTED_ON_ROOT);
						}
						else{
							EnableCommands(SEL_LIST_SELECTED_ON_ROOT_MULTIPLE);
						}
					}
					else{
						//deselected
						if(count == 0){
							if(group > GROUP_RECYCLE){
								EnableCommands(SEL_LIST_ORD_DESELECTED);
							}
							else if(group == GROUP_RECYCLE){
								EnableCommands(SEL_LIST_REC_DESELECTED);
							}
						}
						else if(count == 1){
							//single item selected
							if(group > GROUP_RECYCLE){
								DefineFavoriteStatus(nml->iItem);
								EnableCommands(SEL_LIST_ORD_SELECTED_SINGLE);
							}
							else if(group == GROUP_RECYCLE){
								EnableCommands(SEL_LIST_REC_SELECTED_SINGLE);
							}
						}
						else{
							//multiple items selected
							if(group > GROUP_RECYCLE){
								EnableCommands(SEL_LIST_ORD_SELECTED_MULTIPLE);
							}
							else if(group == GROUP_RECYCLE){
								EnableCommands(SEL_LIST_REC_SELECTED_MULTIPLE);
							}
						}
					}
				}
				return TRUE;
			case NM_RCLICK:
				//show popup menu
				if(lpnm->idFrom == IDC_LVW_CONTROL){
					int index = -1;
					
					m_hMenuCtrlTemp = m_hCtrlPUMenu;
					index = SendDlgItemMessageW(hwnd, IDC_LVW_CONTROL, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
					if(index >= 0){
						LVITEMW		lvi;

						ZeroMemory(&lvi, sizeof(lvi));
						lvi.iItem = index;
						lvi.mask = LVIF_PARAM;
						SendMessageW(m_hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
						PrepareDockMenu((PMEMNOTE)lvi.lParam, m_hMenuCtrlTemp, GetMenuPosition(m_hMenuCtrlTemp, IDM_DOCK));
					}
					ShowPopUp(hwnd, m_hCtrlPUMenu);
				}
				else if(lpnm->idFrom == IDC_CTRL_TREEVIEW){
					m_tHighlighted = TreeView_GetDropHilight(m_hTree);
					//in case we are on selected item the highlighted item will be NULL
					if(m_tHighlighted == NULL)
						m_tHighlighted = m_tSelected;
					tvid = GetTVItemId(m_tHighlighted);
					if(tvid > GROUP_RECYCLE){
						EnableTreeMenus(m_tHighlighted);
						m_hMenuCtrlTemp = m_hTreePUMenu;
						ShowTreePopUp();
						EnableTreeMenus(m_tSelected);
					}
					else if(tvid == GROUP_RECYCLE){
						m_hMenuCtrlTemp = m_hRecyclePUMenu;
						DisableTreeToolbar();
						ShowRecyclePopUp();
					}
				}
				return TRUE;
			case NM_DBLCLK:
				//show note window on double click on item
				if(lpnm->idFrom == IDC_LVW_CONTROL && GetSelectedTVItemId() != -2){
					//do not apply to Recycle Bin
					LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE) lParam;
					ShowItems();
					SendMessageW(m_hList, LVM_ENSUREVISIBLE, lpnmitem->iItem, FALSE);
					ShowNotePreview(ListView_GetSelectedCount(m_hList));
				}
				return TRUE;
			case TTN_NEEDTEXTW:
				//show toolbar button tooltip
				if(lpnm->idFrom >= IDM_NEW && lpnm->idFrom < IDM_NEW_GROUP){
					GetTooltip(szTooltip, m_CtrlMenus, NELEMS(m_CtrlMenus), lpnm->idFrom);
					ttp = (TOOLTIPTEXTW *)lParam;
					wcscpy(ttp->szText, szTooltip);
				}
				else if(lpnm->idFrom >= IDM_NEW_GROUP){
					GetTooltip(szTooltip, m_TreeMenu, NELEMS(m_TreeMenu), lpnm->idFrom);
					ttp = (TOOLTIPTEXTW *)lParam;
					wcscpy(ttp->szText, szTooltip);
				}
				return TRUE;
			case LVN_BEGINDRAG:{
				POINT			pt;
				RECT			rc;
				LVITEMW			lvi;
				NMLISTVIEW *plv = (NMLISTVIEW *)lParam;
				int				count;

				if(GetTVItemId(m_tSelected) == -1)
					return TRUE;
				if(!IsTreeItemSuitable()){
					SetCursor(m_CurNo);
				}
				count = ListView_GetSelectedCount(m_hList);
				ClearDragStructure();
				rc.left = LVIR_BOUNDS;
				ListView_GetItemRect(m_hList, plv->iItem, &rc, LVIR_BOUNDS);
				m_xMouse = plv->ptAction.x - rc.left;
				m_yMouse = plv->ptAction.y - rc.top;
				m_DragStruct.fStarted = TRUE;
				m_DragStruct.hCurrent = m_hList;
				ZeroMemory(&lvi, sizeof(lvi));
				lvi.mask = LVIF_PARAM;
				lvi.iItem = plv->iItem;
				SendMessageW(m_hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
				m_DragStruct.idGroup = ((PMEMNOTE)lvi.lParam)->pData->idGroup;
				pt.x = 8;
				pt.y = 8;
				if(count == 1){
					m_DragStruct.hIml = ListView_CreateDragImage(m_hList, plv->iItem, &pt);
				}
				else{
					BOOL		bFirst = TRUE;
					int			pos, height, firstHeight, offset = 16;
					HIMAGELIST 	hOneImageList, hTempImageList;
					IMAGEINFO 	imf;

					pos = ListView_GetNextItem(m_hList, -1, LVNI_SELECTED);
					while(pos != -1){
						if(bFirst){
							m_DragStruct.hIml = ListView_CreateDragImage(m_hList, pos, &pt);
							ImageList_GetImageInfo(m_DragStruct.hIml, 0, &imf);
							height = firstHeight = imf.rcImage.bottom;
							bFirst = FALSE;
						}
						else{
							hOneImageList = ListView_CreateDragImage(m_hList, pos, &pt);
							hTempImageList = ImageList_Merge(m_DragStruct.hIml, 0, hOneImageList, 0, offset, height);
							ImageList_Destroy(m_DragStruct.hIml);
							ImageList_Destroy(hOneImageList);
							m_DragStruct.hIml = hTempImageList;
							ImageList_GetImageInfo(m_DragStruct.hIml, 0, &imf);
							height = imf.rcImage.bottom;
							offset += firstHeight;
							if(pos <= plv->iItem){
								m_xMouse += firstHeight;
								m_yMouse += firstHeight;
							}
						}
						pos = ListView_GetNextItem(m_hList, pos, LVNI_SELECTED);
					}
				}
				ImageList_BeginDrag(m_DragStruct.hIml, 0, 0, 0);
				ImageList_DragEnter(hwnd, plv->ptAction.x, plv->ptAction.y);
				SetCapture(hwnd);
				return TRUE;
			}
			default:
				return FALSE;
		}
	default: return FALSE;
	}
}

static void Control_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu)
{
	PrepareMenuGradientColors(GetSysColor(COLOR_BTNFACE));
}

static void Control_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	LVITEMW		lvi;
	int			count, lastId, currId, targetId, itemGroup, targetGroup;
	P_PNGROUP	ppg;
	HTREEITEM	hItem;
	
	if(m_DragStruct.fStarted){
		ImageList_DragShowNolock(FALSE);
		TreeView_SelectDropTarget(m_hTree, NULL);
		if(m_DragStruct.target){
			if(m_DragStruct.hCurrent == m_hList){
				count = ListView_GetItemCount(m_hList);
				targetGroup = GetTVItemId(m_DragStruct.target);
				for(int i = 0; i < count; i++){
					ZeroMemory(&lvi, sizeof(lvi));
					lvi.iItem = i;
					lvi.mask = LVIF_PARAM | LVIF_STATE;
					lvi.stateMask = LVIS_SELECTED;
					SendMessageW(m_hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
					if((lvi.state & LVIS_SELECTED) == LVIS_SELECTED){
						PMEMNOTE		pNote = (PMEMNOTE)lvi.lParam;
						NOTE_DATA		data;

						itemGroup = pNote->pData->idGroup;
						if(targetGroup > GROUP_RECYCLE){
							//gragging onto regular group
							if(itemGroup > GROUP_RECYCLE){
								pNote->pData->idGroup = targetGroup;
								//we should save only new group, leaving other data as is
								if(pNote->pFlags->fromDB){
									if(GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
										data.idGroup = pNote->pData->idGroup;
										WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
									}
								}
							}
							else if(itemGroup == GROUP_RECYCLE){
								//restore item to target group
								RestoreFromBin(targetGroup);
							}
						}
						else if(targetGroup == GROUP_RECYCLE){
							//dragging onto recycle bin
							if(itemGroup > GROUP_RECYCLE){
								DeleteToBin(pNote, -1);
							}
						}
					}
				}
				UpdateGroupText(itemGroup, m_tSelected);
				UpdateGroupText(targetGroup, m_DragStruct.target);
				TreeView_SelectItem(m_hTree, m_tSelected);
				UpdateListView();
			}
			else if(m_DragStruct.hCurrent == m_hTree){
				currId = GetTVItemId(m_DragStruct.source);
				targetId = GetTVItemId(m_DragStruct.target);
				if(currId > GROUP_RECYCLE){
					LockWindowUpdate(m_hCtrlPanel);
					if(targetId > GROUP_RECYCLE){
						lastId = GetTVItemId(m_tSelected);
						ppg = GetGroup(currId);
						ppg->parent = targetId;
						SaveGroup(ppg);
						hItem = InsertTreeviewItems();
						TreeView_Expand(m_hTree, hItem, TVE_EXPAND);
						//select previously selected item
						m_tItem = NULL;
						FindItemById(TreeView_GetRoot(m_hTree), lastId);
						if(m_tItem){
							TreeView_SelectItem(m_hTree, m_tItem);
						}
					}
					else if(targetId == GROUP_RECYCLE && currId > 0){
						SendMessageW(m_hCtrlPanel, WM_COMMAND, MAKEWPARAM(IDM_DELETE_GROUP, 0), (LPARAM)m_hTbrTree);
					}
					LockWindowUpdate(NULL);
				}
			}
		}
		ClearDragStructure();
		m_TreeDragReady = FALSE;
		ReleaseCapture();
	}
}

static void Control_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	HTREEITEM	hItem;
	int			id;
	RECT		rc;
	POINT		pt;

	if(m_DragStruct.fStarted && keyFlags == MK_LBUTTON){
		pt.x = x;
		pt.y = y;
		x += m_OffHor - m_xMouse;
		y += m_OffVert - m_yMouse;
		ImageList_DragMove(x, y);
		ImageList_DragShowNolock(FALSE);
		hItem = GetDropHighLightedItem(pt.x, pt.y);
		if(m_DragStruct.hCurrent == m_hList){
			if(hItem){
				id = GetTVItemId(hItem);
				if(id == -1 || id == m_DragStruct.idGroup){
					SetCursor(m_CurNo);
					m_DragStruct.target = NULL;
				}
				else{
					SetCursor(m_CurArrow);
					TreeView_SelectDropTarget(m_hTree, hItem);
					m_DragStruct.target = hItem;
				}
			}
			else{
				SetCursor(m_CurArrow);
				m_DragStruct.target = NULL;
			}
		}
		else{
			GetWindowRect(m_hList, &rc);
			MapWindowPoints(HWND_DESKTOP, m_hCtrlPanel, (LPPOINT)&rc, 2);
			if(PtInRect(&rc, pt)){
				SetCursor(m_CurNo);
				m_DragStruct.target = NULL;
			}
			else{
				if(hItem){
					id = GetTVItemId(hItem);
					if(id == m_DragStruct.idGroup || ((m_DragStruct.idGroup == GROUP_ROOT || m_DragStruct.idGroup == 0) && id == GROUP_RECYCLE) || IsItemFromChildBranch(m_DragStruct.source, hItem)){
						SetCursor(m_CurNo);
						m_DragStruct.target = NULL;
					}
					else{
						SetCursor(m_CurArrow);
						TreeView_SelectDropTarget(m_hTree, hItem);
						m_DragStruct.target = hItem;
					}
				}
				else{
					SetCursor(m_CurArrow);
					m_DragStruct.target = NULL;
				}
			}
		}
		ImageList_DragShowNolock(TRUE);
	}
}

static void Control_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if(m_hMenuCtrlTemp == m_hCtrlPUMenu)
		DrawMItem(lpDrawItem, g_hBmpMenuNormal, g_hBmpMenuGray, CLR_MASK);
	else if(m_hMenuCtrlTemp == m_hRecyclePUMenu)
		DrawMItem(lpDrawItem, g_hBmpMenuNormal, g_hBmpMenuGray, CLR_MASK);
}

static void Control_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem)
{
	if(m_hMenuCtrlTemp == m_hCtrlPUMenu)
		MeasureMItem(g_hMenuFont, lpMeasureItem);
	else if(m_hMenuCtrlTemp == m_hRecyclePUMenu)
		MeasureMItem(g_hMenuFont, lpMeasureItem);
}

static void Control_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	RECT		rc, rc2;

	if(state != SIZE_MINIMIZED){
		//size and position list view and toolbar
		GetWindowRect(m_hTbrCtrl, &rc);
		MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc, 2);
		MoveWindow(m_hTbrCtrl, rc.left, rc.top, cx, cy, TRUE);
		GetWindowRect(m_hStbControl, &rc2);
		MapWindowPoints(HWND_DESKTOP, hwnd, (LPPOINT)&rc2, 2);
		MoveWindow(m_hStbControl, rc2.left, cy - (rc2.bottom - rc2.top), cx, rc2.bottom - rc2.top, TRUE);
		SendMessageW(m_hSplitter, SPM_MAIN_RESIZED, (WPARAM)hwnd, MAKELPARAM(cx, cy));
	}
}

static void Control_OnClose(HWND hwnd)
{
	WINDOWPLACEMENT		wp;
	HTREEITEM			hItem;
	TVITEMW				tvi;
	wchar_t				szId[12];
	int					width[4];

	//clean up and quit
	if(m_hImlSmall)
		ImageList_Destroy(m_hImlSmall);
	if(m_hImlLarge)
		ImageList_Destroy(m_hImlLarge);
	if(m_hImlTbrNormal)
		ImageList_Destroy(m_hImlTbrNormal);
	if(m_hImlTbrGray)
		ImageList_Destroy(m_hImlTbrGray);
	if(m_hImlTreeNormal)
		ImageList_Destroy(m_hImlTreeNormal);
	if(m_hImlTreeGray)
		ImageList_Destroy(m_hImlTreeGray);
	if(m_hImlDragSmall)
		ImageList_Destroy(m_hImlDragSmall);
	if(m_hImlDragLarge)
		ImageList_Destroy(m_hImlDragLarge);
	if(m_hCtrlMenu){
		FreeMenus(m_hCtrlPUMenu);
		DestroyMenu(m_hCtrlMenu);
		m_hCtrlMenu = 0;
	}
	if(m_hTreeMenu){
		FreeMenus(m_hTreePUMenu);
		DestroyMenu(m_hTreeMenu);
		m_hTreeMenu = 0;
	}
	if(m_hRecycleMenu){
		FreeMenus(m_hRecyclePUMenu);
		DestroyMenu(m_hRecycleMenu);
		m_hRecycleMenu = 0;
	}
	//save last window position
	ZeroMemory(&wp, sizeof(wp));
	wp.length = sizeof(wp);
	GetWindowPlacement(hwnd, &wp);
	if(wp.showCmd != SW_SHOWMINIMIZED){
		WritePrivateProfileStructW(S_CP_DATA, IK_CP_POSITION, &wp, sizeof(wp), g_NotePaths.INIFile);
	}
	//save last selected group
	hItem = TreeView_GetSelection(m_hTree);
	if(hItem){
		ZeroMemory(&tvi, sizeof(tvi));
		tvi.mask = TVIF_HANDLE | TVIF_PARAM;
		tvi.hItem = hItem;
		if(TreeView_GetItem(m_hTree, &tvi)){
			_itow(tvi.lParam, szId, 10);
		}
		else{
			wcscpy(szId, L"-1");
		}
	}
	else{
		wcscpy(szId, L"-1");
	}
	WritePrivateProfileStringW(S_CP_DATA, IK_CP_LAST_GROUP, szId, g_NotePaths.INIFile);
	//save list view columns width
	if(m_View == LVS_REPORT){
		for(int i = 0, j = 0; i <= COL_SCHEDULE; i++){
			if(i != COL_GROUP && i != COL_PRIORITY && i != COL_COMPLETED && i != COL_PROTECTED){
				width[j] = ListView_GetColumnWidth(m_hList, i);
				j++;
			}
			else if(i == COL_GROUP){
				if( ListView_GetColumnWidth(m_hList, i) > 0)
					width[j] = ListView_GetColumnWidth(m_hList, i);
				else
					width[j] = m_GroupWidth;
				j++;
			}
		}
		WritePrivateProfileStructW(S_CP_DATA, IK_CP_COL_WIDTH, width, sizeof(width), g_NotePaths.INIFile);
	}
	//save sort order
	WritePrivateProfileStructW(S_CP_DATA, IK_CP_SORT_DATA, &m_lvsm, sizeof(m_lvsm), g_NotePaths.INIFile);
	//enable next dialog appearance
	g_hCPDialog = NULL;
	EndDialog(hwnd, 0);
}

static void Control_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	int			idGroup, index;
	BOOL		showPreview = FALSE;
	wchar_t		value[8];

	//the actions are clear
	switch(id){
		case IDM_PVW_COLOR_VALUE:
			if(ChoosePreviewColor()){
				_itow(m_PvwColor, value, 10);
				WritePrivateProfileStringW(S_CP_DATA, IK_CP_PVW_COLOR, value, g_NotePaths.INIFile);
			}
			showPreview = TRUE;
			break;
		case IDM_PVW_USE_COLOR:
			m_PvwUseColor = !m_PvwUseColor;
			_itow(m_PvwUseColor, value, 10);
			if(m_PvwUseColor)
				CheckMenuItem(m_hCtrlPUMenu, IDM_PVW_USE_COLOR, MF_CHECKED | MF_BYCOMMAND);
			else
				CheckMenuItem(m_hCtrlPUMenu, IDM_PVW_USE_COLOR, MF_UNCHECKED | MF_BYCOMMAND);
			WritePrivateProfileStringW(S_CP_DATA, IK_CP_PVW_USE_COLOR, value, g_NotePaths.INIFile);

			showPreview = TRUE;
			break;
		case IDM_V_VIEW:
			ShowButtonDropDown(IDM_V_VIEW, IDM_V_VIEW);
			showPreview = TRUE;
			break;
		case IDM_PVW_COLOR_SET:
			ShowButtonDropDown(IDM_PVW_COLOR_SET, IDM_PVW_COLOR_SET);
			showPreview = TRUE;
			break;
		case IDM_NOTE_MARKS:
			ShowButtonDropDown(IDM_NOTE_MARKS, IDM_NOTE_MARKS);
			showPreview = TRUE;
			break;
		case IDM_DOCK_ALL:
			ShowButtonDropDown(IDM_DOCK_ALL, IDM_DOCK_ALL);
			showPreview = TRUE;
			break;
		case IDM_SEARCH_SUBMENU:
			ShowButtonDropDown(IDM_SEARCH_SUBMENU, IDM_SEARCH_SUBMENU);
			showPreview = TRUE;
			break;
		case IDM_VISIBILITY:
			ShowButtonDropDown(IDM_VISIBILITY, IDM_VISIBILITY);
			showPreview = TRUE;
			break;
		case IDM_OPTIONS:
			SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(IDM_OPTIONS, 0), 0);
			showPreview = TRUE;
			break;
		case IDM_V_ICONS:
			ChangeListViewView(m_hList, LVS_ICON);
			showPreview = TRUE;
			break;
		case IDM_V_LIST:
			ChangeListViewView(m_hList, LVS_LIST);
			showPreview = TRUE;
			break;
		case IDM_V_REPORT:
			ChangeListViewView(m_hList, LVS_REPORT);
			showPreview = TRUE;
			break;
		case IDM_V_SMALL:
			ChangeListViewView(m_hList, LVS_SMALLICON);
			showPreview = TRUE;
			break;
		case IDM_NEW:
			idGroup = GetTVItemId(m_tSelected);
			if(idGroup < 0)
				idGroup = 0;
			SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(IDM_NEW, idGroup), 0);
			showPreview = TRUE;
			break;
		case IDM_NOTE_FROM_CLIPBOARD:
			idGroup = GetTVItemId(m_tSelected);
			if(idGroup < 0)
				idGroup = 0;
			SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(IDM_NOTE_FROM_CLIPBOARD, idGroup), 0);
			showPreview = TRUE;
			break;
		case IDM_LOAD_NOTE:
			SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(IDM_LOAD_NOTE, idGroup), 0);
			showPreview = TRUE;
			break;
		case IDM_SHOW_ALL:
			SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(IDM_SHOW_ALL, 0), 0);
			EnableTreeMenus(m_tSelected);
			showPreview = TRUE;
			break;
		case IDM_SAVE_ALL:
			SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(IDM_SAVE_ALL, 0), 0);
			showPreview = TRUE;
			break;
		case IDM_DOCK_NONE_ALL:
		case IDM_DOCK_LEFT_ALL:
		case IDM_DOCK_TOP_ALL:
		case IDM_DOCK_RIGHT_ALL:
		case IDM_DOCK_BOTTOM_ALL:
			SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(id, 0), 0);
			showPreview = TRUE;
			break;
		case IDM_HIDE_ALL:
			SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(IDM_HIDE_ALL, 0), 0);
			EnableTreeMenus(m_tSelected);
			showPreview = TRUE;
			break;
		case IDM_ALL_TO_FRONT:
			SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(IDM_ALL_TO_FRONT, 0), 0);
			showPreview = TRUE;
			break;
		case IDM_SEARCH_IN_NOTES:
			if(!g_hSearchDialog)
				DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_FIND), NULL, g_SearchProc, 0);
			else
				BringWindowToTop(g_hSearchDialog);
			showPreview = TRUE;
			break;
		case IDM_SEARCH_TITLE:
			if(!g_hSearchTitlesDialog)
				DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_TITLES_SEARCH), NULL, g_SearchTitlesProc, 0);
			else
				BringWindowToTop(g_hSearchTitlesDialog);
			showPreview = TRUE;
			break;
		case IDM_HELP:
			SendMessageW(g_hMain, WM_COMMAND, MAKEWPARAM(IDM_HELP, 0), 0);
			showPreview = TRUE;
			break;
		case IDM_EMPTY_BIN:
			if(EmptyBin()){
				SendMessageW(hwnd, CHM_RELOAD, 0, 0);	
				UpdateGroupText(GROUP_RECYCLE, TreeView_GetNextSibling(m_hTree, TreeView_GetRoot(m_hTree)));
			}
			showPreview = TRUE;
			break;
		case IDM_DEL:
			if(DeleteItems()){
				SendMessageW(hwnd, CHM_RELOAD, 0, 0);
				UpdateGroupText(GROUP_RECYCLE, TreeView_GetNextSibling(m_hTree, TreeView_GetRoot(m_hTree)));
			}
			showPreview = TRUE;
			break;
		case IDM_RESTORE_ALL_BIN:
			if(RestoreAllFromBin()){
				SendMessageW(hwnd, CHM_RELOAD, 0, 0);
				UpdateGroupText(GROUP_RECYCLE, TreeView_GetNextSibling(m_hTree, TreeView_GetRoot(m_hTree)));
			}
			showPreview = TRUE;
			break;
		case IDM_RESTORE_NOTE:
			if(RestoreFromBin(-1)){
				SendMessageW(hwnd, CHM_RELOAD, 0, 0);
				UpdateGroupText(GROUP_RECYCLE, TreeView_GetNextSibling(m_hTree, TreeView_GetRoot(m_hTree)));
			}
			showPreview = TRUE;
			break;
		case IDM_ROLL_UNROLL:
			RollUnrollItems();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_HIDE:
			HideItems();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_SHOW:
			ShowItems();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_PRINT_NOTE:
			PrintItem();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_EMAIL:
			SendItemByEmail();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_ATTACHMENT:
			SendItemAsAttachment();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_SAVE:
			SaveItem();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_SAVE_AS:
			SaveItemAs();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_SHOW_CENTER:
		case IDM_RESTORE_PLACEMENT:
			RestoreCenterItem(id);
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_ADJUST:
			if(g_hAdjust == NULL){
				EnableMenuItem(m_hCtrlPUMenu, IDM_ADJUST_APPEARANCE, MF_BYCOMMAND | MF_ENABLED);
				EnableMenuItem(m_hCtrlPUMenu, IDM_ADJUST_SCHEDULE, MF_BYCOMMAND | MF_ENABLED);
			}
			else{
				EnableMenuItem(m_hCtrlPUMenu, IDM_ADJUST_APPEARANCE, MF_BYCOMMAND | MF_GRAYED);
				EnableMenuItem(m_hCtrlPUMenu, IDM_ADJUST_SCHEDULE, MF_BYCOMMAND | MF_GRAYED);
			}
			ShowButtonDropDown(IDM_ADJUST, IDM_ADJUST);
			showPreview = TRUE;
			break;
		case IDM_ADJUST_APPEARANCE:
			AdjustItem(0);
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_ADJUST_SCHEDULE:
			AdjustItem(1);
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_ADD_TO_FAVORITES:
			AddItemsToFavorites();
			ReselectAfterCommand();
			index = SendMessageW(m_hList, LVM_GETNEXTITEM, -1, LVNI_ALL | LVNI_SELECTED);
			DefineFavoriteStatus(index);
			showPreview = TRUE;
			break;
		case IDM_HIGH_PRIORITY:
			ToggleItemsPriority();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_TOGGLE_PROTECTION:
			ToggleItemsProtection();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_MARK_AS_COMPLETED:
			ToggleItemsCompleted();
			ReselectAfterCommand();
			showPreview = TRUE;
			break;
		case IDM_PREVIEW:
			ShowHidePreview();
			showPreview = TRUE;
			break;
		case IDM_DOCK_LEFT:
		case IDM_DOCK_TOP:
		case IDM_DOCK_RIGHT:
		case IDM_DOCK_BOTTOM:
		case IDM_DOCK_NONE:
			SendNotesMessage(hwnd, id);
			EnableTreeMenus(m_tSelected);
			showPreview = TRUE;
			break;		
	}
	if(showPreview){
		ShowNotePreview(ListView_GetSelectedCount(m_hList));
	}
}

static BOOL Control_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	RECT				rc;
	WINDOWPLACEMENT		wp;
	int					lastId;
	NMLISTVIEW			nml;

	m_hCtrlPanel = hwnd;
	//prevent dialog from addidtional appearance
	g_hCPDialog = hwnd;
	//get needed handles
	m_hTbrCtrl = GetDlgItem(hwnd, IDC_TBR_CTRL);
	m_hList = GetDlgItem(hwnd, IDC_LVW_CONTROL);
	m_hStbControl = GetDlgItem(hwnd, IDC_STB_CONTROL);
	//get needed offsets
	m_OffVert = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME);
	m_OffHor = GetSystemMetrics(SM_CXEDGE);
	//create rich edit
	m_hEdit = CreateWindowExW(WS_EX_STATICEDGE, RICHEDIT_CLASSW, NULL, WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE | ES_NOHIDESEL | WS_VSCROLL | WS_HSCROLL, 0, 0, 0, 0, hwnd, NULL, g_hInstance, NULL);
	//add tree view dialog
	m_hDTree = CreateDialogParamW(g_hInstance, MAKEINTRESOURCEW(DLG_CONTROL_TREE), hwnd, DTree_DlgProc, 0);
	//add splitter
	GetClientRect(m_hTbrCtrl, &rc);
	m_Up = rc.bottom - rc.top;
	GetWindowRect(m_hStbControl, &rc);
	m_Down = rc.bottom - rc.top;
	m_hSplitter = CreateMainPanel(SPLIT_VERT, g_hInstance, hwnd, g_CurV, 36, 0, m_Up, 0, m_Down);
	m_hHorSplitter = CreateMainPanel(SPLIT_HORZ, g_hInstance, hwnd, g_CurH, 36, 0, 0, 0, 0);
	SendMessageW(m_hHorSplitter, SPM_ADD_CHILD, (WPARAM)m_hList, CHILD_TOP);
	SendMessageW(m_hHorSplitter, SPM_ADD_CHILD, (WPARAM)m_hEdit, CHILD_BOTTOM);
	SendMessageW(m_hSplitter, SPM_ADD_CHILD, (WPARAM)m_hDTree, CHILD_LEFT);
	SendMessageW(m_hSplitter, SPM_ADD_CHILD, (WPARAM)m_hHorSplitter, CHILD_RIGHT);
	//load needed cursors
	m_CurNo = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(OCR_NO), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	m_CurArrow = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(OCR_NORMAL), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	//clear drag structure
	ZeroMemory(&m_DragStruct, sizeof(m_DragStruct));
	
	SendMessageW(m_hTbrCtrl, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);

	SetWindowLongPtrW(m_hList, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(m_hList, GWLP_WNDPROC, (LONG_PTR)List_Proc));
	SetWindowLongPtrW(m_hTree, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtrW(m_hTree, GWLP_WNDPROC, (LONG_PTR)Tree_Proc));

	//set icon
	SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIconW(g_hInstance, MAKEINTRESOURCEW(IDR_ICO_MAIN)));
	//prepare dialog
	CreateImageListsW(&m_hImlTbrNormal, &m_hImlTbrGray, g_hInstance, IDB_MENU_NORMAL, IDB_MENU_GRAY, CLR_MASK, 16, 16, NELEMS(m_TBBtnArray));
	PrepareTree(hwnd, m_hTree);
	PrepareList(hwnd, m_hList);
	
	//m_hSplitter = CreateSplitter(hwnd, m_hTree, m_hList, 220, 64, 64, upper, rc.bottom - rc.top - upper - (rc1.bottom - rc1.top), upper);
	//set text for controls and menus
	ApplyControlPanelLanguage(hwnd);
	//allow subitems to have images
	// SendMessageW(m_hList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES, LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES);
	 SendMessageW(m_hList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_SUBITEMIMAGES, LVS_EX_SUBITEMIMAGES);
	//prepare status bar
	SendMessageW(m_hStbControl, SB_SIMPLE, FALSE, 0);
	//fill list view
	UpdateListView();
	
	//get last selected item
	lastId = GetPrivateProfileIntW(S_CP_DATA, IK_CP_LAST_GROUP, -1, g_NotePaths.INIFile);
	m_tItem = NULL;
	FindItemById(TreeView_GetRoot(m_hTree), lastId);
	if(m_tItem){
		TreeView_SelectItem(m_hTree, m_tItem);
	}
	//sort items
	ZeroMemory(&nml, sizeof(nml));
	nml.hdr.code = LVN_COLUMNCLICK;
	nml.hdr.hwndFrom = m_hList;
	nml.hdr.idFrom = IDC_LVW_CONTROL;
	nml.iItem = -1;
	nml.iSubItem = m_lvsm.column;
	switch(m_lvsm.column){
	case COL_NAME:
		if(m_lvsm.order == LVS_SORTASCENDING)
			m_LVSort.iSortName = LVS_SORTDESCENDING;
		else
			m_LVSort.iSortName = LVS_SORTASCENDING;
		break;
	case COL_PRIORITY:
		if(m_lvsm.order == LVS_SORTASCENDING)
			m_LVSort.iSortPriority = LVS_SORTDESCENDING;
		else
			m_LVSort.iSortPriority = LVS_SORTASCENDING;
		break;
	case COL_COMPLETED:
		if(m_lvsm.order == LVS_SORTASCENDING)
			m_LVSort.iSortCompleted = LVS_SORTDESCENDING;
		else
			m_LVSort.iSortCompleted = LVS_SORTASCENDING;
		break;
	case COL_PROTECTED:
		if(m_lvsm.order == LVS_SORTASCENDING)
			m_LVSort.iSortProtected = LVS_SORTDESCENDING;
		else
			m_LVSort.iSortProtected = LVS_SORTASCENDING;
		break;
	case COL_GROUP:
		if(m_lvsm.order == LVS_SORTASCENDING)
			m_LVSort.iSortGroup = LVS_SORTDESCENDING;
		else
			m_LVSort.iSortGroup = LVS_SORTASCENDING;
		break;
	case COL_DATE:
		if(m_lvsm.order == LVS_SORTASCENDING)
			m_LVSort.iSortDate = LVS_SORTDESCENDING;
		else
			m_LVSort.iSortDate = LVS_SORTASCENDING;
		break;
	case COL_SCHEDULE:
		if(m_lvsm.order == LVS_SORTASCENDING)
			m_LVSort.iSortSchedule = LVS_SORTDESCENDING;
		else
			m_LVSort.iSortSchedule = LVS_SORTASCENDING;
		break;
	}	
	SendMessageW(hwnd, WM_NOTIFY, 0, (LPARAM)&nml);

	//position window
	ZeroMemory(&wp, sizeof(wp));
	wp.length = sizeof(wp);
	if(GetPrivateProfileStructW(S_CP_DATA, IK_CP_POSITION, &wp, sizeof(wp), g_NotePaths.INIFile)){
		wp.showCmd = SW_SHOWNORMAL;
		SetWindowPlacement(hwnd, &wp);
		MoveWindow(hwnd, wp.rcNormalPosition.left, wp.rcNormalPosition.top, wp.rcNormalPosition.right - wp.rcNormalPosition.left + 1, wp.rcNormalPosition.bottom - wp.rcNormalPosition.top, TRUE);
	}

	//hide preview window
	if((GetPrivateProfileIntW(S_CP_DATA, IK_CP_PVW_VISIBLE, 0, g_NotePaths.INIFile) == 0))
		SendMessageW(m_hHorSplitter, SPM_HIDE_CHILD, 0, CHILD_BOTTOM);
	else{
		CheckMenuItem(m_hCtrlPUMenu, IDM_PREVIEW, MF_CHECKED | MF_BYCOMMAND);
		SendMessageW(m_hTbrCtrl, TB_CHECKBUTTON, IDM_PREVIEW, (LPARAM)MAKELONG(TBSTATE_CHECKED, 0));
	}
	//get preview color settings
	m_PvwUseColor = (BOOL)GetPrivateProfileIntW(S_CP_DATA, IK_CP_PVW_USE_COLOR, 0, g_NotePaths.INIFile);
	m_PvwColor = GetPrivateProfileIntW(S_CP_DATA, IK_CP_PVW_COLOR, -1, g_NotePaths.INIFile);
	if(m_PvwUseColor)
		CheckMenuItem(m_hCtrlPUMenu, IDM_PVW_USE_COLOR, MF_CHECKED | MF_BYCOMMAND);
	else
		CheckMenuItem(m_hCtrlPUMenu, IDM_PVW_USE_COLOR, MF_UNCHECKED | MF_BYCOMMAND);
	SetPreviewBackgroundColor(TRUE, 0);

	CheckMenuItem(m_hCtrlPUMenu, IDM_V_REPORT, MF_BYCOMMAND | MF_CHECKED);

	SetFocus(m_hTree);
	
	return FALSE;
}

static void SetPreviewBackgroundColor(BOOL clear, COLORREF color){
	if(!clear){
		//set the color
		if(!m_PvwUseColor){
			//use note's color
			RichEdit_SetBkgndColor(m_hEdit, 0, color);
		}
		else{
			//use predefined color
			if(m_PvwColor != -1)
				//if the color is really defined
				RichEdit_SetBkgndColor(m_hEdit, 0, m_PvwColor);
			else
				//if not - use system window background color
				RichEdit_SetBkgndColor(m_hEdit, 1, 0);
		}
	}
	else{
		//clear edit box
		if(!m_PvwUseColor){
			//clear with system window background color
			RichEdit_SetBkgndColor(m_hEdit, 1, 0);
		}
		else{
			//clear with predefined color
			if(m_PvwColor != -1)
				//clear with predefined color
				RichEdit_SetBkgndColor(m_hEdit, 0, m_PvwColor);
			else
				//clear with system window background color
				RichEdit_SetBkgndColor(m_hEdit, 1, 0);
		}
	}
}

static BOOL ChoosePreviewColor(void){
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
	cc.hwndOwner = g_hCPDialog;
	if(m_PvwColor != -1)
		cc.rgbResult = m_PvwColor;
	else
		cc.rgbResult = RGB(255, 255, 255);
	cc.lCustData = (int)szBuffer;
	cc.lpfnHook = CCHookProc;
	if(ChooseColorW(&cc)){
		if(m_PvwColor != cc.rgbResult){
			m_PvwColor = cc.rgbResult;
			return TRUE;
		}
	}
	return FALSE;
}

static void ChangeListViewView(HWND hwnd, int view){
	int			style, id;

	m_View = view;
	style = GetWindowLongPtrW(hwnd, GWL_STYLE);
	style &= (~LVS_TYPEMASK);
	style |= view;
	SetWindowLongPtrW(hwnd, GWL_STYLE, style);
	switch(view){
		case LVS_ICON:
			id = IDM_V_ICONS;
			break;
		case LVS_SMALLICON:
			id = IDM_V_SMALL;
			break;
		case LVS_LIST:
			id = IDM_V_LIST;
			break;
		case LVS_REPORT:
			id = IDM_V_REPORT;
			break;
	}
	for(int i = IDM_V_ICONS; i <= IDM_V_REPORT; i++){
		if(i == id)
			CheckMenuItem(m_hCtrlPUMenu, i, MF_BYCOMMAND | MF_CHECKED);
		else
			CheckMenuItem(m_hCtrlPUMenu, i, MF_BYCOMMAND | MF_UNCHECKED);
	}
}

static void EnableCommands(int flag){
	int count;
	TBBUTTONINFOW	tbi;

	//disable all menus
	EnablemenuItems(m_hCtrlPUMenu, MF_GRAYED);
	//count = GetMenuItemCount(m_hCtrlPUMenu);
	//for(int i = 0; i < count; i++){
		//EnableMenuItem(m_hCtrlPUMenu, i, MF_BYPOSITION | MF_GRAYED);
	//}
	//disable all toolbar buttons
	count = ToolBar_ButtonCount(m_hTbrCtrl);
	ZeroMemory(&tbi, sizeof(tbi));
	tbi.cbSize = sizeof(tbi);
	tbi.dwMask = TBIF_BYINDEX | TBIF_COMMAND | TBIF_STYLE;
	for(int i = 0; i < count; i++){
		SendMessageW(m_hTbrCtrl, TB_GETBUTTONINFOW, i, (LPARAM)&tbi);
		SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, tbi.idCommand, FALSE);
	}
	switch(flag){
		case SEL_TREE_ROOT:
		case SEL_LIST_DESELECTED_ON_ROOT:
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_NEW, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_NEW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_LOAD_NOTE, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_LOAD_NOTE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SAVE_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SAVE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_DOCK_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_DOCK_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SHOW_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SHOW_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_HIDE_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_HIDE_ALL, MF_BYCOMMAND | MF_ENABLED);
			if(IsTextInClipboard()){
				SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_NOTE_FROM_CLIPBOARD, TRUE);
				EnableMenuItem(m_hCtrlPUMenu, IDM_NOTE_FROM_CLIPBOARD, MF_BYCOMMAND | MF_ENABLED);
			}
			break;
		case SEL_TREE_ORD:
		case SEL_LIST_ORD_DESELECTED:
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_NEW, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_NEW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_LOAD_NOTE, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_LOAD_NOTE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SAVE_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SAVE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_DOCK_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_DOCK_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SHOW_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SHOW_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_HIDE_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_HIDE_ALL, MF_BYCOMMAND | MF_ENABLED);
			if(IsTextInClipboard()){
				SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_NOTE_FROM_CLIPBOARD, TRUE);
				EnableMenuItem(m_hCtrlPUMenu, IDM_NOTE_FROM_CLIPBOARD, MF_BYCOMMAND | MF_ENABLED);
			}
			break;
		case SEL_TREE_REC:
		case SEL_LIST_REC_DESELECTED:
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_EMPTY_BIN, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_EMPTY_BIN, MF_BYCOMMAND | MF_ENABLED);
			break;
		case SEL_LIST_ORD_SELECTED_SINGLE:
		case SEL_LIST_AFTER_COMMAND_SINGLE:
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_NEW, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_NEW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_LOAD_NOTE, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_LOAD_NOTE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SAVE, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SAVE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SAVE_AS, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SAVE_AS, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_ADJUST, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_ADJUST, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SAVE_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SAVE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_DOCK_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_DOCK_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_DOCK, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_DOCK, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SHOW, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SHOW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SHOW_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SHOW_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_HIDE, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_HIDE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_HIDE_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_HIDE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SHOW_CENTER, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SHOW_CENTER, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_RESTORE_PLACEMENT, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_RESTORE_PLACEMENT, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_DEL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_DEL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_EMAIL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_EMAIL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_ATTACHMENT, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_ATTACHMENT, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_ADD_TO_FAVORITES, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_ADD_TO_FAVORITES, MF_BYCOMMAND | MF_ENABLED);
			if(IsTextInClipboard()){
				SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_NOTE_FROM_CLIPBOARD, TRUE);
				EnableMenuItem(m_hCtrlPUMenu, IDM_NOTE_FROM_CLIPBOARD, MF_BYCOMMAND | MF_ENABLED);
			}
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_PRINT_NOTE, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_PRINT_NOTE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_NOTE_MARKS, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_NOTE_MARKS, MF_BYCOMMAND | MF_ENABLED);
			break;
		case SEL_LIST_ORD_SELECTED_MULTIPLE:
		case SEL_LIST_AFTER_COMMAND_MULTIPLE:
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_NEW, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_NEW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_LOAD_NOTE, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_LOAD_NOTE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SAVE_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SAVE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_DOCK_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_DOCK_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SHOW_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SHOW_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_HIDE_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_HIDE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_DEL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_DEL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_ADD_TO_FAVORITES, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_ADD_TO_FAVORITES, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SHOW, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SHOW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_HIDE, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_HIDE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_ATTACHMENT, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_ATTACHMENT, MF_BYCOMMAND | MF_ENABLED);
			if(IsTextInClipboard()){
				SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_NOTE_FROM_CLIPBOARD, TRUE);
				EnableMenuItem(m_hCtrlPUMenu, IDM_NOTE_FROM_CLIPBOARD, MF_BYCOMMAND | MF_ENABLED);
			}
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_NOTE_MARKS, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_NOTE_MARKS, MF_BYCOMMAND | MF_ENABLED);
			break;
		// case SEL_LIST_ORD_DESELECTED:

			// break;
		case SEL_LIST_REC_SELECTED_SINGLE:
		case SEL_LIST_REC_SELECTED_MULTIPLE:
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_EMPTY_BIN, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_EMPTY_BIN, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_RESTORE_NOTE, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_RESTORE_NOTE, MF_BYCOMMAND | MF_ENABLED);
			break;
		// case SEL_LIST_REC_DESELECTED:

			// break;
		case SEL_LIST_SELECTED_ON_ROOT_SINGLE:
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_NEW, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_NEW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_LOAD_NOTE, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_LOAD_NOTE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SAVE, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SAVE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SAVE_AS, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SAVE_AS, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_ADJUST, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_ADJUST, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SAVE_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SAVE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_DOCK_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_DOCK_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_DOCK, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_DOCK, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SHOW, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SHOW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SHOW_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SHOW_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_HIDE, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_HIDE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_HIDE_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_HIDE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SHOW_CENTER, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SHOW_CENTER, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_RESTORE_PLACEMENT, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_RESTORE_PLACEMENT, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_DEL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_DEL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_EMAIL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_EMAIL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_ATTACHMENT, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_ATTACHMENT, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_ADD_TO_FAVORITES, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_ADD_TO_FAVORITES, MF_BYCOMMAND | MF_ENABLED);
			if(IsTextInClipboard()){
				SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_NOTE_FROM_CLIPBOARD, TRUE);
				EnableMenuItem(m_hCtrlPUMenu, IDM_NOTE_FROM_CLIPBOARD, MF_BYCOMMAND | MF_ENABLED);
			}
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_PRINT_NOTE, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_PRINT_NOTE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_NOTE_MARKS, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_NOTE_MARKS, MF_BYCOMMAND | MF_ENABLED);
			break;
		case SEL_LIST_SELECTED_ON_ROOT_MULTIPLE:
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_NEW, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_NEW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_LOAD_NOTE, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_LOAD_NOTE, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SAVE_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SAVE_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_DOCK_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_DOCK_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SHOW, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SHOW, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SHOW_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_SHOW_ALL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_HIDE_ALL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_HIDE_ALL, MF_BYCOMMAND | MF_ENABLED);;
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_DEL, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_DEL, MF_BYCOMMAND | MF_ENABLED);
			SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_ATTACHMENT, TRUE);
			EnableMenuItem(m_hCtrlPUMenu, IDM_ATTACHMENT, MF_BYCOMMAND | MF_ENABLED);
			if(IsTextInClipboard()){
				SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_NOTE_FROM_CLIPBOARD, TRUE);
				EnableMenuItem(m_hCtrlPUMenu, IDM_NOTE_FROM_CLIPBOARD, MF_BYCOMMAND | MF_ENABLED);
			}
			break;
		case SEL_LIST_FAV_SELECTED_SINGLE:

			break;
		case SEL_LIST_FAV_SELECTED_MULTIPLE:

			break;
		case SEL_LIST_FAV_DESELECTED:

			break;
	}

	SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_V_VIEW, TRUE);
	EnableMenuItem(m_hCtrlPUMenu, IDM_V_VIEW, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hCtrlPUMenu, IDM_V_ICONS, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hCtrlPUMenu, IDM_V_LIST, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hCtrlPUMenu, IDM_V_REPORT, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hCtrlPUMenu, IDM_V_SMALL, MF_BYCOMMAND | MF_ENABLED);

	EnableMenuItem(m_hCtrlPUMenu, IDM_DOCK_NONE_ALL, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hCtrlPUMenu, IDM_DOCK_LEFT_ALL, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hCtrlPUMenu, IDM_DOCK_TOP_ALL, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hCtrlPUMenu, IDM_DOCK_RIGHT_ALL, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hCtrlPUMenu, IDM_DOCK_BOTTOM_ALL, MF_BYCOMMAND | MF_ENABLED);

	SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_OPTIONS, TRUE);
	EnableMenuItem(m_hCtrlPUMenu, IDM_OPTIONS, MF_BYCOMMAND | MF_ENABLED);

	SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_SEARCH_SUBMENU, TRUE);
	EnableMenuItem(m_hCtrlPUMenu, IDM_SEARCH_SUBMENU, MF_BYCOMMAND | MF_ENABLED);

	if(flag != SEL_TREE_REC && flag != SEL_LIST_REC_SELECTED_SINGLE && flag != SEL_LIST_REC_SELECTED_MULTIPLE && flag != SEL_LIST_REC_DESELECTED){
		SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_VISIBILITY, TRUE);
		EnableMenuItem(m_hCtrlPUMenu, IDM_VISIBILITY, MF_BYCOMMAND | MF_ENABLED);
	}
	else{
		SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_VISIBILITY, FALSE);
		EnableMenuItem(m_hCtrlPUMenu, IDM_VISIBILITY, MF_BYCOMMAND | MF_GRAYED);
	}

	SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_HELP, TRUE);
	EnableMenuItem(m_hCtrlPUMenu, IDM_HELP, MF_BYCOMMAND | MF_ENABLED);

	SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_ALL_TO_FRONT, TRUE);
	EnableMenuItem(m_hCtrlPUMenu, IDM_ALL_TO_FRONT, MF_BYCOMMAND | MF_ENABLED);

	EnableMenuItem(m_hCtrlPUMenu, IDM_SEARCH_IN_NOTES, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hCtrlPUMenu, IDM_SEARCH_TITLE, MF_BYCOMMAND | MF_ENABLED);

	if(g_hAdjust){
		EnableMenuItem(m_hCtrlPUMenu, IDM_ADJUST_APPEARANCE, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(m_hCtrlPUMenu, IDM_ADJUST_SCHEDULE, MF_BYCOMMAND | MF_GRAYED);
	}
	else{
		EnableMenuItem(m_hCtrlPUMenu, IDM_ADJUST_APPEARANCE, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(m_hCtrlPUMenu, IDM_ADJUST_SCHEDULE, MF_BYCOMMAND | MF_ENABLED);
	}
	//always enable preview background items
	SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_PREVIEW, TRUE);
	EnableMenuItem(m_hCtrlPUMenu, IDM_PREVIEW, MF_BYCOMMAND | MF_ENABLED);
	SendMessageW(m_hTbrCtrl, TB_ENABLEBUTTON, IDM_PVW_COLOR_SET, TRUE);
	EnableMenuItem(m_hCtrlPUMenu, IDM_PVW_COLOR_SET, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hCtrlPUMenu, IDM_PVW_USE_COLOR, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hCtrlPUMenu, IDM_PVW_COLOR_VALUE, MF_BYCOMMAND | MF_ENABLED);
	//always enable marks items
	EnableMenuItem(m_hCtrlPUMenu, IDM_HIGH_PRIORITY, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hCtrlPUMenu, IDM_TOGGLE_PROTECTION, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hCtrlPUMenu, IDM_MARK_AS_COMPLETED, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hCtrlPUMenu, IDM_ROLL_UNROLL, MF_BYCOMMAND | MF_ENABLED);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: EnableMenus
 Created  : Fri May 25 22:29:15 2007
 Modified : Fri May 25 22:29:25 2007

 Synopsys : Enables or disables menu items
 Input    : fEnabled - enable flag
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static void EnableMenus(UINT fEnabled, BOOL bMultiSelect){
	EnableMenuItem(m_hCtrlPUMenu, IDM_SAVE, MF_BYCOMMAND | fEnabled);
	EnableMenuItem(m_hCtrlPUMenu, IDM_SAVE_AS, MF_BYCOMMAND | fEnabled);
	if(!bMultiSelect){
		EnableMenuItem(m_hCtrlPUMenu, IDM_SHOW, MF_BYCOMMAND | fEnabled);
		EnableMenuItem(m_hCtrlPUMenu, IDM_HIDE, MF_BYCOMMAND | fEnabled);
		EnableMenuItem(m_hCtrlPUMenu, IDM_DEL, MF_BYCOMMAND | fEnabled);
		EnableMenuItem(m_hCtrlPUMenu, IDM_DOCK, MF_BYCOMMAND | fEnabled);
	}
	else{
		EnableMenuItem(m_hCtrlPUMenu, IDM_SHOW, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(m_hCtrlPUMenu, IDM_HIDE, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(m_hCtrlPUMenu, IDM_DEL, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(m_hCtrlPUMenu, IDM_DOCK, MF_BYCOMMAND | MF_GRAYED);
	}
	EnableMenuItem(m_hCtrlPUMenu, IDM_SHOW_CENTER, MF_BYCOMMAND | fEnabled);

	if(fEnabled == MF_ENABLED){
		if(g_RTHandles.hbSkin)
			EnableMenuItem(m_hCtrlPUMenu, IDM_RESTORE_PLACEMENT, MF_BYCOMMAND | MF_GRAYED);
		else
			EnableMenuItem(m_hCtrlPUMenu, IDM_RESTORE_PLACEMENT, MF_BYCOMMAND | MF_ENABLED);
	}
	else
		EnableMenuItem(m_hCtrlPUMenu, IDM_RESTORE_PLACEMENT, MF_BYCOMMAND | MF_GRAYED);

	EnableMenuItem(m_hCtrlPUMenu, IDM_EMAIL, MF_BYCOMMAND | fEnabled);
	EnableMenuItem(m_hCtrlPUMenu, IDM_ATTACHMENT, MF_BYCOMMAND | fEnabled);
	EnableMenuItem(m_hCtrlPUMenu, IDM_ADD_TO_FAVORITES, MF_BYCOMMAND | fEnabled);

	if(!g_hAdjust){
		EnableMenuItem(m_hCtrlPUMenu, IDM_ADJUST, MF_BYCOMMAND | fEnabled);
	}
	else{
		EnableMenuItem(m_hCtrlPUMenu, IDM_ADJUST, MF_BYCOMMAND | MF_GRAYED);
	}
}

static void DisableTreeToolbar(void){
	TBBUTTONINFOW	tbi;
	int				count;

	count = ToolBar_ButtonCount(m_hTbrTree);
	ZeroMemory(&tbi, sizeof(tbi));
	tbi.cbSize = sizeof(tbi);
	tbi.dwMask = TBIF_BYINDEX | TBIF_COMMAND | TBIF_STYLE;
	for(int i = 0; i < count; i++){
		SendMessageW(m_hTbrTree, TB_GETBUTTONINFOW, i, (LPARAM)&tbi);
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, tbi.idCommand, FALSE);
	}
}

static void EnableTreeMenusForRoot(void){
	EnableMenuItem(m_hTreePUMenu, IDM_NEW_GROUP, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(m_hTreePUMenu, IDM_NEW_SUBGROUP, MF_BYCOMMAND | MF_GRAYED);
	EnableMenuItem(m_hTreePUMenu, IDM_EDIT_GROUP, MF_BYCOMMAND | MF_GRAYED);
	EnableMenuItem(m_hTreePUMenu, IDM_DELETE_GROUP, MF_BYCOMMAND | MF_GRAYED);
	EnableMenuItem(m_hTreePUMenu, IDM_SHOW_GROUP, MF_BYCOMMAND | MF_GRAYED);
	EnableMenuItem(m_hTreePUMenu, IDM_HIDE_GROUP, MF_BYCOMMAND | MF_GRAYED);
	SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_NEW_GROUP, TRUE);
	SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_NEW_SUBGROUP, FALSE);
	SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_EDIT_GROUP, FALSE);
	SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_DELETE_GROUP, FALSE);
	SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_SHOW_GROUP, FALSE);
	SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_HIDE_GROUP, FALSE);
}

static void EnableTreeMenus(HTREEITEM hItem){
	P_PNGROUP		ppg;
	BOOL			bEnabled, bMenu = FALSE;
	CPENUM			cp;
	TVITEMW			tvi;
	int				id, idRoot, idCurrent, idSelected;

	if(hItem == m_tHighlighted)
		bMenu = TRUE;

	idRoot = GetTVItemId(TreeView_GetRoot(m_hTree));
	idSelected = GetTVItemId(m_tSelected);
	// idHighlighted = GetTVItemId(m_tHighlighted);
	idCurrent = GetTVItemId(hItem);
	if(!hItem){
		bEnabled = FALSE;
	}
	else{
		ZeroMemory(&tvi, sizeof(tvi));
		tvi.hItem = hItem;
		tvi.mask = TVIF_HANDLE | TVIF_PARAM;
		SendMessageW(m_hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
		id = tvi.lParam;
		if(id >= 0)
			bEnabled = TRUE;
		else
			bEnabled = FALSE;
	}
	
	SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_NEW_GROUP, TRUE);
	EnableMenuItem(m_hTreePUMenu, IDM_NEW_GROUP, MF_BYCOMMAND | MF_ENABLED);

	if(bEnabled){
		if(bMenu){
			EnableMenuItem(m_hTreePUMenu, IDM_NEW_SUBGROUP, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(m_hTreePUMenu, IDM_EDIT_GROUP, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(m_hTreePUMenu, IDM_DELETE_GROUP, MF_BYCOMMAND | MF_ENABLED);
		}
		if(idSelected != idRoot){
			SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_NEW_SUBGROUP, TRUE);
			SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_EDIT_GROUP, TRUE);
			SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_DELETE_GROUP, TRUE);
		}
		ppg = GetGroup(id);
		if(ppg){
			cp.group = ppg->id;
			cp.result = 0;
			CheckGroupVisibility(&cp);
			if((cp.result & BOTH_VH) == BOTH_VH){
				if(bMenu){
					EnableMenuItem(m_hTreePUMenu, IDM_SHOW_GROUP, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(m_hTreePUMenu, IDM_HIDE_GROUP, MF_BYCOMMAND | MF_ENABLED);
				}
				if(idSelected != idRoot){
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_SHOW_GROUP, TRUE);
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_HIDE_GROUP, TRUE);
				}
			}
			else if((cp.result & VISIBLE_ONLY) == VISIBLE_ONLY){
				if(bMenu){
					EnableMenuItem(m_hTreePUMenu, IDM_SHOW_GROUP, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(m_hTreePUMenu, IDM_HIDE_GROUP, MF_BYCOMMAND | MF_ENABLED);
				}
				if(idSelected != idRoot){
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_SHOW_GROUP, FALSE);
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_HIDE_GROUP, TRUE);
				}
			}
			else if((cp.result & HIDDEN_ONLY) == HIDDEN_ONLY){
				if(bMenu){
					EnableMenuItem(m_hTreePUMenu, IDM_SHOW_GROUP, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(m_hTreePUMenu, IDM_HIDE_GROUP, MF_BYCOMMAND | MF_GRAYED);
				}
				if(idSelected != idRoot){
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_SHOW_GROUP, TRUE);
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_HIDE_GROUP, FALSE);
				}
			}
			else if(cp.result == 0){
				if(bMenu){
					EnableMenuItem(m_hTreePUMenu, IDM_SHOW_GROUP, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(m_hTreePUMenu, IDM_HIDE_GROUP, MF_BYCOMMAND | MF_GRAYED);
				}
				if(idSelected != idRoot){
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_SHOW_GROUP, FALSE);
					SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_HIDE_GROUP, FALSE);
				}
			}
		}
		
		//always disable delete for "General" - id = 0
		if(idCurrent == 0){
		// if(idHighlighted == 0){
			SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_DELETE_GROUP, FALSE);
			EnableMenuItem(m_hTreePUMenu, IDM_DELETE_GROUP, MF_BYCOMMAND | MF_GRAYED);
		}
	}
	else{
		if(bMenu){
			EnableMenuItem(m_hTreePUMenu, IDM_NEW_SUBGROUP, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(m_hTreePUMenu, IDM_EDIT_GROUP, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(m_hTreePUMenu, IDM_DELETE_GROUP, MF_BYCOMMAND | MF_GRAYED);
			// EnableMenuItem(m_hTreePUMenu, IDM_NOTE_TO_GROUP, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(m_hTreePUMenu, IDM_SHOW_GROUP, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(m_hTreePUMenu, IDM_HIDE_GROUP, MF_BYCOMMAND | MF_GRAYED);
		}
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_NEW_SUBGROUP, FALSE);
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_EDIT_GROUP, FALSE);
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_DELETE_GROUP, FALSE);
		// SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_NOTE_TO_GROUP, FALSE);
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_SHOW_GROUP, bEnabled);
		SendMessageW(m_hTbrTree, TB_ENABLEBUTTON, IDM_HIDE_GROUP, bEnabled);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: SendNotesMessage
 Created  : Fri May 25 22:26:00 2007
 Modified : Fri May 25 22:26:00 2007

 Synopsys : Sends WM_COMMAND message for each selected list view item
 Input    : hwnd - dialog handle
            id - message id
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static void SendNotesMessage(HWND hwnd, int id){

	LVITEMW		lvi;
	int			count = 0;
	LONG_PTR	* pNotes, * pTemp, * pFree;
	PMEMNOTE	pNote;

	//get selected items count
	count = SendMessageW(m_hList, LVM_GETSELECTEDCOUNT, 0, 0);
	//prepare lParams (PMEMNOTE) array - some items become unselected during message exchange, 
	//so we cannot send message from within this loop
	if(count > 0){
		pNotes = calloc(count, sizeof(PMEMNOTE));
		pFree = pTemp = pNotes;
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask = LVIF_PARAM;
		lvi.iItem = -1;
		while((lvi.iItem = SendMessageW(m_hList, LVM_GETNEXTITEM, lvi.iItem, LVNI_ALL | LVNI_SELECTED)) != -1){
			SendMessageW(m_hList, LVM_GETITEM, 0, (LPARAM)&lvi);
			*pNotes++ = lvi.lParam;
		}
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pTemp++;
			SendMessageW(pNote->hwnd, WM_COMMAND, MAKEWPARAM(id, 0), 0);
		}
		free(pFree);
	}
}

static LONG_PTR * SelectedPItems(INT_PTR * pCount){
	LVITEMW		lvi;
	LONG_PTR	* pNotes, * pTemp;

	//get selected items count
	*pCount = SendMessageW(m_hList, LVM_GETSELECTEDCOUNT, 0, 0);
	//prepare lParams (PMEMNOTE) array - some items become unselected during message exchange, 
	//so we cannot send message from within this loop
	if(*pCount > 0){
		pNotes = calloc(*pCount, sizeof(PMEMNOTE));
		pTemp = pNotes;
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask = LVIF_PARAM;
		lvi.iItem = -1;
		while((lvi.iItem = SendMessageW(m_hList, LVM_GETNEXTITEM, lvi.iItem, LVNI_ALL | LVNI_SELECTED)) != -1){
			SendMessageW(m_hList, LVM_GETITEM, 0, (LPARAM)&lvi);
			*pNotes++ = lvi.lParam;
		}
	}
	return pTemp;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: PrepareList
 Created  : Fri May 25 22:24:27 2007
 Modified : Fri May 25 22:24:27 2007

 Synopsys : Prepares and sets list view columns, image lists, toolbar image 
            lists and builds toolbar
 Input    : hDlg - dialog handle
            hList - list view handle
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static void PrepareList(HWND hDlg, HWND hList){
	
	HBITMAP			hBmp;
	LVCOLUMNW		lvc;
	int				width[] = {0, 0, 0, 0};

	//prepare and set list view image lists
	hBmp = LoadBitmapW(g_hInstance, MAKEINTRESOURCEW(IDB_CTRL_SMALL));
	m_hImlSmall = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 6, 6);
	ImageList_AddMasked(m_hImlSmall, hBmp, CLR_MASK);
	DeleteObject(hBmp);
	DeleteObject((void *)CLR_MASK);
	//set list view small image list
	ListView_SetImageList(hList, m_hImlSmall, LVSIL_SMALL);
	hBmp = LoadBitmapW(g_hInstance, MAKEINTRESOURCEW(IDB_CTRL_LARGE));
	m_hImlLarge = ImageList_Create(32, 32, ILC_COLOR24 | ILC_MASK, 6, 6);
	ImageList_AddMasked(m_hImlLarge, hBmp, CLR_MASK);
	DeleteObject(hBmp);
	DeleteObject((void *)CLR_MASK);
	//set listview normal image list
	ListView_SetImageList(hList, m_hImlLarge, LVSIL_NORMAL);
	SendMessageW(hDlg, CHM_CTRL_UPD, 0, 0);
	//create toolbar image lists and set toolbar
	DoToolbarW(GetDlgItem(hDlg, IDC_TBR_CTRL), m_hImlTbrNormal, m_hImlTbrGray, NELEMS(m_TBBtnArray), m_TBBtnArray);
	//get saved sort data
	GetPrivateProfileStructW(S_CP_DATA, IK_CP_SORT_DATA, &m_lvsm, sizeof(m_lvsm), g_NotePaths.INIFile);
	
	//get columns width
	GetPrivateProfileStructW(S_CP_DATA, IK_CP_COL_WIDTH, width, sizeof(width), g_NotePaths.INIFile);
	//insert list view columns
	ZeroMemory(&lvc, sizeof(lvc));
	lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH;
	lvc.cchTextMax = 256;
	lvc.cx = (width[0] == 0) ? 160 : width[0];
	lvc.iSubItem = COL_NAME;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_NAME, (LPARAM)&lvc);
	//priority
	lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH | LVCF_IMAGE;
	lvc.fmt = LVCFMT_CENTER | LVCFMT_IMAGE | LVCFMT_COL_HAS_IMAGES;
	lvc.cx = 32;
	lvc.iSubItem = COL_PRIORITY;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_PRIORITY, (LPARAM)&lvc);
	//completed
	lvc.iSubItem = COL_COMPLETED;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_COMPLETED, (LPARAM)&lvc);
	//protected
	lvc.iSubItem = COL_PROTECTED;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_COMPLETED, (LPARAM)&lvc);
	//group
	lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = (width[1] == 0) ? 120 : width[1];
	lvc.iSubItem = COL_GROUP;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_GROUP, (LPARAM)&lvc);
	//store group width
	m_GroupWidth = lvc.cx;
	//date
	lvc.cx = (width[2] == 0) ? 160 : width[2];
	lvc.iSubItem = COL_DATE;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_DATE, (LPARAM)&lvc);
	//schedule
	lvc.cx = (width[3] == 0) ? 160 : width[3];
	lvc.iSubItem = COL_SCHEDULE;
	SendMessageW(hList, LVM_INSERTCOLUMNW, COL_SCHEDULE, (LPARAM)&lvc);
}

static void ShowNotePreview(int selCount){
	if(selCount == 1){
		PMEMNOTE		pNote = SelectedNote();
		if(pNote->pRTHandles->hbSkin){
			COLORREF		color;
			
			OleTranslateColor(0x800000 | GetSysColor(COLOR_WINDOW), NULL, &color);
			SetPreviewBackgroundColor(FALSE, color);
		}
		else{
			SetPreviewBackgroundColor(FALSE, pNote->pAppearance->crWindow);
		}
		ReadRTFFile(pNote->pFlags->id, m_hEdit, NULL);
	}
	else{
		SetPreviewBackgroundColor(TRUE, 0);
		SetWindowTextW(m_hEdit, NULL);
	}
}

static void PrepareTree(HWND hwnd, HWND hTree){
	HBITMAP				hBmp;
	HTREEITEM			hItem;

	hBmp = LoadBitmapW(g_hInstance, MAKEINTRESOURCEW(IDB_GROUPS));
	m_hImlTreeNormal = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 42, 42);
	ImageList_AddMasked(m_hImlTreeNormal, hBmp, CLR_MASK);
	DeleteObject(hBmp);
	DeleteObject((void *)CLR_MASK);
	TreeView_SetImageList(hTree, m_hImlTreeNormal, TVSIL_NORMAL);
	//insert all items
	hItem = InsertTreeviewItems();
	//create toolbar image list and set toolbar
	DoToolbarW(m_hTbrTree, m_hImlTbrNormal, m_hImlTbrGray, NELEMS(m_TreeButtons), m_TreeButtons);
	//expand tree view
	TreeView_Expand(hTree, hItem, TVE_EXPAND);
}

static HTREEITEM InsertTreeviewItems(void){

	TVINSERTSTRUCTW		tvs;
	wchar_t 			szBuffer[128], szFile[MAX_PATH], szCount[16];
	HTREEITEM			hItem;
	int					count;

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);
	TreeView_DeleteAllItems(m_hTree);
	ZeroMemory(&tvs, sizeof(tvs));
	count = ImageList_GetImageCount(m_hImlTreeNormal);
	tvs.hParent = TVI_ROOT;
	tvs.hInsertAfter = TVI_FIRST;
	tvs.item.mask = TVIF_IMAGE | TVIF_PARAM | TVIF_TEXT | TVIF_SELECTEDIMAGE;
	tvs.item.lParam = -1;
	tvs.item.iImage = count - 1;
	tvs.item.iSelectedImage = count - 1;
	tvs.item.pszText = szBuffer;
	GetPrivateProfileStringW(S_CAPTIONS, L"group_main", DS_ALL_NOTES_GROUP, szBuffer, 128, szFile);
	_itow(CountNotesAlive(), szCount, 10);
	wcscat(szBuffer, L" (");
	wcscat(szBuffer, szCount);
	wcscat(szBuffer, L")");
	hItem = (HTREEITEM)SendMessageW(m_hTree, TVM_INSERTITEMW, 0, (LPARAM)&tvs);

	tvs.hInsertAfter = TVI_LAST;
	tvs.item.lParam = -2;
	tvs.item.iImage = count - 2;
	tvs.item.iSelectedImage = count - 2;
	GetPrivateProfileStringW(S_CAPTIONS, L"recycle_bin", L"Recycle Bin", szBuffer, 128, szFile);
	tvs.item.pszText = szBuffer;
	SendMessageW(m_hTree, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	UpdateGroupText(GROUP_RECYCLE, TreeView_GetNextSibling(m_hTree, TreeView_GetRoot(m_hTree)));
	ShowAllGroups(-1, hItem);
	return hItem;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: UpdateListView
 Created  : Fri May 25 22:19:15 2007
 Modified : Fri May 25 22:19:41 2007

 Synopsys : Clears and fills list view
 Input    : 
 Output   : 
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static void UpdateListView(void){
	CPENUM		cp;
	TVITEMW		tvi;
	wchar_t		szBuffer[128], szCaption[128], szTemp[128], szText[512], szTotal[128], szSubtotal[128], szFile[MAX_PATH];
	HDC			hdc;
	int			sbParts[3];
	RECT		rc;
	P_PNGROUP	ppg;

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);
	
	ZeroMemory(&tvi, sizeof(tvi));
	tvi.mask = TVIF_HANDLE | TVIF_PARAM;
	tvi.hItem = TreeView_GetSelection(m_hTree);
	SendMessageW(m_hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);

	switch(tvi.lParam){
		case -1:	//root
			GetPrivateProfileStringW(S_CAPTIONS, L"group_main", DS_ALL_NOTES_GROUP, szBuffer, 128, szFile);
			break;
		case -2:	//recycle
			GetPrivateProfileStringW(S_CAPTIONS, L"recycle_bin", L"Recycle Bin", szBuffer, 128, szFile);
			break;
		default:
			ppg = GetGroup(tvi.lParam);
			if(ppg){
				wcscpy(szBuffer, ppg->szName);
			}
			break;
	}
	
	//set dialog and controls text
	GetPrivateProfileStringW(S_OPTIONS, L"1004", DS_CTRL_PANEL, szCaption, 256, szFile);
	wcscat(szCaption, L" [");
	wcscat(szCaption, szBuffer);
	wcscat(szCaption, L"]");
	SetWindowTextW(m_hCtrlPanel, szCaption);

	GetPrivateProfileStringW(S_STATUS, L"group", DS_GROUP, szText, 512, szFile);
	wcscat(szText, L" ");
	wcscat(szText, szBuffer);
	if(tvi.lParam == GROUP_ROOT || tvi.lParam == GROUP_RECYCLE){
		if(tvi.lParam == GROUP_ROOT){
			GetPrivateProfileStringW(S_STATUS, L"dd_impossible", DS_DD_IMPOSSIBLE, szTemp, 128, szFile);
			wcscat(szText, L" ");
			wcscat(szText, szTemp);
		}
		//make group column visible
		SendMessageW(m_hList, LVM_SETCOLUMNWIDTH, COL_GROUP, m_GroupWidth);
	}
	else{
		//make group column unvisible
		SendMessageW(m_hList, LVM_SETCOLUMNWIDTH, COL_GROUP, 0);
	}

	GetPrivateProfileStringW(S_STATUS, L"group_notes", DS_NOTES_IN_GROUP, szSubtotal, 128, szFile);
	wcscat(szSubtotal, L" ");

	GetPrivateProfileStringW(S_STATUS, L"total", DS_TOTAL_NOTES, szTotal, 128, szFile);
	wcscat(szTotal, L" ");

	cp.reserved = (int)m_hList;
	cp.group = tvi.lParam;
	cp.result = 0;
	cp.total = 0;

	//set list view background color ('disable' for 'All notes')
	if(tvi.lParam == -1){
		ListView_SetBkColor(m_hList, GetSysColor(COLOR_BTNFACE));
		ListView_SetTextBkColor(m_hList, GetSysColor(COLOR_BTNFACE));
	}
	else{
		ListView_SetBkColor(m_hList, GetSysColor(COLOR_WINDOW));
		ListView_SetTextBkColor(m_hList, GetSysColor(COLOR_WINDOW));
	}

	LockWindowUpdate(m_hList);
	SendMessageW(m_hList, LVM_DELETEALLITEMS, 0, 0);
	
	if(tvi.lParam > GROUP_RECYCLE){
		EnableCommands(SEL_TREE_ORD);
	}
	else if(tvi.lParam == GROUP_RECYCLE){
		EnableCommands(SEL_TREE_REC);
	}

	InsertNotesIntoList(&cp);
	ShowNotePreview(ListView_GetSelectedCount(m_hList));
	LockWindowUpdate(NULL);

	// UpdateStatus(szBuffer, tvi.lParam);

	_itow(cp.result, szTemp, 10);
	wcscat(szSubtotal, szTemp);
	_itow(cp.total, szTemp, 10);
	wcscat(szTotal, szTemp);
	hdc = GetDC(m_hStbControl);
	SetRectEmpty(&rc);
	DrawTextW(hdc, szText, -1, &rc, DT_LEFT | DT_SINGLELINE | DT_CALCRECT);
	ReleaseDC(m_hStbControl, hdc);
	sbParts[0] = (rc.right - rc.left) + 4;
	sbParts[1] = sbParts[0] + 180;
	sbParts[2] = -1;
	SendMessageW(m_hStbControl, SB_SETPARTS, 3, (LPARAM)sbParts);
	SendMessageW(m_hStbControl, SB_SETTEXTW, 0 | SBT_POPOUT, (LPARAM)szText);
	SendMessageW(m_hStbControl, SB_SETTEXTW, 1 | SBT_POPOUT, (LPARAM)szSubtotal);
	SendMessageW(m_hStbControl, SB_SETTEXTW, 2 | SBT_POPOUT, (LPARAM)szTotal);
}

static void InsertNotesIntoList(P_CPENUM pcp){

	PMEMNOTE	pTemp = MemoryNotes();
	int			item;

	while(pTemp){
		pcp->total++;
		if(pcp->group != -1){
			//for group -1 show all notes
			if(pTemp->pData->idGroup != pcp->group){
				//do not show note from another group - continue enumeration
				goto _next;
			}
		}
		else{
			if(pTemp->pData->idGroup == GROUP_RECYCLE){
				//do not show deleted note - continue enumeration
				goto _next;
			}
		}
		//insert item into needed position
		item = SendMessageW((HWND)pcp->reserved, LVM_GETITEMCOUNT, 0, 0);
		InsertItem((HWND)pcp->reserved, pTemp, item);
		pcp->result++;
		_next:
		pTemp = pTemp->next;
	}	
	//restore the previous sort order
	switch(m_lvsm.column){
		case COL_NAME:
			SendMessageW(m_hList, LVM_SORTITEMSEX, (WPARAM)m_LVSort.iSortName, (LPARAM)NameCompareFunc);
			m_lvsm.order = m_LVSort.iSortName;
			break;
		case COL_PRIORITY:
			SendMessageW(m_hList, LVM_SORTITEMS, (WPARAM)m_LVSort.iSortPriority, (LPARAM)PriorityCompareFunc);
			m_lvsm.order = m_LVSort.iSortPriority;
			break;
		case COL_COMPLETED:
			SendMessageW(m_hList, LVM_SORTITEMS, (WPARAM)m_LVSort.iSortCompleted, (LPARAM)CompletedCompareFunc);
			m_lvsm.order = m_LVSort.iSortCompleted;
			break;
		case COL_PROTECTED:
			SendMessageW(m_hList, LVM_SORTITEMS, (WPARAM)m_LVSort.iSortProtected, (LPARAM)ProtectedCompareFunc);
			m_lvsm.order = m_LVSort.iSortProtected;
			break;
		case COL_GROUP:
			SendMessageW(m_hList, LVM_SORTITEMSEX, (WPARAM)m_LVSort.iSortGroup, (LPARAM)NameCompareFunc);
			m_lvsm.order = m_LVSort.iSortGroup;
			break;
		case COL_DATE:
			SendMessageW(m_hList, LVM_SORTITEMS, (WPARAM)m_LVSort.iSortDate, (LPARAM)DateCompareFunc);
			m_lvsm.order = m_LVSort.iSortDate;
			break;
		case COL_SCHEDULE:
			SendMessageW(m_hList, LVM_SORTITEMSEX, (WPARAM)m_LVSort.iSortSchedule, (LPARAM)NameCompareFunc);
			m_lvsm.order = m_LVSort.iSortSchedule;
			break;
	}
}

static void UpdateControlStatus(void){
	wchar_t		szTemp[128], szTotal[128], szSubtotal[128], szFile[MAX_PATH];

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);

	GetPrivateProfileStringW(S_STATUS, L"group_notes", DS_NOTES_IN_GROUP, szSubtotal, 128, szFile);
	wcscat(szSubtotal, L" ");
	GetPrivateProfileStringW(S_STATUS, L"total", DS_TOTAL_NOTES, szTotal, 128, szFile);
	wcscat(szTotal, L" ");
	_itow(NotesCount(), szTemp, 10);
	wcscat(szTotal, szTemp);
	_itow(SendMessageW(m_hList, LVM_GETITEMCOUNT, 0, 0), szTemp, 10);
	wcscat(szSubtotal, szTemp);
	SendMessageW(m_hStbControl, SB_SETTEXTW, 1 | SBT_POPOUT, (LPARAM)szSubtotal);
	SendMessageW(m_hStbControl, SB_SETTEXTW, 2 | SBT_POPOUT, (LPARAM)szTotal);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ApplyControlPanelLanguage
 Created  : Fri May 25 22:15:18 2007
 Modified : Fri May 25 22:15:18 2007

 Synopsys : Sets dialog caption prepares and set text for popup menus
 Input    : hwnd - dialog handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static void ApplyControlPanelLanguage(HWND hwnd){

	wchar_t 		szBuffer[256], szFile[MAX_PATH], szCount[16];
	LVCOLUMNW		lvc;
	// int				size;
	TVITEMW			tvi;

	//prepare popup menu
	if(m_hCtrlPUMenu){
		FreeMenus(m_hCtrlPUMenu);
	}
	if(m_hCtrlMenu){
		DestroyMenu(m_hCtrlMenu);
	}
	m_hCtrlMenu = LoadMenuW(g_hInstance, MAKEINTRESOURCEW(IDR_CTRL_PANEL));
	m_hCtrlPUMenu = GetSubMenu(m_hCtrlMenu, 0);
	
	//prepare treeview popup menu
	if(m_hTreePUMenu){
		FreeMenus(m_hTreePUMenu);
	}
	if(m_hTreeMenu){
		DestroyMenu(m_hTreeMenu);
	}
	m_hTreeMenu = LoadMenuW(g_hInstance, MAKEINTRESOURCEW(IDR_TREE_MENU));
	m_hTreePUMenu = GetSubMenu(m_hTreeMenu, 0);

	//prepare recycle bin popup menu
	if(m_hRecyclePUMenu){
		FreeMenus(m_hRecyclePUMenu);
	}
	if(m_hRecycleMenu){
		DestroyMenu(m_hRecycleMenu);
	}
	m_hRecycleMenu = LoadMenuW(g_hInstance, MAKEINTRESOURCEW(IDR_RECYCLE_MENU));
	m_hRecyclePUMenu = GetSubMenu(m_hRecycleMenu, 0);

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);

	//set list view columns text
	ZeroMemory(&lvc, sizeof(lvc));
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	lvc.cchTextMax = 256;
	lvc.pszText = szBuffer;
	//name column
	SendMessageW(m_hList, LVM_GETCOLUMNW, COL_NAME, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, __T("0"), __T("Note Name"), szBuffer, 256, szFile);
	SendMessageW(m_hList, LVM_SETCOLUMNW, COL_NAME, (LPARAM)&lvc);
	//group column
	SendMessageW(m_hList, LVM_GETCOLUMNW, COL_GROUP, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, __T("1"), __T("Group"), szBuffer, 256, szFile);
	SendMessageW(m_hList, LVM_SETCOLUMNW, COL_GROUP, (LPARAM)&lvc);
	//date column
	SendMessageW(m_hList, LVM_GETCOLUMNW, COL_DATE, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, __T("2"), __T("Last Saved"), szBuffer, 256, szFile);
	SendMessageW(m_hList, LVM_SETCOLUMNW, COL_DATE, (LPARAM)&lvc);
	//schedule column
	SendMessageW(m_hList, LVM_GETCOLUMNW, COL_SCHEDULE, (LPARAM)&lvc);
	GetPrivateProfileStringW(S_COLUMNS, __T("3"), __T("Schedule Type"), szBuffer, 256, szFile);
	SendMessageW(m_hList, LVM_SETCOLUMNW, COL_SCHEDULE, (LPARAM)&lvc);
	lvc.mask = LVCF_IMAGE;
	//priority column
	SendMessageW(m_hList, LVM_GETCOLUMNW, COL_PRIORITY, (LPARAM)&lvc);
	lvc.iImage = 17;
	//wcscpy(szBuffer, L"!");
	SendMessageW(m_hList, LVM_SETCOLUMNW, COL_PRIORITY, (LPARAM)&lvc);
	//completed column
	SendMessageW(m_hList, LVM_GETCOLUMNW, COL_COMPLETED, (LPARAM)&lvc);
	lvc.iImage = 18;
	//wcscpy(szBuffer, L"V");
	SendMessageW(m_hList, LVM_SETCOLUMNW, COL_COMPLETED, (LPARAM)&lvc);
	//protected column
	SendMessageW(m_hList, LVM_GETCOLUMNW, COL_PROTECTED, (LPARAM)&lvc);
	lvc.iImage = 19;
	//wcscpy(szBuffer, L"P");
	SendMessageW(m_hList, LVM_SETCOLUMNW, COL_PROTECTED, (LPARAM)&lvc);

	if(FileExists(g_NotePaths.LangDir, g_NotePaths.LangFile)){
		//set menu items text
		PrepareMenuRecursive(m_CtrlMenus, NELEMS(m_CtrlMenus), m_hCtrlPUMenu, szFile, S_MENU);
		PrepareMenuRecursive(m_TreeMenu, NELEMS(m_TreeMenu), m_hTreePUMenu, szFile, S_MENU);
		PrepareMenuRecursive(m_RecycleMenu, NELEMS(m_RecycleMenu), m_hRecyclePUMenu, szFile, S_MENU);
	}
	//initially disable some menus
	EnableMenus(MF_GRAYED, FALSE);
	//set root treeview item text
	ZeroMemory(&tvi, sizeof(tvi));	
	tvi.hItem = TreeView_GetRoot(m_hTree);
	tvi.cchTextMax = 128;
	tvi.pszText = szBuffer;
	tvi.mask = TVIF_HANDLE | TVIF_IMAGE	| TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	SendMessageW(m_hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
	GetPrivateProfileStringW(S_CAPTIONS, L"group_main", DS_ALL_NOTES_GROUP, szBuffer, 128, szFile);
	_itow(CountNotesAlive(), szCount, 10);
	wcscat(szBuffer, L" (");
	wcscat(szBuffer, szCount);
	wcscat(szBuffer, L")");
	SendMessageW(m_hTree, TVM_SETITEMW, 0, (LPARAM)&tvi);
	//for status bar
	UpdateListView();
}

/*-@@+@@------------------------------------------------------------------
 Procedure: DeleteItem
 Created  : Fri May 25 22:10:51 2007
 Modified : Fri May 25 22:10:51 2007

 Synopsys : Removes list view item (by it's lParam member)
 Input    : hList - list view handle
            lParam - needed lParam memeber
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static void DeleteItem(HWND hList, LPARAM lParam){

	LVFINDINFOW		lvf;
	int				index;

	lvf.flags = LVFI_PARAM;
	lvf.lParam = lParam;
	index = SendMessageW(hList, LVM_FINDITEMW, -1, (LPARAM)&lvf);
	if(index != -1){
		SendMessageW(hList, LVM_DELETEITEM, index, 0);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ReplaceItem
 Created  : Fri May 25 22:09:23 2007
 Modified : Fri May 25 22:09:23 2007

 Synopsys : Replaces list view item. There was a problem in updating items 
            (Unicode string were not displayed correctly) on some 
            computers, so I should just insert new item and remove old one.
 Input    : hList - list view handle
            hNote - note window handle
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static void ReplaceItem(HWND hList, PMEMNOTE pNote){

	LVFINDINFOW		lvf;
	int				index;

	lvf.flags = LVFI_PARAM;
	lvf.lParam = (LONG)pNote;
	
	index = SendMessageW(hList, LVM_FINDITEMW, -1, (LPARAM)&lvf);
	if(index != -1){
		InsertItem(hList, pNote, index);
		SendMessageW(hList, LVM_DELETEITEM, index + 1, 0);
	}
}

static void UpdateNoteItem(PMEMNOTE pNote){
	int			count, index;
	LVITEMW		lvi;
	BOOL		found = FALSE;

	count = ListView_GetItemCount(m_hList);
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_PARAM;
	for(int i = 0; i < count; i++){
		lvi.iItem = i;
		index = i;
		SendMessageW(m_hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
		if((PMEMNOTE)lvi.lParam == pNote){
			found = TRUE;
			break;
		}
	}
	if(!found){
		return;
	}

	//update the name column
	lvi.mask = LVIF_IMAGE | LVIF_TEXT;
	lvi.iImage = ItemImageIndex(pNote);
	lvi.pszText = pNote->pData->szName;
	lvi.iSubItem = COL_NAME;
	SendMessageW(m_hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
	//set item priority
	SetItemPriority(pNote, index);
	//set item completed
	SetItemCompleted(pNote, index);
	//set item protected
	SetItemProtected(pNote, index);
	//set item group name
	SetItemGroupName(pNote, index);
	//set item change date
	if(pNote->pData->stChanged.wDay != 0){
		SetItemChangeDate(pNote, index);
	}
	//set item schedule description
	SetItemScheduleDescription(pNote, index);
	//ensure note visible
	SendMessageW(m_hList, LVM_ENSUREVISIBLE, index, FALSE);
	ShowNotePreview(ListView_GetSelectedCount(m_hList));
}

static void SetItemGroupName(PMEMNOTE pNote, int index){

	wchar_t 			szBuffer[256], szFile[MAX_PATH];
	int					treeGroup;
	LVITEMW				lvi;
	P_PNGROUP			pg;

	treeGroup = GetTVItemId(TreeView_GetSelection(m_hTree));
	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);

	szBuffer[0] = '\0';
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.pszText = szBuffer;
	lvi.iItem = index;
	lvi.iSubItem = COL_GROUP;

	if(pNote->pData->idGroup > GROUP_RECYCLE){
		//print group name
		pg = GetGroup(pNote->pData->idGroup);
		if(pg){
			wcscpy(szBuffer, pg->szName);
		}
	}
	else{
		if(treeGroup > GROUP_RECYCLE)
			//print "Recycle Bin" for total group
			GetPrivateProfileStringW(S_CAPTIONS, L"recycle_bin", L"Recycle Bin", szBuffer, 128, szFile);
		else{
			//print previous group name
			pg = GetGroup(pNote->pData->idPrevGroup);
			if(pg)
				wcscpy(szBuffer, pg->szName);
		}
	}
	SendMessageW(m_hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemChangeDate(PMEMNOTE pNote, int index){
	wchar_t 			szDate[128];
	LVITEMW				lvi;

	//get changed time
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.pszText = szDate;
	lvi.iItem = index;
	lvi.iSubItem = COL_DATE;
	ConstructDateTimeString(&pNote->pData->stChanged, szDate, FALSE);
	SendMessageW(m_hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemPriority(PMEMNOTE pNote, int index){
	LVITEMW				lvi;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_IMAGE;
	lvi.iItem = index;
	lvi.iSubItem = COL_PRIORITY;
	if(IsBitOn(pNote->pData->res1, NB_HIGH_PRIORITY))
		lvi.iImage = 17;
	else
		lvi.iImage = -1;
	SendMessageW(m_hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemCompleted(PMEMNOTE pNote, int index){
	LVITEMW				lvi;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_IMAGE;
	lvi.iItem = index;
	lvi.iSubItem = COL_COMPLETED;
	if(IsBitOn(pNote->pData->res1, NB_COMPLETED))
		lvi.iImage = 18;
	else
		lvi.iImage = -1;
	SendMessageW(m_hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemProtected(PMEMNOTE pNote, int index){
	LVITEMW				lvi;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_IMAGE;
	lvi.iItem = index;
	lvi.iSubItem = COL_PROTECTED;
	if(IsBitOn(pNote->pData->res1, NB_PROTECTED))
		lvi.iImage = 19;
	else
		lvi.iImage = -1;
	SendMessageW(m_hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

static void SetItemScheduleDescription(PMEMNOTE pNote, int index){
	wchar_t 			szBuffer[256], szDate[128], szFile[MAX_PATH];
	LVITEMW				lvi;

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);
	szBuffer[0] = '\0';
	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.iItem = index;
	lvi.iSubItem = COL_SCHEDULE;
	lvi.pszText = szBuffer;
	switch(pNote->pSchedule->scType){
		case SCH_NO:
			SendMessageW(m_hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
			return;
		case SCH_ED:
			GetScheduleDescription(IDC_OPT_EV_DAY, szBuffer, szFile, L"Every day at:");
			break;
		case SCH_ONCE:
			GetScheduleDescription(IDC_OPT_ONCE, szBuffer, szFile, L"Once at:");
			break;
		case SCH_REP:
			GetScheduleDescription(IDC_OPT_REPEAT, szBuffer, szFile, L"Repeat every:");
			break;
		case SCH_PER_WD:
			GetScheduleDescription(IDC_OPT_EVERY_WD, szBuffer, szFile, L"Weekly on:");
			break;
		case SCH_PER_YR:
			GetScheduleDescription(IDC_OPT_EVERY_YR, szBuffer, szFile, L"Yearly on:");
		case SCH_AFTER:
			GetScheduleDescription(IDC_OPT_AFTER, szBuffer, szFile, L"After:");
			break;
	}
	ParseScheduleType(pNote->pSchedule, szDate);
	wcscat(szBuffer, L" ");
	wcscat(szBuffer, szDate);
	SendMessageW(m_hList, LVM_SETITEMW, 0, (LPARAM)&lvi);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: InsertItem
 Created  : Fri May 25 21:58:50 2007
 Modified : Fri May 25 21:58:50 2007

 Synopsys : Inserts new item into list view
 Input    : hList - list view handle
            hNote - note window handle
            index - index to insert item at
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static void InsertItem(HWND hList, PMEMNOTE pNote, int index){

	LVITEMW				lvi;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT;
	
	lvi.lParam = (LONG)pNote;
	lvi.iItem = index;
	lvi.pszText = pNote->pData->szName;
	lvi.iImage = ItemImageIndex(pNote);
	//insert item
	SendMessageW(hList, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
	//set item priority
	SetItemPriority(pNote, index);
	//set item completed
	SetItemCompleted(pNote, index);
	//set item protected
	SetItemProtected(pNote, index);
	//set item group name
	SetItemGroupName(pNote, index);
	//set item change date
	if(pNote->pData->stChanged.wDay != 0){
		SetItemChangeDate(pNote, index);
	}
	//set item schedule description
	if(pNote->pSchedule->scType != SCH_NO){
		SetItemScheduleDescription(pNote, index);
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: DateCompareFunc
 Created  : Fri May 25 21:56:54 2007
 Modified : Fri May 25 21:56:54 2007

 Synopsys : Callback function for Date list view column sorting
 Input    : lParam1 - first item lParam member
            lParam2 - first item lParam member
            lParamSort - sort order
 Output   : A negative value if the first item should precede the second, a 
            positive value if the first item should follow the second, or 
            zero if the two items are equivalent
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static int CALLBACK DateCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){

	FILETIME		ft1, ft2;
	P_NOTE_DATA		pD1, pD2;

	pD1 = ((PMEMNOTE)lParam1)->pData;
	pD2 = ((PMEMNOTE)lParam2)->pData;
	SystemTimeToFileTime(&pD1->stChanged, &ft1);
	SystemTimeToFileTime(&pD2->stChanged, &ft2);
	if((int)lParamSort == LVS_SORTASCENDING)
		return CompareFileTime(&ft1, &ft2);
	else
		return CompareFileTime(&ft2, &ft1);
}

static int CALLBACK PriorityCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){
	P_NOTE_DATA		pD1, pD2;
	int				r1, r2;

	pD1 = ((PMEMNOTE)lParam1)->pData;
	pD2 = ((PMEMNOTE)lParam2)->pData;

	r1 = GetBit(pD1->res1, NB_HIGH_PRIORITY);
	r2 = GetBit(pD2->res1, NB_HIGH_PRIORITY);
	if((int)lParamSort == LVS_SORTASCENDING)
		return (r1 - r2);
	else
		return (r2 - r1);
}

static int CALLBACK ProtectedCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){
	P_NOTE_DATA		pD1, pD2;
	int				r1, r2;

	pD1 = ((PMEMNOTE)lParam1)->pData;
	pD2 = ((PMEMNOTE)lParam2)->pData;

	r1 = GetBit(pD1->res1, NB_PROTECTED);
	r2 = GetBit(pD2->res1, NB_PROTECTED);
	if((int)lParamSort == LVS_SORTASCENDING)
		return (r1 - r2);
	else
		return (r2 - r1);
}

static int CALLBACK CompletedCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){
	P_NOTE_DATA		pD1, pD2;
	int				r1, r2;

	pD1 = ((PMEMNOTE)lParam1)->pData;
	pD2 = ((PMEMNOTE)lParam2)->pData;

	r1 = GetBit(pD1->res1, NB_COMPLETED);
	r2 = GetBit(pD2->res1, NB_COMPLETED);
	if((int)lParamSort == LVS_SORTASCENDING)
		return (r1 - r2);
	else
		return (r2 - r1);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: NameCompareFunc
 Created  : Fri May 25 21:55:16 2007
 Modified : Fri May 25 21:55:16 2007

 Synopsys : Callback function for Name list view column sorting
 Input    : lParam1 - index of first item
            lParam2 - index of second item
            lParamSort - sort order
 Output   : A negative value if the first item should precede the second, a 
            positive value if the first item should follow the second, or 
            zero if the two items are equivalent
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static int CALLBACK NameCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort){

	wchar_t		szName1[128], szName2[128];
	LVITEMW		lvi;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_TEXT;
	lvi.cchTextMax = 128;
	lvi.pszText = szName1;
	lvi.iSubItem = m_lvsm.column;
	SendMessageW(m_hList, LVM_GETITEMTEXTW, (WPARAM)lParam1, (LPARAM)&lvi);
	lvi.pszText = szName2;
	SendMessageW(m_hList, LVM_GETITEMTEXTW, (WPARAM)lParam2, (LPARAM)&lvi);
	if((int)lParamSort == LVS_SORTASCENDING)
		return _wcsicmp(szName1, szName2);
	else
		return _wcsicmp(szName2, szName1);
}

static BOOL CALLBACK NewGroup_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_COMMAND, NewGroup_OnCommand);
	HANDLE_MSG (hwnd, WM_INITDIALOG, NewGroup_OnInitDialog);
	HANDLE_MSG (hwnd, WM_DESTROY, NewGroup_OnDestroy);

	default: return FALSE;
	}
}

static void NewGroup_OnDestroy(HWND hwnd)
{
	P_PNGROUP		ppg;

	ppg = (P_PNGROUP)GetPropW(hwnd, PROP_GRP);
	if(ppg){
		free(ppg);
	}
	RemovePropW(hwnd, PROP_GRP);
}

static void NewGroup_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
	case IDCANCEL:
		EndDialog(hwnd, id);
		break;
	case IDOK:{
		P_PNGROUP	ppg = (P_PNGROUP)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
		P_PNGROUP	temp = (P_PNGROUP)GetPropW(hwnd, PROP_GRP);
		ppg->image = temp->image;
		GetDlgItemTextW(hwnd, IDC_EDT_GROUP_NAME, ppg->szName, 128);
		EndDialog(hwnd, id);
		break;
	}
	case IDC_EDT_GROUP_NAME:{
		if(codeNotify == EN_CHANGE){
			int len = GetWindowTextLengthW(GetDlgItem(hwnd, IDC_EDT_GROUP_NAME));
			if(len > 0)
				EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);
			else
				EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
		}
		break;
	}
	case IDC_CMD_GROUP_BMP:
		if(codeNotify == BN_CLICKED){
			P_PNGROUP	ppg = (P_PNGROUP)GetPropW(hwnd, PROP_GRP);
			int 		result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_CHOOSE_BMP), hwnd, Bitmaps_DlgProc, ppg->image);
			if(result > -1){
				SendDlgItemMessageW(hwnd, IDC_ST_GROUP_BMP, STM_SETIMAGE, IMAGE_ICON, (LPARAM)ImageList_GetIcon(m_hImlTreeNormal, result, ILD_TRANSPARENT));
				ppg->image = result;
			}
		}
		break;
	}
}

static BOOL NewGroup_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	P_PNGROUP		ppg, temp;
	int				style;
	wchar_t 		szBuffer[256], szFile[MAX_PATH];

	ppg = (P_PNGROUP)lParam;
	SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)ppg);
	temp = calloc(1, sizeof(PNGROUP));
	memcpy(temp, ppg, sizeof(PNGROUP));
	SetPropW(hwnd, PROP_GRP, (HANDLE)temp);

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);

	if(ppg->id == NEW_GROUP_ID)
		GetPrivateProfileStringW(S_CAPTIONS, L"group_new", DS_GROUP_CAPTION_NEW, szBuffer, 128, szFile);
	else{
		GetPrivateProfileStringW(S_CAPTIONS, L"group_edit", DS_GROUP_CAPTION_EDIT, szBuffer, 128, szFile);
		SetDlgItemTextW(hwnd, IDC_EDT_GROUP_NAME, ppg->szName);
	}
	SetWindowTextW(hwnd, szBuffer);

	SetDlgCtlText(hwnd, IDC_ST_GROUP_NAME, szFile, DS_CHOOSE_GROUP_NAME);
	SetDlgCtlText(hwnd, IDC_CMD_GROUP_BMP, szFile, DS_CHOOSE_GROUP_ICON);
	SetDlgCtlText(hwnd, IDOK, szFile, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, szFile, DS_CANCEL);

	style = GetWindowLongPtrW(GetDlgItem(hwnd, IDC_ST_GROUP_BMP), GWL_STYLE);
	style |= SS_ICON;
	SetWindowLongPtrW(GetDlgItem(hwnd, IDC_ST_GROUP_BMP), GWL_STYLE, style);
	SendDlgItemMessageW(hwnd, IDC_ST_GROUP_BMP, STM_SETIMAGE, IMAGE_ICON, (LPARAM)ImageList_GetIcon(m_hImlTreeNormal, ppg->image, ILD_TRANSPARENT));

	SetFocus(GetDlgItem(hwnd, IDC_EDT_GROUP_NAME));
	SendDlgItemMessageW(hwnd, IDC_EDT_GROUP_NAME, EM_SETSEL, 0, -1);
	return FALSE;
}

static BOOL CALLBACK Bitmaps_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR			lpnm;

	switch(msg)
	{
	HANDLE_MSG (hwnd, WM_COMMAND, Bitmaps_OnCommand);
	HANDLE_MSG (hwnd, WM_INITDIALOG, Bitmaps_OnInitDialog);

	case WM_NOTIFY:
    	lpnm = (LPNMHDR)lParam;
		switch(lpnm->code){
		case NM_DBLCLK:
			if(lpnm->idFrom == IDC_LST_CHOOSE_BMP)
				SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0);
			return TRUE;
		default:
			return FALSE;
		}
	default: return FALSE;
	}
}

static void Bitmaps_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id){
	case IDCANCEL:
		EndDialog(hwnd, -1);
		break;
	case IDOK:{
		int index = ListView_GetNextItem(GetDlgItem(hwnd, IDC_LST_CHOOSE_BMP), -1, LVNI_SELECTED);
		EndDialog(hwnd, index);
		break;
	}
	}
}

static BOOL Bitmaps_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	LV_ITEMW	lvi;
	int 		count;
	wchar_t		szKey[12];
	HBITMAP		hBmp;
	HIMAGELIST	iml;
	wchar_t 	szBuffer[256], szFile[MAX_PATH];
	RECT		rc, rc1;
	HWND		hList;

	hList = GetDlgItem(hwnd, IDC_LST_CHOOSE_BMP);

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);
	GetPrivateProfileStringW(S_OPTIONS, L"1014", DS_PICK_ICON, szBuffer, 128, szFile);
	SetWindowTextW(hwnd, szBuffer);
	SetDlgCtlText(hwnd, IDOK, szFile, DS_OK);
	SetDlgCtlText(hwnd, IDCANCEL, szFile, DS_CANCEL);

	hBmp = LoadBitmapW(g_hInstance, MAKEINTRESOURCEW(IDB_GROUPS));
	iml = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK, 42, 42);
	ImageList_AddMasked(iml, hBmp, CLR_MASK);
	DeleteObject(hBmp);
	DeleteObject((void *)CLR_MASK);
	count = ImageList_GetImageCount(iml);
	ListView_SetImageList(hList, iml, LVSIL_SMALL);
	
	for(int i = 0; i < count - 2; i++){		//except recycle bin and all groups images
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
		lvi.iImage = i;
		lvi.iItem = i;
		lvi.lParam = i + 1;
		if(i == lParam){
			lvi.state = LVIS_SELECTED | LVIS_FOCUSED;
			lvi.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
		}
		_itow(i + 1, szKey, 10);
		lvi.pszText = szKey;
		SendMessageW(hList, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
	}
	
	GetWindowRect(hList, &rc);
	GetClientRect(hwnd, &rc1);
	MoveWindow(hList, 0, 0, rc1.right - rc1.left, rc.bottom - rc.top, TRUE);
	SetFocus(hList);
	return FALSE;
}

static void ShowAllGroups(int iParent, HTREEITEM hItem){
	P_PNGROUP		pAllGroups = GetGroups();
	P_PNGROUP		pTemp = pAllGroups;
	HTREEITEM		hInserted;

	while(pTemp){
		if(pTemp->parent == iParent){
			hInserted = InsertGroup(hItem, pTemp, TRUE, FALSE);
			ShowAllGroups(pTemp->id, hInserted);
		}
		pTemp = pTemp->next;
	}
}

static HTREEITEM InsertGroup(HTREEITEM hItem, P_PNGROUP ppg, BOOL fSetParent, BOOL fSave){
	TV_INSERTSTRUCTW	tvs;
	wchar_t				szBuffer[128], szCount[16];
	HTREEITEM			hInserted = NULL;
	int					count;

	if(fSetParent){
		TVITEMW	tvi;
		ZeroMemory(&tvi, sizeof(tvi));
		tvi.mask = TVIF_HANDLE | TVIF_PARAM;
		tvi.hItem = hItem;
		SendMessageW(m_hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
		ppg->parent = tvi.lParam;
	}

	ZeroMemory(&tvs, sizeof(tvs));
	tvs.hParent = hItem;
	tvs.hInsertAfter = TVI_SORT;
	tvs.item.mask = TVIF_IMAGE | TVIF_PARAM | TVIF_TEXT | TVIF_SELECTEDIMAGE;
	if(fSave){
		ppg->id = NewGroupId();
		AddGroup(ppg);
		SaveGroup(ppg);
	}
	tvs.item.lParam = ppg->id;
	tvs.item.iImage = ppg->image;
	tvs.item.iSelectedImage = ppg->image;
	wcscpy(szBuffer, ppg->szName);
	count = CountNotesInGroup(ppg->id);
	_itow(count, szCount, 10);
	wcscat(szBuffer, L" (");
	wcscat(szBuffer, szCount);
	wcscat(szBuffer, L")");
	tvs.item.pszText = szBuffer;
	
	hInserted = (HTREEITEM)SendMessageW(m_hTree, TVM_INSERTITEMW, 0, (LPARAM)&tvs);
	SendMessageW(m_hTree, TVM_EXPAND, TVE_EXPAND, (LPARAM)hItem);
	
	return hInserted;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: FindItemById
 Created  : Thu Mar  6 08:51:37 2008
 Modified : Thu Mar  6 08:51:37 2008

 Synopsys : Recursively searches for tree view item by group id
 Input    : hItem - tree view item to start from
            id - group id
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void FindItemById(HTREEITEM hItem, int id){
	HTREEITEM	hi;
	TVITEMW		tvi;

	ZeroMemory(&tvi, sizeof(tvi));
	tvi.mask = TVIF_HANDLE | TVIF_PARAM;
	tvi.hItem = hItem;
	SendMessageW(m_hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
	if(tvi.lParam == id){
		m_tItem = hItem;
		return;
	}
	hi = TreeView_GetChild(m_hTree, hItem);
	if(hi){
		FindItemById(hi, id);
	}
	hi = TreeView_GetNextSibling(m_hTree, hItem);
	if(hi){
		FindItemById(hi, id);
	}
}

static int GetSelectedTVItemId(void){
	HTREEITEM	hi;

	hi = TreeView_GetSelection(m_hTree);
	if(hi){
		return GetTVItemId(hi);
	}
	return -1;
}

static void ShowGroup(HTREEITEM hItem){
	int 		group = GetTVItemId(hItem);
	ShowGroupOfNotes(group);
}

static void HideGroup(HTREEITEM hItem){
	int 		group = GetTVItemId(hItem);
	HideGroupOfNotes(group);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: DeleteTreeGroup
 Created  : Thu Mar  6 08:45:58 2008
 Modified : Thu Mar  6 08:45:58 2008

 Synopsys : Recursively deletes group
 Input    : hItem - tree view item to delete
            fGetSibling - whether sibling of hItem should be deleted as 
            well
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void DeleteTreeGroup(HTREEITEM hItem, BOOL fGetSibling){
	HTREEITEM	hi;
	int			id;

	hi = TreeView_GetChild(m_hTree, hItem);
	if(hi){
		//continue until there is no child
		DeleteTreeGroup(hi, TRUE);
	}
	if(fGetSibling){
		//delete siblings
		hi = TreeView_GetNextSibling(m_hTree, hItem);
		if(hi){
			DeleteTreeGroup(hi, fGetSibling);
		}
	}
	id = GetTVItemId(hItem);
	if(id >= 0){
		//delete group
		DeleteGroup(id);
		//delete all notes belong to group
		DeleteNotesGroup(id);
		//delete tree view item
		TreeView_DeleteItem(m_hTree, hItem);
	}
}

static void DeleteNotesGroup(int group){
	PMEMNOTE	pNote = MemoryNotes(), pNext;

	while(pNote){
		pNext = pNote->next;
		if(pNote->pData->idGroup == group){
			DeleteToBin(pNote, 0);
		}
		pNote = pNext;
	}
}

/*-@@+@@------------------------------------------------------------------
 Procedure: GetTVItemId
 Created  : Thu Mar  6 08:43:50 2008
 Modified : Thu Mar  6 08:43:50 2008

 Synopsys : Gets group id of tree view item
 Input    : hItem - tree view item
 Output   : Group id of tree view item
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static int GetTVItemId(HTREEITEM hItem){
	TVITEMW		tvi;

	ZeroMemory(&tvi, sizeof(tvi));
	tvi.mask = TVIF_HANDLE | TVIF_PARAM;
	tvi.hItem = hItem;
	if(SendMessageW(m_hTree, TVM_GETITEMW, 0, (LPARAM)&tvi)){
		return tvi.lParam;
	}
	return -1;
}

/*-@@+@@------------------------------------------------------------------
 Procedure: ShowTreePopUp
 Created  : Wed Feb 13 08:25:33 2008
 Modified : Wed Wed Feb 13 08:25:33 2008

 Synopsys : Shows tree view popup menu
 Input    : Nothing
 Output   : Nothing
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/
static void ShowTreePopUp(void){
	POINT pt;

	GetCursorPos(&pt);
	//set window foreground for further PostMessageW call 
	SetForegroundWindow(m_hDTree);
	//show popup menu
	TrackPopupMenu(m_hTreePUMenu, TPM_RIGHTALIGN, pt.x, pt.y, 0, m_hDTree, NULL);
	//remove popup menu when user clicks outside it
	PostMessageW(m_hDTree, WM_NULL, 0, 0);
}

static void ShowRecyclePopUp(void){
	POINT pt;

	GetCursorPos(&pt);
	//set window foreground for further PostMessageW call 
	SetForegroundWindow(m_hCtrlPanel);
	//show popup menu
	TrackPopupMenu(m_hRecyclePUMenu, TPM_RIGHTALIGN, pt.x, pt.y, 0, m_hCtrlPanel, NULL);
	//remove popup menu when user clicks outside it
	PostMessageW(m_hCtrlPanel, WM_NULL, 0, 0);
}

static HTREEITEM GetDropHighLightedItem(int x, int y){
	TVHITTESTINFO		tvh;
	RECT				rc;

	GetWindowRect(m_hTree, &rc);
	MapWindowPoints(HWND_DESKTOP, m_hCtrlPanel, (LPPOINT)&rc, 2);
	tvh.flags = TVHT_ONITEMICON | TVHT_ONITEMLABEL;
	tvh.hItem = NULL;
	tvh.pt.x = x;
	tvh.pt.y = y - rc.top;
	
	return TreeView_HitTest(m_hTree, &tvh);
}

/*-@@+@@------------------------------------------------------------------
 Procedure: IsItemFromChildBranch
 Created  : Thu Mar  6 08:40:28 2008
 Modified : Thu Mar  6 08:40:28 2008

 Synopsys : Checks whether dragged tree view item belongs to one of child 
            branches of highlighted tree view item
 Input    : hDragged - dragged item
            hHighlighted - highlighted item
 Output   : TRUE/FALSE
 Errors   : 
 ------------------------------------------------------------------@@-@@-*/

static BOOL IsItemFromChildBranch(HTREEITEM hDragged, HTREEITEM hHighlighted){
	HTREEITEM	hTemp;

	hTemp = TreeView_GetParent(m_hTree, hHighlighted);
	while(hTemp){
		if(hTemp == hDragged){
			return TRUE;
		}
		hTemp = TreeView_GetParent(m_hTree, hTemp);
	}
	return FALSE;
}

static PMEMNOTE SelectedNote(void){
	int			index;
	LVITEMW		lvi;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_PARAM;

	index = ListView_GetNextItem(m_hList, -1, LVNI_SELECTED);
	if(index > -1){
		lvi.iItem = index;
		SendMessageW(m_hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
		if(lvi.lParam){
			return (PMEMNOTE)lvi.lParam;
		}
	}
	return NULL;
}

static void ToggleItemsProtection(void){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			ToggleNoteProtection(pNote);
		}
		free(pFree);
	}
}
static void ToggleItemsPriority(void){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			ToggleNotePriority(pNote);
		}
		free(pFree);
	}
}

static void ToggleItemsCompleted(void){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			ToggleNoteCompleted(pNote);
		}
		free(pFree);
	}
}

static void RollUnrollItems(void){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			if(pNote->pRTHandles->rolled)
				UnrollNote(pNote);
			else
				RollNote(pNote);
		}
		free(pFree);
	}
}

static void HideItems(void){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			if(pNote->pData->visible){
				HideNote(pNote);
			}
		}
		free(pFree);
	}
}

static void ShowItems(void){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			ShowNote(pNote);
		}
		free(pFree);
	}
}

static void AddItemsToFavorites(void){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			AddToFavorites(pNote);
		}
		free(pFree);
	}
}

static void PrintItem(void){
	PMEMNOTE	pNote = SelectedNote();
	HWND		hEdit;

	if(pNote){
		if(pNote->pData->visible)
			hEdit = (HWND)GetPropW(pNote->hwnd, PH_EDIT);
		else
			hEdit = GetUnvisibleNoteEdit(pNote->pFlags->id);
		if(hEdit){
			PrintEdit(hEdit, pNote->pData->szName);
		}
	}
}

static void SendItemByEmail(void){
	PMEMNOTE	pNote = SelectedNote();
	if(pNote){
		SendNoteByEmail(pNote);
	}
}

static void SendItemAsAttachment(void){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;

	if(pNotes){
		pFree = pNotes;
		if(count == 1){
			SendNoteAsAttachment((PMEMNOTE)*pNotes);
		}
		else{
			SendMultipleNotesAsAttachments(pNotes, count);
		}
		free(pFree);
	}

	//PMEMNOTE	pNote = SelectedNote();
	//if(pNote){
		//SendNoteAsAttachment(pNote);
	//}
}

static void SaveItem(void){
	PMEMNOTE	pNote = SelectedNote();
	if(pNote){
		SavePNote(pNote);
	}
}

static void SaveItemAs(void){
	PMEMNOTE	pNote = SelectedNote();
	if(pNote){
		SaveNoteAs(pNote);
	}
}

static void RestoreCenterItem(int operation){
	PMEMNOTE	pNote = SelectedNote();
	if(pNote){
		CenterRestorePlacementNote(pNote, operation);
	}
}

static void AdjustItem(int type){
	PMEMNOTE	pNote = SelectedNote();
	if(pNote){
		if(pNote->pData->visible){
			if(type == 0)
				SendMessageW(pNote->hwnd, WM_COMMAND, MAKEWPARAM(IDM_ADJUST_APPEARANCE, 0), 0);
			else
				SendMessageW(pNote->hwnd, WM_COMMAND, MAKEWPARAM(IDM_ADJUST_SCHEDULE, 0), 0);
		}
		else{
			ADJUST_STRUCT		as;

			as.pNote = pNote;
			as.type = type;
			DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_ADJUST), g_hMain, Adjust_DlgProc, (LPARAM)&as);
			g_hAdjust = NULL;
		}
	}
}

static BOOL RestoreAllFromBin(void){
	PMEMNOTE	pNote = MemoryNotes();

	while(pNote){
		if(pNote->pData->idGroup == GROUP_RECYCLE){
			RestoreNoteFromBin(pNote, -1);
			m_tItem = NULL;
			FindItemById(TreeView_GetRoot(m_hTree), pNote->pData->idGroup);
			UpdateGroupText(pNote->pData->idGroup, m_tItem);
		}
		pNote = pNote->next;
	}
	return TRUE;
}

static BOOL RestoreFromBin(int group){
	int			count;
	LONG_PTR	*pNotes = SelectedPItems(&count), *pFree;
	PMEMNOTE	pNote;

	if(pNotes){
		pFree = pNotes;
		//and now we can send message to all selected windows (items)
		for(int i = 0; i < count; i++){
			pNote = (PMEMNOTE)*pNotes++;
			RestoreNoteFromBin(pNote, group);
			m_tItem = NULL;
			FindItemById(TreeView_GetRoot(m_hTree), pNote->pData->idGroup);
			UpdateGroupText(pNote->pData->idGroup, m_tItem);
		}
		free(pFree);
	}
	return TRUE;
	//int			index, count;
	//LVITEMW		lvi;
	
	//ZeroMemory(&lvi, sizeof(lvi));
	//lvi.mask = LVIF_PARAM;

	//index = ListView_GetNextItem(m_hList, -1, LVNI_SELECTED);
	//if(!all && index > -1){
		//lvi.iItem = index;
		//SendMessageW(m_hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
		//if(lvi.lParam){
			//RestoreNoteFromBin((PMEMNOTE)lvi.lParam, group);
			//return TRUE;
		//}
	//}
	//else if(all){
		//count = ListView_GetItemCount(m_hList);
		//for(int i = 0; i < count; i++){
			//lvi.iItem = i;
			//SendMessageW(m_hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
			//if(lvi.lParam){
				//RestoreNoteFromBin((PMEMNOTE)lvi.lParam, group);
			//}
		//}
		//return TRUE;
	//}

	//return FALSE;
}

static BOOL EmptyBin(void){
	if(MessageBoxW(m_hCtrlPanel, g_Strings.EmptyBinMessage, g_Strings.DFCaption, MB_YESNO | MB_ICONQUESTION) == IDYES){
		EmptyRecycleBin();
		return TRUE;
	}
	return FALSE;
}

static BOOL DeleteItems(void){
	int			count, index, id;
	LVITEMW		lvi;
	PMEMNOTE	pNote;
	HTREEITEM	hItem;

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.mask = LVIF_PARAM;

	index = ListView_GetNextItem(m_hList, -1, LVNI_SELECTED);
	count = ListView_GetSelectedCount(m_hList);
	if(count > 1){
		if(MessageBoxW(m_hCtrlPanel, g_Strings.DMessageMultiple, g_Strings.DCaption, MB_YESNO | MB_ICONQUESTION) == IDYES){
			hItem = TreeView_GetSelection(m_hTree);
			id = GetTVItemId(hItem);
			while(index > -1){
				lvi.iItem = index;
				SendMessageW(m_hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
				if(lvi.lParam){
					pNote = (PMEMNOTE)lvi.lParam;
					if(pNote->pData->idGroup > GROUP_RECYCLE){

						DeleteToBin(pNote, -1);
					}
					else if(pNote->pData->idGroup == GROUP_RECYCLE){
						WritePrivateProfileSectionW(pNote->pFlags->id, NULL, g_NotePaths.DataFile);
						DeleteRTFile(pNote->pFlags->id);
					}
				}
				index = ListView_GetNextItem(m_hList, index, LVNI_SELECTED);
			}
			if(id != GROUP_RECYCLE){
				UpdateGroupText(id, hItem);
			}
			return TRUE;
		}
	}
	else if(count == 1 && index > -1){
		if(MessageBoxW(m_hCtrlPanel, g_Strings.DMessage, g_Strings.DCaption, MB_YESNO | MB_ICONQUESTION) == IDYES){
			hItem = TreeView_GetSelection(m_hTree);
			id = GetTVItemId(hItem);
			lvi.iItem = index;
			SendMessageW(m_hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
			if(lvi.lParam){
				pNote = (PMEMNOTE)lvi.lParam;
				if(pNote->pData->idGroup > GROUP_RECYCLE){
					DeleteToBin(pNote, -1);
				}
				else if(pNote->pData->idGroup == GROUP_RECYCLE){
					WritePrivateProfileSectionW(pNote->pFlags->id, NULL, g_NotePaths.DataFile);
					DeleteRTFile(pNote->pFlags->id);
				}
				if(id != GROUP_RECYCLE){
					UpdateGroupText(id, hItem);
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}

static void DefineFavoriteStatus(int index){
	// int				index;
	LVITEMW			lvi;
	PMEMNOTE		pNote;
	wchar_t			szBuffer[256], szFile[MAX_PATH];
	TBBUTTONINFOW	tbi;
	int				size;

	// index = SendMessageW(hList, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
	if(index > -1){
		ZeroMemory(&tbi, sizeof(tbi));
		tbi.cbSize = sizeof(tbi);
		tbi.dwMask = TBIF_IMAGE | TBIF_COMMAND;
		tbi.idCommand = IDM_ADD_TO_FAVORITES;
		// SendMessageW(m_hTbrCtrl, TB_GETBUTTONINFOW, IDM_ADD_TO_FAVORITES, (LPARAM)&tbi);
		ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask = LVIF_PARAM;
		lvi.iItem = index;
		SendMessageW(m_hList, LVM_GETITEMW, 0, (LPARAM)&lvi);
		pNote = (PMEMNOTE)lvi.lParam;
		if(pNote){
			wcscpy(szFile, g_NotePaths.LangDir);
			wcscat(szFile, g_NotePaths.LangFile);
			size = NELEMS(m_CtrlMenus);
			GetPrivateProfileStringW(S_FAVORITES, pNote->pFlags->id, NULL, szBuffer, 256, g_NotePaths.INIFile);
			if(wcslen(szBuffer) > 0){
				tbi.iImage = 69;
				SetMenuText(IDM_REMOVE_FAVORITES, S_MENU, szFile, L"Remove From Favorites", szBuffer);
				SetMIText(m_CtrlMenus, size, IDM_ADD_TO_FAVORITES, szBuffer);
				SetMIImageIndex(m_CtrlMenus, size, IDM_ADD_TO_FAVORITES, 69);
			}
			else{
				tbi.iImage = 68;
				SetMenuText(IDM_ADD_TO_FAVORITES, S_MENU, szFile, L"Add To Favorites", szBuffer);
				SetMIText(m_CtrlMenus, size, IDM_ADD_TO_FAVORITES, szBuffer);
				SetMIImageIndex(m_CtrlMenus, size, IDM_ADD_TO_FAVORITES, 68);
			}
			SendMessageW(m_hTbrCtrl, TB_SETBUTTONINFOW, IDM_ADD_TO_FAVORITES, (LPARAM)&tbi);
		}
	}
}

static void ShowButtonDropDown(int buttonId, int menuId){
	RECT		rc;
	HMENU		hMenu;
	int			index;

	index = GetButtonIndexW(m_hTbrCtrl, buttonId);
	if(index > 0)
		index--;	
	SendMessageW(m_hTbrCtrl, TB_GETITEMRECT, index, (LPARAM)&rc);
	m_hMenuCtrlTemp = m_hCtrlPUMenu;
	hMenu = GetSubMenu(m_hMenuCtrlTemp, GetMenuPosition(m_hMenuCtrlTemp, menuId));
	MapWindowPoints(m_hCtrlPanel, HWND_DESKTOP, (LPPOINT)&rc, 2);
	TrackPopupMenu(hMenu, TPM_LEFTALIGN, rc.left, rc.bottom, 0, m_hCtrlPanel, NULL);
	//remove popup menu when user clicks outside it
	PostMessageW(m_hCtrlPanel, WM_NULL, 0, 0);
}

static void UpdateGroupText(int id, HTREEITEM hItem){
	TVITEMW			tvi;
	wchar_t			szBuffer[256], szCount[16], szFile[MAX_PATH];
	P_PNGROUP		ppg;

	wcscpy(szFile, g_NotePaths.LangDir);
	wcscat(szFile, g_NotePaths.LangFile);
	ZeroMemory(&tvi, sizeof(tvi));
	tvi.mask = TVIF_TEXT;
	tvi.hItem = hItem;
	tvi.pszText = szBuffer;
	tvi.cchTextMax = 256;
	SendMessageW(m_hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
	_itow(CountNotesInGroup(id), szCount, 10);
	if(id != GROUP_RECYCLE){
		ppg = GetGroup(id);
		if(ppg){
			wcscpy(szBuffer, ppg->szName);
		}
	}
	else{
		GetPrivateProfileStringW(S_CAPTIONS, L"recycle_bin", L"Recycle Bin", szBuffer, 128, szFile);
	}
	wcscat(szBuffer, L" (");
	wcscat(szBuffer, szCount);
	wcscat(szBuffer, L")");
	SendMessageW(m_hTree, TVM_SETITEMW, 0, (LPARAM)&tvi);
	//update "All notes"
	tvi.hItem = TreeView_GetRoot(m_hTree);
	SendMessageW(m_hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
	GetPrivateProfileStringW(S_CAPTIONS, L"group_main", L"All notes", szBuffer, 128, szFile);
	_itow(CountNotesAlive(), szCount, 10);
	wcscat(szBuffer, L" (");
	wcscat(szBuffer, szCount);
	wcscat(szBuffer, L")");
	SendMessageW(m_hTree, TVM_SETITEMW, 0, (LPARAM)&tvi);
}

static void CheckGroupVisibility(P_CPENUM pcp){
	PMEMNOTE	pNote = MemoryNotes();

	while(pNote){
		if(pNote->pData->idGroup > GROUP_RECYCLE){
			if(pNote->pData->idGroup == pcp->group){
				if(pNote->pData->visible)
					pcp->result |= VISIBLE_ONLY;
				else
					pcp->result |= HIDDEN_ONLY;
			}
		}
		pNote = pNote->next;
	}
}

static void ShowHidePreview(void){
	int		result = GetMenuState(m_hCtrlPUMenu, IDM_PREVIEW, MF_BYCOMMAND);

	if((result & MF_CHECKED) == MF_CHECKED){
		CheckMenuItem(m_hCtrlPUMenu, IDM_PREVIEW, MF_UNCHECKED | MF_BYCOMMAND);
		SendMessageW(m_hTbrCtrl, TB_CHECKBUTTON, IDM_PREVIEW, (LPARAM)MAKELONG(0, 0));
		SendMessageW(m_hHorSplitter, SPM_HIDE_CHILD, 0, CHILD_BOTTOM);
		WritePrivateProfileStringW(S_CP_DATA, IK_CP_PVW_VISIBLE, L"0", g_NotePaths.INIFile);
	}
	else{
		CheckMenuItem(m_hCtrlPUMenu, IDM_PREVIEW, MF_CHECKED | MF_BYCOMMAND);
		SendMessageW(m_hTbrCtrl, TB_CHECKBUTTON, IDM_PREVIEW, (LPARAM)MAKELONG(TBSTATE_CHECKED, 0));
		SendMessageW(m_hHorSplitter, SPM_SHOW_CHILD, 0, CHILD_BOTTOM);
		WritePrivateProfileStringW(S_CP_DATA, IK_CP_PVW_VISIBLE, L"1", g_NotePaths.INIFile);
	}
}

static BOOL CALLBACK DTree_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		HANDLE_MSG (hwnd, WM_INITDIALOG, DTree_OnInitDialog);
		HANDLE_MSG (hwnd, WM_SIZE, DTree_OnSize);
		HANDLE_MSG (hwnd, WM_COMMAND, DTree_OnCommand);
		HANDLE_MSG (hwnd, WM_DRAWITEM, DTree_OnDrawItem);
		HANDLE_MSG (hwnd, WM_MEASUREITEM, DTree_OnMeasureItem);
		HANDLE_MSG (hwnd, WM_INITMENUPOPUP, DTree_OnInitMenuPopup);

		case WM_NOTIFY:
			return SendMessageW(GetParent(hwnd), WM_NOTIFY, wParam, lParam);
		default: return FALSE;
	}
}

static void DTree_OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu)
{
	PrepareMenuGradientColors(GetSysColor(COLOR_BTNFACE));
}

static void DTree_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
	if(m_hMenuCtrlTemp == m_hTreePUMenu)
		DrawMItem(lpDrawItem, g_hBmpMenuNormal, g_hBmpMenuGray, CLR_MASK);
}

static void DTree_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT * lpMeasureItem)
{
	if(m_hMenuCtrlTemp == m_hTreePUMenu)
		MeasureMItem(g_hMenuFont, lpMeasureItem);
}

static void DTree_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	HTREEITEM		hInserted;

	switch(id){
		case IDM_NEW_GROUP:{
			PNGROUP		pg;
			ZeroMemory(&pg, sizeof(pg));
			pg.id = NEW_GROUP_ID;
			pg.parent = -1;
			int result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_GROUP), g_hCPDialog, NewGroup_DlgProc, (LPARAM)&pg);
			if(result == IDOK){
				hInserted = InsertGroup(TreeView_GetRoot(m_hTree), &pg, FALSE, TRUE);
				if(hInserted)
					TreeView_EnsureVisible(m_hTree, hInserted);
			}
			break;
		}
		case IDM_NEW_SUBGROUP:{
			PNGROUP		pg;
			ZeroMemory(&pg, sizeof(pg));
			pg.id = NEW_GROUP_ID;
			int result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_GROUP), g_hCPDialog, NewGroup_DlgProc, (LPARAM)&pg);
			if(result == IDOK){
				if(hwndCtl == m_hTbrTree)
					hInserted = InsertGroup(m_tSelected, &pg, TRUE, TRUE);
				else
					hInserted = InsertGroup(m_tHighlighted, &pg, TRUE, TRUE);
				if(hInserted)
					TreeView_EnsureVisible(m_hTree, hInserted);
			}
			break;
		}
		case IDM_EDIT_GROUP:{
			P_PNGROUP	ppg;
			TVITEMW	tvi;
			wchar_t		szBuffer[128];

			ZeroMemory(&tvi, sizeof(tvi));
			tvi.mask = TVIF_HANDLE | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
			if(hwndCtl == m_hTbrTree)
				tvi.hItem = m_tSelected;
			else
				tvi.hItem = m_tHighlighted;
			tvi.cchTextMax = 128;
			tvi.pszText = szBuffer;
			SendMessageW(m_hTree, TVM_GETITEMW, 0, (LPARAM)&tvi);
			ppg = GetGroup(tvi.lParam);
			int result = DialogBoxParamW(g_hInstance, MAKEINTRESOURCEW(DLG_GROUP), g_hCPDialog, NewGroup_DlgProc, (LPARAM)ppg);
			if(result == IDOK){
				wchar_t		szCount[16];
				tvi.iImage = ppg->image;
				tvi.iSelectedImage = ppg->image;
				_itow(CountNotesInGroup(ppg->id), szCount, 10);
				wcscpy(szBuffer, ppg->szName);
				wcscat(szBuffer, L" (");
				wcscat(szBuffer, szCount);
				wcscat(szBuffer, L")");
				SendMessageW(m_hTree, TVM_SETITEMW, 0, (LPARAM)&tvi);
				//re-sort items alphabetically
				TreeView_SortChildren(m_hTree, TreeView_GetParent(m_hTree, tvi.hItem), FALSE);
				SaveGroup(ppg);
				UpdateListView();
			}
			break;
		}
		case IDM_DELETE_GROUP:{
			wchar_t		szMessage[768];
			HTREEITEM	hParent;

			wcscpy(szMessage, g_Strings.DeleteGroup1);
			wcscat(szMessage, L"\n");
			wcscat(szMessage, g_Strings.DeleteGroup2);
			// wcscat(szMessage, L"\n");
			// wcscat(szMessage, g_Strings.DeleteGroup3);
			if(MessageBoxW(hwnd, szMessage, g_Strings.DeleteGroup0, MB_YESNO | MB_ICONWARNING) == IDYES){
				if(hwndCtl == m_hTbrTree){
					hParent = TreeView_GetParent(m_hTree, m_tSelected);
					DeleteTreeGroup(m_tSelected, FALSE);
				}
				else{
					hParent = TreeView_GetParent(m_hTree, m_tHighlighted);
					DeleteTreeGroup(m_tHighlighted, FALSE);
				}
				TreeView_SelectItem(m_hTree, hParent);
			}
			break;
		}
		case IDM_SHOW_GROUP:
			if(hwndCtl == m_hTbrTree){
				ShowGroup(m_tSelected);
				EnableTreeMenus(m_tSelected);
			}
			else{
				ShowGroup(m_tHighlighted);
				EnableTreeMenus(m_tHighlighted);
			}
			break;
		case IDM_HIDE_GROUP:
			if(hwndCtl == m_hTbrTree){
				HideGroup(m_tSelected);
				EnableTreeMenus(m_tSelected);
			}
			else{
				HideGroup(m_tHighlighted);
				EnableTreeMenus(m_tHighlighted);
			}
			break;
	}
}

static BOOL DTree_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	m_hTbrTree = GetDlgItem(hwnd, IDC_TBR_TREE);
	m_hTree = GetDlgItem(hwnd, IDC_CTRL_TREEVIEW);
	
	return FALSE;
}

static void DTree_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	RECT		rc;

	GetClientRect(m_hTbrTree, &rc);
	MoveWindow(m_hTbrTree, 0, 0, cx, m_Up, TRUE);
	MoveWindow(m_hTree, 0, m_Up, cx, cy - m_Up, TRUE);
}

