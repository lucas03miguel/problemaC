#include <iostream>
#include <vector>
#include <deque>
#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <iterator>
#include <limits>
#include <functional>
#include <stack>

#include <chrono>

using namespace std;

const vector<pair<int, int>> DIRECTIONS = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

stack<pair<int, int>> bfs_path(const vector<vector<char>>& maze, pair<int, int> start, pair<int, int> goal, const vector<vector<bool>>& water_filled) {
    int n = maze.size();
    int m = maze[0].size();
    deque<pair<int, int>> queue = {start};
    map<pair<int, int>, pair<int, int>> came_from = {{start, {-1, -1}}};

    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop_front();

        if (current == goal) {
            //vector<pair<int, int>> path;
            stack<pair<int, int>> stk; 
            while (current != make_pair(-1, -1)) {
                stk.push(current);
                current = came_from[current];
            }
            //reverse(path.begin(), path.end());
            return stk;
        }

        for (const auto& direction : DIRECTIONS) {
            int nx = current.first + direction.first;
            int ny = current.second + direction.second;
            if (nx >= 0 && nx < n && ny >= 0 && ny < m && maze[nx][ny] != '#' && !water_filled[nx][ny]) {
                pair<int, int> neighbor = {nx, ny};
                if (came_from.find(neighbor) == came_from.end()) {
                    queue.push_back(neighbor);
                    came_from[neighbor] = current;
                }
            }
        }
    }

    return {};
}

vector<vector<bool>> flood_simulation(const vector<vector<char>>& grid, const vector<pair<int, int>>& start_points, const pair<pair<int, int>, pair<int, int>>& block = {{-1, -1}, {-1, -1}}) {
    int rows = grid.size();
    int columns = grid[0].size();
    vector<vector<bool>> visited(rows, vector<bool>(columns, false));
    deque<pair<int, int>> queue;

    for (const auto& p : start_points) {
        visited[p.first][p.second] = true;
        queue.push_back(p);
    }

    int i = 0;
    while (i < (int)queue.size()) {
        auto [x, y] = queue[i++];
        for (const auto& direction : DIRECTIONS) {
            int nx = x + direction.first;
            int ny = y + direction.second;
            if (nx >= 0 && nx < rows && ny >= 0 && ny < columns && (grid[nx][ny] == '.' || grid[nx][ny] == 'M') && !visited[nx][ny]) {
                if (block != make_pair(make_pair(x, y), make_pair(nx, ny)) && block != make_pair(make_pair(nx, ny), make_pair(x, y))) {
                    visited[nx][ny] = true;
                    queue.push_back({nx, ny});
                }
            }
        }
    }

    return visited;
}

