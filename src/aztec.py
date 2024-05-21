from sys import stdin, stdout

def readln():
    return stdin.readline().rstrip()

def outln(n):
    stdout.write(str(n) + '\n')

def aztec():
    pass


def main():
    global maze
    n_cases = int(readln())

    for i in range(n_cases):
        rows, columns = map(int, readln().split())
        maze = [[0 for _ in range(columns)] for _ in range(rows)]
        print(maze)
        
        for j in range(rows):
            linha = readln()
            maze[j] = list(linha)
        
        print(maze)
        C = int(readln())


if __name__ == '__main__':
    main()