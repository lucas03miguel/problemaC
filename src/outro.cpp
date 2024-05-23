#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

// Function to find the articulation points in the graph
vector<int> findArticulationPoints(vector<vector<int>>& graph, int start);

void dfs(int u, int start, int& time, vector<int>& disc, vector<int>& low, vector<int>& parent, vector<int>& ap, vector<vector<int>>& graph) {
    disc[u] = low[u] = time++;
    int children = 0;

    for (int v : graph[u]) {
        if (disc[v] == -1) {
            children++;
            parent[v] = u;
            dfs(v, start, time, disc, low, parent, ap, graph);
            low[u] = min(low[u], low[v]);

            if ((u == start && children > 1) || (u != start && low[v] >= disc[u]))
                ap.push_back(u);
        } else if (v != parent[u]) {
            low[u] = min(low[u], disc[v]);
        }
    }
}

// Function to find bridges in the graph
vector<pair<int, int>> findBridges(vector<vector<int>>& graph);

void dfs(int u, int p, int& time, vector<int>& disc, vector<int>& low, vector<int>& parent, vector<pair<int, int>>& bridges, vector<vector<int>>& graph) {
    disc[u] = low[u] = time++;
    parent[u] = p;

    for (int v : graph[u]) {
        if (disc[v] == -1) {
            dfs(v, u, time, disc, low, parent, bridges, graph);
            low[u] = min(low[u], low[v]);

            if (low[v] > disc[u])
                bridges.push_back({u, v});
        } else if (v != p) {
            low[u] = min(low[u], disc[v]);
        }
    }
}

// Function to find dominant points in the graph
vector<pair<int, int>> findDominantPoints(vector<vector<int>>& graph) {
    vector<pair<int, int>> dominantPoints;
    int n = graph.size();

    for (int u = 0; u < n; u++) {
        for (int v : graph[u]) {
            bool isDominant = true;

            for (int w : graph[v]) {
                if (w != u && find(graph[u].begin(), graph[u].end(), w) != graph[u].end()) {
                    isDominant = false;
                    break;
                }
            }

            if (isDominant)
                dominantPoints.push_back({u, v});
        }
    }

    return dominantPoints;
}

// Function to find a safe path from the door to the exit
vector<pair<int, int>> findSafePath(vector<vector<char>>& maze, pair<int, int> floodGate, vector<pair<int, int>> covers) {
    int n = maze.size(), m = maze[0].size();
    vector<vector<bool>> visited(n, vector<bool>(m, false));
    queue<pair<int, int>> q;
    vector<pair<int, int>> path;

    // Block water flow from manholes
    for (auto cover : covers) {
        int i = cover.first, j = cover.second;
        maze[i][j] = '#';
    }

    // Block water flow across the flood gate
    int r1 = floodGate.first, c1 = floodGate.second;
    int r2, c2;
    for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
            if (abs(di) + abs(dj) == 1) {
                r2 = r1 + di, c2 = c1 + dj;
                if (r2 >= 0 && r2 < n && c2 >= 0 && c2 < m && maze[r2][c2] != '#') {
                    maze[r1][c1] = '#';
                    maze[r2][c2] = '#';
                    break;
                }
            }
        }
    }

    // Find the door and exit positions
    pair<int, int> door, exit;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (maze[i][j] == 'D')
                door = {i, j};
            else if (maze[i][j] == 'E')
                exit = {i, j};
        }
    }

    // BFS to find the safe path
    q.push(door);
    visited[door.first][door.second] = true;
    path.push_back(door);

    while (!q.empty()) {
        int r = q.front().first, c = q.front().second;
        q.pop();

        if (r == exit.first && c == exit.second) {
            path.push_back(exit);
            break;
        }

        int di[] = {-1, 0, 1, 0};
        int dj[] = {0, 1, 0, -1};

        for (int k = 0; k < 4; k++) {
            int ni = r + di[k], nj = c + dj[k];
            if (ni >= 0 && ni < n && nj >= 0 && nj < m && !visited[ni][nj] && maze[ni][nj] != '#') {
                visited[ni][nj] = true;
                q.push({ni, nj});
                path.push_back({ni, nj});
            }
        }
    }

    return path;
}

vector<int> findArticulationPoints(vector<vector<int>>& graph, int start) {
    int n = graph.size();
    vector<int> disc(n, -1), low(n, -1), parent(n, -1), ap;
    int time = 0;

    for (int i = 0; i < n; i++) {
        if (disc[i] == -1)
            dfs(i, start, time, disc, low, parent, ap, graph);
    }

    return ap;
}

vector<pair<int, int>> findBridges(vector<vector<int>>& graph) {
    int n = graph.size();
    vector<int> disc(n, -1), low(n, -1), parent(n, -1);
    vector<pair<int, int>> bridges;
    int time = 0;

    for (int i = 0; i < n; i++) {
        if (disc[i] == -1)
            dfs(i, -1, time, disc, low, parent, bridges, graph);
    }

    return bridges;
}

int main() {
    int T;
    cin >> T;

    while (T--) {
        int n, m;
        cin >> n >> m;

        vector<vector<char>> maze(n, vector<char>(m));
        vector<pair<int, int>> manholes;

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                cin >> maze[i][j];
                if (maze[i][j] == 'M')
                    manholes.push_back({i, j});
            }
        }

        int C;
        cin >> C;

        // Build the graph representation of the maze
        vector<vector<int>> graph(n * m);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                if (maze[i][j] != '#') {
                    int u = i * m + j;
                    if (i > 0 && maze[i - 1][j] != '#')
                        graph[u].push_back((i - 1) * m + j);
                    if (i < n - 1 && maze[i + 1][j] != '#')
                        graph[u].push_back((i + 1) * m + j);
                    if (j > 0 && maze[i][j - 1] != '#')
                        graph[u].push_back(i * m + j - 1);
                    if (j < m - 1 && maze[i][j + 1] != '#')
                        graph[u].push_back(i * m + j + 1);
                }
            }
        }

        // Find the articulation points and bridges
        vector<int> ap = findArticulationPoints(graph, 0);
        vector<pair<int, int>> bridges = findBridges(graph);

        // Find the dominant points
        vector<pair<int, int>> dominantPoints = findDominantPoints(graph);

        // Choose the flood gate location
        pair<int, int> floodGate;
        if (!bridges.empty())
            floodGate = bridges[0];
        else if (!dominantPoints.empty())
            floodGate = dominantPoints[0];
        else
            floodGate = {-1, -1};

        // Choose the manhole cover locations
        vector<pair<int, int>> covers;
        for (int i = 0; i < C && !manholes.empty(); i++) {
            covers.push_back(manholes.back());
            manholes.pop_back();
        }

        // Find the safe path
        vector<pair<int, int>> path = findSafePath(maze, floodGate, covers);

        // Print the output
        cout << floodGate.first << " " << floodGate.second << " " << floodGate.second + (floodGate.first - path[0].first) << " " << floodGate.second + (floodGate.first - path[0].first) << endl;
        cout << covers.size() << endl;
        for (auto cover : covers)
            cout << cover.first << " " << cover.second << endl;
        cout << path.size() << endl;
        for (auto p : path)
            cout << p.first << " " << p.second << endl;
    }

    return 0;
}