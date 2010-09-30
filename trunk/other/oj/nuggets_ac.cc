/*
ID: hankjin1
LANG: C++
TASK: nuggets
*/
#include <fstream>
using namespace std;
ifstream cin("nuggets.in");
ofstream cout("nuggets.out");
bool dp[100000];
int gcd(int x,int y) {
    if (x>y) swap(x,y);
    if (x==0) return y;
    return gcd(y%x,x);
}
int main() {
    int n,num[11],k;
    cin>>n;
    for (int i=1;i<=n;i++) 
	cin>>num[i];
    k=num[1];
    for (int i=2;i<=n;i++) 
	k=gcd(k,num[i]);
    if (k!=1) {
	cout<<0<<endl;
	return 0;
    }
    dp[0]=true;
    for (int i=1;i<=65536;i++) {
	for (int j=1;j<=n;j++)
	    if (i>=num[j]) 
		dp[i]=dp[i]||dp[i-num[j]];
    }
    int res=0;
    for (int i=65536;i>=1;i--)
	if (!dp[i]) {
	    res=i;
	    break;
	}
    cout<<res<<endl;
    return 0;
}
