/*
ID: hankjin1
LANG: C++
TASK: nocows
*/
#include <iostream>
#include <fstream>
using namespace std;
//note: this methods runs out of time, must use DP

int N,K;
int nums[100];
int C[200][200];

int solve(int x,int todo){
    if(x==K){
	if(nums[x-1]*2<todo)
	    return 0;
	return C[nums[x-1]][todo/2];
    }
    int i;
    int methods=0;
    for(i=1;i<=nums[x-1];i++){
	nums[x]=i*2;
	if(todo>i*2){
	    methods+=C[nums[x-1]][i]*solve(x+1,todo-i*2);
	    methods%=9901;
	}
    }
    return methods;
}
int main(){
    ifstream fin("nocows.in");
    ofstream fout("nocows.out");
    fin>>N>>K;
    nums[1]=1;
    C[0][0]=1;
    for(int i=1; i<N;i++){
	C[i][0]=1;
	for(int j=1;j<=i;j++)
	    C[i][j]=C[i-1][j]+C[i-1][j-1];
    }
    fout<<solve(2,N-1)<<endl;
}
