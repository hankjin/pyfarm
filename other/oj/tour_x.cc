#include <fstream>
#include <string>
#include <map>

#define max(a,b) (((a)>(b))?(a):(b))
#define swap(a,b) {a ^= b; b ^= a; a ^= b;}

using namespace std;

int cache[101][101], conn[101][101];
ofstream outfile ("tour.out");
ifstream infile ("tour.in");
map <string, int> lookup;
int n, m, solution;
string j, k, str;

int solve(int a, int b) {
    int k = -1;
    if (a > b) swap (a, b);
    if (cache[a][b] != 0)   return cache[a][b];
    if (a == n && b == n)   return 0;
    if (a == b && a+b != 2) return -100;
    for (int i = a+1; i <= n; i++)
        if (conn[a][i]) k = max (k, 1+solve (i, b));
    return (cache[a][b] = k);
}

int main() {
    infile >> n >> m;
    for(int i = 1; i <= n; i++) {
        infile >> str;
	lookup[str]=i;
    }
    for(int i = 0; i < m; i++) {
        infile >> j >> k;
	conn[lookup[j]][lookup[k]]=true;
	conn[lookup[k]][lookup[j]]=true;
    }
    solution = solve (1, 1);
    outfile<< ((solution == 2) ? solution - 1 : solution)  << endl;
    return 0;
}

