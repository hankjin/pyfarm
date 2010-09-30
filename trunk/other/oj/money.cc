/**
ID: hankjin1
LANG: C++
TASK: money
*/
#include <iostream>
#include <fstream>
using namespace std;
//note: dp must for coins first, 1,1,3 and 1,3,1 will happen twice if for N first

ifstream fin("money.in");
ofstream fout("money.out");

int N,V;
int coins[25];
long long dp[10001];


int main(){
    fin>>V>>N;
    for(int i=0;i<V;i++){
	fin>>coins[i];
    }
    dp[0]=1;
    for(int i=0;i<V;i++){
	for(int j=0;j<N;j++){
	    if(j+coins[i]>N)
		break;
	    dp[j+coins[i]]+=dp[j];
	}
    }
    fout<<dp[N]<<endl;
    return 0;
}
