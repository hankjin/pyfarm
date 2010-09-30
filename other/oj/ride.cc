/**
ID: hankjin1
LANG: C++
TASK: ride
*/
#include <iostream>
#include <fstream>
using namespace std;

int main(){
    ifstream fin("ride.in");
    ofstream fout("ride.out");
    int ta, tb, i;
    ta = tb = 1;
    string a,b;
    fin>>a>>b;
    for(i = 0; i < a.size(); i ++)
	ta = ta*(a[i]-'A'+1)%47;
    for(i = 0; i < b.size(); i ++)
	tb = tb*(b[i]-'A'+1)%47;
    if(ta == tb)
	fout<<"GO"<<endl;
    else
	fout<<"STAY"<<endl;
    
    return 0;
}
