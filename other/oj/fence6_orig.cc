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
    int ln, rn;
    int lp, rp;
    int left[8];
    int right[8];
}lines[101];
int pic[101][101];
inline bool mark(int *lns, int n, int x,int lno){
    for(int i=0; i<n; i++){
	int l = lns[i];
	if(lines[l].lp == -1 && lines[l].rp == -1){
	    for(int j=0;j<lines[l].ln;j++){
		if(lines[l].left[j]==lno){
		    lines[l].lp = x;
		    break;
		}
	    }
	    if(lines[l].lp == -1)
		lines[l].rp = x;
	}
	else if(lines[l].lp != -1 && lines[l].rp == -1){
	    if(lines[l].lp != x)
		lines[l].rp = x;
	}
	else if(lines[l].rp != -1 && lines[l].lp == -1){
	    if(lines[l].rp != x)
		lines[l].lp = x;
	}
    }
}
int main(){
    ifstream fin("fence6.in");
    ofstream fout("fence6.out");
    fin>>N;
    int lineNo;
    for(int i=0;i<N;i++){
	fin>>lineNo;
	fin>>lines[lineNo].length;
	fin>>lines[lineNo].ln>>lines[lineNo].rn;
	for(int j=0;j<lines[lineNo].ln;j++)
	    fin>>lines[lineNo].left[j];
	for(int j=0;j<lines[lineNo].rn;j++)
	    fin>>lines[lineNo].right[j];
	lines[lineNo].lp = lines[lineNo].rp = -1;
    }
    bool flag=true;
    int point = 0;
    lines[1].lp = point;
    lines[1].rp = ++point;
    lineNo = 1;
    struct line *temp;
    while(flag){
	mark(lines[lineNo].left, lines[lineNo].ln, lines[lineNo].lp, lineNo);
	mark(lines[lineNo].right, lines[lineNo].rn, lines[lineNo].rp, lineNo);
	flag = false;
	for(int i=1;i<=N&&!flag;i++){
	    if(lines[i].lp != -1 && lines[i].rp == -1){
		lines[i].rp = ++point;
		lineNo = i;
		flag = true;
	    }
	    else if(lines[i].lp == -1 && lines[i].rp != -1){
		lines[i].lp = ++point;
		lineNo = i;
		flag = true;
	    }
	}
    }
    for(int i=1;i<=N;i++){
	cout<<"Line"<<i<<":"<<lines[i].lp<<","<<lines[i].rp<<endl;
	pic[lines[i].lp][lines[i].rp] = lines[i].length;
	pic[lines[i].rp][lines[i].lp] = lines[i].length;
    }
    int res = INT_MAX;
    for(int i=0;i<=point;i++){
	for(int j=0;j<=point;j++){
	    if(j==i || pic[i][j]==0 || pic[i][j]>res)
		continue;
	    for(int k=0;k<=point;k++){
		if(k==i || k==j || pic[j][k]==0)
		    continue;
		if(pic[k][i]!=0){
		    cout<<"Rec:"<<i<<","<<j<<","<<k<<endl;
		    res = min(res, pic[i][j]+pic[j][k]+pic[k][i]);
		}
	    }
	}
    }
    fout<<res<<endl;
    return 0;
}

