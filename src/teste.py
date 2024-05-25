from collections import deque
import random
from sys import stdin, stdout

DIRECTIONS = [(-1, 0), (1, 0), (0, -1), (0, 1)]

def readln():
    return stdin.readline().rstrip()

def outln(n):
    stdout.write(str(n) + '\n')

def dfs_bridges_iterative(grid):
    rows, columns = len(grid), len(grid[0])
    visited = [[False] * columns for _ in range(rows)]
    disc = [[float('inf')] * columns for _ in range(rows)]
    low = [[float('inf')] * columns for _ in range(rows)]
    parent = [[None] * columns for _ in range(rows)]
    bridges = []
    stack = []
    time = [0]

    for i in range(rows):
        for j in range(columns):
            if (grid[i][j] == 'M' or grid[i][j] == '.') and not visited[i][j]:
                stack.append((i, j, None))

                while stack:
                    x, y, state = stack.pop()

                    if state is None:
                        visited[x][y] = True
                        disc[x][y] = low[x][y] = time[0]
                        time[0] += 1
                        stack.append((x, y, 'post'))

                        for dx, dy in DIRECTIONS:
                            nx, ny = x + dx, y + dy
                            if 0 <= nx < rows and 0 <= ny < columns and (grid[nx][ny] == '.' or grid[nx][ny] == 'M'):
                                if not visited[nx][ny]:
                                    stack.append((nx, ny, None))
                                    parent[nx][ny] = (x, y)
                                elif (nx, ny) != parent[x][y]:
                                    low[x][y] = min(low[x][y], disc[nx][ny])
                    else:
                        for dx, dy in DIRECTIONS:
                            nx, ny = x + dx, y + dy
                            if 0 <= nx < rows and 0 <= ny < columns and (grid[nx][ny] == '.' or grid[nx][ny] == 'M'):
                                if parent[nx][ny] == (x, y):
                                    low[x][y] = min(low[x][y], low[nx][ny])
                                    if low[nx][ny] > disc[x][y]:
                                        bridges.append(((x, y), (nx, ny)))
                                elif (nx, ny) != parent[x][y]:
                                    low[x][y] = min(low[x][y], disc[nx][ny])

    return bridges

def bfs_safe_path(maze, N, M, sx, sy, ex, ey):
    queue = deque([(sx, sy)])
    visited = [[False] * M for _ in range(N)]
    visited[sx][sy] = True
    parent = [[None] * M for _ in range(N)]

    while queue:
        x, y = queue.popleft()
        if x == ex and y == ey:
            path = []
            while parent[x][y] is not None:
                path.append((x, y))
                x, y = parent[x][y]
            path.append((sx, sy))
            path.reverse()
            return path
        for dx, dy in DIRECTIONS:
            nx, ny = x + dx, y + dy
            if 0 <= nx < N and 0 <= ny < M and not visited[nx][ny] and maze[nx][ny] != '#':
                visited[nx][ny] = True
                parent[nx][ny] = (x, y)
                queue.append((nx, ny))
    return []

def evaluate_manhole_cover_effectiveness(maze, N, M, manholes, sx, sy, flood_gate):
    flood_effectiveness = {}
    base_flooded = bfs_flood_control(maze, [(sx, sy)], None)
    
    for i in base_flooded:
        i = "\t".join(map(str, i))
        print(i)
    print()

    for manhole in manholes:
        print("manhole", manhole)
        
        if manhole == flood_gate[0] or manhole == flood_gate[1]:
            continue
        
        flood_with_cover = bfs_flood_control(maze, [(sx, sy)], manhole)
        flood_effectiveness[manhole] = sum(1 for x in range(N) for y in range(M) if base_flooded[x][y] and not flood_with_cover[x][y])
    
    return flood_effectiveness

def bfs_flood_control(grid, start_points, block=None):
    rows, columns = len(grid), len(grid[0])
    queue = deque(start_points)
    visited = [[False] * columns for _ in range(rows)]
    
    for x, y in start_points:
        visited[x][y] = True
    
    while queue:
        x, y = queue.popleft()
        for dx, dy in DIRECTIONS:
            nx, ny = x + dx, y + dy
            if 0 <= nx < rows and 0 <= ny < columns and not visited[nx][ny] and grid[nx][ny] != '#':
                if block and (nx, ny) == block:
                    continue
                visited[nx][ny] = True
                queue.append((nx, ny))
    return visited

def select_effective_covers(manhole_effectiveness, C):
    sorted_by_effectiveness = sorted(manhole_effectiveness.items(), key=lambda item: item[1], reverse=True)
    return [manhole for manhole, effectiveness in sorted_by_effectiveness[:C]]

def valid_flood_gate(maze, N, M, sx, sy, ex, ey, bridge):
    (bx1, by1), (bx2, by2) = bridge
    maze[bx1][by1] = '#'
    maze[bx2][by2] = '#'
    
    path_exists = bfs_safe_path(maze, N, M, sx, sy, ex, ey) != []

    maze[bx1][by1] = '.'
    maze[bx2][by2] = '.'
    
    return path_exists

def main():
    num_cases = int(readln())
    for _ in range(num_cases):
        N, M = map(int, readln().split())
        maze = [list(readln()) for _ in range(N)]
        C = int(readln())

        sx, sy, ex, ey = 0, 0, 0, 0
        manholes = []
        for x in range(N):
            for y in range(M):
                if maze[x][y] == 'D':
                    sx, sy = x, y
                elif maze[x][y] == 'E':
                    ex, ey = x, y
                elif maze[x][y] == 'M':
                    manholes.append((x, y))

        bridges = dfs_bridges_iterative(maze)
        
        valid_bridges = []
        for bridge in bridges:
            if valid_flood_gate(maze, N, M, sx, sy, ex, ey, bridge):
                valid_bridges.append(bridge)

        flood_gate = random.choice(valid_bridges)

        path = bfs_safe_path(maze, N, M, sx, sy, ex, ey)

        manhole_effectiveness = evaluate_manhole_cover_effectiveness(maze, N, M, manholes, sx, sy, flood_gate)
        selected_covers = select_effective_covers(manhole_effectiveness, C)
        
        outln(f"{flood_gate[0][0]} {flood_gate[0][1]} {flood_gate[1][0]} {flood_gate[1][1]}")
        outln(len(selected_covers))
        for cover in selected_covers:
            outln(f"{cover[0]} {cover[1]}")
        outln(len(path))
        for step in path:
            outln(f"{step[0]} {step[1]}")

if __name__ == "__main__":
    main()
