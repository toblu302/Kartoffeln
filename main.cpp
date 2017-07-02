#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>
#include "Chess.h"
using namespace std;

int main()
{
    string line;
    Chess chess;
    chess.setup();

    //main loop
    while( getline(cin, line) ) {
        //split the string into tokens
        istringstream command(line);
        string token;
        vector<string> tokens;
        while(command >> token) {
            tokens.push_back(token);
        }

        //parse the command
        for(vector<string>::size_type i=0; i!=tokens.size(); ++i) {
            string command = tokens[i];
            string arg1 = ".";
            string arg2 = ".";
            if(i+2 < tokens.size()) {
                arg1 = tokens[i+1];
                arg2 = tokens[i+2];
            }
            else if(i+1 < tokens.size()) {
                arg1 = tokens[i+1];
            }

            if(command == "uci") {
                cout << "id name Kartoffeln" << endl;
                cout << "id author Toblu302" << endl;
                cout << "uciok" << endl;
                break;
            }

            else if(command == "isready") {
                cout << "readyok" << endl;
                break;
            }

            else if(command == "ucinewgame") {
                chess.setup();
                break;
            }

            else if(command == "position") {
                if(arg1 == "startpos") {
                    chess.setup();
                }
                else if(arg1 == "fen") {
                    arg2 = "";
                    for(vector<string>::size_type j=i+2; j!=tokens.size(); ++j) {
                        arg2 += tokens[j] + " ";
                    }
                    chess.setup(arg2);
                }
                break;
            }

            else if(command == "go") {
                chess.setTimer(-1);

                if(arg1 == "depth") {
                    chess.getBestMove( atoi(arg2.c_str()) );
                    cout << "bestmove " << chess.bestMove << endl;
                }
                break;
            }

            //custom commands
            else if(command == "print") {
                chess.printBoard();
                break;
            }

            else if(command == "move") {
                chess.pushMove(arg1);
                break;
            }

            else if(command == "perft") {
                clock_t firstTime = clock();
                chess.PerftDivided( atoi(arg1.c_str()) );
                clock_t secondTime = clock();
                cout << "Time: " << (double(secondTime - firstTime) / CLOCKS_PER_SEC)*1000 << " ms" << endl;
                break;
            }
        }
    }

    return 0;
}
