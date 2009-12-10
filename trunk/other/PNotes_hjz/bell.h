#ifndef __BELL_H__
#define __BELL_H__

//icons
#define	IDI_BELL_48				106
#define	IDI_BELL_32				107
#define	IDI_BELL_24				108
#define	IDI_BELL_16				109

HWND CreateBell(HWND hParent, HINSTANCE hInstance, int left, int top, BOOL onTop);

#endif	// BELL_H__
