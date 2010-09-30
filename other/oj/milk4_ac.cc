/**
ID: hankjin1
LANG: C++
TASK: milk4
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
using namespace std;

ifstream fin("milk4.in");
ofstream fout("milk4.out");

int a[100], b[100];
bool f[20001], g[20001];
int n, m;

int i, j, k;

bool calc(const int x) {
    if (g[x]) return f[x];
    g[x] = true;
    for (int i = 0; i < k; ++i)
	if (x >= b[i] && calc(x - b[i])) return (f[x] = true);
    return (f[x] = false);
}

void search(const int dep, const int tot) {
    if (tot == k) {
	memset(g, 0, sizeof g);
	f[0] = g[0] = true;
	if (calc(n)) {
	    fout<<k;
	    for (i = 0; i < k; ++i) 
		fout<<" "<<b[i];
	    fout<<endl;
	    exit(0);
	}
	return ;
    }
    if (dep >= m) return ;
    b[tot] = a[dep];
    search(dep + 1, tot + 1);
    search(dep + 1, tot);
}

int main() {
    fin>>n>>m;
    for (i = 0; i < m; ++i) 
	fin>>a[i];

    for (i = 0; i < m; ++i)
	for (j = i + 1; j < m; ++j)
	    if (a[i] > a[j]) k = a[i], a[i] = a[j], a[j] = k;

    for (k = 1; k <= m; ++k) search(0, 0);
}
