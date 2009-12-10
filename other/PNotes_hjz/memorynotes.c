// Copyright (C) 2009 Andrey Gruber (aka lamer)

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

#include "notes.h"
#include "send.h"

static PMEMNOTE GetLastMemNote(void);
static void FreeSingleMemNote(PMEMNOTE pTemp);
static void ApplyNewSkinOfNote(HWND hNote, P_NOTE_RTHANDLES pH);
static void SaveNoteOnHide(PMEMNOTE pNote);

static PMEMNOTE		m_MemNotes;

PMEMNOTE MemoryNotes(void){
	return m_MemNotes;
}

PMEMNOTE AddMemNote(void){
	PMEMNOTE	pNew, pLast;

	pNew = calloc(1, sizeof(_MEMNOTE));
	if(pNew){
		pNew->pAppearance = calloc(1, sizeof(NOTE_APPEARANCE));
		pNew->pData = calloc(1, sizeof(NOTE_DATA));
		pNew->pFlags = calloc(1, sizeof(NOTE_FLAGS));
		pNew->pRTHandles = calloc(1, sizeof(NOTE_RTHANDLES));
		pNew->pSavedHandles = calloc(1, sizeof(NOTE_RTHANDLES));
		pNew->pSchedule = calloc(1, sizeof(SCHEDULE_TYPE));
		if(!pNew->pAppearance || !pNew->pData || !pNew->pFlags || !pNew->pRTHandles || !pNew->pSchedule){
			return NULL;
		}
		if(!m_MemNotes){
			m_MemNotes = pNew;
		}
		else{
			pLast = GetLastMemNote();
			pLast->next = pNew;
			pNew->prev = pLast;
		}
	}
	return pNew;
}

int NotesCount(void){
	int 		result = 0;
	PMEMNOTE	pTemp = m_MemNotes;

	while(pTemp){
		result++;
		pTemp = pTemp->next;
	}
	return result;
}

int CountNotesInGroup(int id){
	int 		result = 0;
	PMEMNOTE	pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->idGroup == id)
			result++;
		pTemp = pTemp->next;
	}
	return result;
}

int CountNotesAlive(void){
	int 		result = 0;
	PMEMNOTE	pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->idGroup != GROUP_RECYCLE)
			result++;
		pTemp = pTemp->next;
	}
	return result;
}

P_NOTE_APPEARANCE NoteAppearance(HWND hwnd){
	PMEMNOTE	pTemp = MemNoteByHwnd(hwnd);

	if(pTemp)
		return pTemp->pAppearance;
	return NULL;
}

P_NOTE_RTHANDLES NoteRTHandles(HWND hwnd){
	PMEMNOTE	pTemp = MemNoteByHwnd(hwnd);

	if(pTemp)
		return pTemp->pRTHandles;
	return NULL;
}

P_NOTE_RTHANDLES NoteSavedHandles(HWND hwnd){
	PMEMNOTE	pTemp = MemNoteByHwnd(hwnd);

	if(pTemp)
		return pTemp->pSavedHandles;
	return NULL;
}

P_NOTE_FLAGS NoteFlags(HWND hwnd){
	PMEMNOTE	pTemp = MemNoteByHwnd(hwnd);

	if(pTemp)
		return pTemp->pFlags;
	return NULL;
}

P_NOTE_DATA NoteData(HWND hwnd){
	PMEMNOTE	pTemp = MemNoteByHwnd(hwnd);

	if(pTemp)
		return pTemp->pData;
	return NULL;
}

P_SCHEDULE_TYPE NoteSchedule(HWND hwnd){
	PMEMNOTE	pTemp = MemNoteByHwnd(hwnd);

	if(pTemp)
		return pTemp->pSchedule;
	return NULL;
}

PMEMNOTE MemNoteByHwnd(HWND hwnd){
	PMEMNOTE	pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->hwnd == hwnd){
			return pTemp;
		}
		pTemp = pTemp->next;
	}
	return NULL;
}

