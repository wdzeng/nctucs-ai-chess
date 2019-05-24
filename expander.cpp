#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "lang.h"
#define EX_LEFT 1
#define EX_RIGHT 2
#define EX_BOTTOM 3
#define EX_TOP 4
#define qi std::piecewise_construct
#define ls std::forward_as_tuple

using std::pair;
using std::unordered_map;
using std::vector;

vector<int> &replace_val(vector<int> &v, int src, int dest) {
    for (int &i : v) {
        if (i == src) {
            i = dest;
            break;
        }
    }
    return v;
}

// ---------------------
void move(const State &, int, Expansion &);
// void move(const State &, int, unordered_set<State> &);
void hop(const State &, int, Expansion &, const vector<int> &, int);
// void hop(const State &, int, unordered_set<State> &, int);
void move_to(const State &, const int, int (*)(int), Expansion &);
// void move_to(const State &, const int, int (*)(int), unordered_set<State> &);
void hop_to(const State &, const int, int (*)(int), int, Expansion &, const vector<int> &);
// void hop_to(const State &, const int, int (*)(int), int, unordered_set<State> &);

inline bool valid_move(const State &s, int src, int dest) {
    //
    return is_valid_index(dest) && ox(dest, s.o_pieces(), s.x_pieces()) == NONE;
}

void move_to(const State &s, const int origin, int (*dir)(int), Expansion &mapp) {
    int dest = dir(origin);
    if (!valid_move(s, origin, dest)) return;

    auto updated_o = s.o_pieces();
    replace_val(updated_o, origin, dest);

    // quick implementaion
    auto res = mapp.emplace(qi, ls(updated_o, s.x_pieces(), true, false), ls());
    if (res.second) {
        auto &vec = res.first->second;
        vec.push_back(origin);
        vec.push_back(dest);
    }
    // State inserted(o, s.x_pieces(), true, false);
    // mapp[inserted] = {origin, dest};
}

/*
void move_to(const State &s, const int origin, int (*dir)(int), unordered_set<State> &sett) {
    int dest = dir(origin);
    if (!valid_move(s, origin, dest)) return;
    auto o = s.o_pieces();
    replace_val(o, origin, dest);
    sett.insert(State(o, s.x_pieces(), true, false));
}
*/

bool valid_hop(const State &s, int src, int dest, int &shelf_col) {
    // check destination
    if (!is_valid_index(dest) || ox(dest, s.o_pieces(), s.x_pieces()) != NONE) return false;
    // check shelf
    int shelf_pos = (src + dest) / 2;
    if (!is_valid_index(shelf_pos) || (shelf_col = ox(shelf_pos, s.o_pieces(), s.x_pieces())) == NONE) return false;
    return true;
}

void update_hopping_path(vector<int> &inserted, const vector<int> path, int dest) {
    inserted.insert(inserted.end(), path.begin(), path.end());
    inserted.push_back(dest);
}

void hop_to(const State &s, const int origin, int (*dir)(int), int ex, Expansion &mapp, const vector<int> &path) {
    int shelf_pos = dir(origin), dest = dir(shelf_pos), shelf_col;
    if (!valid_hop(s, origin, dest, shelf_col)) return;

    // quick implemetation, but messy
    vector<int> updated_o = s.o_pieces();
    replace_val(updated_o, origin, dest);

    if (shelf_col == X) {
        vector<int> x = s.x_pieces();
        x.erase(find(x.begin(), x.end(), shelf_pos));
        auto res = mapp.emplace(qi, ls(updated_o, x, true, false), ls());
        if (!res.second) return;

        auto &next_state = res.first->first;
        auto &vec = res.first->second;
        update_hopping_path(res.first->second, path, dest);
        hop(next_state, dest, mapp, vec, ex);
    }

    //
    else {
        auto res = mapp.emplace(qi, ls(updated_o, s.x_pieces(), true, false), ls());
        if (!res.second) return;

        auto &next_state = res.first->first;
        auto &vec = res.first->second;
        update_hopping_path(res.first->second, path, dest);
        hop(next_state, dest, mapp, vec, ex);
    }
}

/*
void hop_to(const State &s, const int origin, int (*dir)(int), int ex, unordered_set<State> &sett) {
    const int shelf = (*dir)(origin);
    if (!is_valid_index(shelf)) return;
    const int shelf_col = s.piece_at(shelf);
    if (shelf_col == NONE) return;
    const int dest = (*dir)(shelf);
    if (!is_valid_index(dest) || s.piece_at(dest) != NONE) return;

    auto o = s.o_pieces(), x = s.x_pieces();
    if (shelf_col == X) x.erase(find(x.begin(), x.end(), shelf));
    replace_val(o, origin, dest);

    State new_state(o, x, true, false);
    if (sett.insert(new_state).second) {
        hop(new_state, dest, sett, ex);
    }
}
*/

void move(const State &s, int origin, Expansion &mapp) {
    move_to(s, origin, left, mapp);
    move_to(s, origin, right, mapp);
    move_to(s, origin, top, mapp);
    move_to(s, origin, bottom, mapp);
}

/*
void move(const State &s, int origin, unordered_set<State> &sett) {
    move_to(s, origin, left, sett);
    move_to(s, origin, right, sett);
    move_to(s, origin, top, sett);
    move_to(s, origin, bottom, sett);
}
*/

void hop(const State &s, int origin, Expansion &mapp, const vector<int> &path, int ex = 0) {
    if (ex != EX_LEFT) hop_to(s, origin, left, EX_RIGHT, mapp, path);
    if (ex != EX_RIGHT) hop_to(s, origin, right, EX_LEFT, mapp, path);
    if (ex != EX_TOP) hop_to(s, origin, top, EX_BOTTOM, mapp, path);
    if (ex != EX_BOTTOM) hop_to(s, origin, bottom, EX_TOP, mapp, path);
}

/*
void hop(const State &s, int origin, unordered_set<State> &sett, int ex = 0) {
    if (ex != EX_LEFT) hop_to(s, origin, left, EX_RIGHT, sett);
    if (ex != EX_RIGHT) hop_to(s, origin, right, EX_LEFT, sett);
    if (ex != EX_TOP) hop_to(s, origin, top, EX_BOTTOM, sett);
    if (ex != EX_BOTTOM) hop_to(s, origin, bottom, EX_TOP, sett);
}
*/

// ---------

/*
Expansion expand_state(const State &s) {
    // Add cuurent state to prevent hopping back here
    Expansion mapp({{s, {}}});
    for (int index : s.o_pieces()) {
        // first hop then move
        hop(s, index, mapp, {index});
        move(s, index, mapp);
    }
    mapp.erase(s);
    return mapp;
}
*/

const Expansion &expand_state(const State &s, Record &record) {
    auto it = record.emplace(s, Expansion());
    if (!it.second) {
        return it.first->second;
    }

    // quick implementation
    Expansion &mapp = it.first->second;
    mapp[s] = {};
    for (int index : s.o_pieces()) {
        // first hop then move
        hop(s, index, mapp, {index});
        move(s, index, mapp);
    }
    mapp.erase(s);
    return mapp;
}

/*
unordered_set<State> expand_state_without_path(const State &s) {
    // Add cuurent state to prevent hopping back here
    unordered_set<State> sett({s});
    for (int index : s.o_pieces()) {
        // first hop then move
        hop(s, index, sett);
        move(s, index, sett);
    }
    sett.erase(s);
    return sett;
}
*/