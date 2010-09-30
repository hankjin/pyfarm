/**
ID: hankjin1
LANG: C++
TASK: lamps
*/
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;
//#define DEBUG
//note: only 16 kinds of ops

ifstream fin("lamps.in");
ofstream fout("lamps.out");
int N,C;
int ons[100];
int offs[100];
bool status[100];
int ops[4];
int onNum, offNum;
char db[16][101];
bool check(bool a, bool b, bool c, bool d){
    int i;
#ifdef DEBUG
    cout<<"Method:"<<a<<","<<b<<","<<c<<","<<d<<endl;
#endif
    for(i=0;i<N;i++)
	status[i]=true;
    if(a){
	for(i=0;i<N;i++)
	    status[i]=!status[i];
    }
    if(b){
	for(i=0;i<N;i+=2)
	    status[i]=!status[i];
    }
    if(c){
	for(i=1;i<N;i+=2)
	    status[i]=!status[i];
    }
    if(d){
	for(i=0;i<N;i+=3){
	    status[i]=!status[i];
	}
    }
    for(i=0;i<onNum;i++){
	if(status[ons[i]] != true)
	    return false;
    }
    for(i=0;i<offNum;i++){
	if(status[offs[i]] != false)
	    return false;
    }
    int x = (a?8:0)|(b?4:0)|(c?2:0)|(d?1:0);
    for(i=0;i<N;i++){
	db[x][i]=status[i]?'1':'0';
    }
    db[x][i]='\0';
    return true;
}

int main(){
    fin>>N>>C;
    int i,j,x,y;
    onNum = offNum = -1;
    int temp;
    do{
	fin>>temp;
	ons[++onNum]=temp-1;
    }while(temp!=-1);
    do{
	fin>>temp;
	offs[++offNum]=temp-1;
    }while(temp!=-1);
    for(i=0;i<2;i++){
	for(j=0;j<2;j++){
	    for(x=0;x<2;x++){
		for(y=0;y<2;y++){
		    check(i==1,j==1,x==1,y==1);
		}
	    }
	}
    }
    bool ans[16];
    memset(ans,0,sizeof(bool)*16);

    //last accelerate
    if(C>16){
	if(C%2==0)
	    C=4;
	else
	    C=3;
    }
    for(i=C;i>=0;i--){
	for(j=0;j<=C-i;j++){
	    for(x=0;x<=C-i-j;x++){
		y=C-i-j-x;
		ans[(i%2?8:0)|(j%2?4:0)|(x%2?2:0)|(y%2?1:0)]=true;
	    }
	}
    }

    vector<string> hjz;
    for(i=0;i<16;i++){
	if(ans[i] && db[i][0]!='\0'){
	    string temp(db[i]);
	    hjz.push_back(temp);
	}
    }
    if(hjz.empty()){
	fout<<"IMPOSSIBLE"<<endl;
    }
    sort(hjz.begin(),hjz.end());
    vector<string>::iterator iend=unique(hjz.begin(),hjz.end());
    for(vector<string>::iterator it=hjz.begin();it!=hjz.end();it++){
	fout<<*it<<endl;
    }
}
