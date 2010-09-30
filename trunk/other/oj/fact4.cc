/*
ID: hankjin1
LANG: C++
TASK: fact4
*/
#include <iostream>
#include <fstream>
using namespace std;
//note: the theory is: 2 and 5 need to be get out
//advanced: divide all 2 and 5, and record the number of 2 and 5
//and calculate them at the end

int main(){
    int N;
    ifstream fin("fact4.in");
    ofstream fout("fact4.out");
    fin>>N;
    long long sum = 1;
//    long long mask = 100000000000;
    
    for(int i=2;i<=N;i++){
	int b=i;
	while(b%10==0)
	    b/=10;
	if(b%10==5 && sum%2==0){
	    sum = sum/2 + b/10*sum;
	}
	else if(sum%10==5 && b%2==0){
	    sum = b/2+sum/10*b;
	}
	else
	    sum*=b;
	while(sum%10==0)
	    sum/=10;
	sum%=100000000;
    }
    fout<<sum%10<<endl;
    return 0;
}
