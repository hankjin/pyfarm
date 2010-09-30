/*
ID: hankjin1
LANG: C++
TASK: castle
*/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
using namespace std;
#define DEBUG

int M,N;
int comNo;
int nums[50][50];
int coms[50*50];
vector<int> res;
typedef struct _dir{
    int i,j,d;
}  dir;
dir dirs[4]={
    {0,-1,1},//west
    {-1,0,2},//north
    {0,1,4},//east
    {1,0,8}//south
};
void floor(int x, int y){
    int k;
    for(k=0;k<4;k++){
	if(x+dirs[k].i<0 || x+dirs[k].i==N
		||y+dirs[k].j<0 || y+dirs[k].j == M)//out of block
	    continue;
	if(nums[x+dirs[k].i][y+dirs[k].j] > 0xF)//walked
	    continue;
	if((nums[x][y] & dirs[k].d)>0)//blocked
	    continue;
	nums[x+dirs[k].i][y+dirs[k].j] |= comNo<<4;
	coms[comNo]++;
	floor(x+dirs[k].i, y+dirs[k].j);
    }
}
dir isconnect(int com1, int com2){
    int i,j,k;

	for(j=0;j<M;j++){
    for(i=0;i<N;i++){
	    if(((nums[i][j]&0xFFFF0)>>4)!= com1)
		continue;
	    for(k=1;k<3;k++){
		if(i+dirs[k].i<0 || i+dirs[k].i==N
			||j+dirs[k].j<0 || j+dirs[k].j==M)
		    continue;
		if(((nums[i+dirs[k].i][j+dirs[k].j]&0xFFFF0)>>4) == com2){
		    dir res={i,j,dirs[k].d};
		    return res;
		}
	    }
	}
    }
    dir res = {i,j,k};
    return res;
}

int main(){
    ifstream fin("castle.in");
    ofstream fout("castle.out");
    fin>>M>>N;
    int i,j;
    for(i=0;i<N;i++){
	for(j=0;j<M;j++){
	    fin>>nums[i][j];
#ifdef DEBUG
	    if((nums[i][j]&1)>0)
		cout<<"|";
	    else
		cout<<" ";
	    if((nums[i][j]&2)>0)
		cout<<"^";
	    else
		cout<<" ";
#endif
	}
#ifdef DEBUG
	cout<<endl;
#endif
    }
    comNo = 0x0;
    for(i=0;i<N;i++){
	for(j=0;j<M;j++){
	    if((nums[i][j]&0xFFF0)==0){
		++comNo;
		nums[i][j] |= comNo<<4;
		coms[comNo]++;
		floor(i,j);
	    }
	}
    }
    fout<<comNo<<endl;
    //
    int maxcom = 0;
    for(i=1;i<=comNo;i++){
	maxcom = coms[i]>maxcom ? coms[i] : maxcom;
    }
    fout<<maxcom<<endl;
    //
    for(i=1; i < comNo;i++){
	for(j=i+1; j<=comNo; j++){
	    if(j==i)
		continue;
	    res.push_back(((coms[i]+coms[j])<<16)|(i<<8)|j);
	}
    }
    sort(res.begin(),res.end());
    dir ans;
    int total, com1, com2;
    for(i=res.size()-1; i >=0; i--){
	total = res[i]>>16;
	com1 = (res[i]&0xFF00)>>8;
	com2 = (res[i]&0xFF);
#ifdef DEBUG
	cout<<total<<","<<com1<<endl;
#endif
	ans = isconnect(com1, com2);
	if(ans.i!=N || ans.j != M)
	    break;
	ans = isconnect(com2, com1);
	if(ans.i!=N || ans.j != M)
	    break;
    }
    fout<<total<<endl;
    fout<<(ans.i+1)<<" "<<(ans.j+1)<<" ";
    switch(ans.d){
	case 2:
	    fout<<"N";
	    break;
	case 4:
	    fout<<"E";
	    break;
    }
    fout<<endl;
    return 0;
}