PMEMNOTE MemNoteByTimer(UINT_PTR idEvent){
	PMEMNOTE	pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pSchedule->scType != SCH_NO){
			if(pTemp->pRTHandles->idTimer == idEvent){
				return pTemp;
			}
		}
		pTemp = pTemp->next;
	}
	return NULL;
}

void FreeMemNotes(void){
	PMEMNOTE	pTemp = m_MemNotes, pNext;
	
	while(pTemp){
		pNext = pTemp->next;
		FreeSingleMemNote(pTemp);
		free(pTemp);
		pTemp = pNext;
	}
	m_MemNotes = NULL;
}

void DeleteMemNote(PMEMNOTE pNote){
	if(!pNote->prev && !pNote->next){
		//single note
		FreeSingleMemNote(pNote);
		free(pNote);
		m_MemNotes = NULL;
		return;
	}
	else if(pNote->prev && pNote->next){
		//note is in the middle of the list
		PMEMNOTE	pNext, pPrev;
		pPrev = pNote->prev;
		pNext = pNote->next;
		pPrev->next = pNext;
		pNext->prev = pPrev;
	}
	else if(pNote->next){
		//the first note
		pNote->next->prev = NULL;
		m_MemNotes = pNote->next;
	}
	else if(pNote->prev){
		//the last note
		pNote->prev->next = NULL;
	}
	FreeSingleMemNote(pNote);
	free(pNote);
}

static void FreeSingleMemNote(PMEMNOTE pTemp){
	if(pTemp->pRTHandles->idTimer != 0){
		KillTimer(NULL, pTemp->pRTHandles->idTimer);
	}
	if(pTemp->pRTHandles->hbBackground)
		DeleteObject(pTemp->pRTHandles->hbBackground);
	if((pTemp->pAppearance->nPrivate & F_SKIN) == F_SKIN){
		DeleteObject(pTemp->pRTHandles->hbSkin);
		DeleteObject(pTemp->pRTHandles->hbSys);
		DeleteObject(pTemp->pRTHandles->hbPattern);
		DeleteObject(pTemp->pRTHandles->hbDelHide);
	}
	if((pTemp->pAppearance->nPrivate & F_C_FONT) == F_C_FONT)
		DeleteObject(pTemp->pRTHandles->hFCaption);
	free(pTemp->pAppearance);
	free(pTemp->pData);
	free(pTemp->pFlags);
	free(pTemp->pRTHandles);
	free(pTemp->pSavedHandles);
	free(pTemp->pSchedule);
}

static PMEMNOTE GetLastMemNote(void){
	PMEMNOTE	pTemp, pNext = m_MemNotes;
	
	while(pNext){
		pTemp = pNext;
		pNext = pTemp->next;
	}
	return pTemp;
}

void ApplyNewDockSkin(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->idGroup != GROUP_RECYCLE){
			if(pTemp->pData->visible && DockType(pTemp->pData->dockData) != DOCK_NONE){
				if(g_DockSettings.fCustSkin){
					ApplyNewSkinOfNote(pTemp->hwnd, &g_DRTHandles);
				}
				else{
					ApplyNewSkinOfNote(pTemp->hwnd, &g_RTHandles);
				}
			}
		}
		pTemp = pTemp->next;
	}
}

void ApplyNewCommonSkin(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->idGroup != GROUP_RECYCLE){
			if((pTemp->pAppearance->nPrivate & F_SKIN) != F_SKIN){
				//the note has not individual skin
				wcscpy(pTemp->pAppearance->szSkin, g_Appearance.szSkin);
				int rep = pTemp->pRTHandles->nRepeat;
				int tm = pTemp->pRTHandles->idTimer;
				memcpy(pTemp->pRTHandles, &g_RTHandles, sizeof(NOTE_RTHANDLES));
				pTemp->pRTHandles->nRepeat = rep;
				pTemp->pRTHandles->idTimer = tm;
				if(pTemp->pData->visible){
					if(DockType(pTemp->pData->dockData) == DOCK_NONE){
						//apply new skin for not docked window
						ApplyNewSkinOfNote(pTemp->hwnd, pTemp->pRTHandles);
					}
					else{
						if(!g_DockSettings.fCustSkin){
							//apply new skin if global settings for custom dock skin are FALSE
							ApplyNewSkinOfNote(pTemp->hwnd, pTemp->pRTHandles);
						}
					}
				}
			}
		}
		pTemp = pTemp->next;
	}
}

