#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <queue>

using namespace std;

struct Edge {
    int u, v;
};

void findBridges(int n, const vector<vector<int>>& adj, vector<Edge>& bridges) {
    vector<int> disc(n, -1), low(n, -1), parent(n, -1);
    stack<pair<int, int>> s;
    int time = 0;

    for (int u = 0; u < n; ++u) {
        if (disc[u] == -1) {
            s.push({u, -1});
            while (!s.empty()) {
                int v = s.top().first, p = s.top().second;
                s.pop();
                if (disc[v] == -1) {
                    disc[v] = low[v] = time++;
                    parent[v] = p;
                    for (int w : adj[v]) {
                        if (disc[w] == -1) {
                            s.push({v, p});
                            s.push({w, v});
                        } else if (w != p) {
                            low[v] = min(low[v], disc[w]);
                        }
                    }
                } else {
                    if (p != -1) low[p] = min(low[p], low[v]);
                    if (low[v] > disc[p]) bridges.push_back({p, v});
                }
            }
        }
    }
}

vector<int> bfs(int start, int n, const vector<vector<int>>& adj, const vector<vector<bool>>& blocked) {
    vector<int> prev(n, -1);
    queue<int> q;
    q.push(start);
    prev[start] = start;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v : adj[u]) {
            if (prev[v] == -1 && !blocked[u][v] && !blocked[v][u]) {
                q.push(v);
                prev[v] = u;
                if (v == 1) return prev;  // Exit found
            }
        }
    }
    return prev;
}

int main() {
    int t;  // number of test cases
    cin >> t;
    while (t--) {
        int N, M;
        cin >> N >> M;
        vector<string> maze(N);
        for (int i = 0; i < N; ++i) {
            cin >> maze[i];
        }
        int C;
        cin >> C;

        int start, exit;
        vector<int> manholes;
        vector<vector<int>> adj(N * M);

        auto index = [&](int r, int c) { return r * M + c; };
        auto coords = [&](int i) { return make_pair(i / M, i % M); };

        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < M; ++c) {
                if (maze[r][c] == '#') continue;
                int u = index(r, c);
                if (maze[r][c] == 'D') start = u;
                if (maze[r][c] == 'E') exit = u;
                if (maze[r][c] == 'M') manholes.push_back(u);
                if (r > 0 && maze[r-1][c] != '#') adj[u].push_back(index(r-1, c));
                if (r < N-1 && maze[r+1][c] != '#') adj[u].push_back(index(r+1, c));
                if (c > 0 && maze[r][c-1] != '#') adj[u].push_back(index(r, c-1));
                if (c < M-1 && maze[r][c+1] != '#') adj[u].push_back(index(r, c+1));
            }
        }

        vector<Edge> bridges;
        findBridges(N * M, adj, bridges);

        vector<vector<bool>> blocked(N * M, vector<bool>(N * M, false));
        for (auto& bridge : bridges) {
            blocked[bridge.u][bridge.v] = blocked[bridge.v][bridge.u] = true;
            vector<int> path = bfs(start, N * M, adj, blocked);
            if (path[exit] != -1) {
                auto [r1, c1] = coords(bridge.u);
                auto [r2, c2] = coords(bridge.v);
                cout << r1 << " " << c1 << " " << r2 << " " << c2 << endl;
                break;
            }
            blocked[bridge.u][bridge.v] = blocked[bridge.v][bridge.u] = false;
        }

        vector<pair<int, int>> covers;
        vector<int> path = bfs(start, N * M, adj, blocked);
        vector<bool> water(N * M, false);
        queue<int> q;
        for (int u : manholes) {
            if (!water[u]) q.push(u);
            while (!q.empty()) {
                int v = q.front();
                q.pop();
                water[v] = true;
                for (int w : adj[v]) {
                    if (!blocked[v][w] && !water[w]) q.push(w);
                }
            }
        }

        for (int u : manholes) {
            if (C == 0) break;
            if (water[u]) {
                covers.emplace_back(coords(u));
                C--;
                for (int v : adj[u]) {
                    blocked[u][v] = blocked[v][u] = true;
                }
            }
        }

        cout << covers.size() << endl;
        for (auto& cover : covers) {
            cout << cover.first << " " << cover.second << endl;
        }

        path = bfs(start, N * M, adj, blocked);
        vector<pair<int, int>> safe_path;
        for (int u = exit; u != start; u = path[u]) {
            safe_path.push_back(coords(u));
        }
        safe_path.push_back(coords(start));
        reverse(safe_path.begin(), safe_path.end());

        cout << safe_path.size() << endl;
        for (auto& p : safe_path) {
            cout << p.first << " " << p.second << endl;
        }
    }
    return 0;
}
