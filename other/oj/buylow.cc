/*
ID: hankjin1
LANG: C++
TASK: buylow
*/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <set>
using namespace std;

//400 overflow, while 800 excced mem lim
#define MAX 500
//this thru case 8
class bignum{
    public:
	int data[MAX];
	int len;
    public:
    bignum(){
	len=0;
	fill_n(data, MAX, 0);
	data[len++]=0;
    }
    bignum& operator =(int n){
	len=0;
	fill_n(data, MAX, 0);
	do{
	    data[len++]=n%10;
	    n/=10;
	}while(n>0);
    }
    bignum& operator =(bignum &o){
	fill_n(data, MAX, 0);
	len = o.len;
	for(int i=0;i<len;i++)
	    data[i] = o.data[i];
    }
    bignum& operator +=(bignum &o){
	len = max(len, o.len);
	for(int i=0;i<len;i++){
	    int res = data[i]+o.data[i];
	    data[i] = res%10;
	    data[i+1] += res/10;
	}
	if(data[len]>0)
	    len++;
    }
    bool undef(){
	return len==1 && data[0]==-1;
    }

    bignum& operator -=(bignum &o){
	int n = o.len;
	for(int i=0;i<n;i++){
	    if(data[i]<o.data[i]){
		data[i+1]--;
		data[i]+=10;
	    }
	    data[i]-=o.data[i];
	}
	while(data[n]<0){
	    data[n]+=10;
	    data[++n]--;
	}
	if(data[len-1]==0)
	    len--;
    }
    bool operator <(bignum &o){
	if(len < o.len)
	    return true;
	else if(len > o.len)
	    return false;
	else{
	    for(int i=len-1;i>=0;i--){
		if(data[i]<o.data[i])
		    return true;
		else if(data[i]>o.data[i])
		    return false;
	    }
	}
	return false;
    }
    bool operator ==(bignum &o){
	if(len != o.len)
	    return false;
	for(int i=0;i<len;i++){
	    if(data[i] != o.data[i])
		return false;
	}
	return true;
    }
};
ostream& operator <<(ostream &out, bignum &num){
    for(int i=num.len-1;i>=0;i--){
	out<<num.data[i];
    }
    return out;
}
//note: Test 4 error
int N;
int num[5000];
int dp[5000];
bignum s[5000];

bignum& f(int v){
    if(s[v].undef()){
	set<int> hash; 
	s[v]=0;
	for(int u=v-1;u>=0;u--){
	    if(dp[v]==dp[u]+1 && num[v]<num[u] 
		    && hash.find(num[u])==hash.end()){
		hash.insert(num[u]);
		s[v] += f(u);
	    }
	}
    }
    return s[v];
}
int main(){
    ifstream fin("buylow.in");
    ofstream fout("buylow.out");
    fin>>N;
    for(int i=0;i<N;i++){
	fin>>num[i];
    }
    for(int i=0;i<=N;i++){
	for(int j=0;j<i;j++){
	    if(num[j]>num[i]){
		dp[i]=max(dp[i],dp[j]);
	    }
	}
	dp[i]+=1;
	if(dp[i]==1)
	    s[i]=1;
	else
	    s[i]=-1;
    }
    fout<<dp[N]-1<<" "<<f(N)<<endl;
}
