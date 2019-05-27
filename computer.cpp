#include <ctime>
#include <iostream>
#include <vector>
#include "game.h"
#include "lang.h"
using std::vector;

vector<int> Computer::play(const int board[8][8]) {
    clock_t t0, t1;
    t0 = clock();
    srand(time(NULL));
    const Board b(board, role);
    vector<int> ret = mm.best_step(b.state()).second;
    if (role == WHITE) rev_vec(ret);
    if (b.is_reflected()) ref_vec(ret);
    t1 = clock();
    std::cout << ((t1 - t0) / (double)CLOCKS_PER_SEC * 1000) << " ms" << std::endl;
    return ret;
}