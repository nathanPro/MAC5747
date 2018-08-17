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

namespace ShamosAndHoey {
struct sol {
    int _fst = numeric_limits<int>::max(), _i = 0, _j = 0;
    sol() {}
    sol(vector<int>& X, vector<int>& Y, int i, int j)
        : _fst(norm(complex<int>(X[i] - X[j], Y[i] - Y[j]))), _i(i),
          _j(j) {}
    int dist() { return _fst; }
};
bool operator<(const sol& lhs, const sol& rhs) {
    return lhs._fst < rhs._fst;
}

template <typename It, typename Ot>
Ot filter_distant(vector<int>& X, int x, int d, It fst, It end, Ot out) {
    auto criteria = [&](int i){ return norm<int>(X[i] - x) <= d; };
    fst = find_if(fst, end, criteria);
    while (fst != end) {
        *out = *fst;
        ++out;
        fst = find_if(fst, end, criteria);
    }
    return out;
}

template <typename It>
sol combine(vector<int>& X, vector<int>& Y, It l, It r, It s, It t, sol bst) {
    for (auto i = l; i != r; ++i) {
        while (s != t && Y[*s] <= Y[*i] &&
               bst.dist() < norm<int>(Y[*s] - Y[*i]))
            ++s;
        for (auto j = s;
             j != t && norm<int>(Y[*i] - Y[*j]) < bst.dist(); ++j)
            bst = min(bst, sol(X, Y, *i, *j));
    }
    return bst;
}

template <typename It>
sol rec(vector<int>& X, vector<int>& Y, It l, It r) {
    const int n = r - l;
    if (n == 1) return {};
    if (n == 2) {
        if (Y[l[1]] < Y[l[0]]) swap(l[0], l[1]);
        return sol(X, Y, l[0], l[1]);
    }
    It m     = l + (n / 2);
    auto bst = min(rec(X, Y, l, m), rec(X, Y, m, r));
    bst      = min(bst, combine(X, Y, l, m, begin(aux), a_end, bst));
    bst      = min(bst, combine(X, Y, m, r, begin(aux), a_end, bst));
    inplace_merge(l, m, r, [&](int i, int j) { return Y[i] < Y[j]; });
    return bst;
}

sol closest_pair(vector<int>& X, vector<int>& Y) {
    const int n = X.size();
    vector<int> p(n);
    for (int i = 0; i < n; i++) p[i] = i;
    sort(begin(p), end(p), [&](int i, int j) { return X[i] < X[j]; });
    return rec(X, Y, begin(p), end(p));
}
} // namespace ShamosAndHoey

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);

    const int oo = 1e8;

    int n;
    while (cin >> n && n) {
        vector<int> x(n), y(n);
        for (int i = 0; i < n; i++) cin >> x[i] >> y[i];
        int ans = ShamosAndHoey::closest_pair(x, y).dist();
        if (oo <= ans)
            write(cout, "INFINITY");
        else
            cout << fixed << setprecision(4) << sqrt(ans) << '\n';
    }
}
