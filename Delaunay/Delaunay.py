from collections import namedtuple
Point = namedtuple('Point', 'x y')
Epoint = namedtuple('EPoint', 'inf inner')

def area2(a, b):
    return a.x * b.y - a.y * b.x

def promote(p, inf = False):
    if not isinstance(p, Epoint):
        return Epoint(inf, p)
    return p

def left(a, b):
    a = promote(a)
    b = promote(b)

    def inner(c):
        c = promote(c)
        p = [0, 0, 0]
        points = [a, b, c]
        for i in range(3):
            rank = points[i - 1].inf + points[i].inf
            p[rank] += area2(points[i - 1].inner, points[i].inner)
        i = 2
        while (i > 0 and p[i] == 0): i -= 1;
        return p[i] > 0;

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

    def __str__(self):
        return ' '.join(str(p) for p in self.P)

class DAG:
    def leaf(T):
        return T.C is None

    def split_triangle(T, p):
        assert DAG.leaf(T)

        ans = []
        aux = [p, T.P[1], T.P[2]]
        for i in range(3):
            ans.append(Triangle(aux[0], aux[1], aux[2]))
            if i + 1 < 3:
                aux[i] = T.P[i]
                aux[i + 1] = p

        aux = [T.A[0], ans[1], ans[2]]
        for i in range(3):
            ans[i].A = aux.copy()
            if T.A[i] is not None:
                assert DAG.leaf(T.A[i])
                T.A[i].A[T.A[i].A.index(T)] = ans[i]
            if i + 1 < 3:
                aux[i] = ans[i]
                aux[i + 1] = T.A[i + 1]

        T.C = ans

    def __init__(self):
        self.root = Triangle(promote(Point(-1, -1), True),
                             promote(Point(1, -1), True),
                             promote(Point(0, 1), True))

    def find(self, p):
        ans = self.root
        while not DAG.leaf(ans):
            ans = next(T for T in ans.C if T.contains(p))
        return ans

    def insert(self, p):
        DAG.split_triangle(self.find(p), p)
