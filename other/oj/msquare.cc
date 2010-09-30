/*
ID: hankjin1
LANG: C++
TASK: msquare
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
using namespace std;
//note: dest==init, notice that condition
//simply bfs, only 8! conditions.
//note: use backward bfs, no need to store way!!

ifstream fin("msquare.in");
ofstream fout("msquare.out");

unsigned int dest;
int oldPos;
set<unsigned int> hist;
typedef struct _node{
    unsigned int v;
    string s;
}node;
vector<node> way;
unsigned int TA(unsigned int n){
    unsigned int tmp[8];
    for(int i=1;i<=8;i++)
	tmp[i-1]=(n>>(4*(8-i)))&0xF;
    unsigned int res=0;
    for(int i=1;i<=8;i++){
	res |= tmp[8-i]<<(4*(8-i));
    }
    return res;
}
unsigned int TB(unsigned int n){
    int top=((n&0x000F0000)<<12) | ((n&0xFFF00000)>>4);
    int bot=((n&0x0000F000)>>12) | ((n&0x00000FFF)<<4);
    return top|bot;
}
unsigned int TC(unsigned int n){
    int n2=(n&0x0F000000)>>4;
    int n3=(n&0x00F00000)>>12;
    int n6=(n&0x00000F00)>>4;
    int n7=(n&0x000000F0)<<20;
    return (n&0xF00FF00F)|n2|n3|n6|n7;
}
void addnode(unsigned int v, string s){
    hist.insert(v);
    node n = {v, s};
    way.push_back(n);
}
void print(string s){
    fout<<s.size()<<endl;
    fout<<s<<endl;
}
void bfs(){
    int i=oldPos;
    oldPos = way.size();
    cout<<oldPos-i<<endl;
    for(;i<oldPos;i++){
	unsigned int status=way[i].v;
	int a = TA(status);
	if(hist.count(a)==0){
	    if(a==dest){
		print(way[i].s+"A");
		return;
	    }
	    addnode(a, way[i].s+"A");
	}
	int b = TB(status);
	if(hist.count(b)==0){
	    if(b==dest){
		print(way[i].s+"B");
		return ;
	    }
	    addnode(b, way[i].s+"B");
	}
	int c = TC(status);
	if(hist.count(c)==0){
	    if(c==dest){
		print(way[i].s+"C");
		return ;
	    }
	    addnode(c, way[i].s+"C");
	}
    }
    bfs();
}
int main(){
    unsigned int tmp;
    for(int i=7;i>=0;i--){
	fin>>tmp;
	dest |= tmp<<(4*i);
    }
    unsigned int status=0;
    for(int i=1;i<=8;i++){
	status |= i<<(4*(8-i));
    }
    addnode(status, "");
    if(status==dest)
	fout<<"0\n"<<endl;
    else
	bfs();
    return 0;
}
