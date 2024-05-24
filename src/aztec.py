from collections import deque
from heapq import heappop, heappush
from itertools import combinations
from sys import stdin, stdout

def readln():
    return stdin.readline().rstrip()

def outln(n):
    stdout.write(str(n) + '\n')

"""
def bfs_water(start, maze, N, M):
    directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]
    queue = deque([start])
    visited = set([start])
    water_reach = set()
    
    while queue:
        x, y = queue.popleft()
        water_reach.add((x, y))
        
        for dx, dy in directions:
            nx, ny = x + dx, y + dy
            if 0 <= nx < N and 0 <= ny < M and (nx, ny) not in visited and maze[nx][ny] != '#':
                visited.add((nx, ny))
                queue.append((nx, ny))
    
    return water_reach

def greedy_bfs(start, goal, water_reach, maze, N, M):
    directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]
    heap = [(abs(start[0] - goal[0]) + abs(start[1] - goal[1]), start, [start])]
    visited = set()
    
    while heap:
        _, (x, y), path = heappop(heap)
        
        if (x, y) in visited:
            continue
        
        visited.add((x, y))
        
        if (x, y) == goal:
            return path
        
        for dx, dy in directions:
            nx, ny = x + dx, y + dy
            if 0 <= nx < N and 0 <= ny < M and (nx, ny) not in visited and (nx, ny) not in water_reach and maze[nx][ny] != '#':
                heappush(heap, (abs(nx - goal[0]) + abs(ny - goal[1]), (nx, ny), path + [(nx, ny)]))
    
    return None

def bidirectional_search(start, goal, water_reach, maze, N, M):
    directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]
    queue_start = deque([(start, [start])])
    queue_goal = deque([(goal, [goal])])
    visited_start = {start: [start]}
    visited_goal = {goal: [goal]}
    
    while queue_start and queue_goal:
        if queue_start:
            (x, y), path = queue_start.popleft()
            for dx, dy in directions:
                nx, ny = x + dx, y + dy
                if 0 <= nx < N and 0 <= ny < M and (nx, ny) not in visited_start and (nx, ny) not in water_reach and maze[nx][ny] != '#':
                    new_path = path + [(nx, ny)]
                    visited_start[(nx, ny)] = new_path
                    queue_start.append(((nx, ny), new_path))
                    if (nx, ny) in visited_goal:
                        return new_path + visited_goal[(nx, ny)][::-1][1:]
        
        if queue_goal:
            (x, y), path = queue_goal.popleft()
            for dx, dy in directions:
                nx, ny = x + dx, y + dy
                if 0 <= nx < N and 0 <= ny < M and (nx, ny) not in visited_goal and (nx, ny) not in water_reach and maze[nx][ny] != '#':
                    new_path = path + [(nx, ny)]
                    visited_goal[(nx, ny)] = new_path
                    queue_goal.append(((nx, ny), new_path))
                    if (nx, ny) in visited_start:
                        return visited_start[(nx, ny)] + new_path[::-1][1:]
    
    return None

def place_objects_and_find_path(maze, N, M, manholes, C, door, exit):
    best_path = None
    best_flood_gate = None
    best_covers = None
    
    directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]
    
    for i in range(N):
        for j in range(M):
            for dx, dy in directions:
                ni, nj = i + dx, j + dy
                if 0 <= ni < N and 0 <= nj < M and maze[i][j] != '#' and maze[ni][nj] != '#':
                    flood_gate = ((i, j), (ni, nj))
                    water_reach = set()
                    
                    for mx, my in manholes:
                        water_reach.update(bfs_water((mx, my), maze, N, M))
                    
                    for k in range(len(manholes)):
                        for covers in combinations(manholes, k):
                            if k > C:
                                break
                            
                            covered_water_reach = water_reach.copy()
                            for cover in covers:
                                covered_water_reach.discard(cover)
                            
                            if flood_gate[0] in covered_water_reach:
                                covered_water_reach.discard(flood_gate[0])
                            if flood_gate[1] in covered_water_reach:
                                covered_water_reach.discard(flood_gate[1])
                            
                            if N * M < 1000:
                                path = greedy_bfs(door, exit, covered_water_reach, maze, N, M)
                            else:
                                path = bidirectional_search(door, exit, covered_water_reach, maze, N, M)
                            
                            if path:
                                if not best_path or len(path) < len(best_path):
                                    best_path = path
                                    best_flood_gate = flood_gate
                                    best_covers = covers

    return best_flood_gate, best_covers, best_path
"""

