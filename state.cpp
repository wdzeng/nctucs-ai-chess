#include <algorithm>
#include <cstring>
#include <ostream>
#include <vector>
#include "lang.h"

using std::vector;

long identifier::build(const vector<int> &v) {
    long ret = 0;
    for (int i : v) {
        ret <<= 3;
        ret |= row(i);
        ret <<= 3;
        ret |= col(i);
    }
    ret <<= 4;
    ret |= v.size();
    return ret;
}

long shift_to_neg(long l) {
    for (int i = 1; i < 64; i++) {
        if (l < 0) break;
        l <<= 1;
    }
    return l;
}

identifier State::build_token(const vector<int> &o, const vector<int> &x) {
    bool should_reversed = !o.empty() && col(o[0]) < 4;
    long former, latter;
    if (!should_reversed) {
        former = identifier::build(o);
        latter = identifier::build(x);
    } else {
        vector<int> reversed_o, reversed_x;
        for (int i : o) reversed_o.push_back(hor_rev(i));
        for (int i : x) reversed_x.push_back(hor_rev(i));
        sort(reversed_o.begin(), reversed_o.end());
        sort(reversed_x.begin(), reversed_x.end());
        former = identifier::build(reversed_o);
        latter = identifier::build(reversed_x);
    }
    return {.former = former, .latter = shift_to_neg(latter)};
}

State::State() {
    o = {0x00, 0x02, 0x04, 0x06, 0x11, 0x13, 0x15, 0x22, 0x24};
    x = {0x53, 0x55, 0x62, 0x64, 0x66, 0x71, 0x73, 0x75, 0x77};
    build_token();
}

State::State(const vector<int> &_o, const vector<int> &_x, bool messy_o, bool messy_x) {
    o = _o;
    x = _x;
    if (messy_o) std::sort(o.begin(), o.end());
    if (messy_x) std::sort(x.begin(), x.end());
    build_token();
}

State::State(const int board[8][8], int role) {
    if (role == O) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (board[i][j] == O)
                    o.push_back(to_index(i, j));
                else if (board[i][j] == X)
                    x.push_back(to_index(i, j));
            }
        }
    } else {
        for (int i = 7; i >= 0; i--) {
            for (int j = 7; j >= 0; j--) {
                // reverse vector and position index
                if (board[i][j] == O)
                    x.push_back(rev(to_index(i, j)));
                else if (board[i][j] == X)
                    o.push_back(rev(to_index(i, j)));
            }
        }
    }
    // No need for sorting
    build_token();
}

State State::opposite() const {
    vector<int> oo = o, xx = x;
    reverse(oo.begin(), oo.end());
    reverse(xx.begin(), xx.end());
    return State(rev_vec(oo), rev_vec(xx), false, false);
}

#include <iostream>
using namespace std;

// Output operator overloading
// Not part of project
std::ostream &operator<<(std::ostream &output, const State &state) {
    char sq[8][8];
    memset(sq, NONE, sizeof(sq));
    for (int i : state.o_pieces()) {
        sq[row(i)][col(i)] = O;
    }
    for (int i : state.x_pieces()) {
        sq[row(i)][col(i)] = X;
    }
    output << "~ 0 1 2 3 4 5 6 7";
    for (int i = 7; i >= 0; i--) {
        output << "\n" << i;
        for (int j = 0; j < 8; j++) {
            if (sq[i][j] == O)
                output << " O";
            else if (sq[i][j] == X)
                output << " X";
            else
                output << " -";
        }
    }
    output << "\n";
    return output;
}