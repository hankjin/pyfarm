/**
ID: hankjin1
LANG: C++
TASK: clocks
*/
//note: duplicate shoud be canceld
#include <iostream>
#include <fstream>
#include <algorithm>
#include <climits>
#include <vector>
#include <iterator>
using namespace std;

int orig[9];
int moves[2000];
vector< vector<int> >steps;
vector<int> hist;
int minMove;
int rule[][10]={
    {4,0,1,3,4},
    {3,0,1,2},
    {4,1,2,4,5},
    {3,0,3,6},//4
    {5,1,3,4,5,7},
    {3,2,5,8},
    {4,3,4,6,7},//7
    {3,6,7,8},
    {4,4,5,7,8},
};
void bfs(){
    int i,j,k;
    int temp;
    int oldSize = steps.size();
    bool flag = false;
    for(i = 0; i < oldSize; i++){
	vector<int> v = steps[i];
	temp = 0;
	for(j = 0; j < 9; j++){
	    temp = temp*10 + v[j];
	}
	if(temp > 0){
	    if(find(hist.begin(), hist.end(), temp)!=hist.end())
		continue;
	    else
		hist.push_back(temp);
	}
	if(temp == 0){//done
	    if(v.size()-9 < minMove){
		minMove = v.size() - 9;
		flag = true;
		copy(v.begin()+9, v.end(), moves);

	    }
	    else if(v.size()-9 == minMove){
		//todo
		for(k=9;k<v.size();k++){
		    if(moves[k-9] > v[k]){
			copy(v.begin()+9, v.end(), moves);
			break;
		    }
		    else if(moves[k-9] < v[k])
			break;
		}
	    }
	}
	else{
	    for(j=0;j<9;j++){
		vector<int> child(v.begin(), v.end());
		for(k=1;k<=rule[j][0];k++){
		    child[rule[j][k]] = (child[rule[j][k]]+3)%12;
		}
		child.push_back(j+1);
		steps.push_back(child);
	    }
	}
    }
    if(!flag){
	steps.erase(steps.begin(), steps.begin()+oldSize);
	bfs();
    }
}
int main(){
    ifstream fin("clocks.in");
    ofstream fout("clocks.out");
    vector<int> orig;
    int i, temp;
    for(i = 0; i < 9; i++){
	fin>>temp;
	orig.push_back(temp%12);
    }
    minMove = INT_MAX;
    steps.push_back(orig);
    bfs();
    for(i=0; i< minMove-1; i++)
	fout<<moves[i]<<' ';
    fout<<moves[i]<<endl;
    return 0;
}
