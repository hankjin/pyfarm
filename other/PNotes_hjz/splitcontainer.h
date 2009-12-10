#ifndef __SPLITCONTAINER_H__
#define __SPLITCONTAINER_H__

#define	CHILD_TOP			0
#define	CHILD_BOTTOM		1
#define	CHILD_LEFT			2
#define	CHILD_RIGHT			3
#define	SPLIT_HORZ			0
#define	SPLIT_VERT			1
#define	SPM_BASE			WM_USER + 0x123
#define	SPM_ADD_CHILD		SPM_BASE + 1
#define	SPM_HIDE_CHILD		SPM_BASE + 2
#define	SPM_SHOW_CHILD		SPM_BASE + 3
#define	SPM_MAIN_RESIZED	SPM_BASE + 4
HWND CreateMainPanel(int splitStyle, HINSTANCE hInstance, HWND hParent, HCURSOR hCursor, int nStopper, int nLeft, int nTop, int nRight, int nBottom);

#endif	// SPLITCONTAINER_H__
