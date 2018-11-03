#include <algorithm>
#include <array>
#include <complex>
#include <iostream>
#include <memory>
#include <ostream>

namespace geo {
template <typename N> using point = std::complex<N>;

template <typename N> N cross(point<N> a, point<N> b) {
    return std::imag(std::conj(a) * b);
}
template <typename N> N area(point<N> a, point<N> b, point<N> c) {
    return cross(b - a, c - a);
}

using rank_t = uint16_t;
template <typename N> class e_point;
template <typename N> N      area(e_point<N>, e_point<N>, e_point<N>);
template <typename N> rank_t rank(e_point<N> p);

template <typename N> class e_point {
  public:
    using inner_t = point<N>;

    e_point() {}
    e_point(inner_t _in, bool _inf = false) : inf{_inf}, inner{_in} {}
    e_point(N a, N b, bool _inf = false)
        : e_point(inner_t{a, b}, _inf) {}

    friend N      area<N>(e_point, e_point, e_point);
    friend rank_t rank<N>(e_point);

    friend std::ostream& operator<<(std::ostream&  out,
                                    const e_point& p) {
        if (p.inf) out << "*";
        out << p.inner;
        if (p.inf) out << "*";
        return out;
    }

  private:
    bool    inf;
    inner_t inner;
};

template <typename N> rank_t rank(e_point<N> p) {
    return static_cast<rank_t>(p.inf);
}

template <typename N, typename... Args>
rank_t rank(e_point<N> p, Args... args) {
    return rank(p) + rank(args...);
}

template <typename N>
N area(e_point<N> a, e_point<N> b, e_point<N> c) {
    const e_point<N> aux[]  = {a, b, c, a};
    N                ans[3] = {0, 0, 0};
    for (int i = 0; i < 3; i++)
        ans[rank(aux[i], aux[i + 1])] +=
            cross(aux[i].inner, aux[i + 1].inner);
    for (int i = 2; i + 1; i--)
        if (ans[i]) return ans[i];
    return N{0};
}

template <typename N> auto left(e_point<N> a, e_point<N> b) {
    return [a, b](const e_point<N>& c) { return area(a, b, c) > 0; };
}

template <typename N> struct EmptyInterior { e_point<N> a, b, c; };
template <typename N> struct NotCCW { e_point<N> a, b, c; };

template <typename N> class dag_t;
template <typename N> class triangle_t {
    using pt_t = e_point<N>;

    std::array<pt_t, 3>                        P;
    std::array<triangle_t<N>*, 3>              adj;
    std::array<std::unique_ptr<triangle_t>, 3> C;
    friend class dag_t<N>;

  public:
    triangle_t(pt_t a, pt_t b, pt_t c)
        : P{a, b, c}, adj{nullptr, nullptr, nullptr}, C{} {
        auto val = area(a, b, c);
        if (val == N{0}) throw EmptyInterior<N>{a, b, c};
        if (val < N{0}) throw NotCCW<N>{a, b, c};
    }

    triangle_t(const triangle_t& old, pt_t p) {
        (*this).P   = old.P;
        (*this).adj = old.adj;
        for (int i = 0; i < 3; i++) {
            pt_t aux[3];
            for (int j = 0; j < 3; j++)
                if (i == j)
                    aux[j] = p;
                else
                    aux[j] = P[j];
            C[i] =
                std::make_unique<triangle_t>(aux[0], aux[1], aux[2]);
        }
        for (int i = 0; i < 3; i++) {
            C[i]->adj[i] = adj[i];
            for (int j = (i + 1) % 3; j != i; j = (j + 1) % 3)
                C[i]->adj[j] = C[j].get();
        }
    }

    friend bool inside(const triangle_t& t, pt_t p) {
        bool ans = true;
        for (int i = 0, j = 1; i < 3; i++, j = (j + 1) % 3)
            ans = ans && !left(t.P[j], t.P[i])(p);
        return ans;
    }

    friend std::ostream& operator<<(std::ostream&     out,
                                    const triangle_t& t) {
        for (int i = 0; i < 3; i++) out << t.P[i];
        return out;
    }
};

template <typename N> class dag_t {
    using pt_t     = e_point<N>;
    using tri_t    = std::unique_ptr<triangle_t<N>>;
    using iterator = triangle_t<N>*;

  public:
    dag_t() {
        root = std::make_unique<triangle_t<N>>(pt_t{{-1, -1}, true},
                                               pt_t{{1, -1}, true},
                                               pt_t{{0, 1}, true});
    }

    iterator find(pt_t p) {
        iterator ans = root.get();
        while (ans->C[0]) {
            for (int i = 0; i < 3; i++)
                if (ans->C[i] && inside(*ans->C[i], p)) {
                    ans = ans->C[i].get();
                    break;
                }
        }
        return ans;
    }

    iterator insert(iterator pos, pt_t p) {
        *pos = {*pos, p};
        return pos;
    }
    iterator insert(pt_t p) { return insert(find(p), p); }

  private:
    tri_t root;
};

} // namespace geo
