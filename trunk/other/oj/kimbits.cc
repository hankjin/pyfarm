/*
ID: hankjin1
LANG: C++
TASK: kimbits
*/
#include <iostream>
#include <fstream>
using namespace std;
//note: unsigned int ISTEAD of int, because 2147483648 is a special case
//dp[i][j] means: a bitset of length i, with at most j 1s

unsigned int N,L,I;
int C[32][32];
int dp[32][32];
char res[32];
int main(){
    ifstream fin("kimbits.in");
    ofstream fout("kimbits.out");
    C[0][0]=1;
    for(int i=1;i<32;i++){
	C[i][0]=1;
	for(int j=1;j<=i;j++){
	    C[i][j]=C[i-1][j-1]+C[i-1][j];
	}
    }
    fin>>N>>L>>I;
    for(int i=1;i<=N;i++){
	for(int j=i;j<=N;j++)
	    dp[i][j]=(1<<i);
	for(int j=i-1;j>=0;j--){
	    dp[i][j]=dp[i][j+1]-C[i][j+1];//usable nums
	}
    }
    I--;
    for(int i=0;i<32;i++)
	dp[0][i]=1;
    for(int i=N-1;i>=0;i--){
//	cout<<"I"<<i<<"L"<<L<<"Got"<<dp[i][L]<<"I"<<I<<endl;
	if(dp[i][L]<=I){
	    fout<<"1";
	    I-=dp[i][L];
	    L-=1;
	}
	else
	    fout<<"0";
    }
    fout<<endl;
//  cout<<I<<endl;
    return 0;
}
