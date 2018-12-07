import random
from collections import namedtuple
from geocomp import config
from geocomp.common.guiprim import triang
from geocomp.common.prim import count_area2
from geocomp.common.polygon import Polygon
from geocomp.common import control
from geocomp.common.point import Point

Epoint = namedtuple('EPoint', 'inf inner')

def area2(a, b):
    return a.x * b.y - a.y * b.x

def norm(a):
    return a.x * a.x + a.y * a.y

def dist(a, b):
    if a.inf != b.inf:
        return 42
    a = a.inner
    b = b.inner
    return norm(Point(a.x - b.x, a.y - b.y))

def e_triang(p, q, r, color = config.COLOR_PRIM):
    if max(p.inf, q.inf, r.inf) > 0:
        return
    triang(p.inner, q.inner, r.inner, color)

def e_lineto(p, q, color = config.COLOR_PRIM):
    if max(p.inf, q.inf) > 0:
        return
    keep = p.inner.lineto(q.inner, color)

    control.thaw_update()
    control.update()
    control.freeze_update()
    control.sleep()

    p.inner.remove_lineto(q, keep)

COLOR_TRIANG   = '#ff4e00'
COLOR_CONTAINS = '#437f97'
COLOR_TEST     = '#f2af29'
COLOR_GOOD     = '#00FF00'
COLOR_BAD      = '#FF0000'

def e_area2(points):
    p = [0] * 3
    for i in range(3):
        rank = points[i - 1].inf + points[i].inf
        p[rank] += area2(points[i - 1].inner, points[i].inner)
    return p

def e_incircle6(Q):
    p = [0] * 5
    sgn = 1;
    for i in range(4):
        coef = sgn * norm(Q[i].inner)
        disp = 2 * Q[i].inf
        for j, val in enumerate(e_area2([Q[k] for k in range(4) if k != i])):
            p[disp + j] += coef * val 
        sgn *= -1
    return p

def promote(p, inf = False):
    if not isinstance(p, Epoint):
        return Epoint(inf, p)
    return p

def left(a, b):
    a = promote(a)
    b = promote(b)

    def inner(c):
        p = e_area2([a, b, promote(c)])
        i = 2
        while (i > 0 and p[i] == 0): i -= 1;
        return p[i] > 0

    return inner

def in_circle(a, b, c):
    a = promote(a)
    b = promote(b)
    c = promote(c)

    def inner(d):
        p = e_incircle6([a, b, c, promote(d)])
        i = 4;
        while (i > 0 and p[i] == 0): i -= 1
        return p[i] > 0

    return inner

class Triangle:
    def __init__(self, a, b, c):
        self.P = [a, b, c]
        self.A = [None, None, None]
        self.C = None
        self.rank = sum(p.inf for p in self.P)
        self.drawing = None
        self.plot()
        assert left(a, b)(c)

    def plot(self):
        if self.rank != 0:
            return
        self.drawing = []
        for i in range(3):
            self.drawing.append(
                self.P[i - 1].inner.lineto(self.P[i].inner, COLOR_TRIANG))
        control.thaw_update()
        control.update()
        control.freeze_update()

    def hide(self):
        if self.drawing is None:
            return
        for i, _id in enumerate(self.drawing):
            self.P[i - 1].inner.remove_lineto(self.P[i].inner, _id)

    def contains(self, q):
        e_triang(self.P[0], self.P[1], self.P[2], COLOR_CONTAINS)

        for i in range(3):
            if left(self.P[i], self.P[i - 1])(q):
                return False
        return True

    def fix_adjacent(T, i, new):
        if T.A[i] is not None:
            T.A[i].A[T.A[i].A.index(T)] = new

    def __str__(self):
        return ' '.join(str(p) for p in self.P)

