/**
ID: hankjin1
LANG: C++
TASK: milk4
*/
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

ifstream fin("milk4.in");
ofstream fout("milk4.out");

int Q,P;
int pails[100];
int main(){
    fin>>Q>>P;
    for(int i=0;i<P;i++)
	fin>>pails[i];
    sort(pails,pails+P);
    for(int i=0;i<P;i++){
	if(Q%pails[i]==0){
	    cout<<"1 "<<pails[i]<<endl;
	    return 0;
	}
    }
    for(int i=0;i<P;i++){
	for(int j=i+1;j<P;j++){
	    if(pails[i]*pails[j]-pails[i]-pails[j]<Q){
		cout<<"2 "<<pails[i]<<" "<<pails[j]<<endl;
		return 0;
	    }
	}
    }
    return 0;
}
