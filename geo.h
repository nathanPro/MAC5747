#include <complex>

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
    e_point(N a, N b, bool _inf = false) : e_point(inner_t{a, b}, _inf) {}

    friend N      area<N>(e_point, e_point, e_point);
    friend rank_t rank<N>(e_point);

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

template <typename N> N area(e_point<N> a, e_point<N> b, e_point<N> c) {
    const auto       s     = std::min<rank_t>(2, rank(a, b, c));
    N                ans   = 0;
    const e_point<N> aux[] = {a, b, c, a};
    for (int i = 0; i < 3; i++)
        if (rank(aux[i], aux[i + 1]) == s)
            ans += cross(aux[i].inner, aux[i + 1].inner);
    return ans;
}

template <typename N> auto left(e_point<N> a, e_point<N> b) {
    return [a, b](const e_point<N>& c) { return area(a, b, c) > 0; };
}

} // namespace geo
