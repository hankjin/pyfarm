/**
ID: hankjin1
LANG: C++
TASK: dualpal
*/
#include <iostream>
#include <fstream>
using namespace std;

int N,S;
bool isPal(int n){
    int base, i, j, v;
    char temp[256];
    int count = 0;
    for(base=2;base<=10;base++){
	v = n;
	for(i=0; v>0; i ++){
	    temp[i]=v%base;
	    v=v/base;
	}
	for(j = 0; j < i/2; j++){
	    if(temp[j] != temp[i-1-j]){
		count--;
		break;
	    }
	}
	count++;
	if(count >= 2)
	    return true;
    }
    return false;
}
int main(){
    ifstream fin("dualpal.in");
    ofstream fout("dualpal.out");
    fin>>N>>S;
    int i,j=S+1;
    for(i = 0; i < N; i ++,j++){
	while(!isPal(j)){
	    j++;
	}
	fout<<j<<endl;
    }
    
    return 0;
}

