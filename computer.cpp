#include <vector>
#include <ctime>
#include "game.h"
#include "lang.h"
using std::vector;

vector<int> Computer::play(const int board[8][8]) {
    srand(time(NULL));
    const Board b(board, role);
    vector<int> ret = mm.best_step(b.state()).second;
    if (role == WHITE) rev_vec(ret);
    if (b.is_reflected()) ref_vec(ret);
    return ret;
}