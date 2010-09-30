/*
ID: hankjin1
LANG: C++
TASK: stall4
*/
#include <iostream>
#include <fstream>
#include <climits>
using namespace std;

//note: find the least selectable user
//note: min selectable cow, select the min selectable stall
int N,M;
int milk[201][201];
bool flag[201][201];
int stall[201];
int cow[201];
int main(){
    ifstream fin("stall4.in");
    ofstream fout("stall4.out");
    fin>>N>>M;
    for(int i=1;i<=N;i++){
	fin>>milk[i][0];
	for(int j=1;j<=milk[i][0];j++){
	    fin>>milk[i][j];
	    flag[ milk[i][j] ][i]=true;
	}
    }
    int usedstall=0;
    for(int k=1;k<=N;k++){
	//find a cow
	int cowi, cowmin=INT_MAX;
	for(int i=1;i<=N;i++){
	    if(cow[i]!=0)//cow placed
		continue;
	    //reduce the used
	    if(flag[usedstall][i]){
		for(int j=1;j<=milk[i][0];j++){
		    if(milk[i][j]==usedstall){
			milk[i][j]=milk[i][ milk[i][0] ];
			break;
		    }
		}
		milk[i][0]--;
	    }
	    if(milk[i][0] < cowmin){
		cowmin = milk[i][0];
		cowi = i;
	    }
	}
	//no selectable stall
	if(cowmin == 0){
	    usedstall=0;
	    cow[cowi]=-1;
	}
	else{
	    //select a stall
	    int minstall=INT_MAX;
	    for(int j=1;j<=milk[cowi][0];j++){
		int choice=0;
		for(int x=1;x<=N;x++){
		    if(cow[x] ==0 && flag[milk[cowi][j]])
			choice++;
		}
		if(choice < minstall){
		    minstall = choice;
		    usedstall=milk[cowi][j];
		    cow[cowi]=usedstall;
		}
	    }
	}
    }
    int res = 0;
    for(int i=1;i<=N;i++){
	if(cow[i]>0){
	    res++;
	}
    }
    fout<<res<<endl;
    return 0;
}
