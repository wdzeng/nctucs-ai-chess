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

using std::max;
using std::min;
using std::pair;
using std::unordered_map;
using std::unordered_set;
using std::vector;

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
        if (h >= lim_maxx) return h;
        maxx = max(maxx, h);
    }
    return maxx;
}

double Minimax::next_o(const State& s /* reversed */, const double lim_minn, const int depth) {
    if (depth == 0) {
        return -1.0 * h(s);
    }

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
        if (h <= lim_minn) return h;
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
        if (hr < safe && hr <= SAFETY_HEURISTIC) {
            safe = hr;
            win_step = (Step*)&e;
        }
    }
    if (win_step) return *win_step;

    double minn = MAX_HEURISTIC;
    vector<Step*> candidates;

    // Serach tree
    for (auto&& e : expanded) {
        double h = next_x(e.first, minn, depth - 1);
        // std::cout << e.first << std::endl << h << std::endl;

        if (h < minn) {
            minn = h;
            candidates.clear();
        }
        if (h == minn) {
            candidates.push_back((Step*)&e);
        }
    }

    if (candidates.empty()) {
        auto&& it = record[s].find(s);
        return (Step&)*it;
    }
    return *candidates[rand() % candidates.size()];
}

#endif