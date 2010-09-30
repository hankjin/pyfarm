/*
ID: hankjin1
LANG: C++
PROG: clocks
*/
#include<iostream>
#include<fstream>
using namespace std;

int G[10],T[10],i,ANS[10],SUM,MIN;
int main()
{
    ifstream fin("clocks.in");
    ofstream fout("clocks.out");
    int i,j;
    for(i=1;i<=9;i++)
	fin>>G[i];
    MIN=36;
    for(T[1]=0;T[1]<=3;T[1]++)
	for(T[2]=0;T[2]<=3;T[2]++)
	    for(T[3]=0;T[3]<=3;T[3]++)
		for(T[4]=0;T[4]<=3;T[4]++)
		    for(T[5]=0;T[5]<=3;T[5]++)
			for(T[6]=0;T[6]<=3;T[6]++)
			    for(T[7]=0;T[7]<=3;T[7]++)
				for(T[8]=0;T[8]<=3;T[8]++)
				    for(T[9]=0;T[9]<=3;T[9]++) {
					if((G[1]+3*(T[1]+T[2]+T[4]))%12!=0) continue;
					if((G[2]+3*(T[1]+T[2]+T[3]+T[5]))%12!=0) continue;
					if((G[3]+3*(T[2]+T[3]+T[6]))%12!=0) continue;
					if((G[4]+3*(T[1]+T[4]+T[5]+T[7]))%12!=0) continue;
					if((G[5]+3*(T[1]+T[3]+T[5]+T[7]+T[9]))%12!=0) continue;
					if((G[6]+3*(T[3]+T[5]+T[6]+T[9]))%12!=0) continue;
					if((G[7]+3*(T[4]+T[7]+T[8]))%12!=0) continue;
					if((G[8]+3*(T[5]+T[7]+T[8]+T[9]))%12!=0) continue;
					if((G[9]+3*(T[6]+T[8]+T[9]))%12!=0) continue;
					SUM=0;
					for(i=1;i<=9;i++)
					    SUM+=T[i];
					if(SUM>MIN) break;
					if(SUM<=MIN)
					{
					    for(i=1;i<=9;i++)
						ANS[i]=T[i];
					    MIN=SUM;
					}
				    }
    for(i=0,j=0;j<MIN;j++){
	while(ANS[i]==0) 
	    i++;
	ANS[i]--;
	fout<<i;
	if(j<MIN-1)
	    fout<<' ';
    }
    fout<<endl;
    return 0;
}
