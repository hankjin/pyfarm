/**
ID: hankjin1
LANG: C++
TASK: zerosum
*/
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

ifstream fin("zerosum.in");
ofstream fout("zerosum.out");

int N;
int nums[10];
char rule[]=" +-";
char ops[10];
bool test(){
    int op1,op2,j;
    char op=' ';
    for(int i=1;i<=N;i++){
	op2=nums[i];
	for(j=i; ops[j]==' '&&j<N; j++){
	    op2=op2*10+nums[++i];
	}
	if(op != ' '){
	    if(op=='+')
		op1+=op2;
	    else if(op=='-')
		op1-=op2;
	}
	else{
	    op1 = op2;
	}
	op = ops[j];
    }
    if(op1==0){
	for(int i=1;i<=N;i++){
	    fout<<nums[i];
	    if(i<N)
		fout<<ops[i];
	}
	fout<<endl;
    }

    return op1==0;
}
int solve(int x){
    if(x==N){
	if(test())
	    return 1;
	else
	    return 0;
    }
    int methods=0;
    for(int i = 0; i<strlen(rule);i++){
	ops[x]=rule[i];
	methods+=solve(x+1);
    }
    return methods;
}
int main(){
    fin>>N;
    for(int i = 1;i<10;i++)
	nums[i]=i;
    solve(1);
    //cout<<solve(1)<<endl;
    return 0;
}
