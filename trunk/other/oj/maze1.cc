/*
ID: hankjin1
LANG: C++
TASK: maze1
*/
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

//note: bfs instead of dfs, for dfs get wrong depth
//note: if bfs begin with 2 exits in is better!
//#define DEBUG
int W,H;
FILE *fin = fopen("maze1.in","r");
FILE *fout = fopen("maze1.out","w");
struct node{
    bool north;
    bool south;
    bool west;
    bool east;
    int dist;
    bool flag;
}maze[100][38];

//when use 3800, it said memory access violation, maybe
int hist[38000][2];
int histN;
int lastHist;

void travel(int x, int y){
    if(maze[x][y].flag)
	return;
    maze[x][y].flag=true;
    if(!maze[x][y].north && x > 0 && !maze[x-1][y].flag){
	hist[histN][0]=x-1;
	hist[histN][1]=y;
	histN++;
    }if(!maze[x][y].south && x < H-1 &&!maze[x+1][y].flag){
	hist[histN][0]=x+1;
	hist[histN][1]=y;
	histN++;
    }if(!maze[x][y].west && y > 0 && !maze[x][y-1].flag){
	hist[histN][0]=x;
	hist[histN][1]=y-1;
	histN++;
    }if(!maze[x][y].east && y < W-1 && !maze[x][y+1].flag){
	    hist[histN][0]=x;
	    hist[histN][1]=y+1;
	    histN++;
    }
}void bfs(int depth){
    if(histN == lastHist)
	return;
    int histNow = histN;
    for(int i=lastHist;i<histNow;i++){
	if(maze[hist[i][0]][hist[i][1]].dist > depth)
	    maze[hist[i][0]][hist[i][1]].dist = depth;
	travel(hist[i][0],hist[i][1]);
    }
    lastHist = histNow;
    bfs(depth+1);
}
int main(){
    fscanf(fin, "%d %d\n",&W,&H);
    bool wall;
    int j;
    char *buffer = NULL;
    char temp[80];
    size_t tmp = 0;
    for(int i=0;i<2*H+1;i++){
	getline(&buffer, &tmp, fin);
	if(buffer[0]=='\n')
	    buffer = temp;
	for(j=0;j<W;j++){
	    if(i%2==0){//wall
		wall = buffer[j*2+1]=='-';
		if(i!=2*W)
		    maze[i/2][j].north=wall;
		if(i!=0)
		    maze[i/2-1][j].south=wall;
	    }
	    else{//cell
		wall = buffer[j*2]=='|';
		maze[i/2][j].west = wall;
		maze[i/2][j].dist = W*H;
		if(j!=0)
		    maze[i/2][j-1].east = wall; 
	    }
	}
	if(i%2==1){//last column
	    maze[i/2][j-1].east = buffer[j*2]=='|';
	}
    }
    //find entry
    int gx[2],gy[2],gi=0;
    for(int i=0;i<W;i++){
	if(maze[0][i].north==false){
	    gx[gi]=0;
	    gy[gi]=i;
	    gi++;
	}
	else if(maze[H-1][i].south==false){
	    gx[gi]=H-1;
	    gy[gi]=i;
	    gi++;
	}
    }
    for(int i=0;i<H;i++){
	if(maze[i][0].west==false){
	    gx[gi]=i;
	    gy[gi]=0;
	    gi++;
	}
	else if(maze[i][W-1].east==false){
	    gx[gi]=i;
	    gy[gi]=W-1;
	    gi++;
	}
    }
    //go
    for(int i=0;i<gi;i++){
	for(int j=0;j<H;j++){
	    for(int k=0;k<W;k++){
		maze[j][k].flag = false;
	    }
	}
	hist[0][0]=gx[i];
	hist[0][1]=gy[i];
	histN=1;
	lastHist = 0;
	bfs(1);
#ifdef DEBUG
	for(int x=0;x<H;x++){
	    for(int y=0;y<W;y++){
		cout<<maze[x][y].dist<<",";
	    }
	    cout<<endl;
	}
#endif
    }
    int maxdist = 0;
    for(int i=0;i<H;i++){
	for(int j=0;j<W;j++){
	    if(maze[i][j].dist>maxdist)
		maxdist = maze[i][j].dist;
	}
    }
    fprintf(fout, "%d\n", maxdist);
    return 0;
}
