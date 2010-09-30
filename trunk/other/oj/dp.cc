#include <iostream>
#include <fstream>
using namespace std;
#define MAXN 100

ifstream fin("dp.in");
ofstream fout("dp.out");

int main(){
    int temp;
    int i = 0,j;
    int ans = 0;
    int bestrun[MAXN];
    while(fin>>temp){
	bestrun[i++] = -1;
	for(j=0; bestrun[j]>temp;j++)
	    ;
	bestrun[j]=temp;
	ans = max(ans, j+1);
    }
    //output result:
    cout<<ans<<endl;
    for(i=0;i<ans;i++){
	cout<<bestrun[i]<<",";
    }
    cout<<endl;
    return 0;
}
