/**
ID: hankjin1
LANG: C++
TASK: beads
*/
#include <iostream>
#include <fstream>
using namespace std;

int main(){
    int n, cn=0;
    char beads[350];
    int count[350];
    int countL[350];//bug1, rwb can not be 2+2
    int countR[350];
    ifstream fin("beads.in");
    ofstream fout("beads.out");
    fin>>n;
    for(int i = 0; i <n; i ++){
	fin>>beads[i];
    }
    int begin = 0, wn = 0;
    while(beads[begin]=='w'&&begin<n)
	begin++;
    int max = 0;
    if(begin==n)
	max = n;
    else{
    int cur=beads[begin], curI=begin+1;
    count[cn]=1;
    while(curI<begin+n){
	if(beads[curI%n]=='w'){
	    wn++;
	}
	else if(beads[curI%n]==cur){
	    if(wn!=0){
		count[cn]+=wn+1;
		wn=0;
	    }
	    else{
		count[cn]++;
	    }
	}
	else{
	    cur=beads[curI%n];
	    countR[cn]=wn;
	    count[++cn]=1;
	    countL[cn]=wn;
	    wn=0;
	}

	curI++;
    }
    countR[cn]=wn;
    countL[0]=wn;
    /*
    for(int i=0;i <= cn; i ++){
	cout<<count[i]<<",[";
	cout<<countL[i]<<",";
	cout<<countR[i]<<",]";
    }
    */
    if(cn>1){
	if(cur==beads[begin]){
	    count[0]+=count[cn]+countR[cn];
	    count[cn]=count[0];
	    countL[0]=countL[cn];
	    countR[cn]=countR[0];
	}
    }
    if(cn>0)
	max = count[0]+countL[0]+countR[1]+count[1]+countR[0];
    else
	max = count[0];
    for(int i = 1; i < cn; i ++){
	int temp = count[i]+countL[i]+count[i+1]+countR[i+1]+countR[i];
	if(temp>max)
	    max = temp;
    }
    }
    fout<<max<<endl;

    return 0;
}
