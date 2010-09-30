/*
ID: hankjin1
LANG: C++
TASK: packrec
*/
//note: 6=1+4+4*3+...
//
#include <limits.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <set>
#include <iterator>
using namespace std;

int rec[4][2];
map<int,int>tech;
set<int> techI;
int trec[4][2];
int minArea;
void addArea(int h, int w){
    int area = w*h;
    if(area > minArea){
	return;
    }
    if(area < minArea){
	minArea = area;
	tech.clear();
	techI.clear();
    }
    //cout<<"Pack"<<h<<","<<w<<":"<<h*w<<endl;
    int temp = w < h ? w : h;
    h = w > h ? w : h;
    tech[temp] = h;
    techI.insert(temp);
}

void solve(){
    minArea = INT_MAX;
    int i,j,k;
    int sumw,sumh,area;
    int w,h;
    int temp;
    for(i=0;i<16;i++){
	temp=1;
	for(j=0;j<4;j++){
	    w=(i&temp)>0?1:0;
	    temp*=2;
	    trec[j][0]=rec[j][w];
	    trec[j][1]=rec[j][1-w];
	    //cout<<trec[j][0]<<','<<trec[j][1]<<',';
	}
	//cout<<endl;
	sumw=0; sumh=0;
	for(j=0;j<4;j++){//layout 1.
	    sumw += trec[j][0];
	    sumh = sumh > trec[j][1] ? sumh : trec[j][1];
	}
	addArea(sumw,sumh);
	for(k=0; k< 4; k++){
	    sumw=0; sumh=0;
	    for(j=0;j<4;j++){//layout 2
		if(j==k)
		    continue;
		sumw += trec[j][0];
		sumh = sumh > trec[j][1] ? sumh : trec[j][1];
	    }
	    sumw = sumw > trec[k][0] ? sumw : trec[k][0];
	    sumh += trec[k][1];
	    //cout<<"Lay 2"<<endl;
	    addArea(sumw,sumh);
	}
	for(j=0;j<4;j++){
	    for(k=0;k<4;k++){
		if(k==j)
		    continue;
		int r1,r0;
		for(r1=0;r1<4;r1++){
		    if(r1==k || r1==j)
			continue;
		    break;
		}
		r0=6-j-k-r1;
		
		sumw=0; sumh=0;
		sumw = trec[r0][0]+trec[r1][0];//layout 3, r0,r1,j,k
		sumh = trec[r0][1] > trec[r1][1]? trec[r0][1] : trec[r1][1];
		sumw = sumw > trec[j][0] ? sumw : trec[j][0];
		sumh += trec[j][1];
		sumw += trec[k][0];
		sumh = sumh > trec[k][1] ? sumh : trec[k][1];
		//cout<<"lay"<<3<<endl;
		addArea(sumw,sumh);
	    }
	}
	for(j=1;j<4;j++){
	    for(k=0;k<j;k++){
		int r1,r0;
		for(r1=0;r1<4;r1++){
		    if(r1==k || r1==j)
			continue;
		    break;
		}
		r0=6-j-k-r1;
	
		sumw=0; sumh=0;
		//layout 4, r0,j,k,r1
		sumw = trec[j][0]>trec[k][0]?trec[j][0]:trec[k][0];
		sumh = trec[j][1]+trec[k][1];
		sumw += trec[r0][0]+trec[r1][0];
		sumh = sumh > trec[r0][1] ? sumh : trec[r0][1];
		sumh = sumh > trec[r1][1] ? sumh : trec[r1][1];
		addArea(sumw, sumh);
		sumw = 0; sumh=0;
		//layout 5, j,k,r0,r1
		sumw = trec[j][0]>trec[k][0]?trec[j][0]:trec[k][0];
		sumh = trec[j][1]+trec[k][1];
		sumw += trec[r0][0]+trec[r1][0];
		sumh = sumh > trec[r0][1]?sumh : trec[r0][1];
		sumh = sumh > trec[r1][1]?sumh : trec[r1][1];
		addArea(sumw, sumh);
	    }
	}
	for(j=1;j<=3;j+=2){
	    //0,1,   0,3
	    //2,3 && 2,1, 1,3=>3,1 (1=>j,3=>4-j)
	    sumw = 0; sumh=0;
	    sumw = trec[0][0]+trec[j][0];//layout 6
	    temp = trec[2][0]+trec[4-j][0];
	    sumw = sumw > temp ? sumw : temp;
	    sumh = trec[0][1]+trec[2][1];
	    temp = trec[j][1]+trec[4-j][1];
	    sumh = sumh > temp ? sumh : temp;
	    if(((trec[0][0]+trec[4-j][0]) > sumw)//0 and 3 conflict
		    &&((trec[0][1]+trec[4-j][1]) > sumh)){
		addArea((trec[0][0]+trec[4-j][0]), sumh);
		addArea(sumw, (trec[0][1]+trec[4-j][1]));
	    }
	    else if(((trec[j][0]+trec[2][0]) > sumw)//1 and 2 conflickt
		    &&((trec[j][1]+trec[2][1]) > sumh)){
		addArea((trec[j][0]+trec[2][0]), sumh);
		addArea(sumw, (trec[j][1]+trec[2][1]));
	    }
	    else{
		addArea(sumw, sumh);
	    }
	}
    }
}
int main(){
    ifstream fin("packrec.in");
    ofstream fout("packrec.out");
    int i,j;
    for(i=0;i<4;i++)
	for(j=0;j<2;j++)
	    fin>>rec[i][j];
    solve();
    int p,q;
    fout<<minArea<<endl;
    for(set<int>::iterator it=techI.begin();it!=techI.end();it++){
	fout<<*it<<' '<<tech[*it]<<endl;
    }

    return 0;
}
