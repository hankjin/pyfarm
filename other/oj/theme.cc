/**
ID: hankjin1
LANG: C++
TASK: theme
*/
#include <iostream>
#include <fstream>
using namespace std;

ifstream fin("theme.in");
ofstream fout("theme.out");

int N;
int length[5000];
int dp[5000];
int main(){
    fin>>N>>length[0];
    int x;
    for(int i=1;i<N;i++){
	fin>>x;
	length[i-1]=x-length[i-1];
	length[i]=x;
    }
    int l;
    for(int i=0;i<N-1;i++){
	l=1;
	for(int j=i+1,di=0;j<N-1;j++,di++){
	    if(length[j-i-1]==length[j] && l<=i){//that is the point:repeat
		dp[j]=max(dp[j], ++l);
	    }
	    else
		l=1;
	}
    }
    int res = 0;
    for(int i=0;i<N-1;i++){
//	cout<<i<<":"<<length[i]<<","<<dp[i]<<endl;
	res = max(res,dp[i]);
    }
    if(res<5)//point 2: at leat 5
	res = 0;
    fout<<res<<endl;
    return 0;
}
