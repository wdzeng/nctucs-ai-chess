#include <algorithm>
#include <cstring>
#include <vector>
#include "lang.h"

using std::vector;

bool State::is_reflected(const vector<int> &o, const vector<int> &x) {
    if (o.empty()) {
        if (x.empty()) return false;
        return col(x[0]) >= 4;
    }
    return col(o[0]) >= 4;
}

State::State() {
    o = {0x00, 0x02, 0x04, 0x06, 0x11, 0x13, 0x15, 0x22, 0x24};
    x = {0x53, 0x55, 0x62, 0x64, 0x66, 0x71, 0x73, 0x75, 0x77};
    // Not reflected and already ordered
    build_token();
}

State::State(const vector<int> &_o, const vector<int> &_x, bool omessy, bool xmessy) {
    o = _o;
    x = _x;
    require_reflected_and_ordered(omessy, xmessy);
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
    require_reflected_and_ordered(false, false);
    build_token();
}

void State::build_token() {
    // build former token
    // O: .... count
    // X: count ....

    // o, quick for loop
    const int osize = o.size();
    for (int i = 0; i < osize; i++) {
        const int index = o[i];
        token_former <<= 3;
        token_former |= row(index);
        token_former <<= 3;
        token_former |= col(index);
    }
    token_former <<= 4;
    token_former |= osize;

    // x, quick for loop
    const int xsize = x.size();
    for (int i = 0; i < xsize; i++) {
        const int index = x[i];
        token_latter <<= 3;
        token_latter |= row(index);
        token_latter <<= 3;
        token_latter |= col(index);
    }
    token_latter |= ((int64)xsize << 60);
}

void State::require_reflected_and_ordered(bool omessy, bool xmessy) {
    if (!is_reflected(o, x)) {
        if (omessy) sort_vec(o);
        if (xmessy) sort_vec(x);
        return;
    }
    ref_vec(o);
    ref_vec(x);
    sort_vec(o);
    sort_vec(x);
}

State State::opposite() const {
    vector<int> oo = o, xx = x;  // copy
    reverse(oo.begin(), oo.end());
    reverse(xx.begin(), xx.end());
    rev_vec(oo);
    rev_vec(xx);
    // Already ordered
    return State(xx, oo, false, false);
}