/*
ID: hankjin1
LANG: C++
TASK: humble
*/
#include <iostream>
#include <fstream>
#include <cmath>
#include <climits>
#include <set>
#include <algorithm>
using namespace std;
//note: delete used nums in set
//note: notice overflow of multiply

int K,N;
set<long long> nums;
int found;
int ss[100];

int main(){
    ifstream fin("humble.in");
    ofstream fout("humble.out");
    fin>>K>>N;
    for(int i=0;i<K;i++){
	fin>>ss[i];
    }
    nums.insert(1);
    long long res=0;
    for(int i=0;i<=N;i++){
	res = *nums.begin();
	nums.erase(nums.begin());
	for(int j=0;j<K;j++){
	    if(ss[j]*res<INT_MAX)
		nums.insert(ss[j]*res);
	}
//	cout<<res<<","<<nums.size()<<endl;
    }
    fout<<res<<endl;
    return 0;
}
