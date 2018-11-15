import numpy as np
import time
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
apoints = np.array(points)

time1 = time.time()
tri = Delaunay(apoints)
time2 = time.time()
reference_time = time2-time1

print('Scipy took {:.3f} ms'.format(reference_time*1000.0))

sol = [[points[T[i]] for i in range(3)] for T in tri.simplices]

if (n < 2**16):
    plt.plot(apoints[:,0], apoints[:,1], 'o')
    plt.triplot(apoints[:,0], apoints[:,1], tri.simplices.copy())
    plt.show()

time1 = time.time()
dag = nathan.DAG()
for x, y in points:
    dag.insert(nathan.Point(x, y))
time2 = time.time()
print('Mine took {:.3f} ms'.format((time2-time1)*1000.0))
print("Hence, my implementation is {:.3f} times slower".format(
      (time2-time1)/reference_time))
me  = [[to_list(p) for p in pts] for pts in dag]

canonize(sol)
canonize(me)

if sol == me:
    print("OK!")
else:
    print("ERRO:")
    print(sol)
    print(me)
