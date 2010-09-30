/**
ID: hankjin1
LANG: C++
TASK: frac1
*/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <climits>
#include <vector>
using namespace std;
//note: gcd
//0/1 1/1 => (0+1)/(1+1)
//0/1                    1/1
//          1/2
//     1/3         2/3
//  1/4   2/5   3/5   3/4
// .....(ax+bx)/(ay+by)...

int N;

ofstream fout("frac1.out");
void russ(int ax, int ay, int bx, int by){
    if((ay+by)<=N){
	russ(ax,ay,ax+bx,ay+by);
	fout<<(ax+bx)<<"/"<<(ay+by)<<endl;
	russ(ax+bx,ay+by,bx,by);
    }
}
int main(){
    ifstream fin("frac1.in");
    fin>>N;
    fout<<"0/1"<<endl;
    russ(0,1,1,1);
    fout<<"1/1"<<endl;
    return 0;
}
