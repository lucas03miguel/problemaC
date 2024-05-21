from sys import stdin, stdout

def readln():
    return stdin.readline().rstrip()

def outln(n):
    stdout.write(str(n) + '\n')

def aztec():


    return "pila"


def main():
    global maze
    n_cases = int(readln())

    for i in range(n_cases):
        rows, columns = map(int, readln().split())
        maze = [[0 for _ in range(columns)] for _ in range(rows)]
        
        for j in range(rows):
            linha = readln()
            maze[j] = list(linha)
        
        C = int(readln())
        #for row in maze:
        #    outln(row)


        outln(maze)
        aztec()


if __name__ == '__main__':
    main()