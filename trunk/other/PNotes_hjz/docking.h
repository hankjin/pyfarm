#ifndef __DOCKING_H__
#define __DOCKING_H__

#include "structures.h"

PDOCKITEM AddDockItem(PDOCKHEADER pHeader, HWND hwnd, int index);
void FreeDockList(PDOCKHEADER pHeader);
void RemoveDockItem(PDOCKHEADER pHeader, HWND hwnd);
PDOCKITEM NextDockItem(PDOCKHEADER pHeader, HWND hwnd);
void SortDockList(PDOCKHEADER pHeader);
short LastIndex(PDOCKHEADER pHeader);
#endif	// DOCKING_H__
