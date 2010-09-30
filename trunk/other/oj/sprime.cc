/*
ID: hankjin1
LANG: C++
TASK: sprime
*/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <cmath>
using namespace std;

int N;
int prime[]={2,3,5,7};
vector<int> ans;
bool isPrime(int num){
    int i;
    for(i=3; i<=sqrt(num); i+=2){
	if(num%i==0)
	    return false;
    }
    return true;
}
void dfs(int num, int layer){
    int i,j;
    if(layer == N){
	ans.push_back(num);
	return ;
    }
    num *= 10;
    for(i=1;i<=9;i+=2){
	if(isPrime(num+i))
	    dfs(num+i, layer+1);
    }
}
int main(){
    ifstream fin("sprime.in");
    ofstream fout("sprime.out");
    fin>>N;
    int i;
    for(i=0;i<4;i++){
	dfs(prime[i],1);
    }
    sort(ans.begin(), ans.end());
    copy(ans.begin(), ans.end(), ostream_iterator<int>(fout,"\n"));
    return 0;
}
