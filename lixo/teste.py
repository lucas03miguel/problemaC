def dfs_bridges(x, y, grid, visited, disc, low, parent, bridges, time):
    visited[x][y] = True
    disc[x][y] = low[x][y] = time[0]
    time[0] += 1
    
    for dx, dy in [(-1, 0), (1, 0), (0, -1), (0, 1)]:
        nx, ny = x + dx, y + dy
        if grid[nx][ny] == '.':
            if not visited[nx][ny]:  # Tree edge
                parent[nx][ny] = (x, y)
                dfs_bridges(nx, ny, grid, visited, disc, low, parent, bridges, time)
                
                # Check if the subtree rooted at nx, ny has a connection back to one of the ancestors of x, y
                low[x][y] = min(low[x][y], low[nx][ny])
                
                # If the lowest vertex reachable from subtree under nx, ny is above x, y in DFS tree, then x, y - nx, ny is a bridge
                if low[nx][ny] > disc[x][y]:
                    bridges.append(((x, y), (nx, ny)))
            elif (nx, ny) != parent[x][y]:  # Back edge
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


"""
def flood_simulation(grid, start_points, block=None):
    print("Flood simulation with block: ", block)

    directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]
    rows, columns = len(grid), len(grid[0])
    visited = [[False] * columns for _ in range(rows)]
    queue = start_points[:]
    
    for x, y in queue:
        visited[x][y] = True

    i = 0
    while i < len(queue):
        x, y = queue[i]
        if block != None:
            print("Current point:", x, y)
        i += 1
        for dx, dy in directions:
            nx, ny = x + dx, y + dy
            if grid[nx][ny] == '#' or visited[nx][ny]:
                continue

            if block != None:
                print("Next point:", nx, ny)

            if block != None:
                
                print("vamos ver os visitados")
                for a in visited:
                    a = "\t".join(str(j) for j in a)
                    print(a)
                print()
                

            if 0 <= nx < rows and 0 <= ny < columns and grid[nx][ny] == '.' and not visited[nx][ny]:
                if block:
                    print("Flooded point:", nx, ny)
                

                if block and (((x, y) == block[0] and (nx, ny) == block[1]) or ((x, y) == block[1] and (nx, ny) == block[0])):
                    print("skippei")
                    continue  # Skip the blocked bridge

                visited[nx][ny] = True
                queue.append((nx, ny))

    print("Flooded areas")
    for a in visited:
        a = "\t".join(str(j) for j in a)
        print(a)
    print()
    return visited

def critical_bridges_for_flood_control(grid, manholes, bridges, count_trues_initial):
    critical_bridges = []
    
    for bridge in bridges:
        # Simulate flood with the bridge completely blocked
        blocked_flooded_areas = flood_simulation(grid, manholes, block=bridge)

        
        print("initial")
        for a in initial_flooded_areas:
            a = "\t".join(str(j) for j in a)
            print(a)
        print()
        
        print(bridge)
        count_trues = sum([1 for i in range(len(blocked_flooded_areas)) for j in range(len(blocked_flooded_areas[0])) if blocked_flooded_areas[i][j] == True])
        print("Count trues:", count_trues)
        print("Count trues initial:", count_trues_initial)

        
        # Check if blocking this bridge helped reduce flood spread significantly
        for point in bridge:
            print("Point:", point)
            if not blocked_flooded_areas[point[0]][point[1]] and initial_flooded_areas[point[0]][point[1]]:
                critical_bridges.append(bridge)
                print("adicionei carai")
                break  # If any point of the bridge is critical, add the whole bridge and stop checking other points
        
        #break
    
    return critical_bridges
"""

def flood_simulation(grid, start_points, block=None):
    directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]
    rows, columns = len(grid), len(grid[0])
    visited = [[False] * columns for _ in range(rows)]
    queue = start_points[:]
    
    for x, y in queue:
        visited[x][y] = True

    i = 0
    while i < len(queue):
        x, y = queue[i]
        i += 1
        for dx, dy in directions:
            nx, ny = x + dx, y + dy
            if not visited[nx][ny] and grid[nx][ny] == '.':
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

# Main usage
rows, columns = map(int, input().split())
maze = [input() for _ in range(rows)]
manholes = [(i, j) for i in range(rows) for j in range(columns) if maze[i][j] == 'M']
bridges = find_bridges(maze)
print(bridges)
print("Manholes:", manholes)

dominating_set = find_dominating_set_for_flood_control(maze, manholes, bridges)
print("Optimal bridges for flood gates:", dominating_set)

