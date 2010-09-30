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
long move[9] = {18911232, 19136512, 2363904, 16810048, 2134536, 262657, 36936, 73, 4617} ;
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
	if(v[0] == 0){//done
	    if(v.size() < minMove){
		minMove = v.size();
		flag = true;
		copy(v.begin(), v.end(), moves);

	    }
	    else if(v.size() == minMove){
		for(k=0;k<v.size();k++){
		    if(moves[k] > v[k]){
			copy(v.begin(), v.end(), moves);
			break;
		    }
		    else if(moves[k] < v[k])
			break;
		}
	    }
	}
	else{
	    for(j=0;j<9;j++){
		vector<int> child(v.begin(), v.end());
		temp = (child[0] + move[j])&57521883;
		if(find(hist.begin(),hist.end(),temp) == hist.end()){
		    hist.push_back(temp);
		    child[0] = temp;
		    child.push_back(j+1);
		    steps.push_back(child);
		}
	    }
	}
    }
    if(!flag){
	cout<<"Ones More"<<endl;
	steps.erase(steps.begin(), steps.begin()+oldSize);
	bfs();
    }
}
int main(){
    ifstream fin("clocks.in");
    ofstream fout("clocks.out");
    vector<int> orig;
    int i, temp, statu = 0;
    for(i = 0; i < 9; i++){
	fin>>temp;
	temp = (temp/3)%4;
	statu = (statu<<3)+temp;
    }
    orig.push_back(statu);
    minMove = INT_MAX;
    steps.push_back(orig);
    bfs();
    for(i=1; i< minMove-1; i++)
	fout<<moves[i]<<' ';
    fout<<moves[i]<<endl;
    return 0;
}
