/*
ID: hankjin1
LANG: C++
TASK: job
*/
#include <iostream>
#include <fstream>
#include <climits>
#include <algorithm>
using namespace std;

//greedy and dp
int N,M1,M2;
int jobA[1000];
int jobB[1000];
struct m{
    int dur;
    int free;
}m1s[30], m2s[30];
int resA,resB;

int _solve(struct m *ms, int size, int *job){
    int res = 0;
    for(int i=0;i<N;i++){
	int minJ, minV = INT_MAX;
	for(int j=0;j<size;j++){
	    if(ms[j].free+ms[j].dur < minV){
		minV = ms[j].free+ms[j].dur;
		minJ = j;
	    }
	}
	ms[minJ].free += ms[minJ].dur;
	job[i] = ms[minJ].free;
	res = max(res, job[i]);
    }
    return res;
}
void solve(){
    //a
    resA = _solve(m1s, M1, jobA);
    sort(jobA, jobA+N);
    //b
    resB = _solve(m2s, M2, jobB);
    sort(jobB, jobB+N);
    for(int i=0;i<N;i++){
	resB = max(resB, jobA[i]+jobB[N-1-i]);
    }
}
int main(){
    ifstream fin("job.in");
    ofstream fout("job.out");
    fin>>N>>M1>>M2;
    for(int i=0;i<M1;i++)
	fin>>m1s[i].dur;
    for(int i=0;i<M2;i++)
	fin>>m2s[i].dur;
    solve();
    fout<<resA<<" "<<resB<<endl;
}
