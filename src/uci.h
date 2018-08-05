#ifndef UCI_H
#define UCI_H

#include <iostream>
#include "Chess.h"

using std::istream;

class UCI {
    public:
        UCI();
        void loop();

    private:
        Chess chess;

        // UCI-defined commands
        void uci();
        void ucinewgame();
        void position(istream &is);
        void go(istream &is);

        // Custom commands
        void perft(istream &is);
        void move(istream &is);
        void print();
};

#endif
