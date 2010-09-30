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
bool todo[100001];
char pwd[100001];
char buffer[80];

int main(){
    fin>>N;
    fin.getline(buffer,80, '\n');
    for(int i=N;i>0;i-=72){
	fin.getline(buffer, 80, '\n');
	strcat(pwd,buffer);
    }
    fill_n(todo, 100001, true);
    char minc = 'z'+1;
    for(int i=0;i<N;i++){
	minc = 'z'+1;
	for(int j=0;j<N;j++){
	    if(todo[j])
	       	minc = min(minc, pwd[(j+i)%N]);//fix: %N
	}
	for(int j=0;j<N;j++){
	    if(todo[j]){
		if(pwd[(j+i)%N]!=minc)
		    todo[j]=false;
	    }
	}
    }
    for(int i=0;i<N;i++){
	if(todo[i]){
	    fout<<i<<endl;
	    break;
	}
    }
    return 0;
}
