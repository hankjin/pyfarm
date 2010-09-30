/**
ID: hankjin1
LANG: C++
TASK: palsquare
*/
/**
 * note: algorithm: reverse(*begin, *end);
 *
 */

#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

char digits[]="0123456789ABCDEFGHIJ";
int base;
char dest[256];
char src[256];
int toBase(int n, char*buffer){
    int i;
    for(i=0; n>0; i++){
	buffer[i]=digits[n%base];
	n=n/base;
    }
    buffer[i]='\0';
    return i;
}
bool isPal(int n){
    int v= n*n;
    int len = toBase(v, dest);
    int i;
    for(i=0; i<len/2;i++){
	if(dest[i] != dest[len-1-i])
	    return false;
    }
    return true;
}
int main(){
    ifstream fin("palsquare.in");
    ofstream fout("palsquare.out");
    fin>>base;
    int i, n;
    for(i=1; i <= 300; i++){
	if(isPal(i)){
	    n = toBase(i,src);
	    reverse(src, &src[n]);
	    fout<<src<<' '<<dest<<endl;
	}
    }

    return 0;
}

