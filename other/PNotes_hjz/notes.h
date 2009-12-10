#ifndef __NOTES_H__
#define __NOTES_H__

#define WIN32_LEAN_AND_MEAN

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

// #ifndef _WIN32_IE
// #define _WIN32_IE 0x0500
// #endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <winbase.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>
#include <shellapi.h>
#include <shlobj.h>
#include <commdlg.h>
#include <Mmsystem.h>
#include <richedit.h>
//private includes
#include "menu.h"
#include "numericconstants.h"
#include "stringconstants.h"
#include "structures.h"
#include "note.h"
#include "shared.h"
#include "about.h"
#include "transparentbitmap.h"
#include "controlpanel.h"
#include "toolbar.h"
#include "shortcut.h"
#include "combodialog.h"
#include "bell.h"
#include "search.h"
#include "searchtitles.h"
#include "docking.h"
#include "darrow.h"
#include "find.h"
#include "main.h"
#include "memorynotes.h"
#include "overdue.h"
#include "groups.h"

//dialogs and controls
#define DLG_MAIN  				1001
#define DLG_SAVE_AS				1002
#define DLG_ADJUST				1003
#define	DLG_CONTROL				1004
#define	DLG_HOT_KEY				1005
#define	DLG_COMBO				1007
#define	DLG_APPEARANCE			1008
#define	DLG_SCHEDULE			1009
#define	DLG_MISC				1010
#define	DLG_NOTE_APP			1011
#define	DLG_NOTE_SCHEDULE		1012
#define	DLG_GROUP				1013
#define	DLG_CHOOSE_BMP			1014
#define	DLG_FIND				1015
#define	DLG_SKINS				1016
#define	DLG_DOCKING				1017
#define	DLG_BEHAVIOR			1018
#define	DLG_FIND_SINGLE			1019
#define DNS_EVERYDAY  			1020
#define DNS_REPEATEVERY  		1021
#define DNS_WEEKLY  			1022
#define DNS_ONCE  				1006
#define	DNS_AFTER				1023
#define DNS_YEARLY					1024
#define	DLG_TITLES_SEARCH		1025
#define	DLG_OVERDUE				1026
#define	DLG_CONTROL_TREE		1027


#define	NOTE_TIMER_INTERVAL		1000

