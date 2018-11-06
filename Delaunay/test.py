from Delaunay import *
comp = left(Point(0, 2), Point(2, 2))
assert(comp(Point(1, 1)) == False)
assert(comp(promote(Point(1, 1), True)) == True)

comp = left(Point(2, 0), Point(2, 2))
assert(comp(promote(Point(0, 1), True)))
