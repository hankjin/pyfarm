/*
ID: hankjin1
LANG: C++
TASK: charrec
*/
#include <iostream>
#include <fstream>
#include <climits>
using namespace std;

const char letter[30] = " abcdefghijklmnopqrstuvwxyz";
char font[28][21][21]; 
char data[1201][21]; 
int dif[28][21][1201]; 
int cost[1201][4];
int from[1201][4], f[1201], g[1201], ans[1201];

int main() {
    int n, i, j, k, p, total;

    //read dict
    ifstream dict("font.in");
    dict>>n;
    for (i = 1; i <= 27; i ++)
	for (j = 1; j <= 20; j ++)
	    for (k = 1; k <= 20; k ++)
		dict>>font[i][j][k];

    ifstream fin("charrec.in");
    ofstream fout("charrec.out");

    fin>>n;
    for (i = 1; i <= n; i ++)
	for (j = 1; j <= 20; j ++)
	    fin>>data[i][j];

    for (i = 1; i <= 27; i ++)  // 预处理dif数组
	for (j = 1; j <= 20; j ++)
	    for (k = 1; k <= n; k ++)
		for (dif[i][j][k] = 0, p = 1; p <= 20; p ++)
		    dif[i][j][k] += int(data[k][p] != font[i][j][p]);
    for (i = 1; i <= n; i ++){  // 预处理cost数组
	cost[i][1] = cost[i][2] = cost[i][3] = INT_MAX;
	if (i + 18 <= n)
	    for (j = 1; j <= 27; j ++){ // 计算缺失一行的情况
		for (total = 0, k = 2; k <= 20; k ++) 
		    total += dif[j][k][i+k-2];
		if (total < cost[i][1]) 
		    cost[i][1] = total, from[i][1] = j;
		for (k = 2; k <= 20; k ++) {
		    total += dif[j][k-1][i+k-2];
		    total -= dif[j][k][i+k-2];
		    if (total < cost[i][1]) 
			cost[i][1] = total, from[i][1] = j;
		}
	    }
	if (i + 19 <= n)
	    for (j = 1; j <= 27; j ++){ // 计算正好20行的情况
		for (total = 0, k = 1; k <= 20; k ++) 
		    total += dif[j][k][i+k-1];
		if (total < cost[i][2]) 
		    cost[i][2] = total, from[i][2] = j;
	    }
	if (i + 20 <= n)
	    for (j = 1; j <= 27; j ++)  // 计算多余1行的情况
	    {
		for (total = dif[j][1][i], k = 2; k <= 20; k ++)
		    total += dif[j][k][i+k];
		if (total < cost[i][3]) cost[i][3] = total, from[i][3] = j;
		for (k = 2; k <= 20; k ++)
		{
		    total += dif[j][k][i+k-1], total -= dif[j][k][i+k];
		    if (total < cost[i][3]) cost[i][3] = total, from[i][3] = j;
		}
	    }
    }

    for (f[0] = 0, i = 1; i <= n; i ++) f[i] = INT_MAX;
    for (i = 19; i <= n; i ++){ // 动态规划决策划分方案
	if (i >= 19 && f[i-19] < INT_MAX && f[i-19] + cost[i-18][1] < f[i])
	    f[i] = f[i-19] + cost[i-18][1], g[i] = 19;
	if (i >= 20 && f[i-20] < INT_MAX && f[i-20] + cost[i-19][2] < f[i])
	    f[i] = f[i-20] + cost[i-19][2], g[i] = 20;
	if (i >= 21 && f[i-21] < INT_MAX && f[i-21] + cost[i-20][3] < f[i])
	    f[i] = f[i-21] + cost[i-20][3], g[i] = 21;
    }
    for (total = 0, i = n; i; i -= g[i])  // 得到解
	ans[++ total] = from[i-g[i]+1][g[i]-18];

    for (i = total; i; i --) 
	fout<<letter[ans[i]-1];
    fout<<endl;

    return 0;
}