void ApplyNewBackgroundColor(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->idGroup != GROUP_RECYCLE){
			if((pTemp->pAppearance->nPrivate & F_B_COLOR) != F_B_COLOR){
				pTemp->pAppearance->crWindow = g_Appearance.crWindow;
				if(pTemp->pData->visible){
					SendMessageW(pTemp->hwnd, CHM_NEW_BGCOLOR, 0, 0);
				}
			}
		}
		pTemp = pTemp->next;
	}					
}

void ApplyNewCaptionFont(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->idGroup != GROUP_RECYCLE){
			if((pTemp->pAppearance->nPrivate & F_C_FONT) != F_C_FONT){
				pTemp->pRTHandles->hFCaption = g_RTHandles.hFCaption;
				memcpy(&pTemp->pAppearance->lfCaption, &g_Appearance.lfCaption, sizeof(pTemp->pAppearance->lfCaption));
				if(pTemp->pData->visible){
					RedrawWindow(pTemp->hwnd, NULL, NULL, RDW_INVALIDATE);
					ShowNoteMarks(pTemp->hwnd);
				}
			}
		}
		pTemp = pTemp->next;
	}	
}

void ApplyNewCaptionColor(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->idGroup != GROUP_RECYCLE){
			if((pTemp->pAppearance->nPrivate & F_C_COLOR) != F_C_COLOR){
				pTemp->pAppearance->crCaption = g_Appearance.crCaption;
				if(pTemp->pData->visible){
					RedrawWindow(pTemp->hwnd, NULL, NULL, RDW_INVALIDATE);
					ShowNoteMarks(pTemp->hwnd);
				}
			}
		}
		pTemp = pTemp->next;
	}		
}

void ApplyNewFont(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->idGroup != GROUP_RECYCLE){
			if((pTemp->pAppearance->nPrivate & F_RTFONT) != F_RTFONT){
				if(pTemp->pData->visible){
					//set edit box new font
					SendMessageW(pTemp->hwnd, CHM_CHANGE_FONT, (WPARAM)&g_Appearance.lfFont, 0);
					RedrawWindow(pTemp->hwnd, NULL, NULL, RDW_INVALIDATE);
					ShowNoteMarks(pTemp->hwnd);
				}
			}
		}
		pTemp = pTemp->next;
	}
}

void ApplyNewTextColor(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->idGroup != GROUP_RECYCLE){
			if((pTemp->pAppearance->nPrivate & F_RTFONT) != F_RTFONT){
				//redraw
				pTemp->pAppearance->crFont = g_Appearance.crFont;
				if(pTemp->pData->visible){
					SendMessageW(pTemp->hwnd, CHM_CHANGE_FONT_COLOR, (WPARAM)pTemp->pAppearance->crFont, 0);
					RedrawWindow(pTemp->hwnd, NULL, NULL, RDW_INVALIDATE);
					ShowNoteMarks(pTemp->hwnd);
				}
			}
		}
		pTemp = pTemp->next;
	}
}

void ApplyJustRedraw(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->idGroup != GROUP_RECYCLE){
			if(pTemp->pData->visible){
				RedrawWindow(pTemp->hwnd, NULL, NULL, RDW_INVALIDATE);
				ShowNoteMarks(pTemp->hwnd);
			}
		}
		pTemp = pTemp->next;
	}
}

void ApplyNewLanguage(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->idGroup != GROUP_RECYCLE){
			if(pTemp->pData->visible){
				SendMessageW(pTemp->hwnd, CHM_NEW_LANGUAGE, 0, 0);
			}
		}
		pTemp = pTemp->next;
	}
}

void ApplyTransparencyValue(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->idGroup != GROUP_RECYCLE){
			pTemp->pFlags->transparent = TRUE;
			if(pTemp->pData->visible){
				SetLayeredWindowAttributes(pTemp->hwnd, pTemp->pRTHandles->crMask, g_NoteSettings.transValue, LWA_COLORKEY | LWA_ALPHA);
			}
		}
		pTemp = pTemp->next;
	}
}

