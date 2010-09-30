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
//this method is space-free
//if use lots of space, then we can use sort to simplify the program
//this is O(N^3), while space and sort is O(N^2^2)
//

int base[160+1];
int N;
int gcd(int m, int n){
    int r;
    do{
	r=m%n;
	m=n;
	n=r;
    }while(r);
    return m;
}
int main(){
    ifstream fin("frac1.in");
    ofstream fout("frac1.out");
    fin>>N;
    int i,j;
    for(i=1;i<=N;i++){
	base[i]=N;
    }
    float minV = 0;
    float temp;
    int minI;
    int x=1,y=0;
    fout<<"0/1"<<endl;
    while(minV < 1){
	minV = 2;
	for(i=1;i<=N;i++){
	    if(x*base[i]==y*i){
		base[i]--;
	    }

	    temp = (float)i/base[i];
	    if(temp < minV){
		minI = i;
		minV = temp;
	    }
	}
	int frac = gcd(base[minI], minI);
	fout<<minI/frac<<"/"<<base[minI]/frac<<endl;
	x = minI;
	y = base[minI];
	base[minI]--;
    }
    return 0;
}
