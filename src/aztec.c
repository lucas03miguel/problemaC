#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <float.h>

#define MAX_ROWS 500
#define MAX_COLUMNS 500
#define MAX 500

typedef struct {
    int x;
    int y;
    int state;
} Point;

typedef struct {
    Point p1;
    Point p2;
} Bridge;

typedef struct {
    int u, v;
    int x, y;
} Edge;

int N, M;
char maze[MAX_ROWS][MAX_COLUMNS];
Edge bridges[MAX + MAX];
int flood_gate[4];
int covers[MAX][2];
int num_covers;
Point path[MAX + MAX];
int path_len;

int DIRECTIONS[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};


Bridge* dfs_bridges_iterative(char grid[MAX_ROWS][MAX_COLUMNS], int rows, int columns, int sx, int sy, int ex, int ey, int* num_bridges) {
    bool visited[MAX_ROWS][MAX_COLUMNS];
    float dfn[MAX_ROWS][MAX_COLUMNS];
    float low[MAX_ROWS][MAX_COLUMNS];
    Point parent[MAX_ROWS][MAX_COLUMNS];
    Bridge* bridges = NULL;
    int bridges_size = 0;
    Point stack[MAX_ROWS * MAX_COLUMNS];
    int top = 0;
    int time = 0;

    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            visited[i][j] = false;
            dfn[i][j] = FLT_MAX;
            low[i][j] = FLT_MAX;
            parent[i][j] = (Point){-1, -1, -1};
        }
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            //printf("grid[i][j]: %c\n", grid[i][j]);
            if (grid[i][j] != '#' && !visited[i][j]) {
                stack[top++] = (Point){i, j, 0};
                //printf("Stack %d: %d %d %d\n", top - 1, stack[top - 1].x, stack[top - 1].y, stack[top - 1].state);
                while (top > 0) {
                    //printf("Top: %d\n", top);
                    Point current = stack[--top];
                    int x = current.x;
                    int y = current.y;
                    int state = current.state;

                    
                    if (state == 0) {
                        visited[x][y] = true;
                        dfn[x][y] = low[x][y] = time++;
                        stack[top++] = (Point){x, y, 1};

                        for (int k = 0; k < 4; k++) {
                            int nx = x + DIRECTIONS[k][0];
                            int ny = y + DIRECTIONS[k][1];

                            if (nx >= 0 && nx < rows && ny >= 0 && ny < columns && grid[nx][ny] != '#' && !visited[nx][ny]) {
                                stack[top++] = (Point){nx, ny, 0};
                                parent[nx][ny] = (Point){x, y, -1};
                            } else if ((nx != parent[x][y].x || ny != parent[x][y].y) && nx >= 0 && nx < rows && ny >= 0 && ny < columns) {
                                //printf("\n\nLow: %f\n", low[x][y]);
                                //printf("Low nx ny: %f\n", low[nx][ny]);
                                //printf("Dfn: %f\n\n\n", dfn[nx][ny]);
                                low[x][y] = fmin(low[x][y], dfn[nx][ny]);
                            }
                        }
                    } else {
                        //printf("Current: %d %d %d\n", x, y, state);

                        for (int k = 0; k < 4; k++) {
                            int nx = x + DIRECTIONS[k][0];
                            int ny = y + DIRECTIONS[k][1];

                            if (nx >= 0 && nx < rows && ny >= 0 && ny < columns && grid[nx][ny] != '#') {
                                if (nx == parent[x][y].x && ny == parent[x][y].y) {
                                    //printf("Low: %f\n", low[x][y]);
                                    //printf("Low nx ny: %f\n", low[nx][ny]);
                                    low[x][y] = fmin(low[x][y], low[nx][ny]);

                                    //printf("Dfn: %f\n", dfn[x][y]);
                                    if (low[nx][ny] > dfn[x][y]) {
                                        //printf("Bridge: %d %d %d %d\n", x, y, nx, ny);
                                        bridges_size++;
                                        bridges = (Bridge*)realloc(bridges, bridges_size * sizeof(Bridge));
                                        bridges[bridges_size - 1] = (Bridge){(Point){x, y, -1}, (Point){nx, ny, -1}};
                                    }
                                } else if (nx != parent[x][y].x || ny != parent[x][y].y) {
                                    low[x][y] = fmin(low[x][y], dfn[nx][ny]);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    *num_bridges = bridges_size;
    return bridges;
}

bool bfs_safe_path(char maze[MAX][MAX], int sx, int sy, int ex, int ey, bool water_filled[MAX][MAX]) {
    int N = N, M = M;
    bool visited[MAX][MAX] = {false};
    Point queue[MAX + MAX];
    int front = 0, rear = 0;
    Point parent[MAX][MAX];
    Point came_from[MAX][MAX];

    queue[rear++] = (Point){sx, sy};
    visited[sx][sy] = true;

    while (front < rear) {
        Point current = queue[front++];
        int x = current.x, y = current.y;

        if (x == ex && y == ey) {
            int px = x, py = y;
            while (px != -1 && py != -1) {
                path[path_len++] = (Point){px, py};
                Point p = came_from[px][py];
                px = p.x;
                py = p.y;
            }

            // Reverse the path
            for (int i = 0; i < path_len / 2; i++) {
                Point temp = path[i];
                path[i] = path[path_len - 1 - i];
                path[path_len - 1 - i] = temp;
            }

            return true;
        }

        for (int i = 0; i < 4; i++) {
            int nx = x + DIRECTIONS[i][0];
            int ny = y + DIRECTIONS[i][1];

            if (nx >= 0 && nx < N && ny >= 0 && ny < M && maze[nx][ny] != '#' && !water_filled[nx][ny] && !visited[nx][ny]) {
                visited[nx][ny] = true;
                queue[rear++] = (Point){nx, ny};
                came_from[nx][ny] = current;
            }
        }
    }

    return false;
}

bool is_blocked(int x1, int y1, int x2, int y2, Edge block) {
    return ((x1 == block.u && y1 == block.v && x2 == block.x && y2 == block.y) ||
            (x1 == block.x && y1 == block.y && x2 == block.u && y2 == block.v));
}

void bfs_flood_control(char grid[MAX][MAX], Point manholes[MAX], int num_manholes, Edge block, bool flood[MAX][MAX]) {
    int rows = N, columns = M;
    bool visited[MAX][MAX] = {false};
    Point queue[MAX + MAX];
    int front = 0, rear = 0;

    for (int i = 0; i < num_manholes; i++) {
        int x = manholes[i].x, y = manholes[i].y;
        queue[rear++] = manholes[i];
        visited[x][y] = true;
    }

    while (front < rear) {
        Point current = queue[front++];
        int x = current.x, y = current.y;

        for (int i = 0; i < 4; i++) {
            int nx = x + DIRECTIONS[i][0];
            int ny = y + DIRECTIONS[i][1];

            if (nx >= 0 && nx < rows && ny >= 0 && ny < columns && grid[nx][ny] != '#' &&
                !visited[nx][ny] && !is_blocked(x, y, nx, ny, block)) {
                visited[nx][ny] = true;
                queue[rear++] = (Point){nx, ny};
            }
        }
    }

    memcpy(flood, visited, sizeof(bool) * MAX + MAX);
}

Edge evaluate_manhole_cover(char maze[MAX][MAX], Point manholes[MAX], int num_manholes, Edge bridges[MAX + MAX], int bridge_count) {
    bool initial_flooded[MAX][MAX];
    bfs_flood_control(maze, manholes, num_manholes, (Edge){-1, -1, -1, -1}, initial_flooded);
    int count_initial = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (initial_flooded[i][j]) {
                count_initial++;
            }
        }
    }

    Edge bridge_best = {-1, -1, -1, -1};
    int count_best = count_initial;

    for (int i = 0; i < bridge_count; i++) {
        Edge bridge = bridges[i];
        bool flooded[MAX][MAX];
        bfs_flood_control(maze, manholes, num_manholes, bridge, flooded);
        int count = 0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                if (flooded[i][j]) {
                    count++;
                }
            }
        }

        if (count < count_best) {
            count_best = count;
            bridge_best = bridge;
        }
    }

    return bridge_best;
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
        Point manholes[MAX];
        int num_manholes = 0;

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                if (maze[i][j] == 'D') {
                    sx = i;
                    sy = j;
                } else if (maze[i][j] == 'E') {
                    ex = i;
                    ey = j;
                } else if (maze[i][j] == 'M') {
                    manholes[num_manholes++] = (Point){i, j};
                }
            }
        }

        int bridge_count = 0;
        dfs_bridges_iterative(maze, N, M, sx, sy, ex, ey, &bridge_count);

        //printf("%d\n", bridge_count);
        for (int i = 0; i < bridge_count; i++) {
            //printf("%d %d %d %d\n", bridges[i].u, bridges[i].v, bridges[i].x, bridges[i].y);
        }
        
/*
        Edge flood_gate = evaluate_manhole_cover(maze, manholes, num_manholes, bridges, bridge_count);

        // Place manhole covers (example: just cover the first C manholes)
        num_covers = 0;
        for (int i = 0; i < num_manholes && num_covers < C; i++) {
            covers[num_covers][0] = manholes[i].x;
            covers[num_covers][1] = manholes[i].y;
            num_covers++;
        }

        bool water_filled[MAX][MAX];
        bfs_flood_control(maze, manholes, num_manholes - num_covers, flood_gate, water_filled);

        path_len = 0;
        bfs_safe_path(maze, sx, sy, ex, ey, water_filled);

        // Output
        printf("%d %d %d %d\n", flood_gate.u, flood_gate.v, flood_gate.x, flood_gate.y);
        printf("%d\n", num_covers);
        for (int i = 0; i < num_covers; i++) {
            printf("%d %d\n", covers[i][0], covers[i][1]);
        }
        printf("%d\n", path_len);
        for (int i = 0; i < path_len; i++) {
            printf("%d %d\n", path[i].x, path[i].y);
        }
*/
    }

    return 0;
}