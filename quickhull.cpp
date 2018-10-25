#include <algorithm>
#include <complex>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <vector>

namespace gc {

template <typename It> struct RangeLog {
    It                fst;
    It                lst;
    const std::string msg;

    RangeLog(It _f, It _l) : fst(_f), lst(_l) { print(); }
    RangeLog(It _f, It _l, std::string in) : fst(_f), lst(_l), msg(in) {
        print();
    }
    ~RangeLog() { print(); }

  private:
    void print() {
        if (msg.size()) std::cout << msg << std::endl;
        for (auto it = fst; it != lst; ++it) std::cout << *it << " ";
        std::cout << "\n";
    }
};

template <typename T> using point = std::complex<T>;

template <typename T> bool lex_compare(point<T> a, point<T> b) {
    return std::imag(a) == std::imag(b) ? std::real(a) < std::real(b)
                                        : std::imag(a) < std::imag(b);
}

template <typename T> T cross(point<T> a, point<T> b) {
    return std::imag(std::conj(a) * b);
}

template <typename T> T area(point<T> a, point<T> b, point<T> c) {
    return cross(b - a, c - a);
}

template <typename T> auto area_compare(point<T> p, point<T> q) {
    return [=](const point<T>& a, const point<T>& b) {
        return abs(area(p, q, a)) < abs(area(p, q, b));
    };
}

template <typename T> auto left(point<T> p, point<T> q) {
    return [=](const point<T>& a) { return area(p, q, a) > 0; };
}

template <typename It> std::pair<It, It> partition(It fst, It lst) {
    using pt = typename std::iterator_traits<It>::value_type;

    const It p = fst++;
    const It r = --lst;
    std::iter_swap(fst, std::max_element(fst, lst, area_compare(*p, *r)));
    const It q = fst++;

    fst = std::partition(fst, lst, left(*p, *q));
    lst = std::partition(fst, lst, left(*q, *r));
    std::iter_swap(lst++, r);
    std::iter_swap(--fst, q);

    return {fst, lst};
}

template <typename It> It _qhull(It fst, It lst) {
    if (distance(fst, lst) <= 3) return lst;
    It mid;
    std::tie(mid, lst) = partition(fst, lst);

    It lhs = _qhull(fst, std::next(mid));
    return _qhull(mid, std::rotate(lhs, std::next(mid), lst));
}

template <typename It> It quick_hull(It fst, It lst) {
    using pt = typename std::iterator_traits<It>::value_type;

    std::iter_swap(
        fst, std::min_element(fst, lst, [&](const pt& a, const pt& b) {
            return lex_compare(a, b);
        }));
    std::iter_swap(std::prev(lst),
                   std::max_element(std::next(fst), lst,
                                    [&](const pt& a, const pt& b) {
                                        return area(*fst, a, b) < 0;
                                    }));
    return _qhull(fst, lst);
}

} // namespace gc

int main() {
    using pt = gc::point<int>;
    int n;
    scanf(" %d", &n);
    std::vector<pt> P(n);
    for (int i = 0; i < n; i++) {
        int x, y;
        scanf(" %d%d", &x, &y);
        P[i] = {x, y};
    }

    P.erase(gc::quick_hull(std::begin(P), std::end(P)), std::end(P));
    for (auto it : P) std::cout << it << std::endl;
}
