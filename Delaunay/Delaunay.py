from collections import namedtuple
Point = namedtuple('Point', 'x y')
Epoint = namedtuple('EPoint', 'inf inner')

def make_point(*, x, y):
    return Point(x, y)

def cross(a, b):
    return a.x * b.y - a.y * b.x

def make_e_point(*, x, y, inf = False):
    return Epoint(inf = inf, inner = make_point(x = x, y = y))

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
        points = [a, b, c, a]
        for i in range(3):
            rank = points[i].inf + points[i + 1].inf
            p[rank] += cross(points[i].inner, points[i + 1].inner)
        i = 2
        while (i > 0 and p[i] == 0): i -= 1;
        return p[i] > 0;

    return inner
