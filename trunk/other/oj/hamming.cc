/**
ID: hankjin1
LANG: C++
TASK: hamming
*/
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;
//note: output rule

ifstream fin("hamming.in");
ofstream fout("hamming.out");

int N,B,D;
int nums[64];
int lim;
int found = 0;
bool solve(int n, int x){
    int i,j;
    if(n==N)
	return true;
    for(i=x+1;i<lim;i++){
	for(j=0;j<n;j++){
	    int temp = nums[j]^i;
	    int bits = 0;
	    while(temp>0){
		int p = temp&(-temp);
		temp -= p;
		bits++;
	    }
	    if(bits<D)
		break;
	}
	if(j==n){//ok
	    nums[n]=i;
	    if(solve(n+1,i))
		return true;
	}
    }
    return false;
}
int main(){
    fin>>N>>B>>D;
    lim=(2<<B);
    int i;

    for(i=0;i<lim;i++){
	nums[0]=i;
	if(solve(1,i))
	    break;
    }
    for(i=0;i<N;i++){
	fout<<nums[i];
	if(i==(N-1))
	    fout<<endl;
	else if(i%10==9)
	    fout<<endl;
	else
	    fout<<" ";
    }
    return 0;
}
