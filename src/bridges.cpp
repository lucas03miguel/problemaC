#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>
#include <cstring>
#include <queue>
using namespace std;

const int MAX = 500;
vector<pair<int, int>> adj[MAX * MAX];
int disc[MAX * MAX], low[MAX * MAX], parent[MAX * MAX];
bool visited[MAX * MAX];
vector<pair<int, int>> articulationPoints;
vector<pair<pair<int, int>, pair<int, int>>> bridges;
int t;

int row[] = {-1, 1, 0, 0};
int col[] = {0, 0, -1, 1};
int n, m;

void dfs(int u) {
    static int time = 0;
    disc[u] = low[u] = ++time;
    visited[u] = true;
    int children = 0;

    for (auto v : adj[u]) {
        if (!visited[v.first]) {
            children++;
            parent[v.first] = u;
            dfs(v.first);
            low[u] = min(low[u], low[v.first]);
            if (parent[u] == -1 && children > 1)
                articulationPoints.push_back({u / m, u % m});
            if (parent[u] != -1 && low[v.first] >= disc[u])
                articulationPoints.push_back({u / m, u % m});
            if (low[v.first] > disc[u])
                bridges.push_back({{u / m, u % m}, {v.first / m, v.first % m}});
        } else if (v.first != parent[u])
            low[u] = min(low[u], disc[v.first]);
    }
}

void findArticulationPointsAndBridges() {
    memset(disc, -1, sizeof(disc));
    memset(low, -1, sizeof(low));
    memset(parent, -1, sizeof(parent));
    memset(visited, false, sizeof(visited));

    for (int i = 0; i < n * m; i++) {
        if (!visited[i])
            dfs(i);
    }
}

bool isValid(int x, int y) {
    return (x >= 0 && y >= 0 && x < n && y < m);
}

pair<pair<int, int>, pair<int, int>> findFloodGate(vector<string> &maze) {
    for (auto bridge : bridges) {
        int x1 = bridge.first.first, y1 = bridge.first.second;
        int x2 = bridge.second.first, y2 = bridge.second.second;
        if (maze[x1][y1] == '.' && maze[x2][y2] == '.') {
            return {{x1, y1}, {x2, y2}};
        }
    }
    return {{-1, -1}, {-1, -1}};
}

vector<pair<int, int>> bfs(vector<string> &maze, int sx, int sy) {
    vector<vector<bool>> visited(n, vector<bool>(m, false));
    queue<pair<int, int>> q;
    vector<pair<int, int>> path;
    vector<vector<pair<int, int>>> parent(n, vector<pair<int, int>>(m, {-1, -1}));
    q.push({sx, sy});
    visited[sx][sy] = true;

    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();
        path.push_back({x, y});

        if (maze[x][y] == 'E') {
            vector<pair<int, int>> fullPath;
            pair<int, int> step = {x, y};
            while (step.first != -1) {
                fullPath.push_back(step);
                step = parent[step.first][step.second];
            }
            reverse(fullPath.begin(), fullPath.end());
            return fullPath;
        }

        for (int i = 0; i < 4; i++) {
            int nx = x + row[i], ny = y + col[i];
            if (isValid(nx, ny) && !visited[nx][ny] && maze[nx][ny] != '#' && maze[nx][ny] != 'M') {
                visited[nx][ny] = true;
                parent[nx][ny] = {x, y};
                q.push({nx, ny});
            }
        }
    }
    return {};
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);

    int T;
    cin >> T;
    while (T--) {
        cin >> n >> m;
        vector<string> maze(n);
        for (int i = 0; i < n; ++i) {
            cin >> maze[i];
        }
        int C;
        cin >> C;

        vector<pair<int, int>> manholes;
        int door_x, door_y, exit_x, exit_y;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                if (maze[i][j] == 'M') {
                    manholes.push_back({i, j});
                } else if (maze[i][j] == 'D') {
                    door_x = i;
                    door_y = j;
                } else if (maze[i][j] == 'E') {
                    exit_x = i;
                    exit_y = j;
                }
            }
        }

        // Build graph
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                if (maze[i][j] == '#') continue;
                int u = i * m + j;
                for (int k = 0; k < 4; ++k) {
                    int ni = i + row[k], nj = j + col[k];
                    if (isValid(ni, nj) && maze[ni][nj] != '#') {
                        int v = ni * m + nj;
                        adj[u].push_back({v, 1});
                    }
                }
            }
        }

        // Find articulation points and bridges
        findArticulationPointsAndBridges();

        // Place floodgate
        auto floodgate = findFloodGate(maze);
        cout << floodgate.first.first << " " << floodgate.first.second << " "
             << floodgate.second.first << " " << floodgate.second.second << endl;

        // Place manhole covers
        cout << min(C, (int)manholes.size()) << endl;
        for (int i = 0; i < min(C, (int)manholes.size()); ++i) {
            cout << manholes[i].first << " " << manholes[i].second << endl;
        }

        // Find and print safe path
        vector<pair<int, int>> safePath = bfs(maze, door_x, door_y);
        cout << safePath.size() << endl;
        for (auto [x, y] : safePath) {
            cout << x << " " << y << endl;
        }

        // Clear for next test case
        for (int i = 0; i < n * m; i++) adj[i].clear();
        articulationPoints.clear();
        bridges.clear();
    }
    return 0;
}
