/*
ID: hankjin1
LANG: C++
TASK: fence8
*/
#include<fstream> 
#include<cstdlib>
#include <algorithm>
using namespace std;
ofstream fout ("fence8.out",ios::out);
ifstream fin ("fence8.in",ios::in);
long n,m,mid,space;
int pack[51]={-1};
int rail[1024]={-1};
int total;
int numr[1024],bag[51];

bool dfsid(int deep,int pos){
    if(deep<=0) return true;
    if(space>total-numr[mid]) 
	return false; //cut 1
    for(int i=pos;i<=n;i++){
	if(bag[i]>=rail[deep]){
	    bag[i]-=rail[deep];
	    if(bag[i]<rail[1])
		space+=bag[i];
	    if(rail[deep]==rail[deep-1]){ //cut 2
		if(dfsid(deep-1,i)) return true;}
	    else if (dfsid(deep-1,1)) return true;
	    if(bag[i]<rail[1])
		space-=bag[i];
	    bag[i]+=rail[deep];
	}
    }
    return false;
}
int main() {
    fin>>n;
    for(int i=1;i<=n;i++){
        fin>>pack[i];
        total+=pack[i];
    }
    fin>>m;
    for(int i=1;i<=m;i++) 
	fin>>rail[i];

    sort(pack+1, pack+n+1);
    sort(rail+1, rail+m+1);
    rail[0]=pack[0]=0;
    for(int i=1;i<=m;i++) 
	numr[i]=numr[i-1]+rail[i];
    while(numr[m]>total) 
	m--;
    int left=0,right=m;
    mid=(left+right)/2;
    while(left<=right){ //2���
        for(int i=1;i<=n;i++) 
	    bag[i]=pack[i];
        space=0;
        if(dfsid(mid,1)){
            left=mid+1;
            mid=(left+right)/2;
        }else{
            right=mid-1;
            mid=(left+right)/2;
        }
    }
    fout<<mid<<endl;
    return 0;
}
