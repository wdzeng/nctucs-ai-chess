#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include "game.h"
using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::string;
using std::vector;

vector<int> Human::play(const int board[8][8]) {
    cout << "Your turn: ";
    string input;
    getline(cin, input);
    vector<int> v;
    if (input == "pass") return v;

    char* tok = strtok((char*)input.c_str(), " ");
    while (tok != NULL) {
        int index = atoi(tok);
        v.push_back(to_index(index % 10, index / 10));
        tok = strtok(NULL, " ");
    }

    int copy[8][8];
    memcpy(copy, board, sizeof(copy));
    if (!Arena::is_valid(copy, v, role)) {
        cout << "Invalid move. Try again." << endl;
        return play(board);
    }
    return v;
}