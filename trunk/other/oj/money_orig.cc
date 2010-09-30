/**
ID: hankjin1
LANG: C++
TASK: money
*/
#include <iostream>
#include <fstream>
using namespace std;
//note: this runs overtime.

ifstream fin("money.in");
ofstream fout("money.out");

int N,V;
int coins[25];
int coinNum[25];

long long found;
void solve(int n, int todo){
    if(n==V-1){//last one
	if(todo%coins[n]==0)
	    found++;
	return ;
    }
    int maxNum = todo/coins[n];
    for(int i=0;i<=maxNum;i++)
	solve(n+1, todo-i*coins[n]);
}
int main(){
    fin>>V>>N;
    for(int i=0;i<V;i++){
	fin>>coins[i];
    }
    found = 0;
    solve(0, N);
    fout<<found<<endl;
    return 0;
}
