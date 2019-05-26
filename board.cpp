#include <vector>
#include "game.h"
#include "lang.h"
using std::vector;

Board::Board(const int board[8][8], int role) {
    vector<int> me, enemy;
    if (role == BLACK) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (board[i][j] == BLACK)
                    me.push_back(to_index(i, j));
                else if (board[i][j] == WHITE)
                    enemy.push_back(to_index(i, j));
            }
        }
    } else {
        for (int i = 7; i >= 0; i--) {
            for (int j = 7; j >= 0; j--) {
                if (board[i][j] == BLACK)
                    enemy.push_back(rev(to_index(i, j)));
                else if (board[i][j] == WHITE)
                    me.push_back(rev(to_index(i, j)));
            }
        }
    }
    reflected = State::is_reflected(me, enemy);
    s = State(me, enemy, false, false);
}