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

static P_PNGROUP GetLastGroup(void);

static P_PNGROUP		m_pGroups = NULL;
static int				m_GroupCount = 0;

int GetGroupsCount(void){
	return m_GroupCount;
}

void GetAllGroups(void){

	wchar_t				szKeys[4096], *pw;
	PNGROUP				pg;

	if(m_pGroups == NULL){
		GetPrivateProfileStringW(S_GROUPS, NULL, NULL, szKeys, 4096, g_NotePaths.INIFile);
		pw = szKeys;
		if(*pw){
			while(*pw || *(pw - 1)){
				if(*pw && *pw != 31888){
					if(GetPrivateProfileStructW(S_GROUPS, pw, &pg, sizeof(pg), g_NotePaths.INIFile)){
						if(_wtoi(pw) != pg.id){
							pg.id = _wtoi(pw);
							WritePrivateProfileStructW(S_GROUPS, pw, &pg, sizeof(pg), g_NotePaths.INIFile);
						}
						AddGroup(&pg);
					}
				}
				while(*pw++)
					;
			}
		}
	}
}

void DeleteGroup(int id){
	P_PNGROUP	pTemp, pNext;
	P_PNGROUP	pPrev = NULL;
	wchar_t		szKey[24];

	pTemp = m_pGroups;
	while(pTemp){
		pNext = pTemp->next;
		if(pTemp->id == id){
			if(pTemp == m_pGroups && pNext == NULL){
				//there is only one group
				free(pTemp);
				m_pGroups = NULL;
			}
			else if(pTemp == m_pGroups && pNext != NULL){
				//delete the very first group
				free(pTemp);
				m_pGroups = pNext;
			}
			else if(pPrev != NULL){
				//delete group in the middle of list or very last group
				free(pTemp);
				pPrev->next = pNext; //in case of last group it will be NULL
			}
			//delete group from INI file
			_itow(id, szKey, 10);
			WritePrivateProfileStructW(S_GROUPS, szKey, NULL, sizeof(PNGROUP), g_NotePaths.INIFile);
			m_GroupCount--;
			break;
		}
		pPrev = pTemp;
		pTemp = pTemp->next;
	}
}

int NewGroupId(void){
	P_PNGROUP	pTemp = m_pGroups;
	int			id = 0;

	if(!pTemp)
		return 0;
	while(pTemp){
		if(pTemp->id > id)
			id = pTemp->id;
		pTemp = pTemp->next;
	}
	id++;

	return id;
}

static P_PNGROUP GetLastGroup(void){
	P_PNGROUP	pTemp, pNext = m_pGroups;
	
	while(pNext){
		pTemp = pNext;
		pNext = pTemp->next;
	}
	return pTemp;
}

void AddGroup(P_PNGROUP pg){
	P_PNGROUP	pTemp, pNew;

	pg->next = NULL;
	pNew = (P_PNGROUP)malloc(sizeof(PNGROUP));
	memcpy(pNew, pg, sizeof(PNGROUP));
	if(!m_pGroups){
		m_pGroups = pNew;
	}
	else{
		pTemp = GetLastGroup();
		pTemp->next = pNew;
	}
	m_GroupCount++;
}

P_PNGROUP GetGroup(int id){
	P_PNGROUP	pTemp = m_pGroups;

	do{
		if(pTemp->id == id){
			return pTemp;
		}
		pTemp = pTemp->next;
	}while(pTemp);
	return NULL;
}

void FreeGroups(void){
	P_PNGROUP	pTemp = m_pGroups, pNext;

	while(pTemp){
		pNext = pTemp->next;
		free(pTemp);
		pTemp = pNext;
	}
}

void SaveGroup(P_PNGROUP ppg){
	wchar_t		szKey[24];

	_itow(ppg->id, szKey, 10);
	WritePrivateProfileStructW(S_GROUPS, szKey, ppg, sizeof(PNGROUP), g_NotePaths.INIFile);
}

P_PNGROUP GetGroups(void){
	return m_pGroups;
}

