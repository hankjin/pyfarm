/*
ID: hankjin1
LANG: C++
TASK: spin
*/
#include <iostream>
#include <fstream>
using namespace std;

//note: while, it is ugly, but it is AC.
//it is better to use split method like rect1 to make it faster
struct wheel{
    int speed;
    int ws;
    int wedge[5][2];
}wheels[5];
bool chkok(){
    int degree[360];
    fill_n(degree,360, 0);
    for(int i=0;i<5;i++){
	for(int j=0;j<wheels[i].ws;j++){
	    for(int k=0;k<=wheels[i].wedge[j][1];k++){
		int pos=(wheels[i].wedge[j][0]+k)%360;
		degree[pos]++;
	    }
	}
    }
    for(int i=0;i<360;i++){
	if(degree[i]==5)
	    return true;
    }
    return false;
}
int main(){
    ifstream fin("spin.in");
    ofstream fout("spin.out");
    for(int i=0;i<5;i++){
	fin>>wheels[i].speed>>wheels[i].ws;
	for(int j=0;j<wheels[i].ws;j++){
	    fin>>wheels[i].wedge[j][0]>>wheels[i].wedge[j][1];
	}
    }
    int min=0;
    for(;min<360;min++){
	if(chkok()){
	    break;
	}
	for(int i=0;i<5;i++){
	    for(int j=0;j<wheels[i].ws;j++){
		wheels[i].wedge[j][0]=
		    (wheels[i].wedge[j][0]+wheels[i].speed)%360;
	    }
	}
    }
    if(min==360){
	fout<<"none"<<endl;
    }
    else{
	fout<<min<<endl;
    }
    return 0;
}
