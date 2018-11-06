from Delaunay import *

comp = left(Point(0, 2), Point(2, 2))
assert comp(Point(1, 1)) == False
assert comp(promote(Point(1, 1), True))

comp = left(Point(2, 0), Point(2, 2))
assert comp(promote(Point(0, 1), True))

T = Triangle(a = promote(Point(1, -1), True),
             b = promote(Point(0, 1), True),
             c = promote(Point(-1, -1), True))

assert T.contains(Point(10213123123, 1823719823))

division = DAG();

n = int(input())
for i in range(n):
    x, y = [int(s) for s in input().split()]
    print("Inserting", Point(x, y))
    division.insert(Point(x, y))

n = int(input())
for i in range(n):
    x, y = (int(s) for s in input().split())
    p = Point(x, y)
    print(p, ':', division.find(p))

def dfs(T, seen = None):
    print('dfs', T)
    for V in T.A:
        print(" " * 4, V)
    if seen is None:
        seen = set()

    seen.add(T)
    for V in T.A:
        if V is not None and V not in seen:
            dfs(V, seen)

dfs(division.find(Point(1, 1)))
