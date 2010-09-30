/*
ID: hankjin1
LANG: C++
TASK: kimbits
*/
#include <iostream>
#include <fstream>
using namespace std;
//note: it is ok fot lots of numbers, but not all!

int N,L,I;
int dp[2048];
int main(){
    ifstream fin("kimbits.in");
    ofstream fout("kimbits.out");
    fin>>N>>L>>I;
    int i,j,k;
    for(i=0;i<2048;i++){
	int tmp = i;
	for(j=0;tmp!=0;j++){
	    tmp -= tmp&(-tmp);
	}
	dp[i]=j;
    }
    for(i=0,j=0;j<I;i++){
	if(dp[i>>22]+dp[(i>>11)&0x7FF]+dp[i&0x7FF]<=L)
	    j++;
    }
    i--;
    for(j=N-1;j>=0;j--){
	if( (i&(1<<j)) >0)
	    fout<<"1";
	else
	    fout<<"0";
    }
    fout<<endl;
    return 0;
}
