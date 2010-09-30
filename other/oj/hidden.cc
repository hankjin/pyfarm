/*
ID: hankjin1
LANG: C++
TASK: hidden
 */
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
using namespace std;
//note: Case 8 TLE

ifstream fin("hidden.in");
ofstream fout("hidden.out");
int N;
char pwd[200001];
char buffer[80];

int main(){
    fin>>N;
    fin.getline(buffer,80, '\n');
    for(int i=N;i>0;i-=72){
	fin.getline(buffer, 80, '\n');
	strcat(pwd,buffer);
    }
    strncat(pwd, pwd, N);
    int i=0,j=1,k;
    while(i<N&&j<N){
	if(i==j)
	    j++;
	for(k=0;k<N;k++){
	    if(pwd[i+k]!=pwd[j+k])
		break;
	}
	if(k==N)
	    break;
	else if(pwd[i+k]>pwd[j+k])
	    i += k+1;
	else
	    j += k+1;
    }
    fout<<i<<endl;
    
    return 0;
}
