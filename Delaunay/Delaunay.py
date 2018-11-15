from collections import namedtuple
Point = namedtuple('Point', 'x y')
Epoint = namedtuple('EPoint', 'inf inner')

def area2(a, b):
    return a.x * b.y - a.y * b.x

def norm(a):
    return a.x * a.x + a.y * a.y

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
        assert left(a, b)(c)

    def contains(self, q):
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

        DAG.fix_edge(ans[0], 1)
        DAG.fix_edge(ans[1], 0)

    def fix_edge(T, i):
        if T.A[i] is None: return

        U = T.A[i]
        j = U.A.index(T)

        if not in_circle(T.P[0], T.P[1], T.P[2])(U.P[j]):
            return

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

    def split_triangle(T, p):
        assert DAG.leaf(T)
        for i, U in enumerate(T.A):
            if U is not None and U.contains(p):
                DAG.edge_split(T, i, U, U.A.index(T), p)
                return
        DAG.interior_split(T, p)

    def dfs(T, out, seen = None):
        if seen is None:
            seen = set()
        seen.add(T)

        if DAG.leaf(T) and sum(p.inf for p in T.P) == 0:
            out.append(T)
        elif not DAG.leaf(T):
            for V in T.C:
                if V is not None and V not in seen:
                    DAG.dfs(V, out, seen)

    def __init__(self):
        self.root = Triangle(promote(Point(-1, -1), True),
                             promote(Point(1, -1), True),
                             promote(Point(0, 1), True))
        self.tri = []

    def find(self, p):
        ans = self.root
        while not DAG.leaf(ans):
            ans = next(T for T in ans.C if T.contains(p))
        return ans

    def insert(self, p):
        DAG.split_triangle(self.find(p), promote(p))

    def __iter__(self):
        DAG.dfs(self.root, self.tri)
        return ((p.inner for p in T.P) for T in self.tri)
