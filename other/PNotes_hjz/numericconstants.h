//colors
#define CLR_MASK					RGB(255, 0, 255)
#define CR_IN_MASK					RGB(255, 0, 254)
//note toolbar constants
#define	CMD_TBR_BUTTONS				12
#define	SMALL_RECT					12
#define	CMD_SYS_BUTTONS				2
//default skinless dimensions
#define	DEF_FLAT_WIDTH				236
#define	DEF_FLAT_HEIGHT				236
#define	DEF_SIZE_MAX				800
#define	DEF_SIZE_MIN				64
//apply changes constants
#define APP_NEW_FONT				9001
#define APP_NEW_SKIN				9002
#define APP_NEW_COLOR				9003
#define	APP_TR_ALWD					9004
#define	APP_TR_NALWD				9005
#define	APP_TR_VALUE				9006
#define	APP_SAVE_EXIT				9007
#define	APP_CLOSE_NOTE				9008
#define	APP_NEW_CAP_FONT			9009
#define	APP_NEW_CAP_COLOR			9010
#define	APP_NEW_BCKGRND				9011
#define	APP_NEW_LANGUAGE			9012
#define	APP_SAVE_NOTE				9013
#define	APP_AUTOSAVE				9014
#define	APP_SAVE_ALL				9015
#define	APP_SAVE_BEFORE_RELOAD		9016
#define	APP_ON_TOP_CHANGED			9017
#define	APP_DOCK_DIST_CHANGED		9018
#define	APP_DOCK_SKIN_CHANGED		9019
#define	APP_DOCK_SIZE_CHANGED		9020
#define	APP_DOCK_ORDER_CHANGED		9021
#define	APP_DOCK_FONT_CHANGED		9022
#define	APP_DOCK_SKNLSS_FC_CHANGED	9023
#define	APP_DOCK_BGC_CHANGED		9024
#define	APP_SESSION_LOGON			9025
#define	APP_JUST_REDRAW				9026

//color menu identifiers
// #define	IDI_COLOR_START				13000
#define	IDI_FORMAT_COLOR_START		13100
#define	IDI_FORMAT_COLOR_END		13116
#define	IDI_FORMAT_HIGHLIGHT_START	13200
#define	IDI_FORMAT_HIGHLIGHT_END	13216
#define	ID_FC_CAPTION				5100
#define	ID_FH_CAPTION				5101
//delete/hide buttons identifiers
// #define	IDI_CMD_DEL				10012
// #define	IDI_CMD_HIDE			10011
//size menu identifiers
#define	IDI_SIZE_START				13300
#define	MAX_FONT_SIZE				72
//skin resources
#define IDS_SKIN_BODY				25000
#define IDS_SKIN_SYS				25001
#define	IDS_SKIN_DEL_HIDE			25002
#define IDS_EDIT_RECT				25010
#define	IDS_TT_RECT					25011
#define	IDS_SCH_RECT				25012
#define	IDS_NOT_SAVED_RECT			25013
#define	IDS_PROTECTED_RECT			25014
#define	IDS_PRIORITY_RECT			25015
#define	IDS_COMPLETED_RECT			25023
#define	IDS_MASK_COLOR				25016
#define	IDS_SKIN_COMMANDS			25017
#define	IDS_COMMANDS_RECT			25018
#define IDS_DH_RECT					25019
#define	IDS_SKIN_INFO				25020
#define	IDS_VERTICAL_TBR			25021
#define	IDS_MARKS_COUNT				25022

//language menu item addition
#define LANG_ADDITION				21001
//group menu addition
#define	GROUP_ADDITION				15001
//show groups addition
#define	SHOW_GROUP_ADDITION			13001
//hide group addition
#define	HIDE_GROUP_ADDITION			14001
//favorites menu addition
#define FAVORITES_ADDITION			9001
#define	FAVORITES_MAX				128

//custom messages
#define	CHM_BASE					WM_USER + 1234
#define CHM_RESIZE					CHM_BASE + 1
#define CHM_SAVE					CHM_BASE + 2
#define	CHM_TT_UPDATE				CHM_BASE + 3
#define	CHM_CTRL_UPD				CHM_BASE + 4
#define	CHM_CTRL_UPD_LANG			CHM_BASE + 5
#define	CHM_CTRL_INS				CHM_BASE + 6
#define	CHM_CTRL_DEL				CHM_BASE + 7
#define	CHM_MAIN_CLOSE				CHM_BASE + 8
#define	CHM_DRAW_SN_PVW				CHM_BASE + 9
#define	CHM_GET_RT_HANDLES			CHM_BASE + 10
#define	CHM_CHANGE_FONT				CHM_BASE + 11
#define	CHM_CHANGE_FONT_COLOR		CHM_BASE + 12
#define	CHM_NEW_LANGUAGE			CHM_BASE + 13
#define	CHM_NEW_FONT_FACE			CHM_BASE + 14
#define	CHM_SAVE_ON_SHUTDOWN		CHM_BASE + 15
#define CHM_RELOAD					CHM_BASE + 16
#define	CHM_NEW_BGCOLOR				CHM_BASE + 17
#define	CHM_ON_TOP_CHANGED			CHM_BASE + 18
#define	CHM_SHOW_AFTER_SEARCH		CHM_BASE + 19
#define CHM_BECOMES_HIDDEN			CHM_BASE + 20
#define	CHM_MOVE_DOCK_NOTE			CHM_BASE + 21
#define	CHM_UPDATE_CTRL_STATUS		CHM_BASE + 22
#define	CHM_APPLY_NEW_SKIN			CHM_BASE + 23
#define	CHM_NEW_FIND_PARENT			CHM_BASE + 24
#define CHM_CTRL_ITEM_UPDATE		CHM_BASE + 25
#define	CHM_CTRL_GROUP_UPDATE		CHM_BASE + 26

#define	CHN_DEL_WITH_GROUP			1

#define	GROUP_ROOT					-1
#define	GROUP_RECYCLE				-2
#define	NEW_GROUP_ID				-777
#define	MAX_GROUP_COUNT				1000

#define	MAX_SEARCH_COUNT			16

//tray + shellnotify
#define IDI_TRAY					0
#define WM_SHELLNOTIFY				WM_USER + 5

//session constants
#define WM_WTSSESSION_CHANGE  		0x02B1
#define	NOTIFY_FOR_THIS_SESSION		0
#define	NOTIFY_FOR_ALL_SESSIONS		1
#define WTS_CONSOLE_CONNECT  		0x1
#define WTS_CONSOLE_DISCONNECT  	0x2
#define WTS_REMOTE_CONNECT  		0x3
#define WTS_REMOTE_DISCONNECT  		0x4
#define WTS_SESSION_LOGON  			0x5
#define WTS_SESSION_LOGOFF  		0x6
#define WTS_SESSION_LOCK  			0x7
#define WTS_SESSION_UNLOCK  		0x8
#define WTS_SESSION_REMOTE_CONTROL  0x9
