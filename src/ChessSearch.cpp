#include <iostream>
#include <utility>
#include "Chess.h"
#include <stdlib.h>
#include <time.h>

using namespace std;

void Chess::setTimer(uint64_t time_ms) {
    this->allowedTime = time_ms;
    this->startTime = clock();
}

bool Chess::hasTimeLeft() {
    clock_t currentTime = clock();
    uint64_t elapsed_time = uint64_t( (double(currentTime - this->startTime) / CLOCKS_PER_SEC)*1000 );
    return elapsed_time < allowedTime;
}

string Chess::getBestMove() {
    //make sure there is always some move to return
    vector< string > candidates;
    getAllMoves(candidates);
    string randomMove = "0000";
    if( candidates.size() >= 1 ) {
        randomMove = candidates[0];
    }

    if( depthOnly ) {
        setTimer( -1 );
        bestMoveSearch( depthLimit );
    }
    else {
        uint64_t inc = (turn=='w') ? winc : binc;
        uint64_t time = (turn=='w') ? wtime : btime;

        //Assume a game is 75 moves
        //If the game has gone on for more than 75 moves, it will pretend that there is 50 moves left
        uint64_t movesLeft = 50;
        if( fullTimeMove < 75 ) {
            uint64_t movesLeft = (75-fullTimeMove);
        }
        uint64_t timeLeft = movesLeft*inc + time;
        uint64_t timePerMove = timeLeft/movesLeft;

        //Set the depth limit depending on how much time we have per move
        setTimer( timeLeft/movesLeft );

        //Iterative deepening
        depthLimit = 1;
        string lastMove = "0000";
        while( hasTimeLeft() ) {

            bestMoveSearch(depthLimit);

            //If the search was interrupted, set the best move the the best move from the last search
            if( searchInterrupted ) {
                this->bestMove = lastMove;
                break;
            }

            lastMove = bestMove;
            depthLimit += 1;
        }

    }

    if( this->bestMove != "0000" ) {
        return this->bestMove;
    }

    return randomMove;
}

int64_t Chess::bestMoveSearch(uint8_t depth) {

    clock_t currentTime = clock();
    if( !hasTimeLeft() ) {
        this->searchInterrupted = true;
        return 0;
    }

    if( depth == 0 ) {
        return Evaluate();
    }

    vector< string > candidates;
    getAllMoves(candidates);

    int64_t bestScore = (turn == 'w') ? -100000 : 100000;
    string bestMove = "0000";

    for(uint32_t i=0; i<candidates.size(); ++i) {
            pushMove( candidates[i] );
            int64_t score = bestMoveSearch(depth-1);
            popMove();
            if( turn=='w' && score > bestScore) {
                bestScore = score;
                bestMove = candidates[i];
            }
            else if( turn=='b' && score < bestScore) {
                bestScore = score;
                bestMove = candidates[i];
            }
    }

    //if we're at the root, bestMove should be set
    if( depth == depthLimit ) {
        this->bestMove = bestMove;
    }

    return bestScore;
}

void Chess::getAllMoves(vector<string> &moves) {

    uint64_t blockers = 0;
    if( this->turn == 'w' ) {
        blockers = BOARD[ALL_WHITE];
    }
    else {
        blockers = BOARD[ALL_BLACK];
    }

    //iterate over all positions on the board
    for(int x=0; x<64; ++x) {
        uint64_t shifted = uint64_t(1) << x;
        string start = bitPositionToStringPosition(x);
        uint64_t bitMoves = 0;

        if( turn == 'w' ) {
            if( BOARD[WHITE_PAWN] & shifted ) {
                bitMoves = getWhitePawnMoves( shifted );
            }
            else if( BOARD[WHITE_KING] & shifted ) {
                bitMoves = getKingMoves( x, blockers );
                bitMoves |= getKingCastles(x);
            }
            else if( BOARD[ WHITE_KNIGHT ] & shifted ) {
                bitMoves = getKnightMoves( x, blockers );
            }
            else if( BOARD[ WHITE_ROOK ] & shifted ) {
                bitMoves = getSlindingAlongRank( x, blockers );
                bitMoves |= getSlindingAlongFile( x, blockers );
            }
            else if( BOARD[ WHITE_BISHOP ] & shifted ) {
                bitMoves = getSlindingAlongDiagonalA1H8( x, blockers );
                bitMoves |= getSlindingAlongDiagonalA8H1( x, blockers );
            }
            else if( BOARD[ WHITE_QUEEN ] & shifted ) {
                bitMoves = getSlindingAlongDiagonalA1H8( x, blockers );
                bitMoves |= getSlindingAlongDiagonalA8H1( x, blockers );
                bitMoves |= getSlindingAlongRank( x, blockers );
                bitMoves |= getSlindingAlongFile( x, blockers );
            }
        }

        if( turn == 'b' ) {
            if( BOARD[BLACK_PAWN] & shifted ) {
                bitMoves = getBlackPawnMoves( shifted );
            }
            else if( BOARD[BLACK_KING] & shifted ) {
                bitMoves = getKingMoves( x, blockers );
                bitMoves |= getKingCastles(x);
            }
            else if( BOARD[ BLACK_KNIGHT ] & shifted ) {
                bitMoves = getKnightMoves( x, blockers );
            }
            else if( BOARD[ BLACK_ROOK ] & shifted ) {
                bitMoves = getSlindingAlongRank( x, blockers );
                bitMoves |= getSlindingAlongFile( x, blockers );
            }
            else if( BOARD[ BLACK_BISHOP ] & shifted ) {
                bitMoves = getSlindingAlongDiagonalA1H8( x, blockers );
                bitMoves |= getSlindingAlongDiagonalA8H1( x, blockers );
            }
            else if( BOARD[ BLACK_QUEEN ] & shifted ) {
                bitMoves = getSlindingAlongDiagonalA1H8( x, blockers );
                bitMoves |= getSlindingAlongDiagonalA8H1( x, blockers );
                bitMoves |= getSlindingAlongRank( x, blockers );
                bitMoves |= getSlindingAlongFile( x, blockers );
            }
        }



        //if the position has some moves, find where they're leading
        if( bitMoves != 0 ) {
            for(int stopPos=0; stopPos<64; ++stopPos) {
                uint64_t shifted = uint64_t(1) << stopPos;
                if( bitMoves & shifted ) {

                    string mv = start + bitPositionToStringPosition(stopPos);

                    pushMove( mv );

                    if( !isChecking() ) {
                        //check if we're promoting
                        if( (BOARD[BLACK_PAWN] & shifted & 0xFF) ||
                            (BOARD[WHITE_PAWN] & shifted & (uint64_t(0xFF) << 56)) ) {
                            moves.push_back(mv + 'q');
                            moves.push_back(mv + 'b');
                            moves.push_back(mv + 'n');
                            moves.push_back(mv + 'r');
                        }
                        else {
                            moves.push_back( mv );
                        }

                    }
                    popMove();
                }
            }
        }

    }
}
