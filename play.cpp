#include <iostream>
#include <unistd.h>
#include "game.h"
using namespace std;

int main() {
    Computer cp2(WHITE, Minimax(5, simple_heuristic));
    Computer cp1(BLACK, Minimax(5, simple_heuristic));

    Arena a(cp1, cp2);

    while (a.next_move()) {
        // system("clear");
        cout << "Round " << a.cuurent_round() << endl;
        if (a.last_move().empty()) cout << "PASS";;
        for (int i : a.last_move()) cout << row(i) << col(i) << " ";
        cout << endl << a << endl;
        usleep(500000);
    }
}