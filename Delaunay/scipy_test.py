import numpy as np
import matplotlib.pyplot as plt
import Delaunay as nathan 
from scipy.spatial import Delaunay

def to_list(x):
    return [x.x, x.y]

def tri_cmp(T, U):
    return sorted(T) == sorted(U)

def canonize(ans):
    for T in ans:
        T.sort()
    ans.sort()

n = int(input())
points = [[int(s) for s in input().split()] for _ in range(n)]

dag = nathan.DAG()
for x, y in points:
    dag.insert(nathan.Point(x, y))

tri = Delaunay(np.array(points))
sol = [[points[T[i]] for i in range(3)] for T in tri.simplices]
me  = [[to_list(p) for p in pts] for pts in dag]

canonize(sol)
canonize(me)

if sol == me:
    print("OK!")
else:
    print("ERRO:")
    print(points)
    print(sol)
    print(me)
