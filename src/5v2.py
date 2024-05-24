import random
from sys import stdin, stdout
from collections import deque
import itertools

def readln():
    return stdin.readline().rstrip()

def outln(n):
    stdout.write(str(n) + '\n')

# Direcoes possiveis para se mover no labirinto: cima, baixo, esquerda, direita
DIRECTIONS = [(-1, 0), (1, 0), (0, -1), (0, 1)]

def bfs_path(maze, start, goal, water_filled):
    n, m = len(maze), len(maze[0])
    queue = deque([start])
    came_from = {start: None}

    while queue:
        current = queue.popleft()
        if current == goal:
            return reconstruct_path(came_from, current)

        for direction in DIRECTIONS:
            neighbor = (current[0] + direction[0], current[1] + direction[1])
            if 0 <= neighbor[0] < n and 0 <= neighbor[1] < m and maze[neighbor[0]][neighbor[1]] != '#' and not water_filled[neighbor[0]][neighbor[1]]:
                if neighbor not in came_from:
                    queue.append(neighbor)
                    came_from[neighbor] = current
    return None

def reconstruct_path(came_from, current):
    total_path = [current]
    while came_from[current] is not None:
        current = came_from[current]
        total_path.append(current)
    total_path.reverse()
    return total_path

def flood_simulation(grid, start_points, block=None):
    rows, columns = len(grid), len(grid[0])
    visited = [[False] * columns for _ in range(rows)]
    queue = start_points[:]
    
    for x, y in queue:
        visited[x][y] = True

    i = 0
    while i < len(queue):
        x, y = queue[i]
        i += 1

        for dx, dy in DIRECTIONS:
            nx, ny = x + dx, y + dy
            if grid[nx][ny] == '#' or visited[nx][ny]:
                continue

            if 0 <= nx < rows and 0 <= ny < columns and grid[nx][ny] == '.' and not visited[nx][ny]:
                if block and (((x, y) == block[0] and (nx, ny) == block[1]) or ((x, y) == block[1] and (nx, ny) == block[0])):
                    continue

                visited[nx][ny] = True
                queue.append((nx, ny))

    return visited



def dfs_bridges(x, y, grid, visited, disc, low, parent, bridges, time):
    visited[x][y] = True
    disc[x][y] = low[x][y] = time[0]
    time[0] += 1
    
    for dx, dy in DIRECTIONS:
        nx, ny = x + dx, y + dy
        if 0 <= nx < len(grid) and 0 <= ny < len(grid[0]) and grid[nx][ny] == '.':
            if not visited[nx][ny]:
                parent[nx][ny] = (x, y)
                dfs_bridges(nx, ny, grid, visited, disc, low, parent, bridges, time)
                
                low[x][y] = min(low[x][y], low[nx][ny])
                
                if low[nx][ny] > disc[x][y]:
                    bridges.append(((x, y), (nx, ny)))
            elif (nx, ny) != parent[x][y]:
                low[x][y] = min(low[x][y], disc[nx][ny])


def find_bridges(grid):
    rows, columns = len(grid), len(grid[0])
    visited = [[False] * columns for _ in range(rows)]
    disc = [[float('inf')] * columns for _ in range(rows)]
    low = [[float('inf')] * columns for _ in range(rows)]
    parent = [[None] * columns for _ in range(rows)]
    bridges = []
    time = [0]
    
    for i in range(rows):
        for j in range(columns):
            if (grid[i][j] == 'M' or grid[i][j] == '.') and not visited[i][j]:
                dfs_bridges(i, j, grid, visited, disc, low, parent, bridges, time)
    
    return bridges

def find_dominating_set_for_flood_control(grid, manholes, bridges):
    rows, columns = len(grid), len(grid[0])
    initial_flooded_areas = flood_simulation(grid, manholes)
    
    bridge_best = (-1, -1)
    count_inicial = sum([1 for i in range(len(initial_flooded_areas)) for j in range(len(initial_flooded_areas[0])) if initial_flooded_areas[i][j] == True])
    for bridge in bridges:
        blocked_flooded_areas = flood_simulation(grid, manholes, block=bridge)
        count = sum([1 for i in range(len(blocked_flooded_areas)) for j in range(len(blocked_flooded_areas[0])) if blocked_flooded_areas[i][j] == True])

        if count < count_inicial:
            count_inicial = count
            bridge_best = bridge

    return bridge_best


def main():
    num_cases = int(readln())
    results = []

    for _ in range(num_cases):
        n, m = map(int, readln().split())
        maze = [list(readln()) for _ in range(n)]
        num_covers = int(readln())

        door = None
        exit = None
        manholes = []

        for r in range(n):
            for c in range(m):
                if maze[r][c] == 'D':
                    door = (r, c)
                elif maze[r][c] == 'E':
                    exit = (r, c)
                elif maze[r][c] == 'M':
                    manholes.append((r, c))
        
        bridges = find_bridges(maze)
        
        solution = []
        for cover_combination in itertools.combinations(manholes, num_covers):
            manhole_aux = list(set(manholes) - set(cover_combination))
            dominating_set = find_dominating_set_for_flood_control(maze, manhole_aux, bridges)
            water_filled = flood_simulation(maze, manhole_aux, dominating_set)
                
            path = bfs_path(maze, door, exit, water_filled)
            if path:
                solution.append([dominating_set, cover_combination, path])
            else:
                solution.append([dominating_set, cover_combination, []])

        flood_gate, covers, path = random.choice(solution)
        outln(f"{flood_gate[0][0]} {flood_gate[0][1]} {flood_gate[1][0]} {flood_gate[1][1]}")
        outln(str(len(covers)))
        for cover in covers:
            outln(f"{cover[0]} {cover[1]}")
        outln(str(len(path)))
        for step in path:
            outln(f"{step[0]} {step[1]}")

if __name__ == '__main__':
    main()
