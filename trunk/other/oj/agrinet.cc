/*
ID: hankjin1
LANG: C++
TASK: agrinet
*/
#include <iostream>
#include <fstream>
#include <climits>
using namespace std;

int N;
int dist[100][100];
bool flag[100];
int main(){
    ifstream fin("agrinet.in");
    ofstream fout("agrinet.out");
    fin>>N;
    for(int i=0;i<N;i++){
	for(int j=0;j<N;j++){
	    fin>>dist[i][j];
	}
    }
    flag[0]=true;
    int minI, minL;
    int total=0;
    for(int k=1;k<N;k++){
	minL = INT_MAX;
	for(int i=0;i<N;i++){//each scaned
	    if(!flag[i])
		continue;
	    for(int j=0;j<N;j++){//each NON scaned
		if(flag[j])
		    continue;
		if(dist[i][j]<minL){
		    minL=dist[i][j];
		    minI = j;
		}
	    }
	}
	flag[minI]=true;
	total+=minL;
    }
    fout<<total<<endl;
    return 0;
}