#define	IDAPPLY					8002
#define IDR_ICO_MAIN  			1
#define IDC_ST_BMP				8004
#define	IDC_GRP_SKIN			8006
#define IDC_LST_SKIN			8007
#define IDC_CMD_FONT			8011
#define	IDC_CHK_TRANS			8014
#define IDC_GRP_TRANS			8015
#define IDC_CHK_SHOW			8017
#define IDC_ALWAYS_SAVE			8018
#define IDC_ASK_B_SAVE			8019
#define	IDC_TAB_ADJUST			8021
#define IDC_ST_TAB				8022
#define	IDC_OPT_NO_SCH			8023
#define	IDC_OPT_EV_DAY			8024
#define	IDC_OPT_ONCE			8025
#define	IDC_DTP_EV_DAY			8026
#define	IDC_DTP_ONCE			8027
#define	IDC_GRP_SHARED			8028
#define	IDC_EDT_SAVE_AS			8029
#define	IDC_ST_SAVE_AS			8031
#define	IDC_LVW_CONTROL			8032
#define	IDC_TBR_CTRL			8033
#define	IDC_CMD_DEF				8034
#define	IDC_CHK_AUTORUN			8035
#define	IDC_ST_DATE_FMTS		8039
#define	IDC_ST_TIME_FMTS		8040
#define	IDC_EDT_DATE_FMTS		8041
#define	IDC_EDT_TIME_FMTS		8042
#define	IDC_TAB_OPTIONS			8043
#define	IDC_ST_OPTIONS			8044
#define	IDC_DATE_SAMPLE			8045
#define	IDC_TIME_SAMPLE			8046
#define	IDC_DATE_HELP			8047
#define	IDC_TIME_HELP			8048
#define	IDC_SINGLE_ALARM		8049
#define	IDC_REPEATED_ALARM		8050
#define	IDC_GRP_SOUND			8051
#define	IDC_CHK_ALLOW_SOUND		8052
#define IDC_GRP_FORMATS			8054
#define	IDC_GRP_HOTKEYS			8055
#define	IDC_CBO_COMMANDS		8056
#define	IDC_CMD_HOTKEY			8057
#define	IDC_ST_HOTKEY			8058
#define	IDC_ST_CHNGE_HK			8059
#define	IDC_EDT_HOT				8060
#define	IDC_CMD_DEL_HOT			8061
#define	IDC_CMD_ADD_SOUND		8062
#define	IDC_CMD_DEL_SOUND		8063
#define	IDC_CMD_LISTEN			8064
#define	IDC_LST_SOUND			8065
#define	IDC_OPT_REPEAT			8066
// #define	IDC_DT_REPEAT			8067
#define	IDC_CMD_STD_VIEW		8069
#define	IDC_CHK_DEL_CONFIRM		8072
#define	IDC_CHK_SCROLLBAR		8073
#define	IDC_COMBO_FONT			8074
#define	IDC_GRP_AUTOSAVE		8075
#define	IDC_CHK_AUTOSAVE		8076
#define	IDC_ST_AUTOSAVE			8078
#define	IDC_CMD_PREV_VERSION	8079
#define	IDC_GRP_DEF_SIZE		8080
#define	IDC_ST_DEF_SIZE_W		8081
#define	IDC_ST_DEF_SIZE_H		8082
#define	IDC_EDT_DEF_SIZE_W		8083
#define	IDC_EDT_DEF_SIZE_H		8084
#define	IDC_PLACEHOLDER_1		8085
#define	IDC_LBL_MORE_SKINS		8086
#define	IDC_GRP_START_OPT		8087
#define	IDC_GRP_SAVE_OPT		8088
#define	IDC_ST_RELOAD_WARNING	8089
// #define	IDC_ADJ_RELOAD_WARNING	8090
#define	IDC_CTRL_TREEVIEW		8091
#define	IDC_ST_GROUP_NAME		8092
#define	IDC_EDT_GROUP_NAME		8093
#define	IDC_ST_GROUP_BMP		8094
#define	IDC_CMD_GROUP_BMP		8095
#define	IDC_LST_CHOOSE_BMP		8096
#define	IDC_TBR_TREE			8097
#define	IDC_STB_CONTROL			8098
#define	IDC_VISUAL_NOTIFY		8100
#define	IDC_REDUCE_TO_CAPTION	8101
#define	IDC_GRP_SKINLESS_PROPS	8102
#define	IDC_ST_SEARCH			8103
#define	IDC_CBO_SEARCH			8104
#define	IDC_ST_RESULTS			8105
#define	IDC_LST_RESULTS			8106
#define	IDC_CHK_MATCH_CASE		8107
#define	IDC_CMD_FIND			8108
#define	IDC_CHK_WHOLE_WORD		8109
#define	IDC_CMD_SKNLESS_BCOLOR	8110
#define	IDC_CMD_SKNLESS_CFONT	8111
#define	IDC_TRANS_TRACK_BAR		8112
#define	IDC_TRANS_LEFT_ST		8113
#define	IDC_TRANS_RIGHT_ST		8114
#define	IDC_TRANS_WARNING		8115
#define	IDC_TRANS_VALUE			8116
#define	IDC_GRP_APPEARANCE		8117
#define	IDC_GRP_SCHEDULE		8118
#define	IDC_GRP_SEARCH			8119
#define	IDC_GRP_BEHAVIOR		8120
#define	IDC_GRP_DOCKING			8121
#define	IDC_GRP_MISC			8122
#define	IDC_SRCH_IN_HIDDEN		8123
#define	IDC_SRCH_HIST_CLEAR		8124
#define	IDC_GRP_UPDATE			8125
#define	IDC_CHK_NEW_V			8126
#define	IDC_CMD_NEW_V			8127
#define	IDC_CHK_NEW_ONTOP		8128
#define	IDC_CHK_RANDOM_COLOR	8129
#define	IDC_GRP_DOCK_SKINS		8130
#define	IDC_GRP_DOCK_SKINLESS	8131
#define	IDC_OPT_DOCK_DEF_SKIN	8132
#define	IDC_OPT_DOCK_RND_SKIN	8133
#define	IDC_DOCK_SLIDER			8134
#define	IDC_DOCK_ST_DISTANCE	8135
#define	IDC_DOCK_ST_SLIDER_FROM	8136
#define	IDC_DOCK_ST_SLIDER_END	8137
// #define	IDC_DOCK_ST_SLIDER_VAL	8138
#define	IDC_ST_ADJUST_DISABLE	8139
#define	IDC_OPT_DOCK_RND_SIZE	8140
#define	IDC_CBO_SKINS			8141
#define	IDC_INV_DOCK_ORDER		8142
#define	IDC_HIDE_TOOLBAR		8143
#define	IDC_CBO_REPLACE			8144
#define	IDC_ST_REPLACE			8145
#define	IDC_CMD_REPLACE_ALL		8146
#define	IDC_CMD_REPLACE			8147
#define	IDC_CMD_FIND_NEXT		8148
#define	IDC_OPT_FIND_UP			8149
#define	IDC_OPT_FIND_DOWN		8150
#define	IDC_UPD_AUTOSAVE		8151
#define	IDC_EDT_AUTOSAVE		8152
#define	IDC_ST_DAYS				8153
#define	IDC_ST_HOURS			8154
#define	IDC_ST_MINS				8155
#define	IDC_ST_SECS				8156
#define	IDC_EDT_DAYS			8157
#define	IDC_EDT_HOURS			8158
#define	IDC_EDT_MINS			8159
#define	IDC_EDT_SECS			8160
#define	IDC_UPD_DAYS			8161
#define	IDC_UPD_HOURS			8162
#define	IDC_UPD_MINS			8163
#define	IDC_UPD_SECS			8164
#define	IDC_GRP_DOCK_SZ			8165
#define	IDC_GRP_DOCK_FC			8166
#define	IDC_CHK_DOCK_C			8167
#define	IDC_CHK_DOCK_F			8168
#define	IDC_ST_DOCK_SKIN		8171
#define	IDC_ST_DOCK_SKINLESS	8172
#define	IDC_ST_DEF_BROWSER		8173
#define	IDC_EDT_DEF_BROWSER		8174
#define	IDC_CMD_DEF_BROWSER		8175
#define	IDC_CHK_REL_POSITION	8176
#define	IDC_USE_CUST_FONTS		8177
#define	IDC_OPT_EVERY_WD		8178
#define IDC_OPT_EVERY_YR			8179
#define	IDC_ST_TIME_EVERY_WD	8180
#define	IDC_DTP_TIME_EVERY_WD	8181
#define	IDC_PLACEHOLDER			8182
#define	IDC_CHK_SHOW_CP			8183
#define	IDC_CHK_WD_0			8184
#define	IDC_CHK_WD_1			8185
#define	IDC_CHK_WD_2			8186
#define	IDC_CHK_WD_3			8187
#define	IDC_CHK_WD_4			8188
#define	IDC_CHK_WD_5			8189
#define	IDC_CHK_WD_6			8190
#define	IDC_CBO_NOTE_SCHEDULE	8191
#define	IDC_OPT_AFTER			8192
#define	IDC_ST_YEARS			8193
#define	IDC_ST_MONTHS			8194
#define	IDC_ST_WEEKS			8195
#define	IDC_EDT_YEARS			8196
#define	IDC_EDT_MONTHS			8197
#define	IDC_EDT_WEEKS			8198
#define	IDC_UPD_YEARS			8199
#define	IDC_UPD_MONTHS			8200
#define	IDC_UPD_WEEKS			8201
#define	IDC_ST_STARTING			8202
#define	IDC_DTP_AFTER			8203
#define IDC_CBO_DBL_CLICK		8204
#define	IDC_GRP_DBL_CLICK		8205
#define	IDC_ST_TITLE_SEARCH		8206
#define	IDC_EDI_TITLE_SEARCH	8207
#define	IDC_OPT_MAIN_MENU		8208
#define	IDC_OPT_SINGLE_NOTE		8209
#define	IDC_CBO_SINGLE_NOTE		8210
#define	IDC_CHK_OVERDUE			8211
#define	IDC_LST_OVERDUE			8212
#define	IDC_EDT_OVERDUE			8213
#define	IDC_CHK_NO_TRACK		8214
#define	IDC_CHK_INVERT_COLOR	8215
#define IDC_DTP_YEAR				8216
#define IDC_RDO_LUNAR				8217
#define IDC_RDO_SOLAR				8218

