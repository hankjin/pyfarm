/*
ID: hankjin1
LANG: C++
TASK: telecow
*/
#include <stdio.h>
#include <string.h>

const long MAXN = 222;
const long MAXM = 3333;

long head[MAXN], next[MAXM], edge[MAXM], weight[MAXM];
long n, s, t;

long a[MAXN], b[MAXN], c[MAXN];
long h[MAXN];

long i, j, k, l, m, tot = 1;

inline void add_edge(const long u, const long v, const long w) {
    edge[++tot] = v;
    weight[tot] = w;
    next[tot] = head[u];
    head[u] = tot;

    edge[++tot] = u;
    weight[tot] = 0;
    next[tot] = head[v];
    head[v] = tot;
}

void fill(const long vtx) {
    h[vtx] = 1;
    for (long i = head[vtx]; i; i = next[i])
	if (weight[i] > 0 && !h[edge[i]]) fill(edge[i]);
}

int main() {
    freopen("telecow.in", "rt", stdin);
    freopen("telecow.out", "wt", stdout);
    memset(head, 0, sizeof head);
    for (scanf("%d%d%d%d", &n, &m, &s, &t); m; --m) {
	scanf("%d%d", &i, &j);
	add_edge(i + 100, j, 2147483647);
	add_edge(j + 100, i, 2147483647);
    }

    a[0] = (s += 100);
    for (i = 1; i <= n; ++i) add_edge(i, i + 100, 6000 + i);
    for (tot = 0; ; ) {
	memset(h, 255, sizeof h), h[s] = 0;
	for (j = -1, k = 0; j < k; )
	    for (m = a[++j], i = head[m]; i; i = next[i])
		if (weight[i] > 0 && h[(l = edge[i])] < 0) a[++k] = l, h[l] = h[m] + 1;
	if (h[t] < 0) break;

	a[1] = s, b[1] = head[s], c[1] = 2147483647;
	for (m = 1; m; ) {
	    k = a[m], l = c[m];
	    if (k == t) {
		tot += l;
		for (i = --m; i; --i) {
		    weight[b[i]] -= l;
		    weight[b[i] ^ 1] += l;
		    if (!(c[i] -= l)) m = i - 1;
		}
		continue;
	    }
	    for (; b[m]; b[m] = next[b[m]]) {
		i = edge[b[m]], j = weight[b[m]];
		if (j > 0 && h[i] == h[k] + 1) {
		    a[++m] = i;
		    b[m] = head[i];
		    c[m] = j < l ? j : l;
		    break;
		}
	    }
	    if (!b[m]) {
		h[k] = -1, --m;
		b[m] = next[b[m]];
	    }
	}
    }

    memset(h, 0, sizeof h);
    fill(s);
    printf("%d\n", tot / 6000);
    for (j = 0, i = 1; i <= n; ++i)
	if (h[i] && !h[i + 100]) {
	    j ? printf(" ") : j = 1;
	    printf("%d", i);
	}
    printf("\n");
    return 0;
}
