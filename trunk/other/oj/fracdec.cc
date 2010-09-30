/*
ID: hankjin1
LANG: C++
TASK: fracdec
*/
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
//note: 22/5 should be 4.4 NOT 4.4(0)
//note: 3/3  should be 1.0 NOT 1.
//note: if len>76, there shoud be newline

int N,D;
vector<int> res;
vector<int> mod;

FILE *fin = fopen("fracdec.in","r");
FILE *fout = fopen("fracdec.out", "w");
int ds[100000];

int len;
void output(int n){
    if(len>0 && len%76==0)
	fprintf(fout,"\n");
    fprintf(fout,"%d",n);
    len++;
}
inline void outputx(char c){
    if(len>0 && (len++)%76==0)
	fprintf(fout,"\n");
    fprintf(fout,"%c",c);
}
void outputN(int n){
    if(n==0){
	fprintf(fout,"0.");
	len=2;
	return;
    }
    while(n>0){
	res.push_back(n%10);
	n/=10;
    }
    for(int i=res.size()-1;i>=0;i--){
	output(res[i]);
    }
    outputx('.');
    res.clear();
}

int main(){
    fscanf(fin, "%d %d", &N,&D);
    len=0;
    outputN(N/D);
    N=N%D;
    res.reserve(100000);
    fill_n(ds,100000,-1);
    int pos=0;
    if(N!=0){
	ds[N]=0;
	for(int i=1;;i++){
	    N*=10;
	    res.push_back(N/D);
	    N=N%D;

	    if(ds[N]==-1){
		ds[N]=i;
	    }
	    else{
		pos=ds[N];
		break;
	    }
	}
    }
    for(int i=0;i<pos;i++){
	output(res[i]);
    }
    if(N!=0){
	outputx('(');
	for(int i=pos;i<res.size();i++){
	    output(res[i]);
	}
	outputx(')');
    }
    else{
	if(pos==0)
	    output(0);
    }
    fprintf(fout,"\n");
    return 0;
}
