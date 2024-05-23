#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <tuple>
#include <algorithm>

using namespace std;

const vector<pair<int, int>> DIRECTIONS = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

struct Maze {
    vector<string> grid;
    pair<int, int> door;
    pair<int, int> exit;
    vector<pair<int, int>> manholes;
};

Maze read_maze(int n, int m) {
    Maze maze;
    maze.grid.resize(n);
    for (int i = 0; i < n; ++i) {
        cin >> maze.grid[i];
        for (int j = 0; j < m; ++j) {
            if (maze.grid[i][j] == 'D') {
                maze.door = {i, j};
            } else if (maze.grid[i][j] == 'E') {
                maze.exit = {i, j};
            } else if (maze.grid[i][j] == 'M') {
                maze.manholes.push_back({i, j});
            }
        }
    }
    return maze;
}

bool is_within_bounds(int n, int m, int x, int y) {
    return x >= 0 && x < n && y >= 0 && y < m;
}

set<pair<int, int>> simulate_water_flow(const vector<string> &maze, const set<pair<int, int>> &manholes, pair<pair<int, int>, pair<int, int>> flood_gate) {
    int n = maze.size(), m = maze[0].size();
    set<pair<int, int>> water_filled;
    queue<pair<int, int>> q;

    for (const auto &mh : manholes) {
        q.push(mh);
    }

    while (!q.empty()) {
        auto current = q.front();
        q.pop();
        if (water_filled.count(current)) continue;
        water_filled.insert(current);

        for (const auto &dir : DIRECTIONS) {
            int nx = current.first + dir.first;
            int ny = current.second + dir.second;

            if (!is_within_bounds(n, m, nx, ny)) continue;
            if (maze[nx][ny] == '#' || water_filled.count({nx, ny})) continue;
            if ((current == flood_gate.first && make_pair(nx, ny) == flood_gate.second) ||
                (current == flood_gate.second && make_pair(nx, ny) == flood_gate.first)) continue;

            q.push({nx, ny});
        }
    }

    return water_filled;
}

vector<pair<int, int>> bfs_path(const vector<string> &maze, pair<int, int> start, pair<int, int> goal, const set<pair<int, int>> &water_filled) {
    int n = maze.size(), m = maze[0].size();
    queue<pair<int, int>> q;
    map<pair<int, int>, pair<int, int>> came_from;
    q.push(start);
    came_from[start] = {-1, -1};

    while (!q.empty()) {
        auto current = q.front();
        q.pop();

        if (current == goal) {
            vector<pair<int, int>> path;
            while (current != make_pair(-1, -1)) {
                path.push_back(current);
                current = came_from[current];
            }
            reverse(path.begin(), path.end());
            return path;
        }

        for (const auto &dir : DIRECTIONS) {
            int nx = current.first + dir.first;
            int ny = current.second + dir.second;
            auto neighbor = make_pair(nx, ny);

            if (is_within_bounds(n, m, nx, ny) && maze[nx][ny] != '#' && !water_filled.count(neighbor) && !came_from.count(neighbor)) {
                q.push(neighbor);
                came_from[neighbor] = current;
            }
        }
    }

    return {};
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    int num_cases;
    cin >> num_cases;

    for (int case_idx = 0; case_idx < num_cases; ++case_idx) {
        int n, m;
        cin >> n >> m;

        Maze maze = read_maze(n, m);

        int num_covers;
        cin >> num_covers;

        set<pair<pair<int, int>, pair<int, int>>> possible_flood_gates;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                if (maze.grid[i][j] != '#') {
                    for (const auto &dir : DIRECTIONS) {
                        int ni = i + dir.first;
                        int nj = j + dir.second;
                        if (is_within_bounds(n, m, ni, nj) && maze.grid[ni][nj] != '#') {
                            possible_flood_gates.insert({{i, j}, {ni, nj}});
                        }
                    }
                }
            }
        }

        tuple<pair<int, int>, pair<int, int>, set<pair<int, int>>, vector<pair<int, int>>> solution;

        for (const auto &flood_gate : possible_flood_gates) {
            for (int mask = 0; mask < (1 << maze.manholes.size()); ++mask) {
                set<pair<int, int>> uncovered_manholes;
                set<pair<int, int>> covered_manholes;

                for (int i = 0; i < maze.manholes.size(); ++i) {
                    if (mask & (1 << i)) {
                        covered_manholes.insert(maze.manholes[i]);
                    } else {
                        uncovered_manholes.insert(maze.manholes[i]);
                    }
                }

                if (covered_manholes.size() > num_covers) continue;

                auto water_filled = simulate_water_flow(maze.grid, uncovered_manholes, flood_gate);
                auto path = bfs_path(maze.grid, maze.door, maze.exit, water_filled);

                if (!path.empty()) {
                    solution = {flood_gate.first, flood_gate.second, covered_manholes, path};
                    break;
                }
            }
        }

        auto [fg1, fg2, covers, path] = solution;
        cout << fg1.first << " " << fg1.second << " " << fg2.first << " " << fg2.second << "\n";
        cout << covers.size() << "\n";
        for (const auto &cover : covers) {
            cout << cover.first << " " << cover.second << "\n";
        }
        cout << path.size() << "\n";
        for (const auto &step : path) {
            cout << step.first << " " << step.second << "\n";
        }
    }

    return 0;
}
