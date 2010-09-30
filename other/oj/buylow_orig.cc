/*
ID: hankjin1
LANG: C++
TASK: buylow
*/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
using namespace std;

//note: Test 4 error
int N;
int num[5000];
int dp[5000];
int dpN[5000];
int maxV;

int q[5001];
int tail;
map<int,int> dict;
bool addq(int v){
    for(int i=0;i<tail;i++){
	if(q[i]==v)
	    return false;
    }
    q[tail++]=v;
    return true;
}
int main(){
    ifstream fin("buylow.in");
    ofstream fout("buylow.out");
    fin>>N;
    for(int i=0;i<N;i++){
	fin>>num[i];
    }
    maxV = 1;
    fill_n(dp, N, 1);
    fill_n(dpN, N, 1);
    for(int i=1;i<N;i++){
	tail=0;
	for(int j=0;j<i;j++){
	    if(num[j]<=num[i] || dp[j]+1<dp[i])
		continue;
	    if(dp[j]+1>dp[i]){
		dp[i]=dp[j]+1;
		dpN[i]=dpN[j];
		maxV = max(maxV, dp[i]);
		tail=0;
		addq(num[j]);//fix 1, for case 5
	    }
	    else{// dp[j]+1==dp[i]){
		if(addq(num[j]))
		    dpN[i]+=dpN[j];
	    }
	}
    }
    int res = 0;
    tail=0;//fix 2, for case 7
    for(int i=0;i<N;i++){
//	cout<<num[i]<<","<<dp[i]<<","<<dpN[i]<<endl;
	if(dp[i] == maxV){
	    if(addq(num[i])){
		res+=dpN[i];
		dict[num[i]]=dpN[i];
	    }
	    else if(dict[ num[i] ] < dpN[i]){
		res+=dpN[i]-dict[ num[i] ];
		dict[res] = dpN[i];
	    }
	}
    }
    fout<<maxV<<" "<<res<<endl;
}
