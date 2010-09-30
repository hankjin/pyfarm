/**
ID: hankjin1
LANG: C++
TASK: barn1
*/
//note, the input may be not sequenced
//note2, 
#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <vector>
using namespace std;

struct Stall{
    int min;
    int max;
    struct Stall *next;
};

Stall stall[200];
vector<int> as;
int main(){
    int M,S,C;
    int i,j,k,temp;
    ifstream fin("barn1.in");
    ofstream fout("barn1.out");
    fin>>M>>S>>C;
    if(M>=C){
	fout<<C<<endl;
	return 0;
    }
    for(i = 0; i < C; i ++){
	fin>>temp;
	as.push_back(temp);
    }
    sort(as.begin(),as.end());
    i = 0;
    memset(stall, 0, sizeof(Stall)*200);
    for(vector<int>::iterator it = as.begin(); it!=as.end();it++,i++){
	stall[i].min = stall[i].max = *it;
	stall[i].next = &stall[i+1];
    }
    int minValue;
    Stall *min, *stallP;
    for(i = 0; i < C-M; i++){//delete x block
	min = &stall[0];
	minValue = stall[0].next->min - stall[0].max;
	stallP = min;
	for(j=1;j < C-1-i; j++){//find min block
	    stallP = stallP->next;
	    if(stallP->next->min - stallP->max < minValue){
		minValue = stallP->next->min - stallP->max;
		min = stallP;
	    }
	}
//	cout<<"Combile"<<min->max<<","<<min->next->min<<endl;
	min->max = min->next->max;
	min->next = min->next->next;
    }
    int sum = 0;
    stallP = &stall[0];
    for(i = 0; i < M; i++){
	sum += stallP->max - stallP->min + 1;
	stallP = stallP->next;
    }
    fout<<sum<<endl;

    return 0;
}
