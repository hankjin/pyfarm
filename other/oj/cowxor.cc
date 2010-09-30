/**
ID: hankjin1
LANG: C++
TASK: cowxor
*/
#include <iostream>
#include <fstream>
using namespace std;

ifstream fin("cowxor.in");
ofstream fout("cowxor.out");
int N;
int cow[100001];
typedef struct _node{
    int i,v;
    struct _node *son[2];
    _node(){
	son[0]=son[1]=NULL;
    }
}node;
node root;
void add(int ind,int v){
    node *cur = &root;
    for(int i=20;i>=0; i--){
	int pos = (v&(1<<i))>0;
	if(cur->son[pos]==NULL){
	    cur->son[pos]=new node();
	}
	cur=cur->son[pos];
    }
    cur->i = ind;
    cur->v = v;
}
node* findmax(int v){
    node *cur = &root;
    for(int i=20;i>=0;i--){
	int pos = (v&(1<<i))>0;
	if(cur->son[(pos+1)%2]!=NULL)
	    cur = cur->son[(pos+1)%2];
	else
	    cur = cur->son[pos];
    }
    return cur;
}

int main(){
    fin>>N;
    int i,j;
    int maxV=-1, maxI1, maxI2;
    add(0,0);//it makes sure findmax return NO NULL
    for(i=1,cow[0]=0;i<=N;i++){
	fin>>cow[i];
	cow[i]^=cow[i-1];

	node *tmp = findmax(cow[i]);//go through tri
	if( ((tmp->v)^cow[i])>maxV ){//find a better solution
	    maxV = (tmp->v)^cow[i];
	    maxI1 = 1+tmp->i;
	    maxI2 = i;
	}

	add(i, cow[i]);//fix1. add after findmax
    }
    fout<<maxV<<" "<<maxI1<<" "<<maxI2<<endl;

    return 0;
}
