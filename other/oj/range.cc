/*
ID: hankjin1
LANG: C++
TASK: range
*/
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
using namespace std;

//use dp to accelerate
//use int[251] instead of map<int,int> to accelerate
//this solution uses top-left corner, while use top-right cornor is better
int N;
int mat[250][250];
int res[251];
void solve(int x, int y){
    if(0==mat[x][y])
	return ;
    int i, size = mat[x][y];
    for(; x+size<N && y+size<N; size++){
	for(i=0;i<=size;i++){
	    if(!mat[x+size][y+i])
		break;
	    if(!mat[x+i][y+size])
		break;
	}
	if(i!=size+1)
	    break;
    }
    if(mat[x][y]!=size){
	for(i=1;i<size-1;i++){
	    for(int j=0;j<=i;j++){
		mat[x+i][y+j]=max(mat[x+i][y+j],size-i);
		mat[x+j][y+i]=max(mat[x+j][y+i],size-i);
	    }
	}
    }
    mat[x][y]=size;
}
int main(){
    ifstream fin("range.in");
    ofstream fout("range.out");
    fin>>N;
    char tmp;
    for(int i=0;i<N;i++){
	for(int j=0;j<N;j++){
	    fin>>tmp;
	    mat[i][j] = tmp-'0';
	}
    }
    for(int i=0;i<N-1;i++){
	for(int j=0;j<N-1;j++){
	    solve(i,j);
	}
    }
    for(int i=0;i<N-1;i++){
	for(int j=0;j<N-1;j++){
	    if(mat[i][j]>1){
		for(int k=2;k<=mat[i][j];k++)
		    res[k]++;
	    }
	}
    }
    for(int i=2;i<=N;i++){
	if(res[i]!=0)
	    fout<<i<<" "<<res[i]<<endl;
    }
    return 0;
}