//bitmaps
#define	IDB_MENU_NORMAL			101
#define IDB_MENU_GRAY			102
#define	IDB_CTRL_SMALL			103
#define	IDB_CTRL_LARGE			104
#define	IDB_GROUPS				105
#define	IDB_ARROW_LEFT			106
#define	IDB_ARROW_TOP			107
#define	IDB_ARROW_RIGHT			108
#define	IDB_ARROW_BOTTOM		109

//icons
#define	IDR_ICO_APPEARANCE		202
#define	IDR_ICO_SKINS			203
#define	IDR_ICO_SCHEDULE		204
#define	IDR_ICO_DOCK			205
#define	IDR_ICO_BEHAVIOR		206
#define	IDR_ICO_MISC			207
#define	IDR_CUR_V_SPLIT			302
#define	IDR_CUR_H_SPLIT			303
//edit box identifier
#define IDC_NOTE_EDIT			8500
//menus
#define	IDR_MAIN				10000
#define IDR_NOTE				10100
#define	IDR_CTRL_PANEL			10200
#define	IDR_TREE_MENU			10300
#define	IDR_NOTE_POPUP			10400
#define	IDR_RECYCLE_MENU		10500
#define IDR_DROP_MENU			10600
//menu items
#define	IDM_NEW					10001
#define	IDM_OPTIONS				10002
#define	IDM_SHOW_ALL			10003
#define	IDM_HIDE_ALL			10004
#define	IDM_ABOUT				10005
#define	IDM_HELP				10006
#define	IDM_EXIT				10007
#define IDM_CTRL				10008
#define IDM_SAVE				10009
#define IDM_ADJUST				10010
#define IDM_HIDE				10011
#define IDM_DEL					10012
#define	IDM_SAVE_AS				10013
#define	IDM_SHOW				10014
#define	IDM_SHOW_CENTER			10015
#define	IDM_V_ICONS				10016
#define	IDM_V_SMALL				10017
#define	IDM_V_LIST				10018
#define	IDM_V_REPORT			10019
#define	IDM_ON_TOP				10020
#define	IDM_LANGUAGES			10021
#define	IDM_L_EMPTY				10022
#define	IDM_EMAIL				10023
#define	IDM_ON_THE_WEB			10024
#define	IDM_SAVE_ALL			10025
#define	IDM_NOTE_GROUPS			10026
#define	IDM_UNDO				10027
#define	IDM_REDO				10028
#define	IDM_CUT					10029
#define	IDM_COPY				10030
#define	IDM_PASTE				10031
#define	IDM_SELECT_ALL			10032
#define	IDM_RESTORE_PLACEMENT	10033
#define	IDM_SEARCH_IN_NOTES		10034
#define	IDM_DOCK				10035
#define	IDM_DOCK_NONE			10036
#define	IDM_DOCK_LEFT			10037
#define	IDM_DOCK_TOP			10038
#define	IDM_DOCK_RIGHT			10039
#define	IDM_DOCK_BOTTOM			10040
#define	IDM_DOCK_ALL			10041
#define	IDM_SEARCH_NOTE			10042
#define	IDM_SEARCH_NOTE_NEXT	10043
#define	IDM_REPLACE_NOTE		10044
#define	IDM_DOCK_NONE_ALL		10045
#define	IDM_DOCK_LEFT_ALL		10046
#define	IDM_DOCK_TOP_ALL		10047
#define	IDM_DOCK_RIGHT_ALL		10048
#define	IDM_DOCK_BOTTOM_ALL		10050
#define	IDM_FORMAT_NOTE_TEXT	10051
#define	IDM_FAVORITES			10052
#define	IDM_ADD_TO_FAVORITES	10053
#define	IDM_REMOVE_FAVORITES	10054
#define	IDM_ATTACHMENT			10055
#define	IDM_LOAD_NOTE			10056
#define	IDM_EMPTY_BIN			10057
#define	IDM_RESTORE_ALL_BIN		10058
#define	IDM_RESTORE_NOTE		10059
#define	IDM_SHOW_GROUPS			10060
#define	IDM_HIDE_GROUPS			10061
#define	IDM_HIGH_PRIORITY		10062
#define	IDM_DROP_SHORTCUT		10063
#define	IDM_DROP_CONTENT		10064
#define	IDM_NOTE_FROM_CLIPBOARD	10065
#define	IDM_ALL_TO_FRONT		10066
#define	IDM_SEARCH_SUBMENU		10067
#define	IDM_SEARCH_TITLE		10068
#define	IDM_V_VIEW				10069
#define	IDM_VISIBILITY			10070
#define	IDM_TOGGLE_PROTECTION	10071
#define	IDM_PRINT_NOTE			10072
#define	IDM_PREVIEW				10073
#define	IDM_ADJUST_APPEARANCE	10074
#define	IDM_ADJUST_SCHEDULE		10075
#define	IDM_NOTE_MARKS			10076
#define	IDM_MARK_AS_COMPLETED	10077
#define	IDM_PVW_COLOR_SET		10078
#define	IDM_PVW_USE_COLOR		10079
#define	IDM_PVW_COLOR_VALUE		10080
#define	IDM_ROLL_UNROLL			10081
	
