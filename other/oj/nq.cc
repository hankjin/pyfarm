#include <iostream>
#include <cstring>
#include <algorithm>
#include <vector>
#include <iterator>
#include <map>
using namespace std;

typedef unsigned int uint;


int N = 4;
int lim = (1<<N)-1;
int found = 0;

void nq(int row, int ld, int rd){
    if(row!=lim){
	int pos = lim & (~(row|ld|rd));
	while(pos!=0){
	    int p = pos & (-pos);
	    nq(row+p, (ld+p)<<1, (rd+p)>>1);
	    pos -= p;
	}
    }
    else
	found++;
}
int main(){
    nq(0,0,0);
    cout<<found<<endl;
    return 0;
}
