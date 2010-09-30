/**
ID: hankjin1
LANG: C++
TASK: frameup
*/
#include <iostream>
#include <fstream>
#include <climits>
#include <algorithm>
using namespace std;

ifstream fin("frameup.in");
ofstream fout("frameup.out");

int H,W;
char data[30][30];
bool path[26][26];
struct frame{
    char c;
    int top,left,right,bottom;
}frames[26];
int frameN;

int seq[26];
char res[200000][27];
bool used[26];
int resN;
bool mycmp(const struct frame f1, const struct frame f2){
    return f1.c < f2.c;
}
void topo(int n){
    if(n==frameN){
	for(int i=0;i<frameN;i++){
	    res[resN][i]=frames[seq[i]].c;
	}
	resN++;
    }
    int d[26];
    fill_n(d,26,-1);
    for(int i=0;i<frameN;i++){
	if(used[i]) continue;
	d[i]=0;
	for(int j=0;j<frameN;j++){
	    if(used[j]) continue;
	    if(path[j][i]>0) 
		d[i]++;
	}
    }
    for(int i=0;i<frameN;i++){
	if(d[i]==0){
	    used[i]=true;
	    seq[n]=i;
	    topo(n+1);
	    used[i]=false;
	}
    }
}
int get(char c){
    for(int i=0;i<frameN;i++)
	if(frames[i].c == c)
	    return i;
    return -1;
}
void add(int x,int y,char c){
    int v;
    for(v=0;v<frameN;v++){
	if(frames[v].c==c)
	    break;
    }
    if(v==frameN){
	frames[frameN++].c=c;
	frames[v].top=frames[v].left = INT_MAX;
	frames[v].right=frames[v].bottom=0;
    }
    frames[v].top = min(x,frames[v].top);
    frames[v].left = min(y,frames[v].left);
    frames[v].bottom = max(x,frames[v].bottom);
    frames[v].right = max(y,frames[v].right);
}
bool in(int x, int y, int c){
    if((frames[c].top<=x && frames[c].bottom>=x)
	    &&(frames[c].left==y || frames[c].right==y))
	return true;
    if((frames[c].left<=y && frames[c].right>=y)
	    &&(frames[c].top==x || frames[c].bottom==x))
	return true;
    return false;
}
int main(){
    fin>>H>>W;
    frameN=0;
    //input
    for(int i=0;i<H;i++){
	for(int j=0;j<W;j++){
	    fin>>data[i][j];
	    if(data[i][j]!='.')
		add(i,j,data[i][j]);
	}
    }
    sort(frames,frames+frameN,mycmp);//fix 1: sequense
    //build path
    for(int i=0;i<H;i++){
	for(int j=0;j<W;j++){
	    if(data[i][j]=='.')
		continue;
	    for(int k=0;k<frameN;k++){
		if(frames[k].c==data[i][j])
		    continue;
		if(in(i,j,k))
		    path[k][get(data[i][j])]=true;
	    }
	}
    }
    resN=0;
    //resolve
    topo(0);
    for(int i=0;i<resN;i++){
	fout<<res[i]<<endl;
    }
    return 0;
}
