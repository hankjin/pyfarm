/*
ID: hankjin1
LANG: C++
TASK: fence6
*/
#include <iostream>
#include <fstream>
#include <climits>
using namespace std;

//note: notice the diff of left and right
//note: not triangle, but any kind of circle
int N;
struct line{
    int length;
    int ln, rn;//lenth of left, length of right
    int left[8];
    int right[8];
    int *next[101];//eg left is 1,3, and right is 2,4, then next[1]=right
    int nextN[101];//if x is in left, then nextN[x] is rn
}lines[101];
bool walked[101];
int res;

inline void walk(int prev, int now, int dest, int len){
    for(int i=0;i<lines[now].nextN[prev];i++){
	int l = lines[now].next[prev][i];//next line
	if(walked[l])
	    continue;
	walked[l] = true;
	if(l == dest)
	    res = min(res, len);
	else if(len+lines[l].length<res)
	    walk(now, l, dest, len+lines[l].length);
    }
}
int main(){
    ifstream fin("fence6.in");
    ofstream fout("fence6.out");
    fin>>N;
    int lineNo;
    int tmp;
    for(int i=0;i<N;i++){
	fin>>lineNo;
	fin>>lines[lineNo].length;
	fin>>lines[lineNo].ln>>lines[lineNo].rn;
	for(int j=0;j<lines[lineNo].ln;j++){
	    fin>>tmp;
	    lines[lineNo].left[j] = tmp;
	    lines[lineNo].next[tmp] = lines[lineNo].right;
	    lines[lineNo].nextN[tmp] = lines[lineNo].rn;
	}
	for(int j=0;j<lines[lineNo].rn;j++){
	    fin>>tmp;
	    lines[lineNo].right[j] = tmp;
	    lines[lineNo].next[tmp] = lines[lineNo].left;
	    lines[lineNo].nextN[tmp] = lines[lineNo].ln;
	}
    }
    res = INT_MAX;
    for(int i=1; i<=N;i++){
	//makr as not walked
	fill_n(walked, N+1,false);
	for(int j=0;j<lines[i].ln;j++){
	    //walke from left only, because from right is equal
	    int now = lines[i].left[j];
	    walk(i, now, i, lines[i].length+lines[now].length);
	}
    }
    fout<<res<<endl;
    return 0;
}

