#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "lang.h"
#define EX_LEFT 0x123456
#define EX_RIGHT ~EX_LEFT
#define EX_BOTTOM 0xbababa
#define EX_TOP ~EX_BOTTOM
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
void move(const vector<int> &, const vector<int> &, int, Expansion &);
void hop(const vector<int> &, const vector<int> &, int, Expansion &, const vector<int> &, int);
void move_to(const vector<int> &, const vector<int> &, const int, int (*)(int), Expansion &);
void hop_to(const vector<int> &, const int, int (*)(int), int, Expansion &, const vector<int> &);

bool valid_move(const vector<int> &o, const vector<int> &x, int src, int dest) {  //
    return is_valid_index(dest) && ox(dest, o, x) == NONE;
}

bool valid_hop(const vector<int> &o, const vector<int> &x, int src, int dest, int &shelf_col) {
    // check destination
    if (!is_valid_index(dest) || ox(dest, o, x) != NONE) return false;
    // check shelf
    int shelf_pos = (src + dest) >> 1;  // half
    if (!is_valid_index(shelf_pos) || (shelf_col = ox(shelf_pos, o, x)) == NONE) return false;
    return true;
}

void move_to(const vector<int> &o, const vector<int> &x, const int src, int (*dir)(int), Expansion &mapp) {
    int dest = dir(src);
    if (!valid_move(o, x, src, dest)) return;

    vector<int> updated_o = o;  // copy
    replace_val(updated_o, src, dest);

    // quick implementaion
    const auto &res = mapp.emplace(qi, ls(updated_o, x, true, false), ls());
    if (res.second) {
        // New child is found
        auto &next_path = res.first->second;
        next_path.push_back(src);
        next_path.push_back(dest);
    }
}

void update_hopping_path(vector<int> &inserted, const vector<int> &path, int dest) {
    inserted.insert(inserted.end(), path.begin(), path.end());  // copy
    inserted.push_back(dest);                                   // append
}

void hop_to(const vector<int> &o, const vector<int> &x, const int src, int (*dir)(int), int ex, Expansion &mapp,
            const vector<int> &path) {
    int shelf_pos = dir(src), dest = dir(shelf_pos), shelf_col;
    if (!valid_hop(o, x, src, dest, shelf_col)) return;

    // quick implemetation
    vector<int> updated_o = o;  // copy
    replace_val(updated_o, src, dest);

    if (shelf_col == X) {  // delete the hopped piece

        vector<int> updated_x = x;  // copy
        updated_x.erase(find(x.begin(), x.end(), shelf_pos));
        const auto &res = mapp.emplace(qi, ls(updated_o, updated_x, true, false), ls());
        if (!res.second) return;

        // New child is found
        auto &next_path = res.first->second;
        update_hopping_path(next_path, path, dest);
        hop(updated_o, updated_x, dest, mapp, next_path, ex);
    }

    //
    else {  // hopped piece has same color

        const auto res = mapp.emplace(qi, ls(updated_o, x, true, false), ls());
        if (!res.second) return;

        // New child is found
        auto &next_path = res.first->second;
        update_hopping_path(next_path, path, dest);
        hop(updated_o, x, dest, mapp, next_path, ex);
    }
}

void move(const vector<int> &o, const vector<int> &x, int src, Expansion &mapp) {
    move_to(o, x, src, left, mapp);
    move_to(o, x, src, right, mapp);
    move_to(o, x, src, top, mapp);
    move_to(o, x, src, bottom, mapp);
}

void hop(const vector<int> &o, const vector<int> &x, int src, Expansion &mapp, const vector<int> &path, int ex = 0) {
    if (ex != EX_LEFT) hop_to(o, x, src, left, EX_RIGHT, mapp, path);
    if (ex != EX_RIGHT) hop_to(o, x, src, right, EX_LEFT, mapp, path);
    if (ex != EX_TOP) hop_to(o, x, src, top, EX_BOTTOM, mapp, path);
    if (ex != EX_BOTTOM) hop_to(o, x, src, bottom, EX_TOP, mapp, path);
}

const Expansion &expand_state(const State &s, Record &record) {
    const auto &it = record.emplace(s, Expansion());
    if (!it.second) return it.first->second;

    // No record found. Search it.
    // quick implementation
    Expansion &mapp = it.first->second;
    mapp[s] = {};
    for (int index : s.o_pieces()) {
        // first check hopping, then moving
        hop(s.o_pieces(), s.x_pieces(), index, mapp, {index});
        move(s.o_pieces(), s.x_pieces(), index, mapp);
    }

    return mapp;
}
