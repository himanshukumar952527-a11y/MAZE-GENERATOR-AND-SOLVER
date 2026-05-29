#include <stdio.h>
#include <stdlib.h>

#define MAX 350

typedef struct { int r,c; } Point;

int grid[MAX][MAX], visited[MAX][MAX];
Point parent[MAX][MAX];
int n;

int dr[]={-1,1,0,0};
int dc[]={0,0,-1,1};

// QUEUE (BFS) 
Point queue[MAX*MAX];
int front=0,rear=0;

void enqueue(Point p){
    if(rear >= MAX*MAX){
        printf("Queue Overflow\n");
        exit(1);
    }
    queue[rear++]=p;
}
Point dequeue(){ return queue[front++]; }
int isEmpty(){ return front==rear; }

// STACK (DFS) 
Point stack[MAX*MAX];
int top = -1;

void push(Point p){
    if(top >= MAX*MAX-1){
        printf("Stack Overflow\n");
        exit(1);
    }
    stack[++top]=p;
}
Point pop(){ return stack[top--]; }
int isEmptyStack(){ return top==-1; }

// RESET 
void reset(){
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++){
            visited[i][j]=0;
            parent[i][j]=(Point){-1,-1};
        }
}

// VALID 
int valid(int r,int c){
    return r>=0 && c>=0 && r<n && c<n &&
           !visited[r][c] && grid[r][c]==0;
}

// BFS 
int bfs(Point s, Point e, Point path[], int *steps){
    front=rear=0;
    *steps=0;

    reset();

    enqueue(s);
    visited[s.r][s.c]=1;

    while(!isEmpty()){
        Point cur=dequeue();
        (*steps)++;

        if(cur.r==e.r && cur.c==e.c){
            int len=0;
            Point p=e;
            while(!(p.r==s.r && p.c==s.c)){
                path[len++]=p;
                p=parent[p.r][p.c];
            }
            path[len++]=s;
            return len;
        }

        for(int i=0;i<4;i++){
            int nr=cur.r+dr[i], nc=cur.c+dc[i];
            if(valid(nr,nc)){
                visited[nr][nc]=1;
                parent[nr][nc]=cur;
                enqueue((Point){nr,nc});
            }
        }
    }
    return -1;
}

// DFS 
int dfs(Point s, Point e, Point path[], int *steps){
    top=-1;
    *steps=0;

    reset();

    push(s);
    visited[s.r][s.c]=1;

    while(!isEmptyStack()){
        Point cur=pop();
        (*steps)++;

        if(cur.r==e.r && cur.c==e.c){
            int len=0;
            Point p=e;
            while(!(p.r==s.r && p.c==s.c)){
                path[len++]=p;
                p=parent[p.r][p.c];
            }
            path[len++]=s;
            return len;
        }

        for(int i=0;i<4;i++){
            int nr=cur.r+dr[i], nc=cur.c+dc[i];
            if(valid(nr,nc)){
                visited[nr][nc]=1;
                parent[nr][nc]=cur;
                push((Point){nr,nc});
            }
        }
    }
    return -1;
}

// MAIN 
int main(){

    FILE *f=fopen("input.txt","r");
    if(!f){
        printf("File not found\n");
        return 1;
    }

    fscanf(f,"%d",&n);

    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            fscanf(f,"%d",&grid[i][j]);

    Point s,e;
    fscanf(f,"%d %d",&s.r,&s.c);
    fscanf(f,"%d %d",&e.r,&e.c);
    fclose(f);

    // start/end validation
    if(grid[s.r][s.c]==1 || grid[e.r][e.c]==1){
        printf("Invalid start or end point\n");
        return 1;
    }

    Point bfs_path[MAX*MAX], dfs_path[MAX*MAX];
    int bfs_steps=0, dfs_steps=0;

    int bfs_len=bfs(s,e,bfs_path,&bfs_steps);
    int dfs_len=dfs(s,e,dfs_path,&dfs_steps);

    FILE *out=fopen("output.json","w");
    if(!out){
        printf("Error opening output file\n");
        return 1;
    }

    fprintf(out,"{\n");

    // BFS
    fprintf(out,"\"bfs\":{");
    if(bfs_len==-1){
        fprintf(out,"\"found\":false},\n");
    }else{
        fprintf(out,"\"found\":true,\"steps\":%d,\"path_length\":%d,\"path\":[",bfs_steps,bfs_len);
        for(int i=bfs_len-1;i>=0;i--){
            fprintf(out,"[%d,%d]%s",bfs_path[i].r,bfs_path[i].c,i?",":"");
        }
        fprintf(out,"]},\n");
    }

    // DFS
    fprintf(out,"\"dfs\":{");
    if(dfs_len==-1){
        fprintf(out,"\"found\":false}\n");
    }else{
        fprintf(out,"\"found\":true,\"steps\":%d,\"path_length\":%d,\"path\":[",dfs_steps,dfs_len);
        for(int i=dfs_len-1;i>=0;i--){
            fprintf(out,"[%d,%d]%s",dfs_path[i].r,dfs_path[i].c,i?",":"");
        }
        fprintf(out,"]}\n");
    }

    fprintf(out,"}");

    fclose(out);
    return 0;
}
