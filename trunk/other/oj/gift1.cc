/*
ID: hankjin1
LANG: C++
TASK: gift1
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
using namespace std;
vector<string> ps;
map<string,int> person;
map<string,int> gifts;

int main(){
    int np;
    string p, fri;
    ofstream fout("gift1.out");
    ifstream fin("gift1.in");
    fin>>np;
    //get input
    for(int i = 0; i < np; i ++){
	fin>>p;
	ps.push_back(p);
	person[p]=0;
	gifts[p]=0;
    }
    while(fin>>p){
	int money, friendNum;
	string friendName;
	fin>>money>>friendNum;
	if(friendNum==0)
	    continue;
	int left = money%friendNum;
	//money per friend
	int gift = (money-left)/friendNum;
	gifts[p]=money-left;
	for(int i = 0; i < friendNum; i ++){
	    fin>>friendName;
	    person[friendName]+=gift;//gift from friend
	}
    }
    for(vector<string>::iterator it=ps.begin();it!=ps.end();it++){
	int diff=person[*it] - gifts[*it];
	fout<<*it<<" "<<diff<<endl;
    }
    return 0;
}
