/*
ID: hankjin1
LANG: C++
TASK: stamps
*/
#include <iostream>
#include <fstream>
#include <set>
using namespace std;

//note: bag problem
//note: no dp solution: over time

int K,N;
int stamp[50];
set<int> dp[201];
bool res[2000002];
int main(){
    ifstream fin("stamps.in");
    ofstream fout("stamps.out");
    fin>>K>>N;
    for(int i=0;i<N;i++){
	fin>>stamp[i];
	dp[1].insert(stamp[i]);
    }
    for(int i=2;i<=K;i++){//2=1+1, 3=1+2
	for(int j=1;j<=i/2;j++){
	    int k=i-j;
	    set<int>::iterator itj = dp[j].begin();
	    for(;itj!=dp[j].end();itj++){
		set<int>::iterator itk = dp[i-j].begin();
		for(;itk!=dp[i-j].end();itk++){
		    dp[i].insert((*itj)+(*itk));
		}
	    }
	}
    }
    for(int i=1;i<=K;i++){
	for(set<int>::iterator it=dp[i].begin();it!=dp[i].end();it++){
	    res[*it]=true;
	}
    }
    for(int i=1;;i++){
	if(!res[i]){
	    fout<<(i-1)<<endl;
	    break;
	}
    }

    return 0;
}