void ApplyRestrictTransparency(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->idGroup != GROUP_RECYCLE){		
			pTemp->pFlags->transparent = FALSE;
			if(pTemp->pData->visible){
				SetLayeredWindowAttributes(pTemp->hwnd, pTemp->pRTHandles->crMask, 255, LWA_COLORKEY);
			}
		}
		pTemp = pTemp->next;
	}
}

void ApplyAutosaveNotes(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(!pTemp->pFlags->saved && pTemp->pData->idGroup != GROUP_RECYCLE){
			AutosaveNote(pTemp);
		}
		pTemp = pTemp->next;
	}
}

void ApplySaveAllNotes(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->idGroup != GROUP_RECYCLE){
			if(!pTemp->pFlags->saved){
				if(pTemp->pFlags->fromDB)
					SaveNote(pTemp);
				else
					SaveNoteAs(pTemp);
			}
		}
		pTemp = pTemp->next;
	}
}

void ApplyCloseAllNotes(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->visible){
			SendMessageW(pTemp->hwnd, WM_CLOSE, 0, 0);
		}
		pTemp = pTemp->next;
	}
}

void ApplyOnTopChanged(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->visible){
			if(pTemp->pData->onTop){
				SetWindowPos(pTemp->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
		}
		pTemp = pTemp->next;
	}
}

void ApplyDockingColors(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->visible){
			if(DockType(pTemp->pData->dockData) != DOCK_NONE){
				ApplyDockColors(pTemp);
				RedrawWindow(pTemp->hwnd, NULL, NULL, RDW_INVALIDATE);
				ShowNoteMarks(pTemp->hwnd);
			}
		}
		pTemp = pTemp->next;
	}
}

void ApplyHideAllNotes(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->visible){
			HideNote(pTemp);
		}
		pTemp = pTemp->next;
	}
	//if(g_hCPDialog){
		//SendMessageW(g_hCPDialog, CHM_RELOAD, 0, 0);
	//}
}

void ApplySessionLogOn(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->visible){
			ShowWindow(pTemp->hwnd, SW_HIDE);
			ShowWindow(pTemp->hwnd, SW_SHOW);
		}
		pTemp = pTemp->next;
	}
}

void ApplyDockSizeOrderChanged(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->visible){
			if(DockType(pTemp->pData->dockData) != DOCK_NONE){
				HWND hTray = MoveDockWindow(pTemp->hwnd, DockType(pTemp->pData->dockData), DockIndex(pTemp->pData->dockData));
				if(hTray)
					SetWindowPos(hTray, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
			}
		}
		pTemp = pTemp->next;
	}
}

void BringNotesToFront(void){
	PMEMNOTE			pTemp = m_MemNotes;

	while(pTemp){
		if(pTemp->pData->visible){
			ShowNote(pTemp);
		}
		pTemp = pTemp->next;
	}
}

