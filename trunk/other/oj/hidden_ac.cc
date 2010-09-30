#include <cstdio>
#include <cstring>
 
using namespace std;
 
const int MAXL = 100000+1;
 
char s[MAXL];
int l;
 
int work() {
	int i = 0,j = 1,k = 0,t;
	while (i<l && j<l && k<l) {
		t = s[(i+k)>=l?i+k-l:i+k]-s[(j+k)>=l?j+k-l:j+k];
		if (!t) k++;
		else {
			if (t>0) i += k+1;
			else j += k+1;
			if (i==j) j++;
			k = 0;
		}
	}
	printf("%d\n",i<j?i:j);
}
 
int main() {
	freopen("hidden.in","r",stdin);
	freopen("hidden.out","w",stdout);
	scanf("%d\n",&l);
	for (int i = 0; gets(s+i); i += 72);
	work();
	return 0;
}
