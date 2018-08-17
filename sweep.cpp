#include "bits/stdc++.h"
using namespace std;
template <typename T> using iit = istream_iterator<T>;
template <typename T> using oit = ostream_iterator<T>;
template <typename T> void write(ostream& out, T t) {
    out << t << '\n';
}
template <typename T, typename... Args>
void write(ostream& out, T t, Args... args) {
    out << t << ' ';
    write(out, args...);
}

template <typename cood> struct point {
    using value_type = cood;
    cood x;
    cood y;

    operator complex<cood>() { return {x, y}; }
};
template <typename cood>
bool operator<(const point<cood>& lhs, const point<cood>& rhs) {
    return lhs.x < rhs.x;
}
template <typename cood> cood distance_sqr(const cood& lhs) {
    return norm(complex<cood>(lhs));
}
template <typename cood>
cood distance_sqr(const point<cood>& lhs, const point<cood>& rhs) {
    return distance_sqr(lhs.x - rhs.x) + distance_sqr(lhs.y - rhs.y);
}
template <typename It> bool y_compare(It lhs, It rhs) {
    return lhs->y < rhs->y;
}

template <typename It> class Strip {
    using pt   = typename It::value_type;
    using cood = typename pt::value_type;
    cood d;
    queue<It> range;
    set<It, bool (*)(It, It)> y_set;

  public:
    Strip()
        : d(numeric_limits<cood>::max()), range(),
          y_set(&y_compare<It>) {}
    cood value() const { return d; }
    void insert(It point) {
        while (!range.empty() &&
               d < distance_sqr(point->x - range.front()->x)) {
            y_set.erase(range.front());
            range.pop();
        }
        range.push(point);

        auto lb = y_set.lower_bound(point);
        for (auto it = lb; it != end(y_set); ++it) {
            if (d < distance_sqr((*it)->y - point->y)) break;
            d = min(d, distance_sqr(**it, *point));
        }
        for (auto _it = lb; _it != begin(y_set); --_it) {
            const auto it = prev(_it);
            if (d < distance_sqr((*it)->y - point->y)) break;
            d = min(d, distance_sqr(**it, *point));
        }
        y_set.insert(range.back());
    }
};

using strip_t = Strip<vector<point<int>>::iterator>;
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    const int oo = 1e8;

    int n;
    while (cin >> n && n) {
        vector<point<int>> P(n);
        for (int i = 0; i < n; i++) {
            int x, y;
            cin >> x >> y;
            P[i] = {x, y};
        }
        strip_t seen;
        sort(begin(P), end(P));
        for (auto i = begin(P); i != end(P); ++i) seen.insert(i);
        if(oo <= seen.value())
            write(cout, "INFINITY");
        else
            cout << fixed << setprecision(4) << sqrt(seen.value()) << '\n';
    }
}
