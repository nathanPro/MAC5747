#include <algorithm>
#include <complex>
#include <iostream>
#include <iterator>
#include <tuple>
#include <vector>

namespace order {

template <typename T> auto from_sup(T sup) {
    return [sup](const T&, const T& b) { return (b == sup); };
}

template <typename T> auto from_inf(T inf) {
    return [inf](const T& a, const T&) { return (a == inf); };
}

template <typename Callable> auto dual(Callable&& f) {
    return [f](const auto& a, const auto& b) { return f(b, a); };
}

template <typename Callable> auto compose(Callable&& f) { return f; }

template <typename Callable, typename... Args>
auto compose(Callable&& f, Args... args) {
    return [f, args...](const auto& a, const auto& b) {
        if (f(a, b)) return true;
        if (f(b, a)) return false;
        return compose(args...)(a, b);
    };
}

} // namespace order

namespace geo {

template <typename T> using point = std::complex<T>;

template <typename T> T area(point<T> a, point<T> b, point<T> c) {
    return std::imag(std::conj(b - a) * (c - a));
}

const auto by_x = [](const auto& a, const auto& b) {
    return std::real(a) < std::real(b);
};

const auto by_y = [](const auto& a, const auto& b) {
    return std::imag(a) < std::imag(b);
};

const auto by_lex = order::compose(by_y, by_x);

template <typename T> auto by_distance(point<T> p) {
    return [p](const point<T>& a, const point<T>& b) {
        return std::norm(a - p) < std::norm(b - p);
    };
}

template <typename T> auto by_area(point<T> p, point<T> q) {
    return [p, q](const point<T>& a, const point<T>& b) {
        return area(p, q, a) < area(p, q, b);
    };
}

template <typename T> auto by_angle(point<T> p) {
    return [p](const point<T>& a, const point<T>& b) {
        return area(p, a, b) > 0;
    };
}

template <typename T> auto left(point<T> p, point<T> q) {
    return [p, q](const point<T>& a) { return area(p, q, a) > 0; };
}

namespace __detail {

template <typename It> std::pair<It, It> partition(It fst, It lst) {
    const It p = fst++;
    const It q = fst++;
    const It r = --lst;

    std::iter_swap(
        q, std::max_element(fst, lst,
                            order::compose(by_area(*r, *p), by_lex)));

    fst = std::partition(fst, lst, left(*q, *p));
    lst = std::partition(fst, lst, left(*r, *q));
    std::iter_swap(lst++, r);
    std::iter_swap(--fst, q);

    return {fst, lst};
}

template <typename It> It qhull(It fst, It lst) {
    if (distance(fst, lst) <= 3) return lst;
    It mid;
    std::tie(mid, lst) = partition(fst, lst);
    lst                = qhull(mid, lst);
    return std::rotate(qhull(fst, std::next(mid)), std::next(mid), lst);
}

} // namespace __detail

template <typename It> It quick_hull(It fst, It lst) {
    if (distance(fst, lst) <= 2) return lst;
    std::iter_swap(fst, std::min_element(fst, lst, by_lex));
    std::iter_swap(std::prev(lst),
                   std::max_element(
                       std::next(fst), lst,
                       order::compose(by_angle(*fst), by_distance(*fst))));
    return __detail::qhull(fst, lst);
}

template <typename It> It gift_wrapping(It fst, It lst) {
    if (distance(fst, lst) <= 2) return lst;
    std::iter_swap(fst, std::min_element(fst, lst, by_lex));

    It out = fst;

    const auto criteria = [](const auto& p) {
        return order::compose(order::from_sup(p), by_angle(p),
                              order::dual(by_distance(p)));
    };

    for (It nxt = std::min_element(fst, lst, criteria(*out)); nxt != fst;
         nxt    = std::min_element(fst, lst, criteria(*out)))
        std::iter_swap(++out, nxt);
    return ++out;
}

template <typename It> It graham(It fst, It lst) {
    if (distance(fst, lst) <= 2) return lst;
    std::iter_swap(fst, std::min_element(fst, lst, by_lex));
    std::sort(std::next(fst), lst,
              order::compose(by_angle(*fst), by_distance(*fst)));

    size_t hull_size = 1;
    It     out       = fst;
    for (auto i = std::next(fst); i != lst; ++i) {
        while (hull_size > 1 && !left(*std::prev(out), *out)(*i))
            --out, --hull_size;
        std::iter_swap(++out, i);
        hull_size++;
    }
    while (hull_size > 2 && !left(*std::prev(out), *out)(*fst))
        --out, --hull_size;
    return ++out;
}

} // namespace geo

int main() {
    using pt = geo::point<int>;

    int n;
    std::cin >> n;
    std::vector<pt> P(n);
    for (int i = 0; i < n; i++) {
        int x, y;
        std::cin >> x >> y;
        P[i] = {x, y};
    }

    std::vector<pt> Q = P;

    P.erase(geo::quick_hull(std::begin(P), std::end(P)), std::end(P));
    std::cout << "Quickhull:\n";
    for (auto it : P) std::cout << it << std::endl;

    P = Q;
    P.erase(geo::gift_wrapping(std::begin(P), std::end(P)), std::end(P));
    std::cout << "Gift wrapping:\n";
    for (auto it : P) std::cout << it << std::endl;

    P = Q;
    P.erase(geo::graham(std::begin(P), std::end(P)), std::end(P));
    std::cout << "Graham:\n";
    for (auto it : P) std::cout << it << std::endl;
}
