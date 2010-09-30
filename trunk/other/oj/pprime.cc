/*
ID: hankjin1
LANG: C++
TASK: pprime
*/

#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

ifstream fin("pprime.in");
ofstream fout("pprime.out");

int A,B;
int num[10];
bool isPrime(int n){
    if(n%2==0)
	return false;
    int i = 3;
    for(i=3; i <=sqrt(n); i++){
	if(n%i==0)
	    return false;
    }
    return true;
}
void solve(int n, int x){
    if(x==(n+1)/2){
	int i,sum=0;
	for(i=0;i<n;i++){
	    sum = sum*10+num[i];
	}
	if(sum < A || sum > B)
	    return ;
	if(isPrime(sum))
	    fout<<sum<<endl;
	return ;
    }
    int i;
    for(i=0;i<=9;i++){
	if(x==0 && i==0)
	    continue;
	num[x]=num[n-1-x]=i;
	solve(n, x+1);
    }
}
void solve(){
    int minLen=0, maxLen=0;
    int temp = A;
    while(temp > 0){
	temp/=10;
	minLen++;
    }
    temp = B;
    while(temp > 0){
	temp/=10;
	maxLen++;
    }
    int i;
    for(i=minLen;i<=maxLen;i++){//len
	solve(i, 0);
    }
}
int main(){
    fin>>A>>B;
    solve();
    return 0;
}
