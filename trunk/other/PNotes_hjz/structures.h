#ifndef __STRUCTURES_H__
#define __STRUCTURES_H__

typedef union _TIMEUNION {
	FILETIME 		fileTime;
	ULARGE_INTEGER 	ul;
} TIMEUNION;

typedef struct {
	BOOL		mouseIn;
	BOOL		saved;
	BOOL		transparent;
	BOOL		fromDB;
	BOOL		inDrag;
	WORD		maskChanged;
	wchar_t		id[28];
} NOTE_FLAGS, *P_NOTE_FLAGS;

typedef struct {
	wchar_t		szName[128];
	SYSTEMTIME	stChanged;
	BOOL		onTop;		
	int			idGroup;
	int			dockData;
	BOOL		visible;
	RECT		rcp;
	BOOL		prevOnTop;
	BOOL		rolled;
	int			res1;
	int			idPrevGroup;
} NOTE_DATA, *P_NOTE_DATA;

typedef struct {
	HBITMAP		hbSkin;
	HBITMAP		hbSys;
	HBITMAP		hbBack;
	HBITMAP		hbDelHide;
	HBITMAP		hbCommand;
	HBRUSH		hbPattern;
	HBRUSH		hbBackground;
	COLORREF	crMask;
	HFONT		hFont;
	HFONT		hFCaption;
	UINT_PTR	idTimer;
	HWND		hTooltip;
	RECT		rcEdit;
	RECT		rcTooltip;
	RECT		rcSchedule;
	RECT		rcNotSaved;
	RECT		rcProtected;
	RECT		rcPriority;
	RECT		rcCompleted;
	RECT		rcDH;
	SIZE		szDef;
	RECT		rcCaption;
	RECT		rcGrip;
	RECT		rcTextCaption;
	RECT		rcCommands;
	int			nRepeat;
	RECT		rcSize;
	BOOL		rolled;
	wchar_t		szSkinInfo[256];
	BOOL		vertTbr;
	int			marks;
} NOTE_RTHANDLES, *P_NOTE_RTHANDLES;

typedef struct {
	wchar_t		szSkin[64];
	LOGFONTW	lfFont;
	COLORREF	crFont;
	LOGFONTW	lfCaption;
	COLORREF	crCaption;
	COLORREF	crWindow;
	BOOL		fFontSet;
	int			nPrivate;
} NOTE_APPEARANCE, *P_NOTE_APPEARANCE;

typedef struct {
	BOOL		transAllow;
	BYTE		transValue;
	BOOL		oneMoreBool;
	BOOL		saveOnExit;
	BOOL		confirmSave;
	BOOL		onStartup;
	BOOL		confirmDelete;
	BOOL		showScrollbar;
	BOOL		autoSave;
	BYTE		autoSValue;
	BOOL		checkOnStart;
	BOOL		visualAlert;
	BOOL		rollOnDblClick;
	BOOL		newOnTop;
	int			reserved1;
	int			reserved2;
} NOTE_SETTINGS, *P_NOTE_SETTINGS;

typedef struct {
	double		left;
	double		top;
	int			width;
	int			height;
} NOTE_REL_POSITION, *P_NOTE_REL_POSITION;

typedef struct {
	WORD		scType;
	SYSTEMTIME	scDate;
	SYSTEMTIME	scStart;
	SYSTEMTIME	scLastRun;
	int			reserved;
} SCHEDULE_TYPE, *P_SCHEDULE_TYPE;

typedef struct {
	wchar_t		INIFile[MAX_PATH];
	wchar_t		DataFile[MAX_PATH];
	wchar_t		DataDir[MAX_PATH];
	wchar_t		LangDir[MAX_PATH];
	wchar_t		LangFile[256]; 
	wchar_t		SkinDir[MAX_PATH];
	wchar_t		SoundDir[MAX_PATH];
	wchar_t		ProgName[256];
	wchar_t		ProgFullPath[MAX_PATH];
	wchar_t		FontsPath[MAX_PATH];
} NOTE_PATHS, *P_NOTE_PATHS;

typedef struct {
	wchar_t		DateFormat[128];
	wchar_t		TimeFormat[128];
} NT_DT_FORMATS, *P_NT_DT_FORMATS;

typedef struct {
	int			allowSound;
	int			defSound;
	wchar_t		custSound[128];
} SOUND_TYPE, *P_SOUND_TYPE;

typedef struct {
	int			id;
	int			identifier;
	UINT		fsModifiers;
	UINT		vk;
	wchar_t		szKey[24];
} HK_TYPE, * P_HK_TYPE;

typedef struct {
	wchar_t		szCommand[128];
} PNCOMMAND, * P_PNCOMMAND;

typedef struct {
	HWND		hCmd;
	HWND		hTtp;
} NCMD_BAR, *P_NCMD_BAR;

