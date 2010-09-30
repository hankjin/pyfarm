/*
ID: hankjin1
LANG: C++
TASK: lgame
*/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <map>
using namespace std;

//lgame.in
char dest[8];

//lgame.dict
char dict[40002][8];
int dictN;

int filter[26];//max count of alpha
int filler[26];
int leftn[26];//non used alphas
int total;
//answer
int best;
char res[40002][9];
int resN;
const int v[26]={2,5,4,4,1,6,5,5,1,7,6,3,5,2,3,5,7,2,1,2,4,6,6,7,5,7};

void check(int x, int y){
    int ret=total;
    for(int i=0;dest[i]!=0;i++){
	ret -= v[dest[i]-'a'] * leftn[dest[i]-'a'];
	leftn[dest[i]-'a']=0;
    }
    if(ret < best)
	return;
    if(ret > best){
	best = ret;
	resN = 0;
    }
    if(y==-1){//one word
	strncpy(res[resN], dict[x], 8);
    }
    else{//two word
	if(strcmp(dict[x],dict[y])){
	    strncpy(res[resN],dict[x],8);
	    res[resN][strlen(dict[x])]=' ';
	    strcat(res[resN], dict[y]);
	}
	else{
	    strncpy(res[resN],dict[y],8);
	    res[resN][strlen(dict[y])]=' ';
	    strcat(res[resN], dict[x]);
	}
    }
    resN++;
}
int hcmp(const void *p1, const void *p2){
    char *c1 = (char *)p1;
    char *c2 = (char *)p2;
    return strcmp(c1,c2);
}
int main(){
    ifstream fin("lgame.in");
    ifstream din("lgame.dict");
    ofstream fout("lgame.out");
    fin>>dest;
    total=0;
    best=0;
    int i,j,k,n;
    for(i=0;dest[i]!=0;i++){
	filter[ dest[i]-'a' ] ++;
	total += v[dest[i]-'a'];
    }
    char word[8];
    dictN=0;
    for(din>>word; word[0]!='.'; din>>word){
	for(i=0;dest[i]!=0;i++)
	    filler[dest[i]-'a'] = filter[dest[i]-'a'];
	for(i=0;word[i]!=0;i++){
	    if(--filler[ word[i]-'a' ] < 0)
		break;
	}
	//1. only reasonable word insert to dict
	if(word[i]!=0)
	    continue;
	strncpy(dict[dictN++], word, 8);
    }
    resN=0;
    for(i=0;i<dictN;i++){
	//
	fill_n(filler, 26, 0);
	for(j=0;dict[i][j]!=0;j++)
	    filler[ dict[i][j]-'a' ] ++;
	for(j=0;dest[j]!=0;j++){
	    leftn[dest[j]-'a']=filter[dest[j]-'a']-filler[dest[j]-'a'];
	}
	//only i,
	check(i,-1);
	//if strlen(dict[i])>4 continue;
	for(j=i+1;j<dictN;j++){
	    //if strlen(dict[i]) + strlen(dict[i])>strlen(dest) continue;
	    for(k=0;dest[k]!=0;k++){
		leftn[dest[k]-'a']=filter[dest[k]-'a']-filler[dest[k]-'a'];
	    }
	    for(k=0;dict[j][k]!=0;k++){
		if(--leftn[dict[j][k]-'a'] < 0)
		    break;
	    }
	    //2. only reasonable combination 
	    if(dict[j][k]==0){
		check(i,j);	
	    }
	}
    }
    qsort(res, resN, 9, hcmp);
    fout<<best<<endl;
    for(i=0;i<resN;i++){
	fout<<res[i]<<endl;
    }
    
    return 0;
}
