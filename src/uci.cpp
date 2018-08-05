#include <iostream>
#include <sstream>
#include "uci.h"

using namespace std;

UCI::UCI() {
    chess.setup();
}

void UCI::loop() {
    string line;
    bool quit = false;
    while( !quit ) {
        getline(cin, line);
        chess.UCIgetReady();

        istringstream command_line(line);
        string command;
        command_line >> command;
        if(command == "uci") {
            uci();
        }
        else if(command == "isready") {
            cout << "readyok" << endl;
        }
        else if(command == "ucinewgame") {
            ucinewgame();
        }
        else if(command == "position") {
            position(command_line);
        }
        else if(command == "go") {
            go(command_line);
        }
        else if(command == "quit") {
            quit = true;
        }

        // Custom commands
        else if(command == "print") {
            print();
        }
        else if(command == "move") {
            move(command_line);
        }
        else if(command == "perft") {
            perft(command_line);
        }

    }
}

void UCI::uci() {
    cout << "id name Kartoffeln" << endl;
    cout << "id author Toblu302" << endl;
    cout << "uciok" << endl;
}

void UCI::ucinewgame() {
    chess.setup();
}

void UCI::position(istream &is) {
    string input;
    is >> input;
    if(input == "startpos") {
        chess.setup();
    }
    else if(input == "fen") {
        string fenstring = "";
        for(int i=0; i<6; ++i) {
            is >> input;
            fenstring += input + " ";
        }
        chess.setup(fenstring);
    }
    if( is >> input && input == "moves") {
        while( is >> input ) {
            chess.board.makeMove( chess.stringToMove(input) );
        }
    }
}

void UCI::go(istream &is) {
    string input;
    string next_input;
    while( is >> input ) {
        is >> next_input;
        if(input == "winc") {
            chess.winc = atoi(next_input.c_str());
        }
        else if(input == "binc") {
            chess.binc = atoi(next_input.c_str());
        }
        else if(input == "wtime") {
            chess.wtime = atoi(next_input.c_str());
        }
        else if(input == "btime") {
            chess.btime = atoi(next_input.c_str());
        }
        else if(input == "movetime") {
            chess.movetime = true;
            chess.movetime_ms = atoi(next_input.c_str());
        }
        else if(input == "depth") {
            chess.depthOnly = true;
            chess.depthLimit = atoi(next_input.c_str());
            break;
        }
    }
    string bestmove = chess.moveToString( chess.getBestMove() );
    cout << "bestmove " << bestmove << endl;
}


// Custom commands
void UCI::perft(istream &is) {
    string depth;
    is >> depth;
    clock_t firstTime = clock();
    chess.PerftDivided(atoi(depth.c_str()));
    clock_t secondTime = clock();
    cout << "Time: " << (double(secondTime - firstTime) / CLOCKS_PER_SEC)*1000 << " ms" << endl;
}

void UCI::move(istream &is) {
    string movestring;
    is >> movestring;
    Move mv = chess.stringToMove( movestring );
    chess.board.makeMove(mv);
}

void UCI::print() {
    chess.printBoard();
}
