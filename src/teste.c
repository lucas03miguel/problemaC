#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 500

typedef struct {
    int x, y;
} Point;

typedef struct {
    int u, v;
} Edge;

int N, M;
char maze[MAX][MAX];
int flood_gate[4];
int covers[MAX][2];
int num_covers;
Point path[MAX * MAX];
int path_len;

int dfn[MAX][MAX], low[MAX][MAX], idx, stack[MAX * MAX][2], top;
int bridges[MAX * MAX][4], bridge_count;
int visited[MAX][MAX];
int parent[MAX][MAX][2];

void add_bridge(int x1, int y1, int x2, int y2) {
    bridges[bridge_count][0] = x1;
    bridges[bridge_count][1] = y1;
    bridges[bridge_count][2] = x2;
    bridges[bridge_count][3] = y2;
    bridge_count++;
}

void tarjan_iterative(int sx, int sy) {
    memset(dfn, 0, sizeof(dfn));
    memset(low, 0, sizeof(low));
    memset(parent, -1, sizeof(parent));
    idx = 1;
    top = 0;

    stack[top][0] = sx;
    stack[top++][1] = sy;
    dfn[sx][sy] = low[sx][sy] = idx++;

    while (top > 0) {
        int x = stack[top-1][0];
        int y = stack[top-1][1];

        int found = 0;
        int adj[4][2] = {{x+1, y}, {x-1, y}, {x, y+1}, {x, y-1}};

        for (int i = 0; i < 4; i++) {
            int nx = adj[i][0], ny = adj[i][1];
            if (nx >= 0 && ny >= 0 && nx < N && ny < M && maze[nx][ny] != '#' && parent[x][y][0] != nx && parent[x][y][1] != ny) {
                if (!dfn[nx][ny]) {
                    stack[top][0] = nx;
                    stack[top++][1] = ny;
                    dfn[nx][ny] = low[nx][ny] = idx++;
                    parent[nx][ny][0] = x;
                    parent[nx][ny][1] = y;
                    found = 1;
                    break;
                } else {
                    low[x][y] = low[x][y] < dfn[nx][ny] ? low[x][y] : dfn[nx][ny];
                }
            }
        }

        if (!found) {
            top--;
            for (int i = 0; i < 4; i++) {
                int nx = adj[i][0], ny = adj[i][1];
                if (nx >= 0 && ny >= 0 && nx < N && ny < M && maze[nx][ny] != '#' && parent[x][y][0] != nx && parent[x][y][1] != ny) {
                    if (low[nx][ny] > dfn[x][y]) {
                        add_bridge(x, y, nx, ny);
                    }
                    low[x][y] = low[x][y] < low[nx][ny] ? low[x][y] : low[nx][ny];
                }
            }
        }
    }
}

void bfs_safe_path(int sx, int sy, int ex, int ey) {
    int queue[MAX * MAX][2], front = 0, rear = 0;
    int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    memset(visited, 0, sizeof(visited));
    memset(parent, -1, sizeof(parent));
    visited[sx][sy] = 1;
    queue[rear][0] = sx;
    queue[rear++][1] = sy;

    while (front < rear) {
        int x = queue[front][0], y = queue[front][1];
        front++;

        if (x == ex && y == ey) break;

        for (int i = 0; i < 4; i++) {
            int nx = x + directions[i][0], ny = y + directions[i][1];
            if (nx >= 0 && ny >= 0 && nx < N && ny < M && maze[nx][ny] != '#' && !visited[nx][ny]) {
                visited[nx][ny] = 1;
                parent[nx][ny][0] = x;
                parent[nx][ny][1] = y;
                queue[rear][0] = nx;
                queue[rear++][1] = ny;
            }
        }
    }

    int cx = ex, cy = ey;
    path_len = 0;

    while (cx != sx || cy != sy) {
        path[path_len].x = cx;
        path[path_len].y = cy;
        int temp_x = parent[cx][cy][0];
        int temp_y = parent[cx][cy][1];
        cx = temp_x;
        cy = temp_y;
        path_len++;
    }
    path[path_len].x = sx;
    path[path_len].y = sy;
    path_len++;
}

int main() {
    int t;
    scanf("%d", &t);

    while (t--) {
        scanf("%d %d", &N, &M);

        for (int i = 0; i < N; i++) {
            scanf("%s", maze[i]);
        }

        int C;
        scanf("%d", &C);

        int sx = 0, sy = 0, ex = 0, ey = 0;

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                if (maze[i][j] == 'D') {
                    sx = i;
                    sy = j;
                } else if (maze[i][j] == 'E') {
                    ex = i;
                    ey = j;
                }
            }
        }

        bridge_count = 0;
        tarjan_iterative(sx, sy);
        
        for (int i = 0; i < bridge_count; i++) {
            printf("%d %d %d %d\n", bridges[i][0], bridges[i][1], bridges[i][2], bridges[i][3]);
        }

        // Use the first bridge as the flood gate location
        if (bridge_count > 0) {
            flood_gate[0] = bridges[0][0];
            flood_gate[1] = bridges[0][1];
            flood_gate[2] = bridges[0][2];
            flood_gate[3] = bridges[0][3];
        } else {
            // Default to (0,0)-(0,1) if no bridges found, adjust as necessary
            flood_gate[0] = 0;
            flood_gate[1] = 0;
            flood_gate[2] = 0;
            flood_gate[3] = 1;
        }

        // Place manhole covers (example: just cover the first C manholes)
        num_covers = 0;
        for (int i = 0; i < N && num_covers < C; i++) {
            for (int j = 0; j < M && num_covers < C; j++) {
                if (maze[i][j] == 'M') {
                    covers[num_covers][0] = i;
                    covers[num_covers][1] = j;
                    num_covers++;
                }
            }
        }

        bfs_safe_path(sx, sy, ex, ey);

        // Output
        printf("%d %d %d %d\n", flood_gate[0], flood_gate[1], flood_gate[2], flood_gate[3]);
        printf("%d\n", num_covers);
        for (int i = 0; i < num_covers; i++) {
            printf("%d %d\n", covers[i][0], covers[i][1]);
        }
        printf("%d\n", path_len);
        for (int i = path_len - 1; i >= 0; i--) {
            printf("%d %d\n", path[i].x, path[i].y);
        }
    }

    return 0;
}