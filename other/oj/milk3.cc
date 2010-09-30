/*
ID: hankjin1
LANG: C++
TASK: milk3
*/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
using namespace std;

#define BFS(a,b,c) bfs(((a)<<16)|((b)<<8)|(c))
int A,B,C;
vector<int> hist;
vector<int> ans;

inline void pour(int &from, int fromX, int &to, int toX){
    if(from < toX-to){//pour all
	to+=from;
	from = 0;
    }
    else{//pour some
	from -= toX-to;
	to = toX;
    }
}
void bfs(int status){
    if(find(hist.begin(),hist.end(),status)!=hist.end())
	return ;
    hist.push_back(status);
    if((status&0xFF0000) == 0){
	ans.push_back(status&0xFF);
	/*
	cout<<((status&0xFF0000)>>16)<<","<<((status&0x00FF00)>>8)
	    <<","<<(status&0x0000FF)<<endl;
	    */
    }
    int a = (status&0xFF0000)>>16;
    int b = (status&0x00FF00)>>8;
    int c = (status&0x0000FF);
    int x,y;
    if(a>0){
	x = a; y = b; //a->b
	pour(x,A,y,B);
	BFS(x,y,c);
	x = a; y = c; //a->c
	pour(x,A,y,C);
	BFS(x,b,y);
    }
    if(b>0){
	x = b; y = a;//b->a
	pour(x,B,y,A);
	BFS(y, x, c);
	x = b; y = c;//b->c
	pour(x,B,y,C);
	BFS(a, x, y);
    }
    if(c>0){
	x = c; y = a; //c->a
	pour(x,C,y,A);
	BFS(y, b, x);
	x = c; y = b;//c->b
	pour(x,C,y,B);
	BFS(a,y,x);
    }
}
int main(){
    ifstream fin("milk3.in");
    ofstream fout("milk3.out");
    fin>>A>>B>>C;
    bfs(C);
    sort(ans.begin(), ans.end());
    int i;
    for(i = 0; i < ans.size()-1;i++)
	fout<<ans[i]<<" ";
    fout<<ans[i]<<endl;

    return 0;
}
