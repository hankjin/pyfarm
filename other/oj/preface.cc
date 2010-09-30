/**
ID: hankjin1
LANG: C++
TASK: preface
*/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
using namespace std;

ifstream fin("preface.in");
ofstream fout("preface.out");
int nums[4][2];
char names[4][3]={
    "IV","XL","CD","M"
};
int N;
void check(int base, int n){
    switch(base){
	case 3:
	    nums[3][0]+=n;
	    break;
	case 0:
	case 1:
	case 2:
	    if(n==9){
		nums[base+1][0]++;
		nums[base][0]++;
	    }
	    else if(n>=4){
		nums[base][1]++;
		nums[base][0]+=abs(n-5);
	    }
	    else
		nums[base][0]+=n;
	    break;
    }
}
void deal(int x){
    int i = 0;
    for(i=0;x>0;i++){
	check(i, x%10);
	x/=10;
    }
}

int main(){
    fin>>N;
    int i,j;
    memset(nums, 0, sizeof(int)*4*2);
    for(i=1;i<=N;i++){
	deal(i);
    }
    for(i=0;i<4;i++){
	for(j=0;j<2;j++){
	    if(nums[i][j]>0){
		fout<<names[i][j]<<" "<<nums[i][j]<<endl;
	    }
	}
    }
    return 0;
}
