#include "geo.h"
#include <cassert>
#include <iostream>
#include <random>
#include <vector>

int main() {
    std::default_random_engine         src;
    std::uniform_int_distribution<int> dist(-1e4, 1e4);

    {
        geo::e_point<int> a{0, 2}, b{1, 2}, c{1, 1},
            inf_c{{1, 1}, true};
        assert(!geo::left(a, b)(c));
        assert(geo::left(a, b)(inf_c));
    }

    int n, m;
    scanf(" %d%d", &n, &m);
    std::vector<geo::point<int>>   P(m);
    std::vector<geo::e_point<int>> Q(m);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++)
            Q[j] = P[j] = {dist(src), dist(src)};

        for (int j = 0; j < m; j++)
            for (int k = 0; k < m; k++)
                for (int l = 0; l < m; l++)
                    assert(geo::area(P[j], P[k], P[l]) ==
                           geo::area(Q[j], Q[k], Q[l]));

        for (int j = 0; j < m; j++) Q[j] = {P[j], true};

        for (int j = 0; j < m; j++)
            for (int k = 0; k < m; k++)
                for (int l = 0; l < m; l++)
                    assert(geo::area(P[j], P[k], P[l]) ==
                           geo::area(Q[j], Q[k], Q[l]));

        auto tst = geo::left<int>({0, 0}, {1, 0});
        auto cmp = geo::left<int>({0, 0}, {{1, 0}, true});

        for (int j = 0; j < m; j++) assert(cmp(P[j]) == tst(P[j]));

        printf("%d passed\n", i);
    }
    {
        using dag_t = geo::dag_t<int>;
        using pt_t  = geo::e_point<int>;
        dag_t division{};
        division.insert({0, 0});
        division.insert({2, 0});
        division.insert({2, 2});

        int n;
        scanf(" %d", &n);
        while (n--) {
            int x, y;
            scanf(" %d%d", &x, &y);
            auto it = division.find(pt_t{x, y});
            std::cout << *it << std::endl;
        }
    }
}
