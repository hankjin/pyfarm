/*
ID: hankjin1
LANG: C++
TASK: twofive
*/
#include <iostream>
#include <fstream>
using namespace std;

#define maxstate 7776
ifstream fin("twofive.in");
ofstream fout("twofive.out");

const char alphabet[]="ABCDEFGHIJKLMNOPQRSTUVWXY";
using namespace std;
int dp[maxstate],row[6],xpos[30],ypos[30];
char task,s[30],ans[30];
inline void init_arrays() {
    for (int i=1;i<7;i++) 
	row[i]=0;
    for (int i=0;i<maxstate;i++) 
	dp[i]=-1; 
    row[0]=5;
    dp[maxstate-1]=1;  
}

inline int dfs(int ch) {
    int nowstate=0;
    for (int i=1;i<6;i++) 
	nowstate=nowstate*6+row[i];
    if (dp[nowstate]<0) {
	dp[nowstate]=0;
	if (!xpos[ch]) {
	    for (int i=1;i<6;i++)
		if (row[i]<row[i-1]) {
		    ++row[i];
		    dp[nowstate]+=dfs(ch+1);
		    --row[i];
		}
	}
	else {
	    if (row[xpos[ch]]<row[xpos[ch]-1]&&row[xpos[ch]]+1==ypos[ch]) {
		++row[xpos[ch]];
		dp[nowstate]+=dfs(ch+1);
		--row[xpos[ch]];
	    }                            
	}
    }
    return dp[nowstate];
}

inline int DP() {
    init_arrays();
    return dfs(0);
}

inline void num2word() {
    int n;
    fin>>n;
    for (int i=0;i<25;i++) {
	xpos[i]=0;
	ypos[i]=0;
    }
    for (int i=1;i<6;i++)
	for (int j=1;j<6;j++)//put a alpha at (i,j)
	    for (int ch=0;ch<25;ch++) 
		if (!xpos[ch]) {//not used
		    xpos[ch]=i;
		    ypos[ch]=j;
		    int temp=DP();
		    if (n>temp) 
			n-=temp;
		    else 
			break;
		    xpos[ch]=0;//put another
		    ypos[ch]=0;
		}
    for (int i=0;i<25;i++)
	ans[(xpos[i]-1)*5+ypos[i]-1]=alphabet[i];
    fout<<ans<<endl;
}

inline int word2num() {
    int ANSWER=1;
    char tmp;
    for (int i=1;i<6;i++)
	for (int j=1;j<6;j++) {
	    fin>>tmp;
	    int nowch=tmp-65;
	    for (int ch=0;ch<nowch;ch++)
		if (!xpos[ch]) {
		    xpos[ch]=i;ypos[ch]=j;
		    ANSWER+=DP();
		    xpos[ch]=0;ypos[ch]=0;
		}
	    xpos[nowch]=i;ypos[nowch]=j;
	}
    return ANSWER;
}

int main() {
    fin>>task;
    if (task=='N') num2word();
    else fout<<word2num()<<endl;
    return 0;
}

