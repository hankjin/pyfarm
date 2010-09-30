/**
ID: hankjin1
LANG: C++
TASK: prefix
*/
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;
//note: raw may be multiline, so use while and strcat to readall
//note: there are 200 pats at most, but consider the ., it should be 201

ifstream fin("prefix.in");
ofstream fout("prefix.out");
struct _pat{
    char value[11];
    int len;
}pats[201];
int patNum;
char raw[200001];
char temp[200001];
bool reachable[200001];

int main(){
    patNum=-1;
    do{
	fin>>pats[++patNum].value;
	pats[patNum].len = strlen(pats[patNum].value);
    }while(strcmp(pats[patNum].value,".")!=0);
    raw[0]='\0';
    while(fin>>temp){
	strcat(raw, temp);
    }
    int n = strlen(raw);
    memset(reachable,0,sizeof(bool)*200001);
    int maxN = 0;
    int i,j;
    reachable[0]=true;
    for(i=0;i<n;i++){
	if(!reachable[i])
	    continue;
	for(j=0;j<patNum;j++){
	    if(strncmp(&raw[i],pats[j].value,pats[j].len)==0){
		reachable[i+pats[j].len]=true;
		maxN = max(maxN, i+pats[j].len);
	    }
	}
    }
    fout<<maxN<<endl;
    return 0;
}
