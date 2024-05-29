from collections import deque
import random
from sys import stdin, stdout
from itertools import combinations

DIRECTIONS = [(-1, 0), (1, 0), (0, -1), (0, 1)]


def readln():
    return stdin.readline().rstrip()

def outln(n):
    stdout.write(str(n) + '\n')


def dfs_bridges_iterative(grid, sx, sy, ex, ey):
    rows, columns = len(grid), len(grid[0])
    visited = [[False] * columns for _ in range(rows)]
    dfn = [[float('inf')] * columns for _ in range(rows)]
    low = [[float('inf')] * columns for _ in range(rows)]
    parent = [[None] * columns for _ in range(rows)]
    bridges = []
    stack = []
    time = [0]

    for i in range(rows):
        for j in range(columns):
            if grid[i][j] != '#' and not visited[i][j]:
                stack.append((i, j, None))

                while stack:
                    x, y, state = stack.pop()

                    if state is None:
                        visited[x][y] = True
                        dfn[x][y] = low[x][y] = time[0]
                        time[0] += 1
                        stack.append((x, y, "post"))

                        for dx, dy in DIRECTIONS:
                            nx, ny = x + dx, y + dy
                            if 0 <= nx < rows and 0 <= ny < columns and grid[nx][ny] != '#' and not visited[nx][ny]:
                                stack.append((nx, ny, None))
                                parent[nx][ny] = (x, y)
                            elif (nx, ny) != parent[x][y] and 0 <= nx < rows and 0 <= ny < columns:
                                low[x][y] = min(low[x][y], dfn[nx][ny])
                    else:
                        for dx, dy in DIRECTIONS:
                            nx, ny = x + dx, y + dy
                            if 0 <= nx < rows and 0 <= ny < columns and grid[nx][ny] != '#':
                                if parent[nx][ny] == (x, y):
                                    low[x][y] = min(low[x][y], low[nx][ny])

                                    if low[nx][ny] > dfn[x][y]:
                                        if (x, y) < (nx, ny):
                                            bridges.append(((x, y), (nx, ny)))
                                        elif (x, y) >= (nx, ny):
                                            bridges.append(((nx, ny), (x, y)))

                                elif (nx, ny) != parent[x][y]:
                                    low[x][y] = min(low[x][y], dfn[nx][ny])

    return bridges


def bfs_safe_path(maze, sx, sy, ex, ey, water_filled):
    N, M = len(maze), len(maze[0])
    queue = deque([(sx, sy)])
    visited = [[False] * M for _ in range(N)]
    visited[sx][sy] = True
    parent = [[None] * M for _ in range(N)]
    came_from = {(sx, sy): (-1, -1)}

    while queue:
        x, y = queue.popleft()
        if x == ex and y == ey:
            path = []
            while (x, y) != (-1, -1):
                path.append((x, y))
                x, y = came_from[(x, y)]
            
            path.reverse()
            return path
        
        for dx, dy in DIRECTIONS:
            nx, ny = x + dx, y + dy
            if 0 <= nx < N and 0 <= ny < M and maze[nx][ny] != '#' and not water_filled[nx][ny] and not visited[nx][ny]:
                visited[nx][ny] = True
                queue.append((nx, ny))
                came_from[(nx, ny)] = (x, y)
    return []


flood_control_cache = {}
def evaluate_manhole_cover(maze, manholes, bridges):
    cache_key = (tuple(manholes), tuple(bridges))
    if cache_key in flood_control_cache:
        return flood_control_cache[cache_key]

    initial_flooded_areas = bfs_flood_control(maze, manholes)
    count_initial = sum(sum(row) for row in initial_flooded_areas)

    bridge_best = None
    for bridge in bridges:
        flood = bfs_flood_control(maze, manholes, bridge)
        count = sum(sum(row) for row in flood)

        if count < count_initial:
            count_initial = count
            bridge_best = bridge
            
    flood_control_cache[cache_key] = bridge_best
    return flood_control_cache[cache_key]


def bfs_flood_control(grid, manholes, block=None):
    rows, columns = len(grid), len(grid[0])
    visited = [[False] * columns for _ in range(rows)]
    queue = deque(manholes)
       
    for x, y in manholes:
        visited[x][y] = True

    while queue:
        x, y = queue.popleft()

        for dx, dy in DIRECTIONS:
            nx, ny = x + dx, y + dy
            if (
                0 <= nx < rows
                and 0 <= ny < columns
                and grid[nx][ny] != '#'
                and not visited[nx][ny]
                and (block is None or not is_blocked(x, y, nx, ny, block))
            ):
                visited[nx][ny] = True
                queue.append((nx, ny))

    return visited


def is_blocked(x1, y1, x2, y2, block):
    return (
        ((x1, y1) == block[0] and (x2, y2) == block[1])
        or ((x1, y1) == block[1] and (x2, y2) == block[0])
    )


"""
def select_effective_covers(manhole_effectiveness):
    best_combination = max(manhole_effectiveness, key=manhole_effectiveness.get)
    return list(best_combination)


def valid_flood_gate(maze, N, M, sx, sy, ex, ey, bridge):
    (bx1, by1), (bx2, by2) = bridge
    maze[bx1][by1] = '#'
    maze[bx2][by2] = '#'
    
    path_exists = bfs_safe_path(maze, N, M, sx, sy, ex, ey) != []

    maze[bx1][by1] = '.'
    maze[bx2][by2] = '.'
    
    return path_exists
"""


def main():
    num_cases = int(readln())
    for _ in range(num_cases):
        N, M = map(int, readln().split())
        maze = [list(readln()) for _ in range(N)]
        C = int(readln())

        ex, ey = [(x, y) for x in range(N) for y in range(M) if maze[x][y] == 'E'][0]
        dx, dy = [(x, y) for x in range(N) for y in range(M) if maze[x][y] == 'D'][0]
        manholes = [(x, y) for x in range(N) for y in range(M) if maze[x][y] == 'M']
        manhole_indices = list(range(len(manholes)))

        bridges = dfs_bridges_iterative(maze, dx, dy, ex, ey)
        
        solution = None
        for cover_combination in combinations(manhole_indices, C):
            remaining_manholes = [manholes[i] for i in set(manhole_indices) - set(cover_combination)]
            flood_gate = evaluate_manhole_cover(maze, remaining_manholes, bridges)
            water_filled = bfs_flood_control(maze, remaining_manholes, flood_gate)

            path = bfs_safe_path(maze, dx, dy, ex, ey, water_filled)
            if path:
                covers = [manholes[i] for i in cover_combination]
                solution = (flood_gate, covers, path)
                

        flood_gate, covers, path = solution
        outln(f"{flood_gate[0][0]} {flood_gate[0][1]} {flood_gate[1][0]} {flood_gate[1][1]}")
        outln(f"{len(covers)}")
        for cover in covers:
            outln(f"{cover[0]} {cover[1]}")
        outln(f"{len(path)}")
        for step in path:
            outln(f"{step[0]} {step[1]}")


if __name__ == "__main__":
    main()