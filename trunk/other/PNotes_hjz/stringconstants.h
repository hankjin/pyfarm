//window class
#define	NOTES_PROG_MAIN_CLASS	L"agNotesMainClass"
#define NWC_SKINNABLE_CLASS		L"agNoteSkinnableClass"
#define	NWC_GRIP_CLASS			L"agNoteGripClass"
#define	NWC_TBR_BUTTON_CLASS	L"agNoteTbrButtonClass"
#define	NWC_TOOLBAR_CLASS		L"agNoteToolbarClass"
#define	NWC_NOTE_MARK_CLASS		L"agNoteMarkClass"

#define	PROG_NAME				L"PNotes"
#define	PROG_EXE_NAME			"PNotes.exe"

//shortcut strings
#define	LINK_NAME				"\\PNotes.lnk"

//note window properties
#define PH_EDIT					L"Edit"
#define	PH_GRIP					L"Grip"
#define	PH_STATIC				L"Static"
#define	PH_COMMANDS				L"TbrCommands"
#define PH_DELHIDE				L"TbrDelHide"
#define PH_BELL					L"bellwindow"

//ini files sections and keys
#define	S_FILE					L"file"
#define	S_NOTE					L"note"
#define	S_SETTINGS				L"settings"
#define	S_APPEARANCE			L"appearance"
#define	S_DOCKING				L"docking"
#define S_SKIN					L"skin"
#define S_FONT					L"font"
#define	S_CAPTION_FONT			L"cfont"
#define	S_COLOR					L"color"
#define S_CAPTION_COLOR			L"ccapcolor"
#define	S_BCKG_COLOR			L"bckgcolor"
#define	S_OPTIONS				L"options"
#define	S_COLUMNS				L"columns"
#define	S_TABS					L"tabs"			
#define	S_TEXT					L"text"
#define	S_POSITION				L"position"
#define	S_SCHEDULE				L"schedule"
#define	S_DATA					L"data"
#define	S_MENU					L"menu"
#define	S_FORMAT				L"format"
#define	S_SOUND					L"sound"
#define	S_HOTKEYS				L"hotkeys"
#define	S_NOTE_HOTKEYS			L"note_hotkeys"
#define	S_COMMANDS				L"commands"
#define	S_EXITFLAG				L"exitflag"
#define	S_DEFSIZE				L"defsize"
#define	S_PREVIEW				L"preview"
#define	S_GROUPS				L"groups"
#define	S_CAPTIONS				L"captions"
#define	S_STATUS				L"status"
#define	S_CP_DATA				L"cp_data"
#define	S_SEARCH				L"search"
#define	S_SEARCH_HIST			L"search_history"
#define	S_REPLACE_HIST			L"replace_history"
#define	S_DATE_FORMAT_CHARS		L"date_format_chars"
#define	S_TIME_FORMAT_CHARS_H12	L"time_format_chars_h12"
#define	S_TIME_FORMAT_CHARS_H24	L"time_format_chars_h24"
#define	S_TIME_FORMAT_CHARS		L"time_format_chars"
#define	S_LEFT_PANEL			L"left_panel"
#define	S_RESULTS_COLS			L"search_columns"
#define	S_TITLES_COLS			L"titles_columns"
#define	S_FAVORITES				L"favorites"
#define	S_WEEKDAYS				L"weekdays"

#define IK_CP_POSITION			L"cp_position"
#define	IK_CP_LAST_GROUP		L"cp_last_group"
#define	IK_CP_COL_WIDTH			L"cp_col_width"
#define	IK_CP_COL_ORDER			L"cp_col_order"
#define	IK_CP_SORT_DATA			L"cp_sort_data"
#define	IK_CP_PVW_VISIBLE		L"cp_pvw_on"
#define	IK_CP_PVW_USE_COLOR		L"cp_pvw_use_color"
#define	IK_CP_PVW_COLOR			L"cp_pvw_color"
#define	IK_RELPOSITION			L"rel_position"

//files
#define FN_DATA_FILE			L"\\notes.pnid"
#define	NOTE_EXTENTION			L".pnote"

