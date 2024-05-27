import random
from sys import stdin, stdout
from collections import deque

def readln():
    return stdin.readline().strip()

def outln(s):
    stdout.write(str(s) + "\n")

def parse_maze(N, M):
    maze = [list(readln()) for _ in range(N)]
    start = end = None
    manholes = []
    for x in range(N):
        for y in range(M):
            if maze[x][y] == 'D':
                start = (x, y)
            elif maze[x][y] == 'E':
                end = (x, y)
            elif maze[x][y] == 'M':
                manholes.append((x, y))
    return maze, start, end, manholes

def setup_bridge_utilities(rows, cols):
    inf = float('inf')
    visited = [[False] * cols for _ in range(rows)]
    disc = low = [[inf] * cols for _ in range(rows)]
    parent = [[None] * cols for _ in range(rows)]
    return visited, disc, low, parent

def process_stack_for_bridges(stack, visited, disc, low, parent, time, bridges, grid, directions):
    while stack:
        x, y, state = stack.pop()
        if state == 0:  # Visiting the node
            visited[x][y] = True
            disc[x][y] = low[x][y] = time[0]
            time[0] += 1
            stack.append((x, y, 1))
            for dx, dy in directions:
                nx, ny = x + dx, y + dy
                if 0 <= nx < len(grid) and 0 <= ny < len(grid[0]) and grid[nx][ny] in 'M.':
                    if not visited[nx][ny]:
                        stack.append((nx, ny, 0))
                        parent[nx][ny] = (x, y)
                    elif (nx, ny) != parent[x][y]:
                        low[x][y] = min(low[x][y], disc[nx][ny])
        else:
            for dx, dy in directions:
                nx, ny = x + dx, y + dy
                if 0 <= nx < len(grid) and 0 <= ny < len(grid[0]) and grid[nx][ny] in 'M.':
                    if parent[nx][ny] == (x, y):
                        low[x][y] = min(low[x][y], low[nx][ny])
                        if low[nx][ny] > disc[x][y]:
                            bridges.append(((x, y), (nx, ny)))

def dfs_bridges_iterative(grid):
    directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]
    rows, cols = len(grid), len(grid[0])
    visited, disc, low, parent = setup_bridge_utilities(rows, cols)
    bridges, stack, time = [], [], [0]

    for i in range(rows):
        for j in range(cols):
            if grid[i][j] in 'M.' and not visited[i][j]:
                stack.append((i, j, 0))
                process_stack_for_bridges(stack, visited, disc, low, parent, time, bridges, grid, directions)
    
    return bridges

def is_valid(x, y, visited, grid):
    return 0 <= x < len(grid) and 0 <= y < len(grid[0]) and not visited[x][y] and grid[x][y] != '#'

def perform_flood_bfs(queue, visited, grid, flood_gate):
    while queue:
        x, y = queue.popleft()
        for dx, dy in [(-1, 0), (1, 0), (0, -1), (0, 1)]:
            nx, ny = x + dx, y + dy
            if is_valid(nx, ny, visited, grid):
                visited[nx][ny] = True
                queue.append((nx, ny))
    return visited



def adjust_blocked_points(grid, block, queue):
    for point in block:
        grid[point[0]][point[1]] = '#'
        if point in queue:
            queue.remove(point)

def bfs_flood_control(grid, start_points, flood_gate, block=None):
    rows, cols = len(grid), len(grid[0])
    queue, visited = deque(start_points), [[False] * cols for _ in range(rows)]
    if block:
        adjust_blocked_points(grid, block, queue)
    return perform_flood_bfs(queue, visited, grid, flood_gate)

def main():
    num_cases = int(readln())
    for _ in range(num_cases):
        N, M = map(int, readln().split())
        maze, start, end, manholes = parse_maze(N, M)
        C = int(readln())

        bridges = dfs_bridges_iterative(maze)
        valid_bridges = [bridge for bridge in bridges if valid_flood_gate(maze, N, M, start[0], start[1], end[0], end[1], bridge)]

        if not valid_bridges:
            outln("No valid flood gate found.")
            continue

        flood_gate = random.choice(valid_bridges)
        manhole_effectiveness = evaluate_manhole_cover_effectiveness(maze, N, M, manholes, start[0], start[1], flood_gate, C)
        selected_covers = select_effective_covers(manhole_effectiveness)

        flood = bfs_flood_control(maze, set(manholes) - set(selected_covers), flood_gate, selected_covers)

        print_results(flood_gate, selected_covers, flood, N, M, start[0], start[1], end[0], end[1])
