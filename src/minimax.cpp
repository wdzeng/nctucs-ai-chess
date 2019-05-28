#ifndef minimax_hyperbola
#define minimax_hyperbola

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "calc.h"
#include "lang.h"
static const double EPSILON = 0.004;
static const double MIN_DOUBLE = 2 * EPSILON;

using std::max;
using std::min;
using std::pair;
using std::unordered_map;
using std::unordered_set;
using std::vector;

inline bool double_eq(const double& d1, const double& d2) { return abs(d1 - d2) <= EPSILON; }
inline bool double_geq(const double& d1, const double& d2) { return double_eq(d1, d2) || d1 > d2; }
inline bool double_leq(const double& d1, const double& d2) { return double_eq(d1, d2) || d1 < d2; }

double Minimax::next_x(const State& s, const double lim_maxx, const int depth) {
    if (depth == 0) return h(s);
    const Expansion& expanded = expand_state(s.opposite(), record);

    // Check leaves
    double safe = 0;
    for (const Step& e : expanded /* reversed */) {  //
        safe = min(safe, h(e.first));
    }
    // If X has some step that must be safe, select this step and stop searching
    if (safe <= SAFETY_HEURISTIC) return -1 * safe;

    // Search tree
    double maxx = MIN_HEURISTIC;  // Current o's heursistic (skip)
    for (const Step& e : expanded) {
        // std::cout << e.first;
        const double h = next_o(e.first, maxx, depth - 1);
        if (double_geq(h, lim_maxx)) return h + MIN_DOUBLE;
        maxx = max(maxx, h);
    }
    return maxx;
}

double Minimax::next_o(const State& s /* reversed */, const double lim_minn, const int depth) {
    if (depth == 0) return -1.0 * h(s);

    const Expansion& expanded = expand_state(s.opposite(), record);

    // Check leaves
    double safe = 0;
    for (const Step& e : expanded) {
        safe = min(safe, h(e.first));
    }
    // If O has a step that must be safe, select this step
    if (safe <= SAFETY_HEURISTIC) return safe;

    // Search tree
    double minn = MAX_HEURISTIC;  // Convert player x's heuristic ot o's heuristic
    for (const Step& e : expanded) {
        const double h = next_x(e.first, minn, depth - 1);
        if (double_leq(h, lim_minn)) return h - MIN_DOUBLE;
        minn = min(minn, h);
    }
    return minn;
}

const Step& Minimax::best_step(const State& s) {
    const Expansion& expanded = expand_state(s, record);

    // Check leaves
    double safe = 0;
    Step* win_step = NULL;
    for (auto&& e : expanded) {
        double hr = h(e.first);
        if (double_leq(hr, safe) && double_leq(hr, SAFETY_HEURISTIC)) {
            safe = hr;
            win_step = (Step*)&e;
        }
    }
    if (win_step) return *win_step;

    // Serach tree
    double minn = MAX_HEURISTIC;
    vector<Step*> candidates;
    int d_depth = depth;
    const int ssize = s.o_pieces().size() + s.x_pieces().size();

    // Determine the depth
    if (ssize <= 4)
        d_depth += 6;
    else if (ssize <= 5)
        d_depth += 5;
    else if (ssize <= 6)
        d_depth += 4;
    else if (ssize <= 7)
        d_depth += 3;
    else if (ssize <= 9)
        d_depth += 2;
    else if (ssize <= 13)
        d_depth += 1;

    for (auto&& e : expanded) {
        double h = next_x(e.first, minn, d_depth - 1);
        if (h < minn) {
            minn = h;
            candidates.clear();
        }
        if (double_eq(h, minn)) {
            candidates.push_back((Step*)&e);
        }
    }

    if (candidates.empty()) {
        auto&& it = record[s].find(s);
        return (Step&)*it;
    }

    // there are multiple candidats
    // choose whose diff is maximize
    vector<Step**> elected;
    int maxdiff = -100;
    for (auto& ss : candidates) {
        int diff = ss->first.o_pieces().size() - ss->first.x_pieces().size();
        if (diff > maxdiff) {
            elected.clear();
            maxdiff = diff;
        }
        if (diff == maxdiff) elected.push_back(&ss);
    }
    return **elected[rand() % elected.size()];
}

#endif