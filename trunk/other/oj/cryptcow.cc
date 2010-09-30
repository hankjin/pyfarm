/*
ID: hankjin1
LANG: C++
TASK: cryptcow
*/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <map>
#include <set>
using namespace std;

//note: first find position of C O W, better then loop the string
//note: hash is usefull
//note: BKDRhash is more efficient, 5 sec
//note:
char dest[]="Begin the Escape execution at the Break of Dawn";
bool hash[1500000];
inline unsigned long BKDRhash(const char *str){ //BKDRhash
    unsigned long seed=131313,hash=0;
    while(*str) hash=hash*seed+(*str++);
    return (hash & 0x7FFFFFFF) % 1000003;
}
inline int dec(char *raw, int depth){
    if(strcmp(raw, dest)==0)
	return depth;
    //1. hash to reduce duplicate
    int h = BKDRhash(raw);
    if(hash[h])
	return -1;
    hash[h]=true;
    
    int n = strlen(raw);
    //2. sub string
    for(int i=0;i<n;i++){
	if(raw[i]=='C')
	    break;
	if(raw[i]!=dest[i])
	    return -1;
    }
    for(int i=n-1;i>=0;i--){
	if(raw[i]=='W')
	    break;
	if(raw[i]!=dest[i+47-n])
	    return -1;
    }
    //3. mid string must be in string
    //todo
    //3. locate position of C O W
    int Cs[8],Os[8],Ws[8];
    int ci=0,oi=0,wi=0;
    for(int i=0;i<n;i++){
	if(raw[i]=='C')
	    Cs[ci++]=i;
	else if(raw[i]=='O')
	    Os[oi++]=i;
	else if(raw[i]=='W')
	    Ws[wi++]=i;
    }
    if(ci!=oi || oi!=wi || ci*3+47>n)
	return -1;

    for(int j=0;j<oi;j++){
	for(int i=0;i<ci;i++){
	    if(Os[j]<Cs[i])
		break;
	    for(int k=wi-1;k>=0;k--){
		if(Ws[k]<Os[j])
		    break;
		char temp[76];
		temp[0]=0;
		strncat(temp, raw, Cs[i]);
		strncat(temp, raw+Os[j]+1, Ws[k]-Os[j]-1);
		strncat(temp, raw+Cs[i]+1, Os[j]-Cs[i]-1);
		strncat(temp, raw+Ws[k]+1, n-Ws[k]-1);
		int res = dec(temp, depth+1);
		if(res!=-1)
		    return res;
	    }
	}
    }
    return -1;
}

int main(){
    ifstream fin("cryptcow.in");
    ofstream fout("cryptcow.out");
    char raw[77];
    size_t len;
    fin.getline(raw, 77, '\n');

    int res = dec(raw,0);
    if(res == -1){
	fout<<"0 0"<<endl;
    }
    else{
	fout<<"1 "<<res<<endl;
    }
    return 0;
}
