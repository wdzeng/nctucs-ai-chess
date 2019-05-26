#include <algorithm>
#include <vector>
#include "lang.h"
using std::max;
using std::min;
using std::vector;

double heuristic_o(const vector<int> &o, const int no, const int nx, int &oarr, int &odist) {
    const int t_size = min(no, nx + 1);
    oarr = 0;
    odist = 0;
    if (t_size == 0) return 0;

    int i = no - 1, hr = 0;
    for (; i >= no - t_size; i--) {
        int d = max(0, 6 - row(o[i]));
        if (d == 0) oarr++;
        hr += d;
        odist += d;
    }
    for (; i >= 0; i--) {
        odist += max(0, 6 - row(o[i]));
    }
    return (double)hr / t_size;
}

double heuristic_x(const vector<int> &x, const int no, const int nx, int &xarr, int &xdist) {
    const int t_size = min(no + 1, nx);
    xarr = 0;
    xdist = 0;
    if (t_size == 0) return 0;

    int i = 0, hr = 0;
    for (int i = 0; i < t_size; i++) {
        int d = max(0, row(x[i]) - 1);
        xdist += d;
        hr += d;
        if (d == 0) xarr++;
    }
    for (; i < nx; i++) {
        xdist += max(0, row(x[i]) - 1);
    }
    return (double)hr / t_size;
}

double simple_heuristic(const State &s) {
    const int no = s.o_pieces().size(), nx = s.x_pieces().size();
    int oarr, xarr, odist, xdist;

    double ho = heuristic_o(s.o_pieces(), no, nx, oarr, odist);
    double hx = heuristic_x(s.x_pieces(), no, nx, xarr, xdist);

    if (oarr == no || xarr == nx) {
        if (oarr > xarr) return MIN_HEURISTIC;
        if (oarr < xarr) return MAX_HEURISTIC;
        return 0;
    }

    if (oarr == nx) {
        // O must win or draw, heuristic in (-1, 0)
        return -1.0 + (double)odist / (no * 6);
    }
    if (xarr == no) {
        // X must win or draw, heuristic in (0, +1)
        return +1.0 - (double)xdist / (nx * 6);
    }
    if (oarr >= nx) {
        // O must win, heuristic in [-2, -1]
        return MIN_HEURISTIC + (double)odist / (no * 6);
    }
    if (xarr >= no) {
        // X must win, heuristic in [+1, +2]
        return MAX_HEURISTIC - (double)xdist / (nx * 6);
    }

    // Heuristic in range (-1, +1)
    return (ho - hx) / (ho + hx);
}