#define	IDM_FORMAT_FONT_FAMILY	5000
#define	IDM_FORMAT_FONT_SIZE	5001
#define	IDM_FORMAT_FONT_COLOR	5002
#define	IDM_FORMAT_FONT_BOLD	5003
#define	IDM_FORMAT_FONT_ITALIC	5004
#define	IDM_FORMAT_FONT_UNDER	5005
#define	IDM_FORMAT_FONT_STRIKE	5006
#define	IDM_FORMAT_FONT_HILIT	5007
#define	IDM_FORMAT_ALIGN_LEFT	5008
#define	IDM_FORMAT_ALIGN_CENTER	5009
#define	IDM_FORMAT_ALIGN_RIGHT	5010
#define	IDM_FORMAT_BULLETS		5011

#define	IDM_NEW_GROUP			10126
#define	IDM_EDIT_GROUP			10127
#define	IDM_DELETE_GROUP		10128
#define	IDM_SHOW_GROUP			10129
#define	IDM_HIDE_GROUP			10130
#define	IDM_NEW_SUBGROUP		10131

//version resource
#define	IDV_VERSION				12001
//edit control styles
#define	E_STYLE_WITHOUT_SCROLL	WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_NOHIDESEL | ES_AUTOVSCROLL
#define	E_STYLE_WITH_SCROLL		WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_NOHIDESEL | WS_VSCROLL
//macros
#define HANDLE_WM_MOUSELEAVE(hwnd,wParam,lParam,fn) ((fn)(hwnd),0)
#define HANDLE_WM_HOTKEY(hwnd, wParam, lParam, fn)  ((fn)((hwnd), (int)(wParam), (UINT)LOWORD(lParam), (UINT)HIWORD(lParam)), 0L)
#define NELEMS(a)  (sizeof(a) / sizeof((a)[0]))
#define	MEMNOTE(hwnd)	((PMEMNOTE)GetWindowLongPtrW(hwnd, OFFSET_MNOTE))

