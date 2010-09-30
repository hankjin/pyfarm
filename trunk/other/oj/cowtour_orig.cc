/*
ID: hankjin1
LANG: C++
TASK: cowtour
*/
#include <climits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;
#define MYMAX 100001
#define ISMAX(a) (a>100000)


FILE *fin=fopen("cowtour.in","r");
FILE *fout=fopen("cowtour.out","w");
int N;
int pos[150][2];
double dist[150][150];
double total[150][150];
int group[150];
bool flag[150];
#define DIST(a,b) sqrt((pos[a][0]-pos[b][0])*(pos[a][0]-pos[b][0])+\
	(pos[a][1]-pos[b][1])*(pos[a][1]-pos[b][1]))

//find max dist of a group
void findmax(){
    for(int i=0;i<N;i++){
	for(int j=0;j<N;j++){
	    total[i][j]=dist[i][j];
	}
    }
    for(int k=0;k<N;k++){
	for(int i=0;i<N;i++){
	    if(i==k)
		continue;
	    for(int j=0;j<N;j++){
		if(j==k || j==i)
		    continue;
		if(total[i][k]+total[k][j]<total[i][j])
		    total[i][j]=total[i][k]+total[k][j];
	    }
	}
    }
}
//group into 2
void mygroup(int x){
    if(group[x]==1)
	return;
    group[x]=1;
    for(int i=0;i<N;i++){
	if(!ISMAX(dist[x][i]) && group[i]!=1){
	    mygroup(i);
	}
    }
}
int main(){
    fscanf(fin,"%d\n",&N);
    for(int i=0;i<N;i++){
	fscanf(fin,"%d %d\n",&pos[i][0],&pos[i][1]);
    }
    char conned[151];
    for(int i=0;i<N;i++){
	fscanf(fin,"%s",conned);
	for(int j=0;j<N;j++){
	    if(j>i)
		continue;
	    else if(j==i)
		dist[i][j]=0;
	    else if(conned[j]=='1')
		dist[i][j]=dist[j][i]=DIST(i,j);
	    else
		dist[i][j]=dist[j][i]=MYMAX;
	}
    }
    //group
    mygroup(0);
    //find max of each group
    findmax();
    double gmax[2];
    gmax[0]=gmax[1]=0;
    for(int i=0;i<N;i++){
	for(int j=i+1;j<N;j++){
	    if(group[i]!=group[j])//unreachable
		continue;
	    if(gmax[group[i]]<total[i][j])
		gmax[group[i]]=total[i][j];
	}
    }
    double gsmax = max(gmax[0],gmax[1]);
    double conlen;
    double res=MYMAX;
    //find max when group conned
    for(int i=0;i<N;i++){//each of group 0
	for(int j=0;j<N;j++){//each of group 1
	    if(i==j)
		continue;
	    if(!ISMAX(total[i][j]))
		continue;
	    gmax[0]=gmax[1]=0;//connect i & j
	    for(int k=0;k<N;k++){//max of left of i, max of right of j
		if(!ISMAX(total[i][k]))
		    gmax[0] = max(gmax[0], total[i][k]);
		if(!ISMAX(total[j][k]))
		    gmax[1] = max(gmax[1], total[j][k]);
	    }
	    conlen = gmax[0]+gmax[1]+DIST(i,j);//conned len
	    conlen = max(conlen, gsmax);//max of left, right, conned
	    res = min(res, conlen);
	    cout<<"i"<<i<<","<<j<<":"<<gmax[1]<<":"<<res<<endl;
	}
    }
    fprintf(fout,"%.6f\n",res);
//    cout<<res<<endl;
    return 0;
}
