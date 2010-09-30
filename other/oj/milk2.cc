/*
ID: hankjin1
LANG: C++
TASK: milk2
*/
//use vector to accelerate
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;
typedef struct {
    int start;
    int end;
}dur;
vector<int> steps;
dur jobs[5000];
int main(){
    int n;
    ifstream fin("milk2.in");
    ofstream fout("milk2.out");
    fin>>n;
    int min=1000000, max=0;
    for(int i = 0; i < n; i ++){
	fin>>jobs[i].start>>jobs[i].end;
	steps.push_back(jobs[i].start);
	steps.push_back(jobs[i].end);
	if(jobs[i].start < min)
	    min = jobs[i].start;
	if(jobs[i].end > max)
	    max = jobs[i].end;
    }
    int maxFeed = 0;
    int maxHungry = 0;
    int fbegin=min, hbegin=min;
    bool found=false,pFound;
    bool debug=false;
    sort(steps.begin(),steps.end());
    for(vector<int>::iterator it=steps.begin(); it!=steps.end(); it++){
	int i=*it;
	pFound=found;
	found=false;
	for(int j = 0; j < n; j ++){
	    if(i>=jobs[j].start && i<jobs[j].end){
		found=true;
	    }
	}
	if(found){
	    if(!pFound){
		if(debug)
		    cout<<i<<"GOT"<<endl;
		if(maxHungry < i-hbegin)
		   maxHungry = i - hbegin;
		fbegin = i;
		if(debug)
		    cout<<"Hung"<<maxHungry<<endl;
	    }
	}
	else{
	    if(pFound){
		if(debug)
		    cout<<i<<"MISS"<<endl;
		if(maxFeed < i-fbegin)
		    maxFeed = i-fbegin;
		hbegin = i;
		if(debug)
		    cout<<"Feed"<<maxFeed<<endl;
	    }
	}

    }
    fout<<maxFeed<<' '<<maxHungry<<endl;
    return 0;
}
