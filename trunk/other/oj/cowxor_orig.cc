/**
ID: hankjin1
LANG: C++
TASK: cowxor
*/
#include <iostream>
#include <fstream>
using namespace std;

//note: passed 5 cases, case 6 tle
ifstream fin("cowxor.in");
ofstream fout("cowxor.out");
int N;
int cow[100000];

int main(){
    fin>>N;
    int maxV=-1,maxI1,maxI2;
    cow[0]=0;
    for(int i=1;i<=N;i++){
	fin>>cow[i];
	cow[i]^=cow[i-1];
    }
    for(int i=1;i<=N;i++){
	for(int j=i;j<=N;j++){
	    if((cow[j-i]^cow[j]) > maxV){
		maxV = cow[j-i]^cow[j];
		maxI1 = j-i+1;
		maxI2 = j;
	    }
	}
    }
    fout<<maxV<<" "<<maxI1<<" "<<maxI2<<endl;

    return 0;
}
