/*
ID: hankjin1
LANG: C++
TASK: comehome
*/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <climits>
using namespace std;
#define MYMAX 20000000

//note: there are duplicate data,eg R a 200 R a 106
int P;
int dist[256][256];
int total[256][256];
set<char> dict;
int main(){
    FILE *fin = fopen("comehome.in","r");
    FILE *fout = fopen("comehome.out","w");
    fscanf(fin, "%d\n", &P);
    char a,b;
    int v;
    fill_n(&dist[0][0],256*256,MYMAX);
    fill_n(&total[0][0],256*256,MYMAX);
    //input
    for(int i=0;i<P;i++){
	fscanf(fin, "%c %c %d\n", &a,&b,&v);
	v = min(v, dist[a][b]);
	dist[b][a]=dist[a][b]=v;
	total[b][a]=total[a][b]=v;
	dict.insert(a);
	dict.insert(b);
    }
    //Floyd-Hashall
    for(set<char>::iterator k=dict.begin();k!=dict.end();k++){
	for(set<char>::iterator i=dict.begin();i!=dict.end();i++){
	    if(*i==*k)
		continue;
	    for(set<char>::iterator j=dict.begin();j!=dict.end();j++){
		if(*j==*i || *j==*k)
		    continue;
		if(total[*i][*k]+total[*k][*j]<total[*i][*j])
		    total[*i][*j]=total[*i][*k]+total[*k][*j];
	    }
	}
    }
    char minI;
    int minV=MYMAX;
    for(set<char>::iterator it=dict.begin();it!=dict.end();it++){
	if(*it<'Z' && total[*it]['Z']<minV){
	    minI = *it;
	    minV = total[*it]['Z'];
	}
    }
    fprintf(fout,"%c %d\n", minI, minV);
    return 0;
}