vector<pair<pair<int, int>, pair<int, int>>> find_bridges(const vector<vector<char>>& grid) {
    int rows = grid.size();
    int columns = grid[0].size();
    vector<vector<bool>> visited(rows, vector<bool>(columns, false));
    vector<vector<int>> disc(rows, vector<int>(columns, numeric_limits<int>::max()));
    vector<vector<int>> low(rows, vector<int>(columns, numeric_limits<int>::max()));
    vector<vector<pair<int, int>>> parent(rows, vector<pair<int, int>>(columns, {-1, -1}));
    vector<pair<pair<int, int>, pair<int, int>>> bridges;
    int time = 0;

    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            if ((grid[i][j] == 'M' || grid[i][j] == '.') && !visited[i][j]) {
                stack<tuple<int, int, bool>> stk; 
                stk.push({i, j, false});

                while (!stk.empty()) {
                    auto [x, y, is_post_visit] = stk.top();
                    stk.pop();

                    if (is_post_visit) {
                        for (const auto& direction : DIRECTIONS) {
                            int nx = x + direction.first;
                            int ny = y + direction.second;
                            if (nx >= 0 && nx < rows && ny >= 0 && ny < columns && (grid[nx][ny] == '.' || grid[nx][ny] == 'M')) {
                                if (parent[nx][ny] == make_pair(x, y)) {
                                    low[x][y] = min(low[x][y], low[nx][ny]);
                                    if (low[nx][ny] > disc[x][y]) {
                                        bridges.push_back({{x, y}, {nx, ny}});
                                    }
                                } else if (make_pair(nx, ny) != parent[x][y]) {
                                    low[x][y] = min(low[x][y], disc[nx][ny]);
                                }
                            }
                        }
                    } else {
                        visited[x][y] = true;
                        disc[x][y] = low[x][y] = ++time;

                        stk.push({x, y, true});

                        for (const auto& direction : DIRECTIONS) {
                            int nx = x + direction.first;
                            int ny = y + direction.second;
                            if (nx >= 0 && nx < rows && ny >= 0 && ny < columns && (grid[nx][ny] == '.' || grid[nx][ny] == 'M')) {
                                if (!visited[nx][ny]) {
                                    parent[nx][ny] = {x, y};
                                    stk.push({nx, ny, false});
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

vector<pair<int, int>> select_manhole_covers(const vector<vector<char>>& grid, int num_covers) {
    int rows = grid.size();
    int columns = grid[0].size();
    vector<pair<int, int>> manholes;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < columns; ++c) {
            if (grid[r][c] == 'M') {
                manholes.push_back({r, c});
            }
        }
    }

    vector<pair<int, int>> covers;
    vector<vector<bool>> water_filled(rows, vector<bool>(columns, false));

    while ((int)covers.size() < num_covers && !manholes.empty()) {
        int max_flooded = 0;
        pair<int, int> best_manhole = {-1, -1};

        for (const auto& manhole : manholes) {
            vector<vector<bool>> current_water_filled = water_filled;
            current_water_filled[manhole.first][manhole.second] = true;
            int flooded_count = count_if(current_water_filled.begin(), current_water_filled.end(), [](const vector<bool>& row) {
                return count(row.begin(), row.end(), true);
            });

            if (flooded_count > max_flooded) {
                max_flooded = flooded_count;
                best_manhole = manhole;
            }
        }

        if (max_flooded > 0) {
            covers.push_back(best_manhole);
            water_filled[best_manhole.first][best_manhole.second] = true;
            manholes.erase(remove(manholes.begin(), manholes.end(), best_manhole), manholes.end());
        } else {
            break;
        }
    }

    return covers;
}

pair<pair<int, int>, pair<int, int>> find_dominating_set_for_flood_control(const vector<vector<char>>& grid, const vector<pair<int, int>>& manholes, const vector<pair<pair<int, int>, pair<int, int>>>& bridges) {
    vector<vector<bool>> initial_flooded_areas = flood_simulation(grid, manholes);
    pair<pair<int, int>, pair<int, int>> best_bridge = {{-1, -1}, {-1, -1}};
    int initial_count = count_if(initial_flooded_areas.begin(), initial_flooded_areas.end(), [](const vector<bool>& row) {
        return count(row.begin(), row.end(), true);
    });

    for (const auto& bridge : bridges) {
        vector<vector<bool>> blocked_flooded_areas = flood_simulation(grid, manholes, bridge);
        int flooded_count = count_if(blocked_flooded_areas.begin(), blocked_flooded_areas.end(), [](const vector<bool>& row) {
            return std::count(row.begin(), row.end(), true);
        });

        if (flooded_count < initial_count) {
            initial_count = flooded_count;
            best_bridge = bridge;
        }
    }

    return best_bridge;
}

int main() {

    int num_cases;
    cin >> num_cases;

    for (int i = 0; i < num_cases; ++i) {
        int n, m;
        cin >> n >> m;
        vector<vector<char>> maze(n, vector<char>(m));
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < m; ++c) {
                cin >> maze[r][c];
            }
        }

        int num_covers;
        cin >> num_covers;

        pair<int, int> door, exit;
        vector<pair<int, int>> manholes;
        
        auto start_time = chrono::high_resolution_clock::now();
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < m; ++c) {
                if (maze[r][c] == 'D') {
                    door = {r, c};
                } else if (maze[r][c] == 'E') {
                    exit = {r, c};
                } else if (maze[r][c] == 'M') {
                    manholes.push_back({r, c});
                }
            }
        }
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
        cout << "Tempo de execução: " << duration.count() << " ms" << endl;
        

        vector<pair<pair<int, int>, pair<int, int>>> bridges = find_bridges(maze);
        vector<tuple<pair<pair<int, int>, pair<int, int>>, vector<pair<int, int>>, stack<pair<int, int>>>> solutions;

        vector<pair<int, int>> cover_combination = select_manhole_covers(maze, num_covers);
        vector<pair<int, int>> remaining_manholes;
        for (const auto& manhole : manholes) {
            if (find(cover_combination.begin(), cover_combination.end(), manhole) == cover_combination.end()) {
                remaining_manholes.push_back(manhole);
            }
        }

        pair<pair<int, int>, pair<int, int>> dominating_set = find_dominating_set_for_flood_control(maze, remaining_manholes, bridges);
        vector<vector<bool>> water_filled = flood_simulation(maze, remaining_manholes, dominating_set);
        stack<pair<int, int>> path = bfs_path(maze, door, exit, water_filled);

        if (!path.empty()) {
            solutions.emplace_back(dominating_set, cover_combination, path);
        }

        if (!solutions.empty()) {
            auto [flood_gate, covers, path] = *min_element(solutions.begin(), solutions.end(), [](const auto& lhs, const auto& rhs) {
                return get<2>(lhs).size() < get<2>(rhs).size();
            });

            cout << flood_gate.first.first << " " << flood_gate.first.second << " " << flood_gate.second.first << " " << flood_gate.second.second << "\n";
            cout << covers.size() << "\n";
            for (const auto& cover : covers) {
                cout << cover.first << " " << cover.second << "\n";
            }
            cout << path.size() << "\n";
            while (path.size() > 0) {
                auto current = path.top();
                cout << current.first << " " << current.second << "\n";
                path.pop();
            }
            //auto current = path.top();
            //cout << current.first << " " << current.second << "\n";
            
        }
        
    }


    return 0;
}