void ApplySaveOnUnload(int args, BOOL beforeReload){
	PMEMNOTE			pTemp = m_MemNotes;
	wchar_t				szMessage[256];
	RECT				rcSave;

	while(pTemp){
		if(pTemp->pData->idGroup != GROUP_RECYCLE){
			if(!pTemp->pFlags->saved){
				if(g_NoteSettings.confirmSave && (args & ARG_SILENT) != ARG_SILENT){
					if(pTemp->pData->visible){
						BringWindowToTop(pTemp->hwnd);
					}
					ConstructSaveMessage(pTemp->pData->szName, szMessage);
					if(MessageBoxW(g_hMain, szMessage, g_Strings.SCaption, MB_YESNO | MB_ICONQUESTION) == IDYES){
						if(pTemp->pFlags->fromDB)
							SaveNote(pTemp);
						else
							SaveNoteAs(pTemp);
					}
					else{
						if(pTemp->pFlags->fromDB){
							if(pTemp->pData->visible){
								//save window position
								GetWindowRect(pTemp->hwnd, &rcSave);
								if(pTemp->pRTHandles->rolled){
									//get unrolled size
									rcSave.bottom = rcSave.top + (pTemp->pRTHandles->rcSize.bottom - pTemp->pRTHandles->rcSize.top);
								}
								pTemp->pData->rolled = pTemp->pRTHandles->rolled;
								// CopyRect(&pD->rcp, &wp.rcNormalPosition);
								CopyRect(&pTemp->pData->rcp, &rcSave);
								if(DockType(pTemp->pData->dockData) == DOCK_NONE){
									WritePrivateProfileStructW(pTemp->pFlags->id, S_DATA, pTemp->pData, sizeof(NOTE_DATA), g_NotePaths.DataFile);
								}
							}
						}
					}
				}
				else{		//just save without prompting
					if((args & ARG_NO_SAVE) != ARG_NO_SAVE){
						if(pTemp->pFlags->fromDB){
							SaveNote(pTemp);
						}
						else{
							if(pTemp->pData->visible){
								SendMessageW(pTemp->hwnd, CHM_SAVE_ON_SHUTDOWN, (WPARAM)pTemp->pFlags, 0);
							}
						}
					}
				}
			}
			else{			//just save note position and rectangle
				//save window position
				if(pTemp->pData->visible){
					GetWindowRect(pTemp->hwnd, &rcSave);
					if(pTemp->pRTHandles->rolled){
						//get unrolled size
						rcSave.bottom = rcSave.top + (pTemp->pRTHandles->rcSize.bottom - pTemp->pRTHandles->rcSize.top);
					}
					pTemp->pData->rolled = pTemp->pRTHandles->rolled;
					CopyRect(&pTemp->pData->rcp, &rcSave);
					if(pTemp->pFlags->fromDB){
						if(DockType(pTemp->pData->dockData) == DOCK_NONE){				
							WritePrivateProfileStructW(pTemp->pFlags->id, S_DATA, pTemp->pData, sizeof(NOTE_DATA), g_NotePaths.DataFile);
						}
					}
					else{
						WritePrivateProfileStringW(S_FAVORITES, pTemp->pFlags->id, NULL, g_NotePaths.INIFile);
						if(beforeReload){
							//save just created and not changed window position in order to reload it
							g_Empties.pPoint = realloc(g_Empties.pPoint, ++g_Empties.count * sizeof(POINT));
							LPPOINT lpp = g_Empties.pPoint;
							lpp += (g_Empties.count - 1) * sizeof(POINT);
							lpp->x = rcSave.left;
							lpp->y = rcSave.top;
							g_Empties.pDockData = realloc(g_Empties.pDockData, g_Empties.count * sizeof(int));
							LPINT	lpi = g_Empties.pDockData;
							lpi += (g_Empties.count - 1) * sizeof(int);
							*lpi = pTemp->pData->dockData;
						}
					}
				}
			}
		}
		pTemp = pTemp->next;
	}
}

void EmptyRecycleBin(void){
	PMEMNOTE			pTemp = m_MemNotes, pNext;

	while(pTemp){
		if(pTemp->pData->idGroup == GROUP_RECYCLE){
			pNext = pTemp->next;
			DeleteNoteCompletely(pTemp);
			pTemp = pNext;
		}
		else{
			pTemp = pTemp->next;
		}
	}
}

void SendMultipleNotesAsAttachments(LONG_PTR *pNotes, int count){
	PMEMNOTE		pNote;
	wchar_t			**pTemp, **pFree, szTemp[MAX_PATH + 128];

	pTemp = (wchar_t **)calloc(count, sizeof(wchar_t *));
	pFree = pTemp;
	for(int i = 0; i < count; i++){
		pNote = (PMEMNOTE)*pNotes++;
		*pTemp = (calloc)(MAX_PATH + wcslen(pNote->pFlags->id) + 8, sizeof(wchar_t));
		wcscpy(*pTemp, g_NotePaths.DataDir);
		wcscat(*pTemp, pNote->pFlags->id);
		wcscat(*pTemp, L".pnote");
		pTemp++;
	}
	SendAsAttachment(g_hMain, pFree, count, DEF_MAIL_SUBJECT, szTemp);
	pTemp = pFree;
	for(int i = 0; i < count; i++){
		free(*pTemp++);
	}
	free(pFree);
}