//default control text
#define	DS_CAPTION				L"Preferences"
#define	DS_CAP_SINGLE			L"Current note options"
#define	DS_CTRL_PANEL			L"Control panel"
#define	DS_OK					L"OK"
#define	DS_CANCEL				L"Cancel"
#define	DS_APPLY				L"Apply"
#define	DS_ALERT				L"Alert sound"
#define	DS_SKIN					L"Skin"
#define	DS_PREVIEW				L"Preview"
#define DS_O_COMMON				L"Common notes options"
#define DS_O_FONT				L"The quick brown fox jumps over the lazy dog"
#define	DS_O_DEFAULT			L"Default preferences"
#define	DS_TRANS				L"Allow transparency"
#define	DS_TR_WARNING			L"Value of 100% will result in fully transparent window!"
#define	DS_SHOW_START			L"Show all notes on program start"
#define	DS_SAVE_EXIT			L"Save all on program exit"
#define	DS_SAVE_CONF			L"Confirm saving"
#define DS_CONF_DELETE			L"Confirmation before note deletion"
#define	DS_SHOW_SCROLLBAR		L"Show scrollbar (all notes will be reloaded)"
#define	DS_O_SINGLE				L"Note font"
#define	DS_GROUP_FONT			L"Notes font"
#define DS_ON_STARTUP			L"Run on system startup"
#define	DS_APPEARANCE			L"Appearance"
#define	DS_SCHEDULE				L"Schedule"
#define	DS_TRANSPARENCY			L"Transparency"
#define	DS_MISC					L"Misc"
#define	DS_DEF_SOUND			L"(Default)"
#define	DS_NO_SKIN				L"(No skin)"
#define	DS_GROUP_CAPTION_NEW	L"New group"
#define	DS_GROUP_CAPTION_EDIT	L"Modify group"
#define	DS_CHOOSE_GROUP_ICON	L"Group icon"
#define	DS_CHOOSE_GROUP_NAME	L"Group name"
#define	DS_PICK_ICON			L"Pick an icon for group"
#define	DS_SHOW_GROUP			L"Show all notes from selected group"
#define	DS_HIDE_GROUP			L"Hide all notes from selected group"
#define	DS_ADD_SUBGROUP			L"Add subgroup to selected group"
#define	DS_DELETE_GROUP			L"Delete group"
#define	DS_NOTE_TO_GROUP		L"Add note to selected group"
#define	DS_ADD_TOP_LEVEL_GROUP	L"Add top level group"
#define	DS_GENERAL_GROUP_NAME	L"General"
#define	DS_ALL_NOTES_GROUP		L"All notes"
#define	DS_GROUP				L"Group:"
#define	DS_TOTAL_NOTES			L"Total notes:"
#define	DS_DD_IMPOSSIBLE		L"(drag-and-drop is impossible)"
#define	DS_NOTES_IN_GROUP		L"Notes in group"

#define	DSK_ALLOW				L"allow"
#define	DSK_SOUND_DEF			L"def_sound"
#define	DSK_SOUND_CUST			L"cust_sound"
#define	DSK_CONF_DELETE			L"confirm_delete"
#define	DSK_CURRENT_STATE		L"current"
#define	DSK_SEARCH_PARAMS		L"params"
#define	DSK_SETTINGS			L"settings"

//datetime format strings
#define	DTS_DATE_FORMAT			L"dd MMM yyyy HH:mm"
#define	DTS_TIME_FORMAT			L"HH:mm"

//mutex - to prevent the program from running twice
#define	NOTES_MUTEX				L"PNotes_Mutex"
#define	UPDATE_CHECK_URL		"pnotes.sourceforge.net"
#define	HOME_PAGE				L"http://pnotes.sf.net"
#define	SKIN_PAGE				L"http://pnotes.sf.net?page=2"
#define	DOWNLOAD_PAGE			L"http://pnotes.sf.net?page=5"
#define	HELP_SUBPATH			L"\\PNotes.chm"
#define	MAIL_SUBJECT			L"Sent from PNotes. Note name: '"
#define	NOTE_CAPTION_FIRST_PART	L"PNotes - ["
#define	OLD_V_OPEN_FILE_FILTER	L"PNotes old versions data base (*.pndb)\0*.pndb\0\0"
#define	LOAD_NOTE_FILTER		L"PNotes files (*.pnote)\0*.pnote\0\0"
#define PROG_PAGE_MENU_ITEM		L"PNotes Homepage"
#define	DEF_PROG_TOOLTIP		L"PNotes - your virtual desktop notes organizer"
#define	DEF_MAIL_SUBJECT		L"Sent from PNotes"
#define	AUTOSAVE_INDEX_FILE		L"~.ni~"

//datetime help strings
#define	DATE_FORMAT_MASKS		L"d\tDay of month as digits with no leading zero for single-digit days.\n"\
								"dd\tDay of month as digits with leading zero for single-digit days.\n"\
								"ddd\tDay of week as a three-letter abbreviation.\n"\
								"dddd\tDay of week as its full name.\n"\
								"M\tMonth as digits with no leading zero for single-digit months.\n"\
								"MM\tMonth as digits with leading zero for single-digit months.\n"\
								"MMM\tMonth as a three-letter abbreviation.\n"\
								"MMMM\tMonth as its full name.\n"\
								"y\tYear as last two digits, but with no leading zero for years less than 10.\n"\
								"yy\tYear as last two digits, but with leading zero for years less than 10.\n"\
								"yyyy\tYear represented by full four digits."
#define	TIME_FORMAT_MASKS		L"h\tHours with no leading zero for single-digit hours; 12-hour clock.\n"\
								"hh\tHours with leading zero for single-digit hours; 12-hour clock.\n"\
								"H\tHours with no leading zero for single-digit hours; 24-hour clock.\n"\
								"HH\tHours with leading zero for single-digit hours; 24-hour clock.\n"\
								"m\tMinutes with no leading zero for single-digit minutes.\n"\
								"mm\tMinutes with leading zero for single-digit minutes.\n"\
								"s\tSeconds with no leading zero for single-digit seconds.\n"\
								"ss\tSeconds with leading zero for single-digit seconds.\n"\
								"t\tOne character time-marker string, such as A or P.\n"\
								"tt\tMulticharacter time-marker string, such as AM or PM.\n"
//skin resources
#define	SKN_INFO				L"SKN_INFO"
#define	SKN_MASK				"SKN_MASK"
#define	SKN_VERT_TBR			"SKN_VERT_TBR"

#define	FONT_NP					L"Courier New"
