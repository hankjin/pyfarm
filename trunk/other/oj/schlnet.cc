/*
ID: hankjin1
LANG: C++
TASK: schlnet
*/
#include <iostream>
#include <fstream>
using namespace std;

ifstream fin("schlnet.in");
ofstream fout("schlnet.out");
int N;
int school[101][101];
int used[101];
bool istop[101];
//fill sub tree
void filltree(int from, int no){
    istop[from]=false;
    used[from]=no;
    for(int i=1;i<=N;i++){
	if(school[from][i]  && used[i]!=no){
	    filltree(i, no);
	}
    }
}
//find a root
int travel(){
    fill_n(used, N+1, 0);
    fill_n(istop, N+1, false);
    int no=0;
    for(int i=1;i<=N;i++){
	if(0==used[i]){
	    filltree(i,++no);
	    istop[i]=true;
	}
    }
    int res=0;
    for(int i=1;i<=N;i++){
	if(istop[i])
	    res++;
    }
    return res;
}
int main(){
    fin>>N;
    int x;
    for(int i=1;i<=N;i++){
	while(true){
	    fin>>x;
	    if(x==0)
		break;
	    school[i][x]=1;
	}
    }
    //case A
    int resA=travel();
    fout<<resA<<endl;
    //revert
    for(int i=1;i<=N;i++){
	for(int j=i;j<=N;j++){
	    int temp=school[i][j];
	    school[i][j]=school[j][i];
	    school[j][i]=temp;
	}
    }
    //case B
    int resB=travel();
    if(resA==1&&resB==1){
	resA=resB=0;
	for(int i=1;i<=N;i++){
	    int in=0,out=0;
	    for(int j=1;j<=N;j++){
		if(school[i][j]>0) out++;
		if(school[j][i]>0) in++;
	    }
	    if((in==0&&out>0) || (in>0&&out==0))
		resA=resB=1;
	}
    }
    fout<<max(resA,resB)<<endl;
    return 0;
}
