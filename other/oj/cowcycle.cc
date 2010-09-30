/*
ID: hankjin1
LANG: C++
TASK: cowcycle
*/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <climits>
using namespace std;
//note: nothing, but

int FN,RN;
int Fmin, Fmax, Rmin, Rmax;
//permunate
int Fs[5];
int Rs[10];
//ans
int bestR[10];
int bestF[5];
double minV;

double calc(){
    double xs[50], ys[50],temp;
    for(int i=0;i<FN;i++){
	for(int j=0;j<RN;j++){
	    temp = Fs[i];
	    xs[i*RN+j] = temp/Rs[j];
	}
    }
    sort(xs, xs+FN*RN);
    double total = 0;
    for(int i=0;i<FN*RN-1;i++){
	ys[i] = xs[i+1]-xs[i];
	total += ys[i];
    }
    double mean = total/(FN*RN-1);
    double res = 0;
    for(int i=0;i<FN*RN-1;i++){
	res += (ys[i]-mean)*(ys[i]-mean);
    }
    return res;
}

bool nextR(int n){
    if(n==RN){
	if(Fs[FN-1]*Rs[RN-1] >= Fs[0]*Rs[0]*3){
	    double res = calc();
	    if(res < minV){//save a better solution
		minV = res;
		for(int j=0;j<FN;j++)
		    bestF[j]=Fs[j];
		for(int j=0;j<RN;j++)
		    bestR[j]=Rs[j];
	    }
	}
	return true;
    }
    int from = n==0 ? Rmin-1:Rs[n-1];
    for(int i=from+1;i<=Rmax;i++){
	Rs[n]=i;
	nextR(n+1);
    }
}
void nextF(int n){
    if(n==FN){
	nextR(0);
	return ; //very important
    }
    int from = (n==0 ? Fmin-1:Fs[n-1]);
    for(int i=from+1;i<=Fmax;i++){
	Fs[n]=i;
	nextF(n+1);
    }
}

int main(){
    ifstream fin("cowcycle.in");
    ofstream fout("cowcycle.out");
    fin>>FN>>RN;
    fin>>Fmin>>Fmax>>Rmin>>Rmax;
    minV = INT_MAX;
    nextF(0);
    for(int i=0;i<FN-1;i++){
	fout<<bestF[i]<<" ";
    }
    fout<<bestF[FN-1]<<endl;
    for(int i=0;i<RN-1;i++){
	fout<<bestR[i]<<" ";
    }
    fout<<bestR[RN-1]<<endl;
    return 0;
}
