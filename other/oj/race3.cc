/**
ID: hankjin1
LANG: C++
TASK: race3
*/
#include <iostream>
#include <fstream>
using namespace std;

ifstream fin("race3.in");
ofstream fout("race3.out");

//note: notice to reduce the cycle
//note: the split condition: can not go back
int n;
int adj[50][50];
int radj[50][50];
bool used[50];
int unavoid[51];

bool dfs(int from, int dest){
    used[from]=true;
    for(int i=1; i<=adj[from][0]; i++){
	if(adj[from][i]==dest)
	    return true;
	if(used[ adj[from][i] ])
	    continue;
	else if(dfs( adj[from][i], dest)==true)
	    return true;
    }
    used[from]=false;
    return false;
}
int main(){
    int x;
    //read input
    for(n=0,fin>>x; x!=-1; n++){
	while(x!=-2){
	    if(x!=n){
		adj[n][ ++adj[n][0] ]=x; //fix 1
		radj[x][ ++radj[x][0] ]=n;
	    }
	    fin>>x;
	}
	fin>>x;
    }
    //find unavoid
    for(int i=1;i<n-1;i++){
	fill_n(used, n, false);
	used[i]=true;
	if(dfs(0, n-1) == false){
	    unavoid[ ++unavoid[0] ]=i;
	}
    }
    //print unavoid
    fout<<unavoid[0];
    for(int i=1; i<=unavoid[0]; i++){
	fout<<" "<<unavoid[i];
    }
    fout<<endl;
    //find split points
    int ans = unavoid[0];
    for(int i=1; i<=unavoid[0]; i++){
	int p = unavoid[i];
	for(int j=1;j<=radj[p][0];j++){// can not go back
	    int v = radj[p][j];
	    //on the right hand  //fix 2
	    fill_n(used, n, false);
	    used[p]=true;
	    if(dfs(v, n-1))
		continue;
	    //on the left hand
	    fill_n(used, n, false);
	    if(dfs(unavoid[i], v) == true){
		unavoid[i] = -1;
		ans--;
		break;
	    }
	}
    }
    fout<<ans;
    for(int i=1; i<=unavoid[0]; i++){
	if(unavoid[i]!=-1)
	    fout<<" "<<unavoid[i];
    }
    fout<<endl;

    return 0;
}
