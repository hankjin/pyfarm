/*
ID: hankjin1
LANG: C++
TASK: nuggets
*/
#include <iostream>
#include <fstream>
using namespace std;

//note: max num which can NOT be made of N and M is: N*M-N-M
//note: 256*256-256-256=65024
#define X 65100
int N;
int nugs[256];
bool dp[X];
int main(){
    ifstream fin("nuggets.in");
    ofstream fout("nuggets.out");
    fin>>N;
    for(int i=0;i<N;i++){
	fin>>nugs[i];
    }
    dp[0]=true;
    int i=0;
    for(;i<X; i++){
	if(!dp[i])
	    continue;
	for(int j=0;j<N;j++){
	    if(i+nugs[j]<X)
		dp[i+nugs[j]]=true;
	}
    }
    if(dp[X-1]==false)
	fout<<0<<endl;
    else{
	for(i=X-1;i>0;i--){
	    if(dp[i]==false){
		break;
	    }
	}
	fout<<i<<endl;
    }
    return 0;
}
