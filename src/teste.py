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
    disc = [[float('inf')] * columns for _ in range(rows)]
    low = [[float('inf')] * columns for _ in range(rows)]
    parent = [[None] * columns for _ in range(rows)]
    bridges = []
    stack = []
    time = [0]

    for i in range(rows):
        for j in range(columns):
            if (grid[i][j] == 'M' or grid[i][j] == '.') and not visited[i][j]:
                stack.append((i, j, 0))

                while stack:
                    x, y, state = stack.pop()

                    if not state:
                        visited[x][y] = True
                        disc[x][y] = low[x][y] = time[0]
                        time[0] += 1
                        stack.append((x, y, 1))

                        for dx, dy in DIRECTIONS:
                            nx, ny = x + dx, y + dy
                            if 0 <= nx < rows and 0 <= ny < columns and (grid[nx][ny] == '.' or grid[nx][ny] == 'M'):
                                if not visited[nx][ny]:
                                    stack.append((nx, ny, 0))
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
                                        if (x, y) < (nx, ny) and valid_flood_gate(grid, rows, columns, sx, sy, ex, ey, ((x, y), (nx, ny))):
                                            bridges.append(((x, y), (nx, ny)))
                                            return bridges
                                        elif (x, y) >= (nx, ny) and valid_flood_gate(grid, rows, columns, sx, sy, ex, ey, ((nx, ny), (x, y))):
                                            bridges.append(((nx, ny), (x, y)))
                                            return bridges
                                        #bridges.append(((x, y), (nx, ny)))
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

def evaluate_manhole_cover_effectiveness(maze, N, M, manholes, sx, sy, flood_gate, C):
    flood_effectiveness = {}

    base_flooded = bfs_flood_control(maze, manholes, flood_gate)

    for comb in combinations(manholes, C):
        flood = bfs_flood_control(maze, set(manholes) - set(comb), flood_gate, comb)

        filtered_comb = tuple(m for m in comb if not flood[m[0]][m[1]])        
        if len(filtered_comb) != len(comb):
            comb = filtered_comb
        
        flood_effectiveness[comb] = sum(1 for x in range(N) for y in range(M) if base_flooded[x][y] and not flood[x][y])

    best_combination = max(flood_effectiveness, key=flood_effectiveness.get)
    return list(best_combination)


memo = {}
def bfs_flood_control(grid, start_points, flood_gate, block=()):
    key = (tuple(start_points), flood_gate, tuple(block))
    if key in memo:
        return memo[key]
    
    #print("start_points", start_points)
    rows, columns = len(grid), len(grid[0])
    queue = deque(start_points)
    
    if block:
        for i in block:
            if i in queue:
                queue.remove(i)
    
    visited = [[False] * columns for _ in range(rows)]
    
    #print("queue", queue)
    #print("flood_gate", flood_gate)
    # Marcar a flood gate como visitada
    #visited[flood_gate[0][0]][flood_gate[0][1]] = True
    #visited[flood_gate[1][0]][flood_gate[1][1]] = True
    
    
#    #print("sadfsdf", start_points)
    #for x, y in start_points:
    #    visited[x][y] = True

    #print("block", block)
    #if block:
    #    for i in block:
            ##print(i)
    #        grid[i[0]][i[1]] = '.'
        #x, y = block
        #grid[x][y] = '.'
        #queue.po
        #visited[block[0]][block[1]] = False
        
        #if (x, y) in start_points:
        #    return visited
        
       
    while queue:
        x, y = queue.popleft()
        #print("x, y:", x, y)
        
        for dx, dy in DIRECTIONS:
            nx, ny = x + dx, y + dy
            if 0 <= nx < rows and 0 <= ny < columns and not visited[nx][ny] and grid[nx][ny] != '#':
                #print("nx, ny", nx, ny, "dx dy", dx, dy)
                #print("sd", subtract_tuples(flood_gate[1], flood_gate[0]), "fff", flood_gate[0], flood_gate[1])
                #print("diferenca", (dx, dy) == subtract_tuples(flood_gate[1], flood_gate[0]))
                if block:
                    if (dx, dy) == (0, -1):
                        if (nx, ny) == flood_gate[0] and (x, y) == flood_gate[1]:
                            #print("continunen", nx, ny, (nx, ny) == flood_gate[0], (nx, ny) == flood_gate[1])
                            continue
                    elif (dx, dy) == (0, 1):
                        if (nx, ny) == flood_gate[1] and (x, y) == flood_gate[0]:
                            #print("continunen", nx, ny, (nx, ny) == flood_gate[0], (nx, ny) == flood_gate[1])
                            continue
                    elif (dx, dy) == (1, 0):
                        if (nx, ny) == flood_gate[1] and (x, y) == flood_gate[0]:
                            #print("continunen", nx, ny, (nx, ny) == flood_gate[0], (nx, ny) == flood_gate[1])
                            continue
                    elif (dx, dy) == (-1, 0):
                        if (nx, ny) == flood_gate[0] and (x, y) == flood_gate[1]:
                            #print("continunen", nx, ny, (nx, ny) == flood_gate[0], (nx, ny) == flood_gate[1])
                            continue
                
                visited[nx][ny] = True
                queue.append((nx, ny))

    memo[key] = visited
    return visited


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


def main():
    num_cases = int(readln())
    for _ in range(num_cases):
        N, M = map(int, readln().split())
        maze = [list(readln()) for _ in range(N)]
        C = int(readln())

        ex, ey = [(x, y) for x in range(N) for y in range(M) if maze[x][y] == 'E'][0]
        dx, dy = [(x, y) for x in range(N) for y in range(M) if maze[x][y] == 'D'][0]
        manholes = [(x, y) for x in range(N) for y in range(M) if maze[x][y] == 'M']

        bridges = dfs_bridges_iterative(maze, dx, dy, ex, ey)
        flood_gate = random.choice(bridges)
        
        selected_covers = evaluate_manhole_cover_effectiveness(maze, N, M, manholes, dx, dy, flood_gate, C)
        #print(selected_covers)
        

        #print(flood)
        
        #for x in range(N):
        #    for y in range(M):
        #        if flood[x][y] and (x, y) in selected_covers:
        #            selected_covers.remove((x, y))
                    
        
        outln(f"{flood_gate[0][0]} {flood_gate[0][1]} {flood_gate[1][0]} {flood_gate[1][1]}")
        outln(len(selected_covers)) 
        for cover in selected_covers:
            outln(f"{cover[0]} {cover[1]}")
        path = bfs_safe_path(maze, N, M, dx, dy, ex, ey)
        outln(len(path))
        for step in path:
            outln(f"{step[0]} {step[1]}")


if __name__ == "__main__":
    main()