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
        chess.UCIgetReady(); //reset some stuff to prepare for UCI

        //split the string into tokens
        istringstream command(line);
        string token;
        vector<string> tokens;
        while(command >> token) {
            tokens.push_back(token);
        }

        //parse the command
        string state = "";
        for(vector<string>::size_type i=0; i!=tokens.size(); ++i) {
            string currentToken = tokens[i];
            string nextToken = "";
            if( i+1 < tokens.size() ) {
                nextToken = tokens[i+1];
            }

            if(state == "") {
                if(currentToken == "uci") {
                    cout << "id name Kartoffeln" << endl;
                    cout << "id author Toblu302" << endl;
                    cout << "uciok" << endl;
                    break;
                }

                else if(currentToken == "isready") {
                    cout << "readyok" << endl;
                    break;
                }

                else if(currentToken == "ucinewgame") {
                    chess.setup();
                    break;
                }

                else if(currentToken == "position") {
                    state = "position";
                }

                else if(currentToken == "go") {
                    state = "go";
                }

            }
            //If we're parsing a "position" command
            else if(state == "position") {
                if(currentToken == "startpos") {
                    chess.setup();
                }
                else if(currentToken == "fen") {
                    string arg2 = "";
                    for(vector<string>::size_type j=i+1; j!=tokens.size(); ++j) {
                        arg2 += tokens[j] + " ";
                    }
                    chess.setup(arg2);
                }
                break;
            }
            //If we're parsing a "go" command
            else if(state == "go") {
                if(currentToken == "winc") {
                    chess.winc = atoi(nextToken.c_str());
                }
                else if(currentToken == "binc") {
                    chess.binc = atoi(nextToken.c_str());
                }
                else if(currentToken == "wtime") {
                    chess.wtime = atoi(nextToken.c_str());
                }
                else if(currentToken == "btime") {
                    chess.btime = atoi(nextToken.c_str());
                }
                else if(currentToken == "depth") {
                    chess.depthOnly = true;
                    chess.depthLimit = atoi(nextToken.c_str());
                    break;
                }

            }

            //custom commands (independent of the state variable)
            if(currentToken == "print") {
                chess.printBoard();
                break;
            }

            else if(currentToken == "move") {
                chess.pushMove( nextToken );
                break;
            }

            else if(currentToken == "perft") {
                clock_t firstTime = clock();
                chess.PerftDivided( atoi(nextToken.c_str()) );
                clock_t secondTime = clock();
                cout << "Time: " << (double(secondTime - firstTime) / CLOCKS_PER_SEC)*1000 << " ms" << endl;
                break;
            }
        }

        //if we've parsed a go command, we should search for a move
        if( state == "go" ) {
            cout << "bestmove " << chess.getBestMove() << endl;
        }
    }

    return 0;
}
