/**
ID: hankjin1
LANG: C++
TASK: milk
*/
/**
 * map-> multimap
 */
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <vector>
using namespace std;

int N,M;
map<int,int>farmer;
vector<int> as;
int main(){
    ifstream fin("milk.in");
    ofstream fout("milk.out");
    fin>>N>>M;
    int i,j, p, a;
    for(i = 0; i < M; i ++){
	fin>>p>>a;
	farmer[p]=a;
	as.push_back(p);
    }
    sort(as.begin(),as.end());
    int sum = 0;
    for(vector<int>::iterator it=as.begin(); it!=as.end() && N > 0;it++){
	cout<<*it<<','<<farmer[*it]<<endl;
	if(N>farmer[*it]){
	    sum+=*it * farmer[*it];
	    N-=farmer[*it];
	}
	else{
	    sum+=*it * N;
	    N-=N;
	}
    }
    fout<<sum<<endl;

    return 0;
}
