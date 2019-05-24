#include <algorithm>
#include <vector>
#include "calc.h"
#include "lang.h"
using std::max;
using std::min;
using std::vector;

double heuristic_o(const vector<int> &v, int n_enemy) {
    int my_size = v.size();
    int xise = min(my_size, n_enemy + 1);
    if (xise == 0) return 0;
    int score = 0;
    for (int i = 1; i <= xise; i++) {
        score += max(0, 6 - row(v[xise - i]));
    }
    return (double)score / xise;
}

double heuristic_x(const vector<int> &v, int n_mine) {
    int e_size = v.size();
    int xise = min(e_size, n_mine + 1);
    if (xise == 0) return 0;
    int score = 0;
    for (int i = 0; i < xise; i++) {
        score += max(0, row(v[i]) - 1);
    }
    return (double)score / xise;
}

double simple_heuristic(const State &s) {
    double ho = heuristic_o(s.o_pieces(), s.x_pieces().size());
    double hx = heuristic_x(s.x_pieces(), s.o_pieces().size());
    if (ho == 0 || hx == 0) {
        int me_dest = 0, e_dest = 0;
        for (int i : s.o_pieces())
            if (row(i) >= 6) me_dest++;
        for (int i : s.x_pieces())
            if (row(i) <= 1) e_dest++;
        if (me_dest == e_dest) return 0.0;
        return me_dest > e_dest ? -1.0 : 1.0;
    }
    return (ho - hx) / (ho + hx);
}