/**
ID: hankjin1
LANG: C++
TASK: numtri
*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include <climits>
using namespace std;

int R;
int nums[1000][1000];
void resolve(){
    int i,j;
    for(i=R-2;i>=0;i--){
	for(j=0;j<=i;j++){
	    nums[i][j]+=max(nums[i+1][j],nums[i+1][j+1]);
	}
    }
}
int main(){
    ifstream fin("numtri.in");
    ofstream fout("numtri.out");
    fin>>R;
    int i,j;
    for(i=0;i<R;i++){
	for(j=0;j<=i;j++){
	    fin>>nums[i][j];
	}
    }
    resolve();
    fout<<nums[0][0]<<endl;
    return 0;
}
