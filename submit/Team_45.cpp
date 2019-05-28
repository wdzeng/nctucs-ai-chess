#include <time.h>
#include <algorithm>
#include <cstdint>
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "STcpClient.h"

// Language

#define O 1
#define X 2
#define BLACK 1
#define WHITE 2
#define DRAW 3
#define NONE 0
#define SAFETY_HEURISTIC -1.0
#define MIN_HEURISTIC -2.0
#define MAX_HEURISTIC 2.0
#define EX_LEFT 1
#define EX_RIGHT 2
#define EX_BOTTOM 3
#define EX_TOP 4
#define EPSILON 0.004
#define MIN_DOUBLE (2 * EPSILON)

using namespace std;

class State;
class Minimax;
class Computer;
class Player;
class Board;
typedef std::pair<State, std::vector<int>> Step;
typedef std::unordered_map<State, std::vector<int>> Expansion;
typedef std::unordered_map<State, Expansion> Record;
typedef uint64_t int64;

// Helper function
static inline int col(int index) { return index & 0x07; }
static inline int row(int index) { return index >> 4; }
static inline int to_index(int row, int col) { return (row << 4) | col; }
static inline int rev(int index) { return index ^ 0x77; }
static inline int ref(int index) { return index ^ 0x07; }
static inline int is_valid_index(int index) { return (index | 0x77) == 0x77; }
static inline int left(int index) { return index - 0x01; }
static inline int right(int index) { return index + 0x01; }
static inline int top(int index) { return index + 0x10; }
static inline int bottom(int index) { return index - 0x10; }
static inline int ox(int pos, const std::vector<int> &o, const std::vector<int> &x) {
    if (std::find(o.begin(), o.end(), pos) != o.end()) return O;
    if (std::find(x.begin(), x.end(), pos) != x.end()) return X;
    return NONE;
}
static inline void rev_vec(std::vector<int> &v) {
    for (int &index : v) index = rev(index);
}
static inline void ref_vec(std::vector<int> &v) {
    for (int &index : v) index = ref(index);
}
static inline void sort_vec(std::vector<int> &v) { sort(v.begin(), v.end()); }

class State {
   private:
    std::vector<int> o;
    std::vector<int> x;
    int64 token_former = 0, token_latter = 0;
    void build_token();
    void require_reflected_and_ordered(bool, bool);

   public:
    State();
    State(const std::vector<int> &, const std::vector<int> &, bool, bool);
    State(const int board[8][8], int);
    State opposite() const;
    int piece_at(int pos) const { return ox(pos, o, x); }
    const std::vector<int> &o_pieces() const { return o; }
    const std::vector<int> &x_pieces() const { return x; }
    size_t hash() const { return token_former ^ token_latter; }
    bool operator==(const State &other) const { return token_former == other.token_former && token_latter == other.token_latter; }
    static bool is_reflected(const std::vector<int> &, const std::vector<int> &);
};

namespace std {
template <>
struct hash<State> {
    long operator()(const State &s) const { return hash<int>()(s.hash()); }
};
}  // namespace std

const Expansion &expand_state(const State &, Record &);

double simple_heuristic(const State &s);

class Minimax {
   private:
    const int depth;
    Record record;
    double (*h)(const State &);
    double next_o(const State &, const double, const int);
    double next_x(const State &, const double, const int);

   public:
    Minimax(int d, double (*_h)(const State &) = simple_heuristic) : depth(d), h(_h) {}
    const Record &explored() const { return record; }
    const Step &best_step(const State &s);
};

class Player {
   public:
    virtual std::vector<int> play(const int[8][8]) = 0;
};

class Computer : public Player {
   private:
    int role;
    Minimax &mm;

   public:
    Computer(int r, Minimax &m) : role(r), mm(m) {}
    std::vector<int> play(const int[8][8]) override;
};

class Board {
   private:
    State s;
    bool reflected;

   public:
    Board(const int[8][8], int);
    const State &state() const { return s; }
    bool is_reflected() const { return reflected; }
};

// Definition
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

vector<int> &replace_val(vector<int> &v, int src, int dest) {
    for (int &i : v) {
        if (i == src) {
            i = dest;
            break;
        }
    }
    return v;
}

