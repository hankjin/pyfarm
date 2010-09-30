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
//fill sub tree
void filltree(int from, int no){
    used[from]=no;
    for(int i=1;i<=N;i++){
	if(used[i]<=0 && school[from][i])
	    filltree(i, no);
    }
}
//find a root
void travel(int x, int no){
    used[x]=-1;
    for(int i=1;i<=N;i++){
	if(0==used[i] && school[i][x]){
	    travel(i, no);
	    return ;
	}
    }
    filltree(x, no);
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
    fill_n(used, N+1, 0);
    int resA=0;
    for(int i=1;i<=N;i++){
	if(!used[i]){
	    travel(i, ++resA);
	}
    }
    fout<<resA<<endl;
    //floyd
    for(int k=1;k<=N;k++){
	for(int i=1;i<=N;i++){
	    for(int j=1;j<=N;j++){
		if(i!=k && j!=k && i!=j){
		    if(school[i][k]>0 && school[k][j]>0)
			school[i][j]=1;
		}
	    }
	}
    }
    fill_n(used,N+1,0);
    int resB=0;
    for(int i=1;i<=N;i++){
	if(0==used[i]){
	    used[i]=++resB;
	    for(int j=1;j<=N;j++){
		if(school[i][j]>0 && school[j][i]>0)
		    used[j]=resB;
	    }
	}
    }
    fout<<(resB-1)<<endl;
    return 0;
}
