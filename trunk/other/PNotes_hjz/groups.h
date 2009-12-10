#ifndef __GROUPS_H__
#define __GROUPS_H__

int GetGroupsCount(void);
void GetAllGroups(void);
void DeleteGroup(int id);
int NewGroupId(void);
void AddGroup(P_PNGROUP pg);
P_PNGROUP GetGroup(int id);
void FreeGroups(void);
void SaveGroup(P_PNGROUP ppg);
P_PNGROUP GetGroups(void);

#endif	// GROUPS_H__
