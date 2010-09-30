/**
ID: hankjin1
LANG: C++
TASK: namenum
*/
#include <fstream>
#include <cstring>
#include <string>
#include <iostream>
using namespace std;
char names[5000][13];
int namesLen = 0;
char rule[8][4] = {"ABC","DEF","GHI","JKL","MNO","PRS","TUV","WXY"};
int myfind(int i){
    
}
int main(){
    ifstream dict("dict.txt");
    ifstream fin("namenum.in");
    ofstream fout("namenum.out");
    char num[13];
    int i,j,k,numLen;
    while(dict>>names[namesLen])
	namesLen++;
    fin>>num;
    numLen=strlen(num);
    int matches=0;
    for(i=0;i<namesLen;i++){
	if(strlen(names[i]) != numLen)
	    continue;
	for(j=0;j<numLen;j++){//word match
	    for(k=0;k<3;k++){
		if(names[i][j]==rule[num[j]-'2'][k])
		    break;
	    }
	    if(k==3)//alpha not match
		break;
	}
	if(j==numLen){//word match
	    fout<<names[i]<<endl;
	    matches++;
	}
    }
    if(matches==0)
	fout<<"NONE"<<endl;
    return 0;
}
