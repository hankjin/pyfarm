/*
ID: hankjin1
LANG: C++
TASK: humble
*/
#include <iostream>
#include <fstream>
#include <cmath>
#include <climits>
#include <set>
#include <algorithm>
using namespace std;
//note: delete used nums in set
//note: notice overflow of multiply
//note: this use myown set, but is more slow than stl set, why?

//#define DEBUG

int K,N;
set<long long> nums;
int found;
int ss[100];
struct node{
    int value;
    struct node* prev;
    struct node* next;
}nodes[100005];
struct node *first;
struct node *end;
struct node *frees;

void init(){
    first = end = NULL;
    for(int i=0;i<100005-1;i++)
	nodes[i].next=&nodes[i+1];
    nodes[100005-1].next=NULL;
    frees = &nodes[0];
}
void addnum(int v){
    struct node *tmp;
    if(frees->next!=NULL){
	tmp = frees;
	frees = frees->next;
    }
    else{
	tmp = end;
	end = end->prev;
    }
    tmp->value = v;
    if(first==NULL){
	first=end=tmp;
	first->next=NULL;
    }
    else if(v<first->value){
	tmp->next = first;
	first->prev = tmp;
	first = tmp;
    }
    else{
	struct node *pos = first;
	for(;pos!=NULL;pos=pos->next){
	    if(v <= pos->value)
		break;
	}

	if(pos==NULL){//add to tail
	    tmp->prev = end;
	    end->next = tmp;
	    end = tmp;
	    end->next = NULL;
	}
	else if(v==pos->value){//duplicate
	    tmp->next=frees;
	    frees = tmp;
	}
	else{//insert 
	    tmp->prev = pos->prev;
	    tmp->next = pos;
	    tmp->prev->next = tmp;
	    pos->prev = tmp;
	}
    }
#ifdef DEBUG
    for(tmp=first;tmp!=NULL;tmp=tmp->next){
	cout<<tmp->value<<" ";
    }
    cout<<endl;
#endif
}
int getnum(){
    struct node *tmp = first;
    int v = first->value;
    first = first->next;
    tmp->next = frees;
    frees = tmp;
    return v;
}

int main(){
    ifstream fin("humble.in");
    ofstream fout("humble.out");
    fin>>K>>N;
    for(int i=0;i<K;i++){
	fin>>ss[i];
    }
    init();
    addnum(1);
    long long res=0;
    for(int i=0;i<=N;i++){
	res = getnum();
	for(int j=0;j<K;j++){
	    if(ss[j]*res<INT_MAX)
		addnum(ss[j]*res);
	}
    }
    cout<<res<<endl;
    return 0;
}
