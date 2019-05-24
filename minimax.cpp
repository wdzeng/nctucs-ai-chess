#ifndef minimax_hyperbola
#define minimax_hyperbola

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <iostream>
#include "calc.h"
#include "lang.h"

using std::max;
using std::min;
using std::pair;
using std::unordered_map;
using std::unordered_set;
using std::vector;

double Minimax::next_x(const State& s, const double lim_maxx, const int depth) {
    if (depth == 0) return h(s);
    auto expanded = expand_state(s.opposite(), record);

    // Check leaves
    for (const Step& e : expanded /* reversed */) {
        if (h(e.first) == MIN_HEURISTIC)  // which means x must win.
            return MAX_HEURISTIC;
    }

    // Search tree
    double maxx = MIN_HEURISTIC;
    for (const Step& e : expanded) {
        const double h = next_o(e.first, maxx, depth - 1);
        if (h >= lim_maxx) return h;
        maxx = max(maxx, h);
    }
    return maxx;
}

double Minimax::next_o(const State& s /* reversed */, const double lim_minn, const int depth) {
    if (depth == 0) return -1.0 * h(s);  // Reversed
    auto expanded = expand_state(s.opposite(), record);

    // Check leaves
    for (const Step& e : expanded) {
        if (h(e.first) == MIN_HEURISTIC) return MIN_HEURISTIC;
    }

    // Search tree
    double minn = MAX_HEURISTIC;
    for (const Step& e : expanded) {
        const double h = next_x(e.first, minn, depth - 1);
        if (h <= lim_minn) return h;
        minn = min(minn, h);
    }
    return minn;
}

const Step& Minimax::best_step(const State& s) {
    const Expansion expanded = expand_state(s, record);

    // Check leaves
    for (const Step& e : expanded) {
        if (h(e.first) == MIN_HEURISTIC) return e;
    }

    double minn = MAX_HEURISTIC;
    vector<Step> candidates;
    // Serach tree
    for (const Step& e : expanded) {
        double h = next_x(e.first, minn, depth - 1);
        // Check if this step must lead to winning
        if (h == MIN_HEURISTIC) return e;
        if (h < minn) {
            minn = h;
            candidates.clear();
        }
        if (h == minn) {
            candidates.push_back(e);
        }
    }

    return candidates[rand() % candidates.size()];
}

#endif