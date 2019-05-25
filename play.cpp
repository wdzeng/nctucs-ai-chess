#include <iostream>
#include "game.h"
using std::cout;
using std::endl;

int main() {
    State s({0x11}, {0x44});
    State t({0x16}, {0x43});
    cout << (s == t) << endl;
    return 0;
}