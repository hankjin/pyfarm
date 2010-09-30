/*
ID: hankjin1
LANG: C++
TASK: friday
*/
#include<iostream>
#include <fstream>
using namespace std;
#define ISLEAP(y) ((y%100==0 && y%400==0) || (y%100!=0 && y%4==0))

int BIG[]={1,3,5,7,8,10,12};
bool isBig(int m){
    for(int i = 0; i < 7; i ++){
	if(BIG[i]==m)
	    return true;
    }
    return false;
}
int main(){
    int n, temp, cur=13%7;
    int week[7];
    ifstream fin("friday.in");
    ofstream fout("friday.out");
    for(int i = 0; i < 7; i ++)
	week[i] = 0;
    fin>>n;
    for(int i = 0; i < n; i ++){
	week[cur]++;
	for(int j = 1; j < 12; j ++){
	    if(isBig(j)){
		cur += 31;
	    }
	    else if(j==2){
		temp = 1900+i;
		if(ISLEAP(temp))
		    cur += 29;
		else
		    cur += 28;
	    }
	    else
		cur+=30;
	    cur=cur%7;
	    week[cur]++;
	}
	cur=(cur+31)%7;//next year
    }
    fout<<week[6];
    for(int i = 0; i < 6; i ++){
	fout<<' '<<week[i];
    }
    fout<<endl;
    return 0;
}
