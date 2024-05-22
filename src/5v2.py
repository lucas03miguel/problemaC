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
            if 0 <= neighbor[0] < n and 0 <= neighbor[1] < m and maze[neighbor[0]][neighbor[1]] != '#' and neighbor not in water_filled:
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

def simulate_water_flow(maze, manholes, flood_gate=None):
    n, m = len(maze), len(maze[0])
    water_filled = set()
    queue = deque(manholes)

    if flood_gate:
        water_filled.add(flood_gate[0])
        water_filled.add(flood_gate[1])

    while queue:
        current = queue.popleft()
        if current in water_filled:
            continue
        water_filled.add(current)
        for direction in DIRECTIONS:
            neighbor = (current[0] + direction[0], current[1] + direction[1])
            if 0 <= neighbor[0] < n and 0 <= neighbor[1] < m and maze[neighbor[0]][neighbor[1]] != '#' and neighbor not in water_filled:
                queue.append(neighbor)
    return water_filled

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

        possible_flood_gates = set()
        for r in range(n):
            for c in range(m):
                if maze[r][c] != '#':
                    for direction in DIRECTIONS:
                        nr, nc = r + direction[0], c + direction[1]
                        if 0 <= nr < n and 0 <= nc < m and maze[nr][nc] != '#':
                            possible_flood_gates.add(((r, c), (nr, nc)))

        solution = None
        for flood_gate in possible_flood_gates:
            for cover_combination in itertools.combinations(manholes, num_covers):
                water_filled = simulate_water_flow(maze, set(manholes) - set(cover_combination), flood_gate)
                path = bfs_path(maze, door, exit, water_filled)
                if path:
                    solution = (flood_gate, cover_combination, path)
                    break
            if solution:
                break

        if solution:
            flood_gate, covers, path = solution
            results.append(f"{flood_gate[0][0]} {flood_gate[0][1]} {flood_gate[1][0]} {flood_gate[1][1]}")
            results.append(str(len(covers)))
            for cover in covers:
                results.append(f"{cover[0]} {cover[1]}")
            results.append(str(len(path)))
            for step in path:
                results.append(f"{step[0]} {step[1]}")

    for result in results:
        outln(result)

if __name__ == '__main__':
    main()
