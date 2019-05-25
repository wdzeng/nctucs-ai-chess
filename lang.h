#ifndef lang_hyperbola
#define lang_hyperbola

#include <algorithm>
#include <ostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Language

#define O 1
#define X 2
#define NONE 0
#define MIN_HEURISTIC -1.0
#define MAX_HEURISTIC 1.0

static inline int col(int index) { return index & 0x07; }
static inline int row(int index) { return index >> 4; }
static inline int to_index(int row, int col) { return (row << 4) | col; }
static inline int rev(int index) { return index ^ 0x77; }
static inline int hor_rev(int index) { return index ^ 0x07; }
static inline int ver_rev(int index) { return index ^ 0x70; }
static inline int is_valid_index(int index) { return (index | 0x77) == 0x77; }
static inline int left(int index) { return index - 0x01; }
static inline int right(int index) { return index + 0x01; }
static inline int top(int index) { return index - 0x10; }
static inline int bottom(int index) { return index + 0x10; }
static inline int hop_left(int index) { return index - 0x02; }
static inline int hop_right(int index) { return index + 0x02; }
static inline int hop_top(int index) { return index - 0x20; }
static inline int hop_bottom(int index) { return index + 0x20; }
static inline std::vector<int> vectorize(int index) { return {row(index), col(index)}; }

static int ox(int pos, const std::vector<int> &o, const std::vector<int> &x) {
    if (std::find(o.begin(), o.end(), pos) != o.end()) return O;
    if (std::find(x.begin(), x.end(), pos) != x.end()) return X;
    return NONE;
}

static std::vector<int> &rev_vec(std::vector<int> &v) {
    for (int &index : v) index = rev(index);
    return v;
}

// State

struct identifier {
    long former, latter;
    static long build(const std::vector<int> &);
    bool operator==(const identifier &other) const { return former == other.former && latter == other.latter; }
};

class State {
   private:
    std::vector<int> o;
    std::vector<int> x;
    identifier token;
    static identifier build_token(const std::vector<int> &, const std::vector<int> &);
    void build_token() { token = build_token(o, x); }

   public:
    State();
    State(const std::vector<int> &, const std::vector<int> &, bool = true, bool = true);
    State(const int board[8][8], int);
    State opposite() const;
    int piece_at(int pos) const { return ox(pos, o, x); }
    const std::vector<int> &o_pieces() const { return o; }
    const std::vector<int> &x_pieces() const { return x; }
    size_t hash() const { return token.former ^ token.latter; }
    bool operator==(const State &other) const { return token == other.token; }
};

std::ostream &operator<<(std::ostream &, const State &);

namespace std {
template <>
struct hash<State> {
    long operator()(const State &s) const { return hash<int>()(s.hash()); }
};
}  // namespace std

// Expand

typedef std::unordered_map<State, std::vector<int>> Expansion;
typedef std::unordered_map<State, Expansion> Record;
// Expansion expand_state(const State &);
const Expansion &expand_state(const State &, Record &);

#endif