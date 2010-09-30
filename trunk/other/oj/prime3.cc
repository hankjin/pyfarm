/*
ID: hankjin1
LANG: C++
TASK: prime3
*/
 
#include <stdio.h>
#include <string.h>
 
const int BIT[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512};
 
bool prime[100000];
int nxt[4000], next[4000][10], prev[4000], tot = 1;
int bit[1100]; // ¿¿2^k¿¿k¿¿¿¿¿¿¿B¿¿¿¿¿¿¿¿¿¿¿
 
int a11, a12, a13, a14, a15, t1,
    a21, a22, a23, a24, a25, t2,
    a31, a32, a33, a34, a35, t3,
    a41, a42, a43, a44, a45, t4,
    a51, a52, a53, a54, a55, t5,
s0,  s1,  s2,  s3,  s4,  s5, s6;
int      b12, b13, b14,
    b21, b22, b23, b24,
    b31, b32, b33, b34,
    b41, b42, b43, b44;
int n;
 
long i, j;
bool none = true;
 
inline void insert(int &x, const int y) {
	if (!x) {
		prev[(x = ++tot)] = j; // ¿¿¿¿¿¿¿¿¿¿
		nxt[j] |= BIT[y];
	}
	j = x;
}
 
int main() {
	freopen("prime3.in", "rt", stdin);
	freopen("prime3.out", "wt", stdout);
	scanf("%d%d", &n, &a11);
 
	memset(prime, 1, sizeof prime);
	memset(next, 0, sizeof next);
	memset(nxt, 0, sizeof nxt);
	for (i = 2; i < 100000; ++i)
		if (prime[i]) {
			for (j = i << 1; j < 100000; j += i) prime[j] = false;
			if (i >= 10000) {
				t5 = (j = i) % 10, t4 = (j /= 10) % 10;
				t3 = (j /= 10) % 10, t2 = (j /= 10) % 10;
				t1 = (j /= 10);
				if (t1 + t2 + t3 + t4 + t5 == n) {
					j = 1;
					insert(next[j][t1], t1), insert(next[j][t2], t2);
					insert(next[j][t3], t3), insert(next[j][t4], t4);
					// ¿¿¿¿¿¿¿¿¿¿¿Trie¿¿¿¿¿¿¿¿¿¿¿¿¿¿
					next[j][0] = t5, next[j][t5] = i;
				}
			}
		}
 
	if (!(t1 = s1 = s6 = next[1][a11])) {
		printf("NONE\n");
		return 0;
	}
	for (i = 0; i < 10; ++i) bit[BIT[i]] = i;
	for (b12 = nxt[t1] & nxt[1]; b12; b12 ^= b12 & -b12) {
	 a12 = bit[b12 & -b12];
	 t1 = next[t1][a12], s2 = next[1][a12];
	 for (b13 = nxt[t1] & nxt[1]; b13; b13 ^= b13 & -b13) {
	  a13 = bit[b13 & -b13];
	  t1 = next[t1][a13], s3 = next[1][a13];
	  for (b14 = nxt[t1] & nxt[1]; b14; b14 ^= b14 & - b14) {
	   a14 = bit[b14 & -b14];
	   t1 = next[t1][a14], s4 = next[1][a14];
	   a15 = next[t1][0];
	   if ((s5 = next[1][a15]))
	    for (b21 = nxt[1] & nxt[s1]; b21; b21 ^= b21 & -b21) {
	     a21 = bit[b21 & -b21];
	     t2 = next[1][a21], s1 = next[s1][a21];
	     for (b22 = nxt[t2] & nxt[s2] & nxt[s6]; b22; b22 ^= b22 & -b22) {
	      a22 = bit[b22 & -b22];
	      t2 = next[t2][a22], s2 = next[s2][a22], s6 = next[s6][a22];
	      for (b23 = nxt[t2] & nxt[s3]; b23; b23 ^= b23 & -b23) {
	       a23 = bit[b23 & -b23];
	       t2 = next[t2][a23], s3 = next[s3][a23];
	       for (b24 = nxt[t2] & nxt[s4]; b24; b24 ^= b24 & -b24) {
	        a24 = bit[b24 & -b24];
	        t2 = next[t2][a24], s4 = next[s4][a24];
	        a25 = next[t2][0];
	        if (next[s5][a25]) {
	         for (s5 = next[s5][a25], b31 = nxt[1] & nxt[s1]; b31; b31 ^= b31 & -b31) {
	          a31 = bit[b31 & -b31];
	          t3 = next[1][a31], s1 = next[s1][a31];
	          for (b32 = nxt[t3] & nxt[s2]; b32; b32 ^= b32 & -b32) {
	           a32 = bit[b32 & -b32];
	           t3 = next[t3][a32], s2 = next[s2][a32];
	           for (b33 = nxt[t3] & nxt[s3] & nxt[s6]; b33; b33 ^= b33 & -b33) {
	            a33 = bit[b33 & -b33];
	            t3 = next[t3][a33], s3 = next[s3][a33], s6 = next[s6][a33];
	            for (b34 = nxt[t3] & nxt[s4]; b34; b34 ^= b34 & -b34) {
	             a34 = bit[b34 & -b34];
	             t3 = next[t3][a34], s4 = next[s4][a34];
	             a35 = next[t3][0];
	             if (next[s5][a35]) {
	              for (s5 = next[s5][a35], b41 = nxt[1] & nxt[s1]; b41; b41 ^= b41 & -b41) {
	               a41 = bit[b41 & -b41];
	               t4 = next[1][a41], s1 = next[s1][a41];
	               a51 = next[s1][0];
	               if ((s0 = t5 = next[1][a51]))
	                for (b42 = nxt[t4] & nxt[s2] & nxt[s0]; b42; b42 ^= b42 & -b42) {
	                 a42 = bit[b42 & -b42];
	                 t4 = next[t4][a42], s2 = next[s2][a42];
	                 if (next[next[next[next[s0][a42]][a33]][a24]][a15]) {
	                  a52 = next[s2][0];
	                  if (next[t5][a52]) {
	                   for (t5 = next[t5][a52], b43 = nxt[t4] & nxt[s3]; b43; b43 ^= b43 & -b43) {
	                    a43 = bit[b43 & -b43];
	                    t4 = next[t4][a43], s3 = next[s3][a43];
	                    a53 = next[s3][0];
	                    if (next[t5][a53]) {
	                     for (t5 = next[t5][a53], b44 = nxt[t4] & nxt[s4] & nxt[s6]; b44; b44 ^= b44 & -b44) {
	                      a44 = bit[b44 & -b44];
	                      t4 = next[t4][a44], s4 = next[s4][a44], s6 = next[s6][a44];
	                      a45 = next[t4][0], a54 = next[s4][0], a55 = next[s6][0];
	                      if (next[next[s5][a45]][a55] && next[next[t5][a54]][a55]) {
	                       none ? none = false : printf("\n");
	                       printf("%d\n%d\n%d\n%d\n%d\n", next[t1][a15], next[t2][a25], next[t3][a35], next[t4][a45], next[next[t5][a54]][a55]);
	                      }
	                      t4 = prev[t4], s4 = prev[s4], s6 = prev[s6];
	                     }
	                     t5 = prev[t5];
	                    }
	                    t4 = prev[t4], s3 = prev[s3];
	                   }
	                   t5 = prev[t5];
	                  }
	                 }
	                 t4 = prev[t4], s2 = prev[s2];
	                }
	               s1 = prev[s1];
	              }
	              s5 = prev[s5];
	             }
	             t3 = prev[t3], s4 = prev[s4];
	            }
	            t3 = prev[t3], s3 = prev[s3], s6 = prev[s6];
	           }
	           t3 = prev[t3], s2 = prev[s2];
	          }
	          s1 = prev[s1];
	         }
	         s5 = prev[s5];
	        }
	        t2 = prev[t2], s4 = prev[s4];
	       }
	       t2 = prev[t2], s3 = prev[s3];
	      }
	      t2 = prev[t2], s2 = prev[s2], s6 = prev[s6];
	     }
	     s1 = prev[s1];
	    }
	   t1 = prev[t1];
	  }
	  t1 = prev[t1];
	 }
	 t1 = prev[t1];
	}
	if (none) printf("NONE\n");
	return 0;
}

