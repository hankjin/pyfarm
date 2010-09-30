/*
ID: hankjin1
LANG: C++
TASK: fence
*/
#include <iostream>
#include <fstream>
#include <stack>
#include <set>
#include <vector>
using namespace std;

//note: 1. begin pos can be 1 or a point with odd edges
//note: 2. there may be more than one path between 2 points
set<int> fence;
stack<int> his;
vector<int> circu;
int loc;
int mat[501][501];
void findpath();
void findcircu(){
    circu.push_back(loc);
    if(!his.empty()){
	loc=his.top();
	his.pop();
	findpath();
    }
}
void findpath(){
    set<int>::iterator it=fence.begin();
    for(;it!=fence.end();it++){
	if(mat[loc][*it]>0){
	    mat[loc][*it]--;
	    mat[*it][loc]--;
	    his.push(loc);
	    loc = *it;
	    break;
	}
    }
    if(it==fence.end()){
	findcircu();
    }
    else
	findpath();
}
int main(){
    ifstream fin("fence.in");
    ofstream fout("fence.out");
    int N, a,b;
    fin>>N;
    for(int i=0;i<N;i++){
	fin>>a>>b;
	fence.insert(a);
	fence.insert(b);
	mat[a][b]++;
	mat[b][a]++;
    }
    loc=*fence.begin();
    for(set<int>::iterator from=fence.begin();from!=fence.end();from++){
	int edge=0;
	for(set<int>::iterator to=fence.begin();to!=fence.end();to++){
	    edge+=mat[*from][*to];
	}
	if(edge%2!=0){
	    loc = *from;
	    break;
	}
    }
    findpath();
    for(vector<int>::reverse_iterator it=circu.rbegin();it!=circu.rend();it++)
	fout<<*it<<endl;
    return 0;
}
