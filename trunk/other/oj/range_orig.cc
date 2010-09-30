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
//note: bruce, time out when N=250 and all 1

int N;
bool mat[250][250];
map<int,int> res;
void solve(int x, int y){
    if(!mat[x][y])
	return ;
    int i, size = 2;
    for(int size=1; x+size<N && y+size<N; size++){
	for(i=0;i<=size;i++){
	    if(!mat[x+size][y+i])
		break;
	    if(!mat[x+i][y+size])
		break;
	}
	if(i==size+1)
	    res[size+1]++;
	else
	    break;
    }
}
int main(){
    ifstream fin("range.in");
    ofstream fout("range.out");
    fin>>N;
    char tmp;
    for(int i=0;i<N;i++){
	for(int j=0;j<N;j++){
	    fin>>tmp;
	    mat[i][j] = tmp=='1';
	}
    }
    for(int i=0;i<N-1;i++){
	for(int j=0;j<N-1;j++){
	    solve(i,j);
	}
    }
    for(map<int,int>::iterator it=res.begin();it!=res.end();it++){
	fout<<it->first<<" "<<it->second<<endl;
    }
    return 0;
}
