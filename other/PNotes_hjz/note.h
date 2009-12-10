#ifndef __NOTE_H__
#define __NOTE_H__

#include "structures.h"

BOOL RegisterNoteClass(HINSTANCE hInstance);
void LoadNoteProperties(PMEMNOTE pNote, P_NOTE_RTHANDLES pHandles, wchar_t * lpID, BOOL loadNew, wchar_t * lpFile);
HWND CreateNote(PMEMNOTE pNote, HINSTANCE hInstance, HWND hwndParent, BOOL loadNew, wchar_t * lpFile);
void SetNoteMIText(int id, wchar_t * lpText);
void SetEditMIText(int id, wchar_t * lpText);
void SetScheduleForPrevious(HWND hwnd, wchar_t * lpID, wchar_t * lpFile);
void AddColorsMenu(void);
void SetNoteMIImageIndex(int id, int index);
PMItem NoteMenu(void);
PMItem EditMenu(void);
PMItem DropMenu(void);
int NoteMenuCount(void);
int EditMenuCount(void);
int DropMenuCount(void);
void SaveNote(PMEMNOTE pNote);
void SaveNoteAs(PMEMNOTE pNote);
void AutosaveNote(PMEMNOTE pNote);
void SaveVisibility(HWND hwnd, BOOL visible);
void DeleteRTFile(wchar_t * id);
int SetRepeatCount(LPSYSTEMTIME lst);
void CALLBACK NoteTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
BOOL CALLBACK SaveAs_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK Adjust_DlgProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void CenterRestorePlacementNote(PMEMNOTE pNote, int operation);
void AddToFavorites(PMEMNOTE pNote);
void DeleteNoteCompletely(PMEMNOTE pNote);
void RollNote(PMEMNOTE pNote);
void UnrollNote(PMEMNOTE pNote);
void ShowNoteMarks(HWND hwnd);
#endif	// NOTE_H__
