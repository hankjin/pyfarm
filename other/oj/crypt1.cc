/**
ID: hankjin1
LANG: C++
TASK: crypt1
*/
/*
 abc
x de
----
 fgh
ijk
----
lmno
a>0
d>0
fgh  < 1000
ijk  < 1000
lmno < 10000
fgh in {}
ijk in {}
lmno in {}
*/
//note: cut branches
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

int cmp(const void *a, const void *b){
    return *(int*)a-*(int*)b;
}
int N;
int digits[10];
bool isOK(int n){
    int temp,i;
    int sum=n;
    while(sum>0){
	temp = sum%10;
	for(i=0; i< N;i++){
	    if(digits[i]==temp)
		break;
	}
	if(i==N)
	    return false;
	sum=sum/10;
    }
    return true;
}
int main(){
    ifstream fin("crypt1.in");
    ofstream fout("crypt1.out");
    fin>>N;
    int i,j,k,x,y;
    int first,second;
    int count=0;
    int temp, sum;
    for(i=0;i<N;i++)
	fin>>digits[i];
    qsort(digits,N,sizeof(int),cmp);
    for(i=0;i<N;i++){
	if(digits[i]*digits[0]>9)//cut brach 1
	    break;
	if(digits[i] == 0)//brach 3
	    continue;
	for(j=0;j<N;j++){
	    for(k=0;k<N;k++){
		first = digits[i]*100+digits[j]*10+digits[k];
		for(x=0;x<N;x++){
		    if(digits[x]==0)//brach 4
			continue;
		    sum=digits[x]*first;
		    if(sum > 999)//cut brach 2
			break;
		    if(!isOK(sum))
			continue;
		    for(y=0;y<N;y++){
			sum=digits[y]*first;
			if(sum>999)//branch 5
			    break;
			if(!isOK(sum))
			    continue;

			second = digits[x]*10+digits[y];
			temp=first*second;
			if(temp > 9999)
			    break;
			if(isOK(temp)){
			    count++;
			    //cout<<first<<"x"<<second<<endl;
			}
			else{
			//    cout<<"Failed"<<first<<"X"<<second<<endl;
			}
		    }
		}
	    }
	}
    }
    fout<<count<<endl;
    return 0;
}
