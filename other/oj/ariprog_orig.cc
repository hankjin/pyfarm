/**
ID: hankjin1
LANG: C++
TASK: ariprog
*/
//note: NONE condition must be considered
//note: user array instead of stl may accelerate
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <climits>
#include <map>
#include <set>
#include <algorithm>
using namespace std;

vector<int> nums;
bool hist[250*250+2];
vector< pair<int,int> > ans;
int main(){
    ifstream fin("ariprog.in");
    ofstream fout("ariprog.out");
    int N,M;
    fin>>N>>M;
    int i,j, k, temp;
    for(i=0; i<= M;i++){
	nums.push_back(i*i);
    }
    for(i=1; i<=M;i++){
	for(j=1; j<=i; j++){
	    nums.push_back(nums[i]+nums[j]);
	}
    }
    nums.push_back(INT_MAX);
    sort(nums.begin(), nums.end());
    vector<int>::iterator myend = unique(nums.begin(), nums.end());
    int maxnum = 2*M*M;
    int stepmax = (2*M*M)/(N-1);
    int numsize = myend - nums.begin() - 1;
    vector<int> ians;
    for(i=1; i<=stepmax;i++){//step
	memset(hist,0, sizeof(bool)*numsize);
	ians.clear();
	for(j=0;j<numsize-N;j++){//begin
	    if(hist[j])
		continue;
	    hist[j]=true;
	    temp = nums[j] + i;
	    for(k=j+1;k<numsize;k++){
		while(nums[k] < temp)
		    k++;
		if(nums[k] > temp)
		    break;
		else{
		    temp += i;
		    hist[k]=true;
		}
	    }
	    for(k=nums[j]; k <= temp-i*N; k+=i){
		ians.push_back(k);
	    }
	}
	sort(ians.begin(), ians.end());
	for(vector<int>::iterator it=ians.begin();it!=ians.end();it++){
	    ans.push_back(make_pair(*it, i));
	}
    }
    if(ans.empty()){
	fout<<"NONE"<<endl;
    }
    for(vector<pair<int,int> >::iterator it=ans.begin();it!=ans.end();it++){
	fout<<it->first<<" "<<it->second<<endl;
    }

    return 0;
}
