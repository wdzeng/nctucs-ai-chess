#ifndef game_hyperbola
#define game_hyperbola
#include <ostream>
#include <vector>
#include "calc.h"
#include "lang.h"

#define BLACK 1
#define WHITE 2
#define DRAW 3

class Player {
   public:
    virtual std::vector<int> play(const int[8][8]) = 0;
};

class Computer : public Player {
   private:
    int role;
    Minimax mm;

   public:
    Computer(int r, const Minimax& m) : role(r), mm(m) {}
    std::vector<int> play(const int[8][8]) override;
};

class Human : public Player {
   private:
    int role;
    static bool valid(const int board[8][8], const std::vector<int>&);

   public:
    Human(int r) : role(r) {}
    std::vector<int> play(const int[8][8]) override;
};

class Arena {
   private:
    int board[8][8] = {NONE};
    int round = 0;
    Player &black, &white;
    int stat = WHITE;
    std::vector<int> lstmov;
    bool done = false;
    int vtr = NONE;

    inline void set(int index, int color) { board[row(index)][col(index)] = color; }
    static inline void set(int board[8][8], int index, int color) { board[row(index)][col(index)] = color; }
    void init_board();
    static bool require_valid_indexes(const std::vector<int>&);
    void update(const std::vector<int>&, const int);
    static bool is_valid_move(int[8][8], const std::vector<int>&, const int);
    static bool is_valid_hop(int[8][8], const std::vector<int>&, const int);
    static bool is_hop(const int[8][8], int, int);
    bool black_ends() const;
    bool white_ends() const;
    int winner() const;

   public:
    Arena(Player& o, Player& x) : black(o), white(x) { init_board(); }
    bool next_move();
    static bool is_valid(int[8][8], const std::vector<int>&, const int);
    static inline int get(const int board[8][8], int r, int c) { return board[r][c]; }
    static inline int get(const int board[8][8], int index) { return get(board, row(index), col(index)); }
    inline int get(int r, int c) const { return board[r][c]; }
    inline int get(int index) const { return get(row(index), col(index)); }
    int cuurent_round() const { return round; }
    const std::vector<int>& last_move() const { return lstmov; }
    int state() const { return stat; }
    int victor() const { return vtr; }
};

extern std::ostream& operator<<(std::ostream& output, const Arena& a);

class Board {
   private:
    State s;
    bool reflected;

   public:
    Board(const int[8][8], int);
    const State& state() const { return s; }
    bool is_reflected() const { return reflected; }
};

#endif