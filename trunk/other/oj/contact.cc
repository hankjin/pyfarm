/*
ID: hankjin1
LANG: C++
TASK: contact
*/
#include <iostream>
#include <fstream>
#include <cstring>
#include <map>
#include <string>
#include <limits>
#include <set>
#include <vector>
#include <algorithm>
using namespace std;
//note: can't believe it, it goes though, with 0.961 secs!!
//note: use bit instead of map, then it will accelerate so much.
// refer contact_ac.c

int A,B,N;
char data[200001];
char buffer[81];
map<string, int> dict;
set<int> found;
map<int,vector<string> > todo;
ifstream fin("contact.in");
ofstream fout("contact.out");

bool cmp(string s1, string s2){
    if(s1.size() < s2.size())
	return true;
    return s1.compare(s2)<0;
}
void print(vector<string> words){
    sort(words.begin(),words.end(), cmp);
    for(int i=0;i<words.size();i++){
	if(i%6==0){
	    if(i!=0)
		fout<<"\n";
	    fout<<words[i];
	}
	else
	    fout<<" "<<words[i];
    }
    fout<<endl;
}
int main(){
    fin>>A>>B>>N;
    while(fin>>buffer){
	strcat(data,buffer);
    }
    int n = strlen(data);
    for(int i=0;i<n;i++){
	for(int j=A;j<=B;j++){
	    if(i+j>n)
		break;
	    string tmp(&data[i],j);
	    dict[tmp]++;
	}
    }
    for(map<string,int>::iterator it=dict.begin();it!=dict.end();it++){
	if(found.size()<N){
	    found.insert(it->second);
	}
	else{
	    if(it->second > *(found.begin()) && found.count(it->second)==0){
		found.erase(found.begin());
		found.insert(it->second);
	    }
	}
    }
    for(set<int>::iterator it=found.begin(); it!=found.end();it++){
	todo[*it]=vector<string>();
    }
    int min = *found.begin();
    for(map<string,int>::iterator it=dict.begin();it!=dict.end();it++){
	if(it->second >= min){
	    todo[it->second].push_back(it->first);
	}
    }
    for(set<int>::reverse_iterator it=found.rbegin();it!=found.rend();it++){
	fout<<*it<<endl;
	print(todo[*it]);
    }
    return 0;
}
