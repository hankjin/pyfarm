/**
ID: hankjin1
LANG: C++
TASK: shuttle
*/
#include <iostream>
#include <fstream>
#include <bitset>
using namespace std;

ifstream fin("shuttle.in");
ofstream fout("shuttle.out");

int N;
int main(){
    fin>>N;
    N++;
    int x,from,step;
    int t=0;
    for(int i=1;i<2*N-1;i++){
	x=i>=N?2*N-2-i:i;
	step = i%2==0?-2:2;
	from = N-(x*step/2);
	for(int j=0;j<=x;j++){
	    fout<<from+step*j;
	    if(++t%20==0)
		fout<<endl;
	    else if(i==2*N-2 && j==x)
		fout<<endl;
	    else
		fout<<" ";
	}
    }
    return 0;
}
