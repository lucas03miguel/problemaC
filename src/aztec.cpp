#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>

using namespace std;

const vector<pair<int, int>> DIRECTIONS = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

stack<pair<int, int>> bfs_path(const vector<vector<char>>& maze, pair<int, int> start, pair<int, int> goal, const vector<vector<bool>>& water_filled) {
    int n = maze.size();
    int m = maze[0].size();
    queue<pair<int, int>> queue;
    vector<vector<pair<int, int>>> came_from(n, vector<pair<int, int>>(m, {-1, -1}));

    queue.push(start);
    came_from[start.first][start.second] = {-2, -2};

    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();

        if (current == goal) {
            stack<pair<int, int>> path;
            while (current != make_pair(-2, -2)) {
                path.push(current);
                current = came_from[current.first][current.second];
            }
            return path;
        }

        for (const auto& direction : DIRECTIONS) {
            int nx = current.first + direction.first;
            int ny = current.second + direction.second;

            if (nx >= 0 && nx < n && ny >= 0 && ny < m && maze[nx][ny] != '#' && !water_filled[nx][ny] && came_from[nx][ny] == make_pair(-1, -1)) {
                queue.push({nx, ny});
                came_from[nx][ny] = current;
            }
        }
    }

    return {};
}

vector<pair<pair<int, int>, pair<int, int>>> find_bridges(const vector<vector<char>>& grid) {
    int rows = grid.size();
    int columns = grid[0].size();
    vector<vector<bool>> visited(rows, vector<bool>(columns, false));
    vector<vector<int>> disc(rows, vector<int>(columns, 0));
    vector<vector<int>> low(rows, vector<int>(columns, 0));
    vector<pair<pair<int, int>, pair<int, int>>> bridges;
    int time = 0;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            if ((grid[i][j] == 'M' || grid[i][j] == '.') && !visited[i][j]) {

                stack<pair<pair<int, int>, pair<int, int>>> stk;
                stk.push({{i, j}, {-1, -1}});

                while (!stk.empty()) {
                    auto [current, parent] = stk.top();
                    int x = current.first;
                    int y = current.second;

                    if (!visited[x][y]) {
                        visited[x][y] = true;
                        disc[x][y] = low[x][y] = ++time;
                    }

                    bool found_unvisited_child = false;
                    for (const auto& direction : DIRECTIONS) {
                        int nx = x + direction.first;
                        int ny = y + direction.second;

                        if (nx >= 0 && nx < rows && ny >= 0 && ny < columns && (grid[nx][ny] == '.' || grid[nx][ny] == 'M')) {
                            if (!visited[nx][ny]) {
                                stk.push({{nx, ny}, {x, y}});
                                found_unvisited_child = true;
                                break;
                            } else if (make_pair(nx, ny) != parent) {
                                low[x][y] = min(low[x][y], disc[nx][ny]);
                            }
                        }
                    }

                    if (!found_unvisited_child) {
                        if (parent.first != -1 && low[x][y] > disc[parent.first][parent.second]) {
                            bridges.push_back({{parent.first, parent.second}, {x, y}});
                        }
                        if (parent.first != -1) {
                            low[parent.first][parent.second] = min(low[parent.first][parent.second], low[x][y]);
                        }
                        stk.pop();
                    }
                }
            }
        }
    }

    return bridges;
}

vector<vector<bool>> flood_simulation(const vector<vector<char>>& grid, const vector<pair<int, int>>& start_points, const pair<pair<int, int>, pair<int, int>>& block = {{-1, -1}, {-1, -1}}) {
    int rows = grid.size();
    int columns = grid[0].size();
    vector<vector<bool>> visited(rows, vector<bool>(columns, false));
    queue<pair<int, int>> queue;

    for (const auto& p : start_points) {
        visited[p.first][p.second] = true;
        queue.push(p);
    }

    while (!queue.empty()) {
        auto [x, y] = queue.front();
        queue.pop();

        for (const auto& direction : DIRECTIONS) {
            int nx = x + direction.first;
            int ny = y + direction.second;
            if (nx >= 0 && nx < rows && ny >= 0 && ny < columns && (grid[nx][ny] == '.' || grid[nx][ny] == 'M') && !visited[nx][ny]) {
                if (block != make_pair(make_pair(x, y), make_pair(nx, ny)) && block != make_pair(make_pair(nx, ny), make_pair(x, y))) {
                    visited[nx][ny] = true;
                    queue.push({nx, ny});
                }
            }
        }
    }

    return visited;
}


vector<pair<int, int>> select_manhole_covers(const vector<vector<char>>& grid, int num_covers, vector<pair<int, int>> manholes) {
    int rows = grid.size();
    int columns = grid[0].size();

    vector<pair<int, int>> covers;
    vector<vector<bool>> water_filled(rows, vector<bool>(columns, false));

    while ((int)covers.size() < num_covers && !manholes.empty()) {
        int max_flooded = 0;
        pair<int, int> best_manhole = {-1, -1};

        for (const auto& manhole : manholes) {
            if (water_filled[manhole.first][manhole.second]) {
                continue;
            }

            vector<vector<bool>> current_water_filled = flood_simulation(grid, {manhole});
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

    while (true) {
        int max_reduction = 0;
        pair<pair<int, int>, pair<int, int>> current_best_bridge = {{-1, -1}, {-1, -1}};

        for (const auto& bridge : bridges) {
            vector<vector<bool>> blocked_flooded_areas = flood_simulation(grid, manholes, bridge);
            int flooded_count = count_if(blocked_flooded_areas.begin(), blocked_flooded_areas.end(), [](const vector<bool>& row) {
                return count(row.begin(), row.end(), true);
            });

            int reduction = initial_count - flooded_count;
            if (reduction > max_reduction) {
                max_reduction = reduction;
                current_best_bridge = bridge;
            }
        }

        if (max_reduction > 0) {
            best_bridge = current_best_bridge;
            initial_count -= max_reduction;
        } else {
            break;
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

        for (int r = 0; r < n; ++r) 
            for (int c = 0; c < m; ++c)
                cin >> maze[r][c];
        
        int num_covers;
        cin >> num_covers;

        pair<int, int> door, exit;
        vector<pair<int, int>> manholes;

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

        vector<pair<pair<int, int>, pair<int, int>>> bridges = find_bridges(maze);
        vector<pair<int, int>> cover_combination = select_manhole_covers(maze, num_covers, manholes);
        
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
            printf("%d %d %d %d\n", dominating_set.first.first, dominating_set.first.second, dominating_set.second.first, dominating_set.second.second);
            printf("%d\n", (int)cover_combination.size());
            for (const auto& cover : cover_combination) {
                printf("%d %d\n", cover.first, cover.second);
            }
            printf("%d\n", (int)path.size());
            while (!path.empty()) {
                auto current = path.top();
                printf("%d %d\n", current.first, current.second);
                path.pop();
            }
        }
        
    }
    return 0;
}