#ifndef __LUNAR_H__
#define __LUNAR_H__

#define SCH_YR_LUNAR 0
#define SCH_YR_SOLAR 1
#include "notes.h"
void SetNextYear(SYSTEMTIME* pStart, SYSTEMTIME* pCurrent, int lunarOrSolar);
/*
int tolunar(SYSTEMTIME *);
int tosolar(SYSTEMTIME *);
*/
#endif
