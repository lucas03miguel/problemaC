#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <tuple>
#include <set>
#include <map>
#include <sstream>
#include <ctime>
#include <climits>

using namespace std;

const vector<pair<int, int>> DIRECTIONS = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

vector<string> read_maze(int N, int M) {
    vector<string> maze(N);
    for (int i = 0; i < N; ++i) {
        cin >> maze[i];
    }
    return maze;
}

bool valid_flood_gate(vector<string>& maze, int N, int M, int sx, int sy, int ex, int ey, pair<pair<int, int>, pair<int, int>> bridge);

vector<pair<pair<int, int>, pair<int, int>>> dfs_bridges_iterative(vector<string>& grid, int sx, int sy, int ex, int ey) {
    int rows = grid.size();
    int columns = grid[0].size();
    vector<vector<bool>> visited(rows, vector<bool>(columns, false));
    vector<vector<int>> disc(rows, vector<int>(columns, INT_MAX));
    vector<vector<int>> low(rows, vector<int>(columns, INT_MAX));
    vector<vector<pair<int, int>>> parent(rows, vector<pair<int, int>>(columns, {-1, -1}));
    vector<pair<pair<int, int>, pair<int, int>>> bridges;
    stack<tuple<int, int, int>> st;
    int time = 0;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            if ((grid[i][j] == 'M' || grid[i][j] == '.') && !visited[i][j]) {
                st.push({i, j, 0});

                while (!st.empty()) {
                    auto [x, y, state] = st.top();
                    st.pop();

                    if (state == 0) {
                        visited[x][y] = true;
                        disc[x][y] = low[x][y] = time++;
                        st.push({x, y, 1});

                        for (auto [dx, dy] : DIRECTIONS) {
                            int nx = x + dx, ny = y + dy;
                            if (nx >= 0 && nx < rows && ny >= 0 && ny < columns && (grid[nx][ny] == '.' || grid[nx][ny] == 'M')) {
                                if (!visited[nx][ny]) {
                                    st.push({nx, ny, 0});
                                    parent[nx][ny] = {x, y};
                                } else if (make_pair(nx, ny) != parent[x][y]) {
                                    low[x][y] = min(low[x][y], disc[nx][ny]);
                                }
                            }
                        }
                    } else {
                        for (auto [dx, dy] : DIRECTIONS) {
                            int nx = x + dx, ny = y + dy;
                            if (nx >= 0 && nx < rows && ny >= 0 && ny < columns && (grid[nx][ny] == '.' || grid[nx][ny] == 'M')) {
                                if (parent[nx][ny] == make_pair(x, y)) {
                                    low[x][y] = min(low[x][y], low[nx][ny]);
                                    if (low[nx][ny] > disc[x][y]) {
                                        pair<int, int> p1 = {x, y};
                                        pair<int, int> p2 = {nx, ny};
                                        if (p1 < p2 && valid_flood_gate(grid, rows, columns, sx, sy, ex, ey, {p1, p2})) {
                                            bridges.push_back({p1, p2});
                                        } else if (p1 >= p2 && valid_flood_gate(grid, rows, columns, sx, sy, ex, ey, {p2, p1})) {
                                            bridges.push_back({p2, p1});
                                        }
                                    }
                                } else if (make_pair(nx, ny) != parent[x][y]) {
                                    low[x][y] = min(low[x][y], disc[nx][ny]);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return bridges;
}

vector<pair<int, int>> bfs_safe_path(vector<string>& maze, int N, int M, int sx, int sy, int ex, int ey) {
    queue<pair<int, int>> q;
    vector<vector<bool>> visited(N, vector<bool>(M, false));
    vector<vector<pair<int, int>>> parent(N, vector<pair<int, int>>(M, {-1, -1}));
    q.push({sx, sy});
    visited[sx][sy] = true;

    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();
        if (x == ex && y == ey) {
            vector<pair<int, int>> path;
            while (parent[x][y] != make_pair(-1, -1)) {
                path.push_back({x, y});
                tie(x, y) = parent[x][y];
            }
            path.push_back({sx, sy});
            reverse(path.begin(), path.end());
            return path;
        }
        for (auto [dx, dy] : DIRECTIONS) {
            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && nx < N && ny >= 0 && ny < M && !visited[nx][ny] && maze[nx][ny] != '#') {
                visited[nx][ny] = true;
                parent[nx][ny] = {x, y};
                q.push({nx, ny});
            }
        }
    }
    return {};
}


unordered_map<tuple<set<pair<int, int>>, pair<pair<int, int>, pair<int, int>>>, vector<vector<bool>>> memo(1);

vector<vector<bool>> bfs_flood_control(vector<string>& grid, const set<pair<int, int>>& start_points, pair<pair<int, int>, pair<int, int>> flood_gate, const set<pair<int, int>>& block) {
    auto key = make_tuple(start_points, flood_gate, block);
    if (memo.find(key) != memo.end()) {
        return memo[key];
    }

    int rows = grid.size();
    int columns = grid[0].size();
    queue<pair<int, int>> q;
    vector<vector<bool>> visited(rows, vector<bool>(columns, false));

    for (auto& sp : start_points) {
        q.push(sp);
    }

    for (auto& b : block) {
        if (q.front() == b) {
            q.pop();
        }
    }

    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();

        for (auto [dx, dy] : DIRECTIONS) {
            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && nx < rows && ny >= 0 && ny < columns && !visited[nx][ny] && grid[nx][ny] != '#') {
                if (block.count({nx, ny}) && ((dx == 0 && (dy == -1 || dy == 1)) || (dy == 0 && (dx == -1 || dx == 1))) &&
                    make_pair(nx, ny) == flood_gate.first && make_pair(x, y) == flood_gate.second) {
                    continue;
                }
                visited[nx][ny] = true;
                q.push({nx, ny});
            }
        }
    }

    memo[key] = visited;
    return visited;
}

unordered_map<vector<pair<int, int>>, int> evaluate_manhole_cover_effectiveness(vector<string>& maze, int N, int M, vector<pair<int, int>>& manholes, int sx, int sy, pair<pair<int, int>, pair<int, int>> flood_gate, int C) {
    unordered_map<vector<pair<int, int>>, int> flood_effectiveness;
    set<pair<int, int>> manhole_set(manholes.begin(), manholes.end());

    auto base_flooded = bfs_flood_control(maze, manhole_set, flood_gate, {});

    auto combination = [](vector<pair<int, int>>& elements, int K) {
        vector<vector<pair<int, int>>> result;
        vector<bool> v(elements.size());
        fill(v.end() - K, v.end(), true);
        do {
            vector<pair<int, int>> combination;
            for (size_t i = 0; i < elements.size(); ++i) {
                if (v[i]) {
                    combination.push_back(elements[i]);
                }
            }
            result.push_back(combination);
        } while (next_permutation(v.begin(), v.end()));
        return result;
    };

    for (auto& comb : combination(manholes, C)) {
        set<pair<int, int>> comb_set(comb.begin(), comb.end());
        auto flood = bfs_flood_control(maze, manhole_set, flood_gate, comb_set);

        vector<pair<int, int>> filtered_comb;
        for (auto& m : comb) {
            if (!flood[m.first][m.second]) {
                filtered_comb.push_back(m);
            }
        }

        flood_effectiveness[filtered_comb] = 0;
        for (int x = 0; x < N; ++x) {
            for (int y = 0; y < M; ++y) {
                if (base_flooded[x][y] && !flood[x][y]) {
                    flood_effectiveness[filtered_comb]++;
                }
            }
        }
    }

    return flood_effectiveness;
}

vector<pair<int, int>> select_effective_covers(unordered_map<vector<pair<int, int>>, int>& manhole_effectiveness) {
    auto best_combination = max_element(manhole_effectiveness.begin(), manhole_effectiveness.end(),
                                        [](const auto& a, const auto& b) { return a.second < b.second; })->first;
    return best_combination;
}

bool valid_flood_gate(vector<string>& maze, int N, int M, int sx, int sy, int ex, int ey, pair<pair<int, int>, pair<int, int>> bridge) {
    auto [p1, p2] = bridge;
    maze[p1.first][p1.second] = '#';
    maze[p2.first][p2.second] = '#';

    bool path_exists = !bfs_safe_path(maze, N, M, sx, sy, ex, ey).empty();

    maze[p1.first][p1.second] = '.';
    maze[p2.first][p2.second] = '.';

    return path_exists;
}

int main() {
    int num_cases;
    cin >> num_cases;
    while (num_cases--) {
        int N, M;
        cin >> N >> M;
        auto maze = read_maze(N, M);
        int C;
        cin >> C;

        pair<int, int> ex, ey;
        pair<int, int> dx, dy;
        vector<pair<int, int>> manholes;

        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < M; ++j) {
                if (maze[i][j] == 'E') {
                    ex = {i, j};
                } else if (maze[i][j] == 'D') {
                    dx = {i, j};
                } else if (maze[i][j] == 'M') {
                    manholes.push_back({i, j});
                }
            }
        }

        auto bridges = dfs_bridges_iterative(maze, dx.first, dx.second, ex.first, ex.second);
        pair<pair<int, int>, pair<int, int>> flood_gate = bridges[rand() % bridges.size()];

        auto manhole_effectiveness = evaluate_manhole_cover_effectiveness(maze, N, M, manholes, dx.first, dx.second, flood_gate, C);
        auto selected_covers = select_effective_covers(manhole_effectiveness);

        cout << flood_gate.first.first << " " << flood_gate.first.second << " " << flood_gate.second.first << " " << flood_gate.second.second << endl;
        cout << selected_covers.size() << endl;
        for (auto& cover : selected_covers) {
            cout << cover.first << " " << cover.second << endl;
        }
        auto path = bfs_safe_path(maze, N, M, dx.first, dx.second, ex.first, ex.second);
        cout << path.size() << endl;
        for (auto& step : path) {
            cout << step.first << " " << step.second << endl;
        }
    }
    return 0;
}
