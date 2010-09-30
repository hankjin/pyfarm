/**
ID: hankjin1
LANG: C++
TASK: runround
*/
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;
//note: make used[nums[i]], used[i], nums[i] clear

ifstream fin("runround.in");
ofstream fout("runround.out");

int nums[32];
int numSize;
bool used[10];
bool walked[10];

bool runok(){
    int i,x=numSize-1;
    memset(walked, 0, sizeof(bool)*10);
    for(i=0;i<numSize;i++){
	if(walked[x])
	    return false;
	walked[x]=true;
	x=(x+10*numSize-nums[x])%numSize; 
    }
    if(x==numSize-1)
	return true;
    return false;
}

void next_num(int base){
    int i,j;
    used[nums[base]]=false;
    for(i=nums[base]+1;i<=9;i++){
	if(!used[i]){
	    nums[base]=i;
	    used[i]=true;
	    break;
	}
    }
    if(i==10){
	//clear
	used[0]=true;
	for(j=base;j>=0;j--){
	    used[nums[j]]=false;
	    nums[j]=0;
	}
	next_num(base+1);
    }
    else if(base>0){
	next_num(base-1);
    }
}
void solve(int x){
    bool isok;
    int i,j;
    int temp = x;
    do{
	temp = x;
	isok=true;
	memset(used, 0, sizeof(bool)*10);
	for(i=0;temp>0;i++){
	    nums[i]=temp%10;
	    if(used[nums[i]]||nums[i]==0)
		isok=false;
	    else
		used[nums[i]]=true;
	    temp/=10;
	}
	numSize = i;
	x++;
    }while(!isok);
    while(true){
	if(runok())
	    return;
	next_num(0);
    }
}
int main(){
    int M;
    fin>>M;
    int i;
    solve(M+1);
    for(i=numSize-1;i>=0;i--)
	fout<<nums[i];
    fout<<endl;

    return 0;
}
