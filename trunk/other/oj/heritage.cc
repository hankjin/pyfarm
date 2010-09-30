/*
ID: hankjin1
LANG: C++
TASK: heritage
*/
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

ifstream fin("heritage.in");
ofstream fout("heritage.out");

char lorder[27];
char morder[27];
void solve(int pos, int l, int r){
    int m;
    for(m=l;m<r;m++){
	if(lorder[m]==morder[pos])
	    break;
    }
    if(m!=l)
	solve(pos+1, l, m);
    if(m!=r-1)
	solve(pos+1+(m-l), m+1, r);
    fout<<morder[pos];
}
int main(){
    fin>>lorder;
    fin>>morder;
    solve(0, 0, strlen(morder));
    fout<<endl;
    return 0;
}
