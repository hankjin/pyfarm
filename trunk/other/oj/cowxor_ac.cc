/* 
PROG: cowxor 
LANG: C++ 
*/  
#include<stdio.h>  
#define SIZE 700000  
#define BIT_AT(x,n) (((x) & (1<<n))>>n)  
int a[100001],n;  
struct Trie  
{  
    int idx,value;         
    int sub[2];  
}node[SIZE];  
int root;  
int node_alloc()  
{  
   static int t=0;  
   node[t].idx=node[t].value=-1;  
   node[t].sub[0]=node[t].sub[1]=-1;  
   return t++;      
}  
void insert(int idx,int value)  
{  
     if(root==-1) root=node_alloc();  
     int t,location=root;  
     for(int i=20;i>=0;i--)  
     {  
         t=BIT_AT(value,i);  
         if(node[location].sub[t]==-1)  
             node[location].sub[t]= node_alloc();                          
         location=node[location].sub[t];                    
     }  
     node[location].idx=idx;  
     node[location].value=value;  
}   
int find_max(int idx,int value)  
{  
    int t,location=root;  
    for(int i=20;i>=0 && location!=-1 ;i--)  
    {  
        t= BIT_AT(value,i);  
        if(node[location].sub[(t+1)%2]!=-1)  
             location= node[location].sub[(t+1)%2];  
        else   
             location= node[location].sub[t];              
    }  
    return location;      
}  
int main()  
{  
    freopen("cowxor.in","r",stdin);  
    freopen("cowxor.out","w",stdout);  
    a[0]=0;   
    root=-1;  
    insert(0,0);   
    scanf("%d",&n);  
    int t,max_i,max_j,max=-1;  
    for(int i=1;i<n+1;i++){
        scanf("%d",&t);  
        a[i]=a[i-1]^t;     
        t=find_max(i,a[i]);    
        if(t!=-1 && (a[i]^node[t].value)>max)  
        {  
                max=(a[i]^node[t].value);        
                max_i=node[t].idx+1;  
                //printf("%d\n",t);  
                max_j=i;  
        }  
          
        insert(i,a[i]);   
    }  
    printf("%d %d %d\n",max,max_i,max_j);  
//printf("%d ",node_alloc());  
} 
