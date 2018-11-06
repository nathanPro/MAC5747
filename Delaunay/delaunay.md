# 2018-10-07 Voronoi Diagram notes

How to build the Voronoi Diagram from the Delaynay Triangulation?

## First algorithm
Given the DECL, it is possible to do it in linear time.

## Second algorithm
There is a quadratic approach to this problem. You can first find
any edge in the convex hull of the points (linear time). Then you
can build it up using the circle criteria. With this idea, you
are able to build the triangulation in quadratic time, since there
are a O(n) edges to find, and you have to look at every point
for every edge.

## Incremental algorithm
Build the algorithm by processing the points in order, keeping
the current triangulation.

For processing the edges, it feels reasonable to keep a reference
to both vertices that "complete the triangle" with it.

Begin by inserting 3 points such that the triangle that they form
containts the set of points and, furthermore, does not disturb it.
Basically, they are 3 sentinel values.

The data structure is a DAG. The root is the triangle formed by
the tree sentinels. (Maybe its a better idea to just have this
sentinel root, and not the sentinel points). I will have to consider
those points in the algorithm itself.

To legalize an edge, there is no need to rotate. You can just copy
the triangles into new ones, that can be reached from the current
ones.

Note that at every iteration, we have a Delaunay Triangulation,
which is a planar graph. Since DT(P[i]) has (i + 3) vertices, 
we know that it has 3(i + 3) - 6 edges. Hence, the expected
degree of a vertex is

    2(3(i + 3) - 6) / i ~ 6 = O(1).

Hence the DAG has an O(n) expected number of vertices.

// Can I do path compression in the DAG?
// Nope, it does not feel reasonable
