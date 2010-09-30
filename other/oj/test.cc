#include <iostream>
#include <cstring>

using namespace std;

int tot[100001];
bool s[1 << 22];

void Update(int x, int loc) {
    int i, tmp;
    for (i = 1; i <= 21; i ++)
	tmp = (x >> (i-1)) + (1 << (22-i)), s[tmp] = true;
}

int main() {
    int n, x, i, j, t, tmp, ans, ans_e;
    freopen("cowxor.in", "r", stdin);
    freopen("cowxor.out", "w", stdout);
    scanf("%d", &n);
    for (tot[0] = 0, i = 1; i <= n; i ++){
	scanf("%d", &x);
	tot[i] = tot[i-1] ^ x;
    }
    memset(s, false, sizeof(s));
    Update(0, 0);
    ans = tot[1], ans_e = 1;
    for (i = 1; i <= n; i ++) {
	for (tmp = 0, j = 1; j <= 21; j ++) {
	    t = (tot[i] >> (21-j)) & 1;
	    if (s[tmp*2 + (! t) + (1 << j)])
		tmp = tmp*2 + (! t);
	    else
		tmp = tmp*2 + t;
	}
	if ((tmp ^ tot[i]) > ans)
	    ans = tmp ^ tot[i], ans_e = i;
	Update(tot[i], i);
    }
    for (i = ans_e-1; i; i --)
	if ((tot[i] ^ tot[ans_e]) == ans) break;
    printf("%d %d %d\n", ans, i+1, ans_e);
    return 0;
}