//register logon/logoff messages procedures
typedef BOOL (*REGSESSIONMESSAGES)(HWND, DWORD);
typedef BOOL (*UNREGSESSIONMESSAGES)(HWND);
//skin files procedures
typedef int (*GETSKINPROC)(HWND, UINT);
typedef int (*GETCTLRECTPROC)(UINT, LPRECT);
//memory offsets for storing data
enum offsets {OFFSET_MNOTE = 0, OFFSET_COMMANDS = 4, OFFSET_DH = 8, OFFSET_SYS = 12};
//schedule types
enum schedules {SCH_NO, SCH_ONCE, SCH_ED, SCH_REP, SCH_PER_WD, SCH_PER_YR, SCH_AFTER};
//command line arguments
enum clargs {ARG_EXIT = 1, ARG_SILENT = 2, ARG_NO_SAVE = 4, ARG_INI_PATH = 8, ARG_DATA_PATH = 16, ARG_PROG_PATH = 32};
enum note_flags {F_TEXT = 1, F_SKIN = 2, F_RTFONT = 4, F_SCHEDULE = 8, F_C_FONT = 16, F_C_COLOR = 32, F_B_COLOR = 64, F_PRIORITY = 128};
enum tbr_buttons {CMD_FONT = 0, CMD_SIZE, CMD_COLOR, CMD_BOLD, CMD_ITALIC, CMD_UNDERLINE, CMD_STRIKETHROUGH, CMD_HIGHLIGHT, CMD_A_LEFT, CMD_A_CENTER, CMD_A_RIGHT, CMD_BULLETS};
typedef enum _dock_type {DOCK_NONE = 0, DOCK_LEFT, DOCK_TOP, DOCK_RIGHT, DOCK_BOTTOM} dock_type;
enum bool_settings_1 {SB1_RANDCOLOR, SB1_HIDETOOLBAR, SB1_RELPOSITION, SB1_CUST_FONTS, SB1_SHOW_CP, SB1_TRACK_OVERDUE, SB1_INVERT_TEXT};
enum bool_settings_2 {SB2_NEW_NOTE, SB2_CONTROL_PANEL, SB2_PREFS, SB2_SEARCH_NOTES};
enum note_bool_settings {NB_PREV_VISIBLE, NB_HIGH_PRIORITY, NB_PROTECTED, NB_NOT_TRACK, NB_COMPLETED};
typedef enum _darrow_type {DA_LEFT_UP, DA_LEFT_DOWN, DA_TOP_LEFT, DA_TOP_RIGHT, DA_RIGHT_UP, DA_RIGHT_DOWN, DA_BOTTOM_LEFT, DA_BOTTOM_RIGHT} darrow_type;
#endif


