 
#include <cstdio>
#define MAXL 129
#define abs(x) (x > 0 ? x : -(x))
const int dr[4][2] = {{-1, 0}, {0, -1}, {0, 1}, {1, 0}};
 
FILE *fin = fopen("snail.in", "r");
FILE *fout = fopen("snail.out", "w");
 
bool map[MAXL][MAXL], vis[MAXL][MAXL];
int N, B, ANS;
char str[6];
 
void DFS(int NowX, int NowY, int Dr, int cnt)
{
//    fprintf(fout, "%d %d\n", NowX, NowY);
    if (map[NowX][NowY] || vis[NowX][NowY]) return ;
    if (cnt > ANS) ANS = cnt;
    vis[NowX][NowY] = true;
    int x = NowX + dr[Dr][0], y = NowY + dr[Dr][1];
    if (map[x][y])
        for (int i = 0; i <= 3; i++)
            DFS(NowX + dr[i][0], y = NowY + dr[i][1], i, cnt + 1);
    else
        DFS(NowX + dr[Dr][0], y = NowY + dr[Dr][1], Dr, cnt + 1);
    vis[NowX][NowY] = false;
    return ;
}
 
void Wall()
{
    for (int i = 0; i <= N + 1; i++)
        map[0][i] = map[N + 1][i] = true;
    for (int i = 0; i <= N + 1; i++)
        map[i][0] = map[i][N + 1] = true;
    return ;
}
 
int getnum()
{
int sum = 0;
    for (int i = 1; str[i]; i++)
        sum = sum * 10 + (str[i] - '0');
    return sum;
}
 
int main()
{
    fscanf(fin, "%d %d", &N, &B);
    Wall();
    while (B--)
    {
        fscanf(fin, "%s", str);
        map[getnum()][str[0] - 'A' + 1] = true;
    }
    DFS(1, 1, 2, 1);
    DFS(1, 1, 3, 1);
    fprintf(fout, "%d\n", ANS);
    return 0;
}
