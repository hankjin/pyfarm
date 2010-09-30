/*
ID: hankjin1
LANG: C++
TASK: sort3
*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
using namespace std;
//[1111111] [22222222] [33333333333]
//    a         b           c
//numsize[] = {x, a, b, c}
//todo[i][j]: count to move from place i to place j

ifstream fin("sort3.in");
ofstream fout("sort3.out");
int N;
int nums[1000];
int numsize[4] = {0,0,0,0};
int todo[4][4] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int main(){
    fin>>N;
    int i;
    for(i=0;i<N;i++){
	fin>>nums[i];
	numsize[nums[i]]++;
    }
    int num12=numsize[1]+numsize[2];
    for(i=0;i<N;i++){
	if(i<numsize[1]){//1 pos
	    if(nums[i]!=1){
		todo[1][nums[i]]++;
	    }
	}
	else if(i>=num12){//3 pos
	    if(nums[i]!=3){
		todo[3][nums[i]]++;
	    }
	}
	else{//2 pos
	    if(nums[i]!=2){
		todo[2][nums[i]]++;
	    }
	}
    }
    int temp = todo[2][1]-todo[1][2];//enough to change back to 2
    temp = temp < 0 ? 0 : temp;
    fout<<(todo[2][1]+todo[3][1]+todo[2][3]+temp)<<endl;

    return 0;
}