void SendNoteAsAttachment(PMEMNOTE pNote){
	wchar_t			szPath[MAX_PATH + 128], szTemp[MAX_PATH + 128], *pTemp[1];

	wcscpy(szPath, g_NotePaths.DataDir);
	wcscat(szPath, pNote->pFlags->id);
	wcscat(szPath, L".pnote");
	pTemp[0] = szPath;
	SendAsAttachment(g_hMain, pTemp, 1, DEF_MAIL_SUBJECT, szTemp);
}

void ShowNote(PMEMNOTE pNote){
	if(!pNote->pData->visible){
		CreateNote(pNote, g_hInstance, g_hMain, FALSE, NULL);
		SaveVisibility(pNote->hwnd, TRUE);
		ShowWindow(pNote->hwnd, SW_SHOW);
		//SetForegroundWindow(pNote->hwnd);
		if(g_hCPDialog){
			SendMessageW(g_hCPDialog, CHM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
		}
		if(g_hOverdueDialog){
			SendMessageW(g_hOverdueDialog, CHM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
		}
	}
	SetWindowPos(pNote->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	if(!pNote->pData->onTop){
		SetWindowPos(pNote->hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	}
	if(pNote->pData->rolled)
		RollNote(pNote);
}

void HideNote(PMEMNOTE pNote){
	if(DockType(pNote->pData->dockData) != DOCK_NONE){
		SendMessageW(pNote->hwnd, WM_COMMAND, MAKEWPARAM(IDM_DOCK_NONE, 0), 0);
	}
	//if(g_hCPDialog && reloadCP)
		//SendMessageW(g_hCPDialog, CHM_CTRL_UPD, 0, (LPARAM)pNote);
	if(g_hSearchDialog)
		SendMessageW(g_hSearchDialog, CHM_BECOMES_HIDDEN, (WPARAM)pNote, 0);
	if(g_hSearchTitlesDialog)
		SendMessageW(g_hSearchTitlesDialog, CHM_BECOMES_HIDDEN, (WPARAM)pNote, 0);
	if(!pNote->pFlags->saved){
		SaveNoteOnHide(pNote);
	}
	//close window
	SaveVisibility(pNote->hwnd, FALSE);
	SendMessageW(pNote->hwnd, WM_CLOSE, 0, 0);
	if(g_hCPDialog){
		SendMessageW(g_hCPDialog, CHM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
	}
	if(g_hOverdueDialog){
		SendMessageW(g_hOverdueDialog, CHM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
	}
}

void RestoreNoteFromBin(PMEMNOTE pNote, int toGroup){
	int			groupTemp;
	NOTE_DATA	data;

	if(toGroup == -1)
		groupTemp = pNote->pData->idPrevGroup;
	else
		//in case of grag onto different group
		groupTemp = toGroup;
	//check whether group exists
	if(GetGroup(groupTemp) == NULL)
		groupTemp = 0;
	pNote->pData->idGroup = groupTemp;
	if(IsBitOn(pNote->pData->res1, NB_PREV_VISIBLE)){
		pNote->pData->visible = TRUE;
		CreateNote(pNote, g_hInstance, g_hMain, FALSE, NULL);
		SaveVisibility(pNote->hwnd, TRUE);
	}
	if(pNote->pFlags->fromDB){
		if(GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
			data.idGroup = pNote->pData->idGroup;
			WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
		}
	}
	if(pNote->pSchedule->scType != 0){
		pNote->isAlarm = TRUE;
		if(pNote->pSchedule->scType == SCH_REP)	//set repeat count according to timer interval
			pNote->pRTHandles->nRepeat = SetRepeatCount(&pNote->pSchedule->scDate);
		pNote->pRTHandles->idTimer = SetTimer(NULL, 0, NOTE_TIMER_INTERVAL, NoteTimerProc);
	}
}

void DeleteToBin(PMEMNOTE pNote, int withGroup){
	NOTE_DATA			data;

	if(withGroup == -1)
		pNote->pData->idPrevGroup = pNote->pData->idGroup;
	else
		pNote->pData->idPrevGroup = 0;
	pNote->pData->idGroup = GROUP_RECYCLE;
	if(pNote->pFlags->fromDB){
		if(GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
			data.idGroup = pNote->pData->idGroup;
			data.idPrevGroup = pNote->pData->idPrevGroup;
			WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
		}
	}
	if(pNote->pData->visible){
		BitOn(&pNote->pData->res1, NB_PREV_VISIBLE);
		SaveVisibility(pNote->hwnd, FALSE);
		if(DockType(pNote->pData->dockData) != DOCK_NONE){
			SendMessageW(pNote->hwnd, WM_COMMAND, MAKEWPARAM(IDM_DOCK_NONE, 0), 0);
		}
		SendMessageW(pNote->hwnd, WM_CLOSE, 0, 0);
	}
	else{
		BitOff(&pNote->pData->res1, NB_PREV_VISIBLE);
	}
	pNote->pData->visible = FALSE;
}

void ShowGroupOfNotes(int group){
	PMEMNOTE	pNote = MemoryNotes();

	while(pNote){
		if(pNote->pData->idGroup > GROUP_RECYCLE && pNote->pData->idGroup == group){
			ShowNote(pNote);
		}
		pNote = pNote->next;
	}
}

void HideGroupOfNotes(int group){
	PMEMNOTE	pNote = MemoryNotes();

	while(pNote){
		if(pNote->pData->idGroup > GROUP_RECYCLE){
			if(pNote->pData->idGroup == group){
				if(pNote->pData->visible){
					HideNote(pNote);
				}
			}
		}
		pNote = pNote->next;
	}
}

void ToggleNoteOnTop(PMEMNOTE pNote){
	NOTE_DATA		data;

	if(!pNote->pData->onTop){
		pNote->pData->onTop = TRUE;
		pNote->pData->prevOnTop = TRUE;
		if(pNote->pData->visible){
			SetWindowPos(pNote->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}
	else{
		pNote->pData->onTop = FALSE;
		pNote->pData->prevOnTop = FALSE;
		if(pNote->pData->visible){
			SetWindowPos(pNote->hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			SendMessageW(g_hMain, CHM_ON_TOP_CHANGED, 0, 0);
		}
	}
	if(pNote->pData->visible){
		RedrawWindow(pNote->hwnd, NULL, NULL, RDW_INVALIDATE);
		ShowNoteMarks(pNote->hwnd);
	}
	if(pNote->pFlags->fromDB){
		if(GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
			if(pNote->pData->onTop){
				data.onTop = TRUE;
				data.prevOnTop = TRUE;
			}
			else{
				data.onTop = FALSE;
				data.prevOnTop = FALSE;
			}
			WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
		}
	}
}

void ToggleNoteProtection(PMEMNOTE pNote){
	NOTE_DATA		data;

	if(!IsBitOn(pNote->pData->res1, NB_PROTECTED)){
		BitOn(&pNote->pData->res1, NB_PROTECTED);
	}
	else{
		BitOff(&pNote->pData->res1, NB_PROTECTED);
	}
	if(pNote->pData->visible){
		RedrawWindow(pNote->hwnd, NULL, NULL, RDW_INVALIDATE);
		ShowNoteMarks(pNote->hwnd);
	}
	if(g_hCPDialog)
		SendMessageW(g_hCPDialog, CHM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
	if(pNote->pFlags->fromDB){
		if(GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
			if(IsBitOn(pNote->pData->res1, NB_PROTECTED))
				BitOn(&data.res1, NB_PROTECTED);
			else
				BitOff(&data.res1, NB_PROTECTED);
			WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
		}
	}
}

void ToggleNoteCompleted(PMEMNOTE pNote){
	NOTE_DATA		data;

	if(!IsBitOn(pNote->pData->res1, NB_COMPLETED)){
		BitOn(&pNote->pData->res1, NB_COMPLETED);
	}
	else{
		BitOff(&pNote->pData->res1, NB_COMPLETED);
	}
	if(pNote->pData->visible){
		RedrawWindow(pNote->hwnd, NULL, NULL, RDW_INVALIDATE);
		ShowNoteMarks(pNote->hwnd);
	}
	if(g_hCPDialog)
		SendMessageW(g_hCPDialog, CHM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
	if(pNote->pFlags->fromDB){
		if(GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
			if(IsBitOn(pNote->pData->res1, NB_COMPLETED))
				BitOn(&data.res1, NB_COMPLETED);
			else
				BitOff(&data.res1, NB_COMPLETED);
			WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
		}
	}
}

void ToggleNotePriority(PMEMNOTE pNote){
	NOTE_DATA		data;

	if(!IsBitOn(pNote->pData->res1, NB_HIGH_PRIORITY)){
		BitOn(&pNote->pData->res1, NB_HIGH_PRIORITY);
	}
	else{
		BitOff(&pNote->pData->res1, NB_HIGH_PRIORITY);
	}
	if(pNote->pData->visible){
		RedrawWindow(pNote->hwnd, NULL, NULL, RDW_INVALIDATE);
		ShowNoteMarks(pNote->hwnd);
	}
	if(g_hCPDialog)
		SendMessageW(g_hCPDialog, CHM_CTRL_ITEM_UPDATE, 0, (LPARAM)pNote);
	if(pNote->pFlags->fromDB){
		if(GetPrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile)){
			if(IsBitOn(pNote->pData->res1, NB_HIGH_PRIORITY))
				BitOn(&data.res1, NB_HIGH_PRIORITY);
			else
				BitOff(&data.res1, NB_HIGH_PRIORITY);
			WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, &data, sizeof(NOTE_DATA), g_NotePaths.DataFile);
		}
	}
}

void SavePNote(PMEMNOTE pNote){
	BOOL		in_favorites = FALSE;
	wchar_t		szBuffer[256];
	int			result;

	if(pNote->pFlags->fromDB)
		SaveNote(pNote);
	else{
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
	}
}

static void ApplyNewSkinOfNote(HWND hNote, P_NOTE_RTHANDLES pH){
	RECT				rc;
	BITMAP				bm;

	GetWindowRect(hNote, &rc);
	GetObjectW(pH->hbSkin, sizeof(bm), &bm);
	//resize controls on note window
	SendMessage(hNote, CHM_RESIZE, (WPARAM)&pH->rcEdit, 0);
	//resize note window
	MoveWindow(hNote, rc.left, rc.top, bm.bmWidth, bm.bmHeight, TRUE);
	RedrawWindow(hNote, NULL, NULL, RDW_INVALIDATE);
	ShowNoteMarks(hNote);
	//update tooltip
	SendMessageW(hNote, CHM_TT_UPDATE, 0, 0);
}

static void SaveNoteOnHide(PMEMNOTE pNote){
	wchar_t				szMessage[256];
	RECT				rcSave;

	BringWindowToTop(pNote->hwnd);
	ConstructSaveMessage(pNote->pData->szName, szMessage);
	if(MessageBoxW(pNote->hwnd, szMessage, g_Strings.SCaption, MB_YESNO | MB_ICONQUESTION) == IDYES){
		if(pNote->pFlags->fromDB)
			SaveNote(pNote);
		else
			SaveNoteAs(pNote);
	}
	else{
		pNote->pFlags->saved = TRUE;
		if(pNote->pFlags->fromDB){
			if(pNote->pData->visible){
				//save window position
				GetWindowRect(pNote->hwnd, &rcSave);
				if(pNote->pRTHandles->rolled){
					//get unrolled size
					rcSave.bottom = rcSave.top + (pNote->pRTHandles->rcSize.bottom - pNote->pRTHandles->rcSize.top);
				}
				pNote->pData->rolled = pNote->pRTHandles->rolled;
				// CopyRect(&pD->rcp, &wp.rcNormalPosition);
				CopyRect(&pNote->pData->rcp, &rcSave);
				if(DockType(pNote->pData->dockData) == DOCK_NONE){
					WritePrivateProfileStructW(pNote->pFlags->id, S_DATA, pNote->pData, sizeof(NOTE_DATA), g_NotePaths.DataFile);
				}
			}
		}
	}	
}