class DAG:
    def leaf(T):
        return T.C is None

    def flip_edge(T, i, U, j):
        count_area2()
        assert DAG.leaf(T)
        assert DAG.leaf(U)
        assert T.P[i - 1] == U.P[j - 2]
        assert U.P[j - 1] == T.P[i - 2]

        ans = [Triangle(U.P[j], T.P[i], T.P[i - 2]),
               Triangle(T.P[i], U.P[j], U.P[j - 2])]
        ans[0].A = [T.A[i - 1], U.A[j - 2], ans[1]]
        ans[1].A = [U.A[j - 1], T.A[i - 2], ans[0]]

        Triangle.fix_adjacent(T, i - 1, ans[0])
        Triangle.fix_adjacent(T, i - 2, ans[1])
        Triangle.fix_adjacent(U, j - 1, ans[1])
        Triangle.fix_adjacent(U, j - 2, ans[0])

        T.C = ans
        U.C = ans
        T.hide()
        U.hide()

        DAG.fix_edge(ans[0], 1)
        DAG.fix_edge(ans[1], 0)

    def fix_edge(T, i):
        if T.A[i] is None: return


        U = T.A[i]
        j = U.A.index(T)

        a, b = T.P[i - 2], T.P[i - 1]

        e_lineto(a, b, COLOR_TEST)
        if not in_circle(T.P[0], T.P[1], T.P[2])(U.P[j]):
            e_lineto(a, b, COLOR_GOOD)
            return
        else:
            e_lineto(a, b, COLOR_BAD)

        DAG.flip_edge(T, i, U, j)

    def interior_split(T, p):
        ans = [Triangle(p, T.P[1], T.P[2]),
               Triangle(T.P[0], p, T.P[2]),
               Triangle(T.P[0], T.P[1], p)]

        ans[0].A = [T.A[0], ans[1], ans[2]]
        ans[1].A = [ans[0], T.A[1], ans[2]]
        ans[2].A = [ans[0], ans[1], T.A[2]]

        for i in range(3):
            Triangle.fix_adjacent(T, i, ans[i])

        T.C = ans
        T.hide()
        for i in range(3):
            DAG.fix_edge(T.C[i], i)

    def edge_split(T, i, U, j, p):
        T.C = [Triangle(T.P[i - 1], T.P[i], p),
               Triangle(p, T.P[i], T.P[i - 2])]
        U.C = [Triangle(p, U.P[j], U.P[j - 2]),
               Triangle(U.P[j - 1], U.P[j], p)]

        T.C[0].A = [T.C[1], U.C[0], T.A[i - 2]]
        T.C[1].A = [T.A[i - 1], U.C[1], T.C[0]]
        U.C[0].A = [U.A[j - 1], T.C[0], U.C[1]]
        U.C[1].A = [U.C[0], T.C[1], U.A[j - 2]]

        Triangle.fix_adjacent(T, i - 2, T.C[0])
        Triangle.fix_adjacent(T, i - 1, T.C[1])
        Triangle.fix_adjacent(U, j - 2, U.C[1])
        Triangle.fix_adjacent(U, j - 1, U.C[0])

        DAG.fix_edge(T.C[0], 2)
        DAG.fix_edge(T.C[1], 0)
        DAG.fix_edge(U.C[0], 0)
        DAG.fix_edge(U.C[1], 2)

        T.hide()
        U.hide()

    def split_triangle(T, p):
        assert DAG.leaf(T)
        for i, U in enumerate(T.A):
            if U is not None and U.contains(p):
                DAG.edge_split(T, i, U, U.A.index(T), p)
                return
        DAG.interior_split(T, p)

    def dfs(T, seen):
        seen.add(T)

        out = set()
        if DAG.leaf(T) and sum(p.inf for p in T.P) == 0:
            for p in T.P:
                out.add((p.inner, T))
        elif not DAG.leaf(T):
            for V in T.C:
                if V is not None and V not in seen:
                    out |= DAG.dfs(V, seen)
        return out

    def __init__(self):
        self.root = Triangle(promote(Point(-1, -1), True),
                             promote(Point(1, -1), True),
                             promote(Point(0, 1), True))
        self.pts_and_ts = None

    def find(self, p):
        ans = self.root
        while not DAG.leaf(ans):
            ans = next(T for T in ans.C if T.contains(p))
        return ans

    def insert(self, p):
        pos = self.find(p)
        p = promote(p)
        if min([dist(p, q) for q in pos.P]) == 0:
            return
        DAG.split_triangle(pos, p)

    def __iter__(self):
        if self.pts_and_ts is None:
            seen = set()
            self.pts_and_ts = DAG.dfs(self.root, seen)
        return (pair for pair in self.pts_and_ts)

    def to_graph(dag):
        V = {}
        for p, T in dag:
            V[p] = []

            pp = promote(p)
            i = T.P.index(pp)
            U = T.A[i - 1]

            while U != T:
                i = U.P.index(pp)
                if U.P[i - 2].inf is False:
                    V[p].append(U.P[i - 2].inner)
                U = U.A[i - 1]
            i = U.P.index(pp)
            if U.P[i - 2].inf is False:
                V[p].append(U.P[i - 2].inner)
        return V


def IncrProb(l):
    for i in range(len(l)):
        j = random.randrange(0, i + 1)
        aux = l[i]
        l[i] = l[j]
        l[j] = aux
    return Incr(l)

def Incr(l):
    dag = DAG()
    for pt in l:
        pt.hilight()
        control.sleep()
        dag.insert(pt)
        pt.unhilight()
    print(DAG.to_graph(dag))
    return dag