def dfs_bridges(x, y, grid, visited, disc, low, parent, bridges, time):
    visited[x][y] = True
    disc[x][y] = low[x][y] = time[0]
    time[0] += 1
    
    for dx, dy in [(-1, 0), (1, 0), (0, -1), (0, 1)]:
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


def flood_simulation(grid, start_points, block=None):
    for i in grid:
        i = "".join(i)
        print(i)
    print()


    directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]
    rows, columns = len(grid), len(grid[0])
    print("rows", rows)
    print("columns", columns)
    visited = [[False] * columns for _ in range(rows)]


    print("start_points", start_points)
    queue = start_points[:]
    print("queue", queue)
    
    for i in visited:
        i = "\t".join(str(j) for j in i)
        print(i)
    print()

    for x, y in queue:
        visited[x][y] = True

    i = 0
    while i < len(queue):
        x, y = queue[i]
        i += 1
        for dx, dy in directions:
            nx, ny = x + dx, y + dy
            if 0 <= nx < rows and 0 <= ny < columns and not visited[nx][ny] and grid[nx][ny] == '.':
                if block and (((x, y), (nx, ny)) == block or ((nx, ny), (x, y)) == block):
                    continue
                
                visited[nx][ny] = True
                queue.append((nx, ny))
    
    return visited

def find_dominating_set_for_flood_control(grid, manholes, bridges):
    rows, columns = len(grid), len(grid[0])
    initial_flooded_areas = flood_simulation(grid, manholes)
    
    bridge_effectiveness = []
    for bridge in bridges:
        blocked_flooded_areas = flood_simulation(grid, manholes, block=bridge)
        flood_prevented = sum(1 for i in range(rows) for j in range(columns)
                              if initial_flooded_areas[i][j] and not blocked_flooded_areas[i][j])
        bridge_effectiveness.append((bridge, flood_prevented))
    
    bridge_effectiveness.sort(key=lambda x: x[1], reverse=True)
    
    dominating_set = []
    covered = set()
    needed_coverage = set((i, j) for i in range(rows) for j in range(columns) if initial_flooded_areas[i][j])
    
    for bridge, effectiveness in bridge_effectiveness:
        if covered >= needed_coverage:
            break
        
        blocked_flooded_areas = flood_simulation(grid, manholes, block=bridge)
        newly_covered = set((i, j) for i in range(rows) for j in range(columns)
                             if initial_flooded_areas[i][j] and not blocked_flooded_areas[i][j])
        if newly_covered > covered: 
            dominating_set.append(bridge)
            covered.update(newly_covered)
    
    return dominating_set



def main():
    global maze

    n_cases = int(readln())

    for i in range(n_cases):
        manholes = []
        door = None
        exit = None
        
        rows, columns = map(int, readln().split())
        maze = [[0 for _ in range(columns)] for _ in range(rows)]
        
        for j in range(rows):
            linha = readln()
            maze[j] = list(linha)
            for k, valor in enumerate(linha):
                if valor == 'M':
                    manholes.append((j, k))
                elif valor == 'D':
                    door = (j, k)
                elif valor == 'E':
                    exit = (j, k)
        
        C = int(readln())


        outln(maze)
        outln(f"onde esta o buraco {manholes}")
        outln(f"onde esta a porta {door}")
        outln(f"onde esta a saida {exit}")

        bridges = find_bridges(maze)
        print(bridges)

        dominating_set = find_dominating_set_for_flood_control(maze, manholes, bridges)
        print("Optimal bridges for flood gates:", dominating_set)

        print(i)



        #flood_gate, covers, path = place_objects_and_find_path(maze, rows, columns, manholes, C, door, exit)

        #outln(flood_gate)
        #outln(covers)
        #outln(path)


if __name__ == '__main__':
    main()