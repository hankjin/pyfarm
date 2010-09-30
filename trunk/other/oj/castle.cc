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
//#define DEBUG
//note: use bit is easy, but make it unable to read, use struct is better
//note: because it need north/east, so loop from bottom-left to top-right

int M,N;
//count of components
int comNo;
//walls: bit0-8 is the wall, bit9-24 is the component no
int nums[50][50];
//size of component,coms[0] is not used
int coms[50*50+1];
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
    int maxsize = 0;
    int temp;
    dir ans;
    for(j=0;j<M;j++){
	for(i=N-1;i>=0;i--){
	    if(i!=0 &&(nums[i][j]&2)>0){
		if((nums[i][j]>>4) != (nums[i-1][j]>>4)){
		    temp = coms[nums[i][j]>>4]+coms[nums[i-1][j]>>4];
		    if(temp>maxsize){
			maxsize = temp;
			ans.i=i;
			ans.j=j;
			ans.d=2;
		    }
		}
	    }
	    if(j!=(M-1) && (nums[i][j]&4)>0){
		if((nums[i][j]>>4) != (nums[i][j+1]>>4)){
		    temp = coms[nums[i][j]>>4]+coms[nums[i][j+1]>>4];
		    if(temp>maxsize){
			maxsize = temp;
			ans.i=i;
			ans.j=j;
			ans.d=4;
		    }
		}
	    }
	}
    }
    fout<<maxsize<<endl;
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
