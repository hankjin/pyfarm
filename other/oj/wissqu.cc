/**
ID: hankjin1
LANG: C++
TASK: wissqu
*/
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
using namespace std;

ifstream fin("wissqu.in");
ofstream fout("wissqu.out");
char squ[4][4];
bool used[4][4];
int pos[16][2];
char seq[17]="DAAABBBCCCDDDEEE";
int seqn[5]={3,3,3,3,3};
struct d{
    int x,y;
}dir[9]={{-1,-1},{-1,0},{-1,1},{0,-1},{0,0},{0,1},{1,-1},{1,0},{1,1}};

int ans;
char ansstr[17];
int anspos[16][2];

char badstr[17];
int badn;
bool ready(int x,int y, char c){
    int nx,ny;
    for(int i=0;i<9;i++){
	nx=x+dir[i].x, ny=y+dir[i].y;
	if(nx>=0 && ny>=0 && nx<4 && ny<4 && squ[nx][ny]==c)
	    return false;
    }
    return true;
}
int work(int n){
    if(n==16){
	if(ans==0){
	    strncpy(ansstr,seq,17);
	    for(int i=0;i<16;i++){
		anspos[i][0]=pos[i][0]+1;
		anspos[i][1]=pos[i][1]+1;
	    }
	}
	ans++;
	return 16;
    }
    int depth=n;
    for(int i=0;i<4;i++){
	for(int j=0;j<4;j++){
	    if(used[i][j]) continue;
	    if(ready(i,j, seq[n])){
		used[i][j]=true;
		char old = squ[i][j];
		squ[i][j]=seq[n];
		pos[n][0]=i,  pos[n][1]=j;
		int d = work(n+1);//deep into
		depth = max(depth, d);
		squ[i][j]=old;
		used[i][j]=false;
	    }
	}
    }
    return depth;
}
void nextseq(int n){
    if(n>=badn && strncmp(seq, badstr, badn)==0){
	return ;
    }
    if(n==16){
	badn = work(0)+1;//seq made
	strncpy(badstr, seq, badn);
//	cout<<seq<<","<<badn<<badstr<<endl;
	return ;
    }
    for(int i=0;i<5;i++){
	if(seqn[i]>0){
	    seq[n]='A'+i;
	    seqn[i]--;//mark as used
	    nextseq(n+1);
	    seqn[i]++;//mark back
	}
    }
}
int main(){
    for(int i=0;i<4;i++){
	for(int j=0;j<4;j++)
	    fin>>squ[i][j];
    }
    ans = 0;
    badn = 17;
    nextseq(1);
    for(int i=0;i<16;i++){
	cout<<ansstr[i]<<" "<<anspos[i][0]<<" "<<anspos[i][1]<<endl;
    }
    cout<<ans<<endl;
    return 0;
}
