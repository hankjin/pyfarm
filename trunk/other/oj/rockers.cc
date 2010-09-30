/*
ID: hankjin1
LANG: C++
TASK: rockers
*/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <climits>
using namespace std;

int N,T,M;
int song[21];
int main(){
    ifstream fin("rockers.in");
    ofstream fout("rockers.out");
    fin>>N>>T>>M;
    for(int i=0;i<N;i++){
	fin>>song[i];
    }
    int res = 0;
    for(int k=0;k<(1<<N);k++){
	int total = 0, count=0, j=0;
	for(int i=0; i<N && j<M;i++){
	    if((k&(1<<i))==0 || song[i]>T)
		continue;
	    if(total+song[i] > T){
		j++;
		total = song[i];
	    }
	    else{
		total += song[i];
	    }
	    count++;
	}
	if(j==M) count--;
	res = max(res, count);
    }
    fout<<res<<endl;
    return 0;
}