//
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
    State inserted(updated_o, x, true, false);
    const auto &res = mapp.find(inserted);
    if (res == mapp.end()) {
        // New child is found
        mapp[inserted] = {src, dest};
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

    if (shelf_col == X) {           // delete the hopped piece
        vector<int> updated_x = x;  // copy
        updated_x.erase(find(updated_x.begin(), updated_x.end(), shelf_pos));
        State inserted(updated_o, updated_x, true, false);
        const auto &res = mapp.find(inserted);
        if (res != mapp.end()) return;

        // New child is found
        vector<int> next_path = {};
        update_hopping_path(next_path, path, dest);
        mapp[inserted] = next_path;
        hop(updated_o, updated_x, dest, mapp, next_path, ex);
    }

    else {  // hopped piece has same color

        State inserted(updated_o, x, true, false);
        const auto &res = mapp.find(inserted);
        if (res != mapp.end()) return;

        // New child is found
        vector<int> next_path = {};
        update_hopping_path(next_path, path, dest);
        mapp[inserted] = next_path;
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
    const auto &it = record.find(s);
    if (it != record.end()) return record[s];

    record.emplace(s, Expansion());

    // No record found. Search it.
    // quick implementation
    Expansion &mapp = record[s];
    mapp[s] = {};
    // quick for loop
    const vector<int> &os = s.o_pieces(), &xs = s.x_pieces();
    const int no = os.size();
    for (int i = 0; i < no; i++) {
        const int index = os[i];
        // first check hopping, then moving
        hop(os, xs, index, mapp, {index});
        move(os, xs, index, mapp);
    }
    return mapp;
}

//
double heuristic_o(const vector<int> &o, const int no, const int nx, int &oarr, int &odist) {
    const int t_size = min(no, nx + 1);
    oarr = 0;
    odist = 0;
    if (t_size == 0) return 0;

    int i = no - 1, hr = 0;
    for (; i >= no - t_size; i--) {
        int d = max(0, 6 - row(o[i]));
        if (d == 0) oarr++;
        hr += d;
        odist += d;
    }
    for (; i >= 0; i--) {
        odist += max(0, 6 - row(o[i]));
    }
    return (double)hr / t_size;
}
double heuristic_x(const vector<int> &x, const int no, const int nx, int &xarr, int &xdist) {
    const int t_size = min(no + 1, nx);
    xarr = 0;
    xdist = 0;
    if (t_size == 0) return 0;

    int i = 0, hr = 0;
    for (int i = 0; i < t_size; i++) {
        int d = max(0, row(x[i]) - 1);
        xdist += d;
        hr += d;
        if (d == 0) xarr++;
    }
    for (; i < nx; i++) {
        xdist += max(0, row(x[i]) - 1);
    }
    return (double)hr / t_size;
}
double simple_heuristic(const State &s) {
    const int no = s.o_pieces().size(), nx = s.x_pieces().size();
    int oarr, xarr, odist, xdist;

    double ho = heuristic_o(s.o_pieces(), no, nx, oarr, odist);
    double hx = heuristic_x(s.x_pieces(), no, nx, xarr, xdist);

    if (oarr == no || xarr == nx) {
        if (oarr > xarr) return MIN_HEURISTIC;
        if (oarr < xarr) return MAX_HEURISTIC;
        return 0;
    }

    if (oarr == nx) {
        // O must win or draw, heuristic in (-1, 0)
        return -1.0 + (double)(odist - xdist) / (odist + xdist);
    }
    if (xarr == no) {
        // X must win or draw, heuristic in (0, +1)
        return +1.0 - (double)(xdist - xdist) / (odist + xdist);
    }
    if (oarr >= nx) {
        // O must win, heuristic in [-2, -1]
        return MIN_HEURISTIC + (double)odist / (no * 6);
    }
    if (xarr >= no) {
        // X must win, heuristic in [+1, +2]
        return MAX_HEURISTIC - (double)xdist / (nx * 6);
    }

    // Heuristic in range (-1, +1)
    return (ho - hx) / (ho + hx);
}

//

inline bool double_eq(const double &d1, const double &d2) { return abs(d1 - d2) <= EPSILON; }
inline bool double_geq(const double &d1, const double &d2) { return double_eq(d1, d2) || d1 > d2; }
inline bool double_leq(const double &d1, const double &d2) { return double_eq(d1, d2) || d1 < d2; }

double Minimax::next_x(const State &s, const double lim_maxx, const int depth) {
    if (depth == 0) return h(s);
    const Expansion &expanded = expand_state(s.opposite(), record);

    // Check leaves
    double safe = 0;
    for (const Step &e : expanded /* reversed */) {  //
        safe = min(safe, h(e.first));
    }
    // If X has some step that must be safe, select this step and stop searching
    if (safe <= SAFETY_HEURISTIC) return -1 * safe;

    // Search tree
    double maxx = MIN_HEURISTIC;  // Current o's heursistic (skip)
    for (const Step &e : expanded) {
        // std::cout << e.first;
        const double h = next_o(e.first, maxx, depth - 1);
        if (double_geq(h, lim_maxx)) return h + MIN_DOUBLE;
        maxx = max(maxx, h);
    }
    return maxx;
}

double Minimax::next_o(const State &s /* reversed */, const double lim_minn, const int depth) {
    if (depth == 0) return -1.0 * h(s);

    const Expansion &expanded = expand_state(s.opposite(), record);

    // Check leaves
    double safe = 0;
    for (const Step &e : expanded) {
        safe = min(safe, h(e.first));
    }
    // If O has a step that must be safe, select this step
    if (safe <= SAFETY_HEURISTIC) return safe;

    // Search tree
    double minn = MAX_HEURISTIC;  // Convert player x's heuristic ot o's heuristic
    for (const Step &e : expanded) {
        const double h = next_x(e.first, minn, depth - 1);
        if (double_leq(h, lim_minn)) return h - MIN_DOUBLE;
        minn = min(minn, h);
    }
    return minn;
}

const Step &Minimax::best_step(const State &s) {
    const Expansion &expanded = expand_state(s, record);

    // Check leaves
    double safe = 0;
    Step *win_step = NULL;
    for (auto &&e : expanded) {
        double hr = h(e.first);
        if (double_leq(hr, safe) && double_leq(hr, SAFETY_HEURISTIC)) {
            safe = hr;
            win_step = (Step *)&e;
        }
    }
    if (win_step) return *win_step;

    // Serach tree
    double minn = MAX_HEURISTIC;
    vector<Step *> candidates;
    int d_depth = depth;
    const int ssize = s.o_pieces().size() + s.x_pieces().size();

    // Determine the depth
    if (ssize <= 4)
        d_depth += 6;
    else if (ssize <= 5)
        d_depth += 5;
    else if (ssize <= 6)
        d_depth += 4;
    else if (ssize <= 7)
        d_depth += 3;
    else if (ssize <= 9)
        d_depth += 2;
    else if (ssize <= 13)
        d_depth += 1;

    for (auto &&e : expanded) {
        double h = next_x(e.first, minn, d_depth - 1);
        if (h < minn) {
            minn = h;
            candidates.clear();
        }
        if (double_eq(h, minn)) {
            candidates.push_back((Step *)&e);
        }
    }

    if (candidates.empty()) {
        auto &&it = record[s].find(s);
        return (Step &)*it;
    }

    // there are multiple candidats
    // choose whose diff is maximize
    vector<Step **> elected;
    int maxdiff = -100;
    for (auto &ss : candidates) {
        int diff = ss->first.o_pieces().size() - ss->first.x_pieces().size();
        if (diff > maxdiff) {
            elected.clear();
            maxdiff = diff;
        }
        if (diff == maxdiff) elected.push_back(&ss);
    }
    return **elected[rand() % elected.size()];
}

//
vector<int> Computer::play(const int board[8][8]) {
    srand(time(NULL));
    const Board b(board, role);
    vector<int> ret = mm.best_step(b.state()).second;
    if (role == WHITE) rev_vec(ret);
    if (b.is_reflected()) ref_vec(ret);
    return ret;
}
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

Minimax mm(4, simple_heuristic);
Computer cp_black(BLACK, mm);
Computer cp_white(WHITE, mm);

// -----------------------------
std::vector<std::vector<int>> GetStep(std::vector<std::vector<int>> &board, bool is_black) {
    int cboard[8][8];
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) cboard[i][j] = board[j][i];
    vector<int> step = is_black ? cp_black.play(cboard) : cp_white.play(cboard);
    vector<vector<int>> ret;
    for (int &index : step) ret.push_back({col(index), row(index)});
    return ret;
}
int main() {
    int id_package;
    std::vector<std::vector<int>> board, step;
    bool is_black;
    while (true) {
        if (GetBoard(id_package, board, is_black)) break;

        step = GetStep(board, is_black);
        SendStep(id_package, step);
    }
}
