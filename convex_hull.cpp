#include <algorithm>
#include <complex>
#include <iostream>
#include <iterator>
#include <tuple>
#include <vector>

namespace geo {

template <typename T> using point = std::complex<T>;

auto lex_compare = [](const auto& a, const auto& b) {
    return std::imag(a) == std::imag(b) ? std::real(a) < std::real(b)
                                        : std::imag(a) < std::imag(b);
};

template <typename T> T area(point<T> a, point<T> b, point<T> c) {
    return std::imag(std::conj(b - a) * (c - a));
}

template <typename T> auto area_compare(point<T> p, point<T> q) {
    return [p, q](const point<T>& a, const point<T>& b) {
        return area(p, q, a) < area(p, q, b);
    };
}

template <typename T> auto angle(point<T> p) {
    return [p](const point<T>& a, const point<T>& b) {
        if (a == p) return false;
        if (b == p) return true;
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
    std::iter_swap(q, std::max_element(fst, lst, area_compare(*r, *p)));

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
    std::iter_swap(fst, std::min_element(fst, lst, lex_compare));
    std::iter_swap(std::prev(lst),
                   std::max_element(std::next(fst), lst, angle(*fst)));
    return __detail::qhull(fst, lst);
}

template <typename It> It gift_wrapping(It fst, It lst) {
    if (distance(fst, lst) <= 2) return lst;
    std::iter_swap(fst, std::min_element(fst, lst, lex_compare));

    It out = fst;
    for (It nxt = std::min_element(fst, lst, angle(*out)); nxt != fst;
         nxt    = std::min_element(fst, lst, angle(*out)))
        std::iter_swap(++out, nxt);
    return ++out;
}

template <typename It> It graham(It fst, It lst) {
    if (distance(fst, lst) <= 2) return lst;
    std::iter_swap(fst, std::min_element(fst, lst, lex_compare));
    std::sort(std::next(fst), lst, angle(*fst));

    size_t hull_size = 1;
    It     out       = fst;
    for (auto i = std::next(fst); i != lst; ++i) {
        while (hull_size > 1 && !left(*std::prev(out), *out)(*i))
            --out, --hull_size;
        std::iter_swap(++out, i);
        hull_size++;
    }
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
