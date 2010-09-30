/*
ID:GeorgeG1
PROB:concom
LANG:C++
*/
#include <fstream>
#include <cstdlib>
#include <string.h>
 
using namespace std;
 
 
ifstream fin ("concom.in");
ofstream fout ("concom.out");
 
const long comLimit=101;
 
long inputState[comLimit][comLimit]; //inputState[i][j]=x  means i owns x% stocks of j
 
void init()
{
     memset (inputState,0,sizeof(inputState));
     long conCount;
     fin >>conCount;
     long i,j,p;
     for (long x=1;x<=conCount;x++)
     {
         fin >>i >>j;
         fin >>p;
         inputState[i][j]+=p;
     }
     return;
}
 
 
int main()
{
    init();
 
    long conState[comLimit][comLimit]; //conState[i][j]=x  means i owns x% stocks of j
    bool owns[comLimit][comLimit];
 
    memset (owns,0,sizeof(owns));
    for (long i=1;i<comLimit;i++) owns[i][i]=true;
    bool changed=true;
    //work
    while (changed)
    {
          changed=false;
          memset (conState,0,sizeof(conState));
          //根据owns从inputState重建conState 
          for (long i=1;i<comLimit;i++)
              for (long j=1;j<comLimit;j++)
                  if (owns[i][j])
                  {
                     for (long j2=1;j2<comLimit;j2++)
                         conState[i][j2]+=inputState[j][j2];
                  }
          //根据conState更新owns 
          for (long i=1;i<comLimit;i++)
              for (long j=1;j<comLimit;j++)
                  if (conState[i][j]>50 && owns[i][j]==false)
                  {
                     changed=true;
                     owns[i][j]=true;
                  }
    }    
 
 
    //output
    for (long i=1;i<comLimit;i++)           
    {
        for (long j=1;j<comLimit;j++)
        {
            if (owns[i][j] && i!=j)
               fout <<i <<' ' <<j <<endl;
        }
    }
    return 0;
}
