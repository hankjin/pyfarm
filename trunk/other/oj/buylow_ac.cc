#include <iostream>
#include <fstream>
#define MAX 5002
#define MAXP 8
#define LIM 1000000000
using namespace std;

int cnt=0;

class hpnum {
    public:
	long sec[MAXP];
	int seccnt;

	hpnum() {
	    sec[seccnt=1]=0;
	}
	void plus(hpnum &P) {
	    cnt++;
	    int sect=seccnt>P.seccnt?seccnt:P.seccnt;
	    long T,up=0;
	    for(int i=1;i<=sect;i++) {
		if (i>seccnt)sec[i]=0;
		if (i>P.seccnt)P.sec[i]=0;
		T=sec[i]+P.sec[i]+up;
		up=T/LIM;
		sec[i]=T%LIM;
	    }
	    seccnt=sect;
	    if (up)
		sec[++seccnt]=up;
	}
	void cpy(hpnum &P) {
	    seccnt=P.seccnt;
	    for (int i=1;i<=seccnt;i++)
		sec[i]=P.sec[i];
	}
};

ifstream fi("buylow.in");
ofstream fo("buylow.out");

int N;
long s[MAX],MaxLength[MAX],Next[MAX];
hpnum MaxCnt[MAX];

void init() {
    int i,j;
    fi >> N;
    for (i=1;i<=N;i++)
	fi >>s[i];
    for (i=1;i<=N-1;i++)
	for (j=i+1;j<=N;j++)
	    if (s[j]==s[i]) {
		Next[i]=j;
		break;
	    }
    s[++N]=0;
}

void dynamic()
{
    int i,j;
    MaxLength[1]=1;
    MaxCnt[1].sec[1]=1;
    for (i=2;i<=N;i++) {
	MaxCnt[i].sec[1]=1;
	MaxLength[i]=1;
	for (j=1;j<=i-1;j++) {
	    if (Next[j] && Next[j]<i) continue;
	    if (s[j]>s[i]) {
		if (MaxLength[j]+1>MaxLength[i]) {
		    MaxLength[i]=MaxLength[j]+1;
		    MaxCnt[i].cpy(MaxCnt[j]);
		}
		else if (MaxLength[j]+1==MaxLength[i]) {
		    MaxCnt[i].plus(MaxCnt[j]);
		}
	    }
	}
    }
}

void writehp(hpnum &P)
{
    int i;
    long k;
    for (i=P.seccnt;i>=1;i--) {
	k=LIM/10;
	if (i!=P.seccnt && P.sec[i]<k) {
	    //补0输出
	    while (P.sec[i]<k) {
		fo << 0;
		k/=10;
	    }
	}
	if (P.sec[i])
	    fo << P.sec[i];
    }
}

void print() {
    fo << MaxLength[N]-1 << ' ';
    writehp(MaxCnt[N]);
    fo << endl;
    fi.close();
    fo.close();
}

int main() {
    init();
    dynamic();
    print();
    return 0;
}
