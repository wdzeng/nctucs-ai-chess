#include <cstring>
#include <vector>
#include "game.h"
#define DRAW 3
#define NONE 0
using std::endl;
using std::ostream;
using std::vector;

ostream& operator<<(ostream& output, const Arena& a) {
    for (int i = 7; i >= 0; i--) {
        output << i;
        for (int j = 0; j < 8; j++) {
            int color = a.get(i, j);
            output << (color == NONE ? " -" : (color == BLACK ? " O" : " X"));
        }
        output << "\n";
    }
    return output << "~ 0 1 2 3 4 5 6 7\n" << endl;
}

void Arena::init_board() {
    board[0][0] = BLACK;
    board[0][2] = BLACK;
    board[0][4] = BLACK;
    board[0][6] = BLACK;
    board[1][1] = BLACK;
    board[1][3] = BLACK;
    board[1][5] = BLACK;
    board[2][2] = BLACK;
    board[2][4] = BLACK;
    board[7][7] = WHITE;
    board[7][5] = WHITE;
    board[7][3] = WHITE;
    board[7][1] = WHITE;
    board[6][6] = WHITE;
    board[6][4] = WHITE;
    board[6][2] = WHITE;
    board[5][5] = WHITE;
    board[5][3] = WHITE;
}

bool Arena::require_valid_indexes(const vector<int>& steps) {
    for (int index : steps)
        if (!is_valid_index(index)) return false;
    return true;
}

bool Arena::is_valid_move(int board[8][8], const vector<int>& steps, const int p) {
    if (steps.size() != 2) return false;
    if (get(board, steps[0]) != p || get(board, steps[1]) != NONE) return false;
    if (left(steps[0]) == steps[1] || right(steps[0]) == steps[1] || top(steps[0]) == steps[1] || bottom(steps[0]) == steps[1]) {
        set(board, steps[0], NONE);
        set(board, steps[1], p);
        return true;
    }
    return false;
}

bool Arena::is_hop(const int board[8][8], int src, int dest) {
    if (col(src) == col(dest)) {
        return abs(row(src) - row(dest)) == 2;
    }
    if (row(src) == row(dest)) {
        return abs(col(src) - col(dest)) == 2;
    }
    return false;
}

bool Arena::is_valid_hop(int board[8][8], const vector<int>& steps, const int p) {
    if (get(board, steps[0]) != p) return false;
    int copy[8][8];
    memcpy(copy, board, sizeof(copy));
    set(copy, steps[0], NONE);
    auto it = steps.begin();
    while (++it != steps.end()) {
        if (!is_hop(copy, *(it - 1), *it)) return false;
        if (get(copy, *it) != NONE) return false;
        int shelf = ((*it) + *(it - 1)) / 2;
        int shelf_col = get(copy, shelf);
        if (shelf_col == NONE) return false;
        if (shelf_col != p) set(copy, shelf, NONE);
    }
    set(copy, steps.back(), p);
    memcpy(board, copy, sizeof(copy));
    return true;
}

bool Arena::is_valid(int board[8][8], const vector<int>& steps, const int p) {
    for(int i: steps) printf("%02X " , i);
    printf("\n");
    if (steps.empty()) return true;
    if (steps.size() == 1) return false;
    if (!require_valid_indexes(steps)) return false;
    return is_valid_move(board, steps, p) || is_valid_hop(board, steps, p);
}

void Arena::update(const vector<int>& step, const int p) {
    lstmov = is_valid(board, step, p) ? step : vector<int>();
}

bool Arena::black_ends() const {
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 8; j++)
            if (board[i][j] == BLACK) return false;
    }
    return true;
}

bool Arena::white_ends() const {
    for (int i = 2; i < 8; i++) {
        for (int j = 0; j < 8; j++)
            if (board[i][j] == WHITE) return false;
    }
    return true;
}

int Arena::winner() const {
    if (round <= 200 && !black_ends() && !white_ends()) {
        return NONE;
    }
    int k = 0, w = 0;
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 8; j++)
            if (board[i][j] == WHITE) w++;
    for (int i = 6; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (board[i][j] == BLACK) k++;

    if (k > w) return BLACK;
    if (k < w) return WHITE;
    return DRAW;
}

bool Arena::next_move() {
    if (done) return false;
    if (stat == BLACK) round++;

    int w = winner();
    if (w != NONE) {
        done = true;
        return false;
    }

    stat = stat == BLACK ? WHITE : BLACK;
    vector<int> updated = (stat == BLACK ? black.play(board) : white.play(board));
    update(updated, stat);
    return true;
}