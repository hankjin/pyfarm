/*
ID: hankjin1
LANG: C++
TASK: ratios
*/
#include <iostream>
#include <fstream>
#include <climits>
using namespace std;

//note: 1. min  a*i+b*j+c*k
int dst[3];//dest mixture
int t[3];//temp store the total of all
int src[3][3];//src mixture
int res[4];//result
int main(){
    ifstream fin("ratios.in");
    ofstream fout("ratios.out");
    for(int i=0;i<3;i++)
	fin>>dst[i];
    for(int i=0;i<3;i++){
	for(int j=0;j<3;j++)
	    fin>>src[i][j];
    }
    int min = 300;
    for(int i=0;i<100;i++){
	for(int j=0;j<100;j++){
	    if(i+j>=min)//good 
		continue;
	    for(int k=0;k<100;k++){
		if(i+j+k==0)
		    continue;
		for(int x=0;x<3;x++){
		    t[x]=i*src[0][x]+j*src[1][x]+k*src[2][x];
		}
		if(i+j+k>=min)
		    continue;
		if(dst[0]!=0 && t[0]%dst[0]!=0)
		    continue;
		if(dst[1]!=0 && t[1]%dst[1]!=0)
		    continue;
		if(dst[2]!=0 && t[2]%dst[2]!=0)
		    continue;
		if((t[0]*dst[1]==t[1]*dst[0])&&(t[1]*dst[2]==t[2]*dst[1])){
		    min=i+j+k;
		    res[0]=i;
		    res[1]=j;
		    res[2]=k;
		    for(int x=0;x<3;x++){
			if(dst[x]!=0)
			    res[3]=t[x]/dst[x];
		    }
		}
	    }
	}
    }
    if(min==300)
	fout<<"NONE"<<endl;
    else
	fout<<res[0]<<" "<<res[1]<<" "<<res[2]<<" "<<res[3]<<endl;
    return 0;
}
