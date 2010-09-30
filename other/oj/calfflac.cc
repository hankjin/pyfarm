/*
ID: hankjin1
LANG: C++
TASK: calfflac
*/
//note: BOTH odd and even
//note: the size of array
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <algorithm>
#include <map>
using namespace std;
#define N 20000
int main(){
    char orig[N];
    char buffer[N];
    map<int,int> dict;


    FILE *fin = fopen("calfflac.in","r");
    ofstream fout("calfflac.out");
    int i,j,n;
    char temp[85];
    orig[0]='\0';
    while(fgets(temp, 85, fin)){
	strcat(orig,temp);
    }
    //cout<<strlen(orig);
    //cout<<orig<<endl;
    for(i=0,j=0;i<strlen(orig);i++){
	if(orig[i]>='A' && orig[i]<='Z'){
	    dict[j]=i;
	    buffer[j++]=orig[i]-'A'+'a';
	}
	else if(orig[i]>='a' && orig[i]<='z'){
	    dict[j]=i;
	    buffer[j++]=orig[i];
	}
    }
    n=j;
    int max = 0, maxLen = 0, extraLen, tempLen;
    for(i=1; i< n-1;i++){
	for(extraLen=0; extraLen<2; extraLen++){//odd and ...
	    tempLen = 0;
	    if(extraLen>0){//ommo
		if(buffer[i+1] != buffer[i])
		    break;
	    }
	    for(j=1;j<=i;j++){
		if(buffer[i-j]!=buffer[i+extraLen+j])
		    break;
		tempLen++;
	    }
	    tempLen = tempLen*2+1+extraLen;
	    if(tempLen > maxLen){
		maxLen = tempLen;
		max = i;
	    }
	}
    }
    int begin = dict[max-(maxLen-1)/2];
    int end = dict[max+maxLen/2];
    fout<<maxLen<<endl;
    for(i = begin; i <= end; i++)
	fout<<orig[i];
    fout<<endl;
    return 0;
}
