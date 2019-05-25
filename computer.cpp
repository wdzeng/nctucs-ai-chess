#include <vector>
#include "game.h"
using std::vector;

vector<int> Computer::play(const int board[8][8]) {
    const State s(board, role);
    if (role == O) return mm.best_step(s).second;
    vector<int> ret = mm.best_step(s).second;
    for (int& i : ret) i = rev(i);
    return ret;
}