typedef struct {
	HWND		hParent;
	HWND		hButton;
	HWND		hEdit;
} CBCREATION, *P_CBCREATION;

typedef struct {
	wchar_t		szFace[LF_FACESIZE];
	int			nCharSet;
} FONT_TYPE, *P_FONT_TYPE;

typedef struct {
	wchar_t		SNote[64];
	wchar_t		SQuestion[128];
	wchar_t		SCaption[64];
	wchar_t		DMessage[256];
	wchar_t		DMessageBin[256];
	wchar_t		DMessageComplete[256];
	wchar_t		DMessageMultiple[256];
	wchar_t		EmptyBinMessage[256];
	wchar_t		DCaption[64];
	wchar_t		TooManyFiles[256];
	wchar_t		DPrevQuestion[128];
	wchar_t		DeleteGroup0[96];
	wchar_t		DeleteGroup1[256];
	wchar_t		DeleteGroup2[256];
	wchar_t		DeleteGroup3[256];
	wchar_t		DFCaption[128];
	wchar_t		DFormats[1536];
	wchar_t		TFCaption[128];
	wchar_t		TFormats[1408];
	wchar_t		SearchComplete[128];
	wchar_t		ReplaceComplete[128];
	wchar_t		MatchesFound[128];
	wchar_t		NoMatchesFound[128];
	wchar_t		CheckUpdate[60];
	wchar_t		NewVersion1[112];
	wchar_t		NewVersion2[112];
	wchar_t		SameVersion[128];
	wchar_t		FontSample[256];
	wchar_t		SaveBeforeSend[256];
	wchar_t		DefaultSettingsWarning[256];
} PNSTRINGS, *P_PNSTRINGS;

typedef struct _PNGROUP *P_PNGROUP;

typedef struct _PNGROUP {
	int			id;
	int			parent;
	int			image;
	wchar_t		szName[128];
	P_PNGROUP	next;
} PNGROUP;

typedef struct {
	BOOL		fCustSkin;
	BOOL		fCustSize;
	BOOL		fCustColor;
	BOOL		fCustCaption;
	// BOOL		fCustFont;
	BOOL		fInvOrder;
	LOGFONTW	lfCaption;
	COLORREF	crCaption;
	COLORREF	crWindow;
	// LOGFONTW	lfFont;
	// COLORREF	crFont;
	int			dist;
	SIZE		custSize;
	wchar_t		szCustSkin[64];
} PNDOCK, *P_PNDOCK;


typedef struct _DOCKITEM * PDOCKITEM;
typedef struct _DOCKITEM {
	PDOCKITEM		pPrev;
	HWND			hwnd;
	PDOCKITEM		pNext;
	short			index;
}DOCKITEM;
typedef struct _DOCKHEADER {
	short			count;
	PDOCKITEM		pNext;
}DOCKHEADER, *PDOCKHEADER;

typedef struct {
	PDOCKHEADER		pLeft;
	PDOCKHEADER		pTop;
	PDOCKHEADER		pRight;
	PDOCKHEADER		pBottom;
}PDHEADERS, *P_PDHEADERS;

typedef struct {
	HWND			hlu;
	HWND			hld;
	HWND			htl;
	HWND			htr;
	HWND			hru;
	HWND			hrd;
	HWND			hbl;
	HWND			hbr;
}DARROWS, *P_DARROWS;

typedef struct {
	HWND			hNote;
	int				iFindReplace;
}NFIND, *P_NFIND;

typedef struct _MEMNOTE * PMEMNOTE;
typedef struct _MEMNOTE {
	HWND				hwnd;
	PMEMNOTE			next;
	PMEMNOTE			prev;
	P_NOTE_RTHANDLES	pRTHandles;
	P_NOTE_RTHANDLES	pSavedHandles;
	P_NOTE_FLAGS		pFlags;
	P_NOTE_APPEARANCE	pAppearance;
	P_SCHEDULE_TYPE		pSchedule;
	P_NOTE_DATA			pData;
	BOOL				isAlarm;
	int					reserved;
} _MEMNOTE;

typedef struct {
	LPPOINT	pPoint;
	LPINT	pDockData;
	int		count;
}EMPTY_NOTES;

typedef struct _ADJUST_STRUCT {
	PMEMNOTE	pNote;
	int			type;
} ADJUST_STRUCT, *P_ADJUST_STRUCT;

typedef struct _GROUP_MENU_STRUCT {
	int			id;
	wchar_t		*pName;
} GROUP_MENU_STRUCT, *P_GROUP_MENU_STRUCT;

typedef struct _FAV_MENU_STRUCT {
	int			id;
	wchar_t		pName[128];
	wchar_t		pReserved[128];
} FAV_MENU_STRUCT, *P_FAV_MENU_STRUCT;

#endif	// __STRUCTURES_H__
