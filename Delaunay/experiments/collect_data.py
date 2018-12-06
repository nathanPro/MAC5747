import time
import numpy as np
import pandas as pd
from standalone import *
from scipy.spatial import Delaunay

def to_list(x):
    return [x.x, x.y]

def tri_cmp(T, U):
    return sorted(T) == sorted(U)

def canonize(ans):
    for T in ans:
        T.sort()
    ans.sort()

categories     = int(input())
tests_per_cat  = int(input())

tests = categories * tests_per_cat

rel_time = np.ndarray(tests)
flip_cnt = np.ndarray(tests)
inp_size = np.ndarray(tests)

for cat in range(categories):
    n = int(input())
    points = [[float(s) for s in input().split()] for _ in range(n)]
    apoints = np.array(points)

    for tc in range(tests_per_cat):
        time1 = time.time()
        tri = Delaunay(apoints)
        time2 = time.time()
        reference_time = time2-time1

        sol = [[points[T[i]] for i in range(3)] for T in tri.simplices]

        time1 = time.time()
        reset_area2()
        dag = IncrProb([Point(l[0], l[1]) for l in points])
        time2 = time.time()

        rel_time[cat * tests_per_cat + tc] = (time2-time1)/reference_time
        flip_cnt[cat * tests_per_cat + tc] = report_count()
        inp_size[cat * tests_per_cat + tc] = n

        me  = [[to_list(p) for p in pts] for pts in dag]
        canonize(sol)
        canonize(me)

        if sol != me:
            print("ERRO:")
            print(sol)
            print(me)

d = pd.DataFrame({'flips' : flip_cnt, 'time' : rel_time, 'size' : inp_size})
d.to_csv('data.csv')
