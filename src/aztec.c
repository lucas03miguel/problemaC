#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 500

typedef struct
{
    int x, y;
} Point;

int N, M, C, dx, dy, ex, ey;
char maze[MAX][MAX];
int flood_gate[4];
int covers[MAX][2];
int num_covers = 0, bridge_count = 0, idx = 1;
Point path[MAX * MAX];
int path_len;

int dfn[MAX][MAX], low[MAX][MAX], stack[MAX * MAX][2], top = 0;
int bridges[MAX * MAX][4];
int visited[MAX][MAX];
int parent[MAX][MAX][2];


void add_bridge(int x1, int y1, int x2, int y2) {
    bridges[bridge_count][0] = x1;
    bridges[bridge_count][1] = y1;
    bridges[bridge_count][2] = x2;
    bridges[bridge_count][3] = y2;
    bridge_count++;

    maze[x1][y1] = 'F';
    maze[x2][y2] = 'F';
}


int canStillReachExit(int x1, int y1, int x2, int y2) {
    char temp = maze[x2][y2]; // Salva o estado original
    maze[x2][y2] = '#';       // Bloqueia temporariamente a célula

    int queue[MAX * MAX][2], front = 0, rear = 0; // BFS para verificar conectividade
    int visited[MAX][MAX] = {0};
    int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    // Inicia a partir de (x1, y1), o outro lado da ponte
    queue[rear][0] = x1;
    queue[rear++][1] = y1;
    visited[x1][y1] = 1;

    while (front < rear)
    {
        int x = queue[front][0], y = queue[front][1];
        front++;
        for (int i = 0; i < 4; i++)
        {
            int nx = x + directions[i][0], ny = y + directions[i][1];
            if (nx >= 0 && ny >= 0 && nx < N && ny < M && !visited[nx][ny] && maze[nx][ny] != '#')
            {
                if (nx == ex && ny == ey)
                {                        // Verifica se alcançou a saída
                    maze[x2][y2] = temp; // Restaura o estado
                    return 1;            // A saída ainda é acessível
                }
                queue[rear][0] = nx;
                queue[rear++][1] = ny;
                visited[nx][ny] = 1;
            }
        }
    }
    maze[x2][y2] = temp; // Restaura o estado
    return 0;            // A saída não é acessível
}
void tarjan_iterative() {
    memset(dfn, -1, sizeof(dfn));
    memset(low, -1, sizeof(low));
    memset(parent, -1, sizeof(parent));
    // memset(visited, 0, sizeof(visited));
    int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    stack[top][0] = dx;
    stack[top++][1] = dy;
    dfn[dx][dy] = low[dx][dy] = idx++;

    while (top > 0) {
        int x = stack[top - 1][0];
        int y = stack[top - 1][1];
        if (!visited[x][y]) {
            visited[x][y] = 1;
            for (int i = 0; i < 4; i++)
            {
                int nx = x + directions[i][0], ny = y + directions[i][1];
                if (nx >= 0 && ny >= 0 && nx < N && ny < M && (maze[nx][ny] == 'M' || maze[nx][ny] == '.') && !visited[nx][ny])
                {
                    stack[top][0] = nx;
                    stack[top++][1] = ny;
                    parent[nx][ny][0] = x;
                    parent[nx][ny][1] = y;
                    dfn[nx][ny] = low[nx][ny] = idx++;
                }
                else if (nx >= 0 && ny >= 0 && nx < N && ny < M && (maze[nx][ny] == 'M' || maze[nx][ny] == '.') && (parent[x][y][0] != nx || parent[x][y][1] != ny))
                {
                    low[x][y] = (low[x][y] < dfn[nx][ny]) ? low[x][y] : dfn[nx][ny];
                }
            }
        }
        else {
            top--;
            int px = parent[x][y][0], py = parent[x][y][1];
            if (px != -1) {
                low[px][py] = (low[px][py] < low[x][y]) ? low[px][py] : low[x][y];
                if (low[x][y] > dfn[px][py] && (maze[px][py] == 'M' || maze[px][py] == '.')) {
                    if (canStillReachExit(px, py, x, y)) {
                        add_bridge(px, py, x, y);
                    }
                }
            }
        }
    }
}

void bfs_safe_path() {
    int queue[MAX * MAX][2], front = 0, rear = 0;
    int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    memset(visited, 0, sizeof(visited));
    memset(parent, -1, sizeof(parent));
    visited[dx][dy] = 1;
    queue[rear][0] = dx;
    queue[rear++][1] = dy;

    while (front < rear)
    {
        int x = queue[front][0], y = queue[front][1];
        front++;

        if (x == ex && y == ey)
            break;

        for (int i = 0; i < 4; i++)
        {
            int nx = x + directions[i][0], ny = y + directions[i][1];
            if (nx >= 0 && ny >= 0 && nx < N && ny < M && maze[nx][ny] != '#' && !visited[nx][ny])
            {
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

    while (cx != dx || cy != dy)
    {
        path[path_len].x = cx;
        path[path_len].y = cy;
        int temp_x = parent[cx][cy][0];
        int temp_y = parent[cx][cy][1];
        cx = temp_x;
        cy = temp_y;
        path_len++;
    }
    path[path_len].x = dx;
    path[path_len].y = dy;
    path_len++;
}

void evaluate_and_cover_manholes() {
    // Reinicializa a contagem para esta execução
    num_covers = 0;

    // Itera por todo o labirinto buscando manholes
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (maze[i][j] == 'M') { // Verifica se é um manhole
                int isProtected = 0;

                // Verifica se o manhole está dentro da área de uma flood gate
                for (int k = 0; k < bridge_count; k++) {
                    if ((bridges[k][0] == i && bridges[k][1] == j) || (bridges[k][2] == i && bridges[k][3] == j)) {
                        isProtected = 1;
                        break; // Sai do loop se protegido
                    }
                }

                // Adiciona cover se não estiver protegido e ainda tiver covers disponíveis
                if (!isProtected && num_covers < C) {
                    covers[num_covers][0] = i;
                    covers[num_covers][1] = j;
                    num_covers++;
                    //maze[i][j] = 'C'; // Marca o manhole como coberto
                }
            }
        }
    }

    // Debug: Imprimir número de covers colocados
    printf("Covers colocados: %d\n", num_covers);
}



int main() {
    srand(time(NULL));

    int t;
    scanf("%d", &t);

    for (int z = 0; z < t; ++z) {
        scanf("%d %d", &N, &M);

        for (int i = 0; i < N; i++) {
            scanf("%s", maze[i]);
        }
        scanf("%d", &C);

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                if (maze[i][j] == 'D') {
                    dx = i;
                    dy = j;
                }
                else if (maze[i][j] == 'E') {
                    ex = i;
                    ey = j;
                }
            }
        }

        tarjan_iterative();

        for (int i = 0; i < bridge_count; i++) {
            printf("%d %d %d %d\n", bridges[i][0], bridges[i][1], bridges[i][2], bridges[i][3]);
        }

        int selected_index = rand() % bridge_count;
        // printf("%d, %d\n", bridge_count, selected_index);
        flood_gate[0] = bridges[selected_index][0];
        flood_gate[1] = bridges[selected_index][1];
        flood_gate[2] = bridges[selected_index][2];
        flood_gate[3] = bridges[selected_index][3];

        evaluate_and_cover_manholes();

        bfs_safe_path();

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
