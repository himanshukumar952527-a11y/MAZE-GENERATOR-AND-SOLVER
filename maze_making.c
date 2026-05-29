#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 351

int maze[MAX][MAX];
int n;

int dr[] = {-2,2,0,0};
int dc[] = {0,0,-2,2};

int steps = 0;   // STEP COUNTER

// === VALID ===
int valid(int r,int c){
    return r>0 && c>0 && r<n-1 && c<n-1 && maze[r][c]==1;
}

//  DFS MAZE 
void dfs(int r,int c){
    int dir[4]={0,1,2,3};

    for(int i=0;i<4;i++){
        int j=rand()%4;
        int temp=dir[i];
        dir[i]=dir[j];
        dir[j]=temp;
    }

    for(int i=0;i<4;i++){
        int nr=r+dr[dir[i]];
        int nc=c+dc[dir[i]];

        if(valid(nr,nc)){
            maze[(r+nr)/2][(c+nc)/2]=0;
            maze[nr][nc]=0;

            steps++;   // COUNT STEP

            dfs(nr,nc);
        }
    }
}

//  BFS MAZE 
typedef struct{int r,c;} Point;

Point q[MAX*MAX];
int front,rear;

void bfs(){
    front=rear=0;
    q[rear++] = (Point){1,1};
    maze[1][1]=0;

    while(front<rear){
        Point cur=q[front++];

        int dir[4]={0,1,2,3};
        for(int i=0;i<4;i++){
            int j=rand()%4;
            int t=dir[i]; dir[i]=dir[j]; dir[j]=t;
        }

        for(int i=0;i<4;i++){
            int nr=cur.r+dr[dir[i]];
            int nc=cur.c+dc[dir[i]];

            if(valid(nr,nc)){
                maze[(cur.r+nr)/2][(cur.c+nc)/2]=0;
                maze[nr][nc]=0;

                steps++;   // COUNT STEP

                q[rear++] = (Point){nr,nc};
            }
        }
    }
}

// MAIN 
int main(int argc,char* argv[]){
    if(argc<3){
        printf("Usage: maze_making size method\n");
        return 1;
    }

    n=atoi(argv[1]);
    int method=atoi(argv[2]); // 1=DFS, 2=BFS

    srand(time(0));

    steps = 0;   // RESETING STEPS HERE

    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            maze[i][j]=1;

    maze[1][1]=0;

    if(method==1)
        dfs(1,1);
    else
        bfs();

    FILE *f=fopen("input.txt","w");

    fprintf(f,"%d\n",n);
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++)
            fprintf(f,"%d ",maze[i][j]);
        fprintf(f,"\n");
    }

    fprintf(f,"1 1\n");
    fprintf(f,"%d %d\n",n-2,n-2);

    fprintf(f,"#steps %d\n",steps);   //SAVEING  STEPS HERE

    fclose(f);

    printf("Generation Steps: %d\n", steps);  //  PRINT

    return 0;
}