#include <iostream>
#include <utility>
#include <limits.h>
#include "Chess.h"
#include <stdlib.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

void Chess::setTimer(uint64_t time_ms) {
    allowedTime = milliseconds(time_ms);
    startTime = steady_clock::now();
}

bool Chess::hasTimeLeft() {
    milliseconds elapsed_time = duration_cast<milliseconds>( steady_clock::now() - startTime );
    return elapsed_time < allowedTime;
}

uint64_t getMoveTime(uint8_t fulltimeMove, uint64_t inc, uint64_t timeLeft) {
    double factor = 0.025;
    if( fulltimeMove < 19 ) {
        factor = (fulltimeMove/19.0)*0.01+0.015;
    }
    return (factor*timeLeft) + inc/2.0;
}

Move Chess::getBestMove() {
    //if we're only doing depth, do a simple alpha-beta search
    if( depthOnly ) {
        setTimer( -1 );
        alphaBetaSearch(INT64_MIN, INT64_MAX, depthLimit);
        return this->bestMove;
    }

    //set the timer
    uint64_t inc = (board.side==WHITE) ? winc : binc;
    uint64_t time = (board.side==WHITE) ? wtime : btime;
    uint64_t timeForMove = getMoveTime(fullTimeMove, inc, time);
    setTimer(timeForMove);

    // handle "go movetime"
    if( this->movetime ) {
        setTimer(this->movetime_ms);
    }

    //Iterative deepening
    depthLimit = 1;
    Move lastMove;
    while( !searchInterrupted && hasTimeLeft() ) {
        timeToCheckTime=30;
        alphaBetaSearch(INT64_MIN, INT64_MAX, depthLimit);

        //If the search was interrupted, set the best move the the best move from the last search
        if( searchInterrupted ) {
            this->bestMove = lastMove;
            break;
        }

        //send info using UCI
        cout << "info depth " << int(this->depthLimit) << " pv " << moveToString(this->bestMove) << endl;


        lastMove = bestMove;
        depthLimit += 1;
    }


    return this->bestMove;
}


int64_t Chess::alphaBetaSearch(int64_t alpha, int64_t beta, uint8_t depth) {
    // check if we have enough time left, and interrupt if we don't
    if( searchInterrupted ) {
        return 0;
    }
    --timeToCheckTime;
    if( timeToCheckTime == 0 ) {
        timeToCheckTime = 30;
        if( !hasTimeLeft() ) {
            searchInterrupted = true;
            return 0;
        }
    }

    if( depth == 0 ) {
        return Evaluate();
    }

    vector< Move > candidates;
    getAllMoves(candidates);

    // Check for mates/stalemates
    if( candidates.size() == 0 ) {
        uint64_t retval = 0;
        board.side = (board.side==WHITE) ? BLACK : WHITE;
        if( movegen.isChecking(board) ) {
            retval = (board.side==WHITE) ? INT64_MAX-1 : INT64_MIN+1;
        }
        board.side = (board.side==WHITE) ? BLACK : WHITE;
        return retval;
    }

    int64_t bestScore = (board.side==WHITE) ? alpha : beta;
    Move bestMove;

    for(uint32_t i=0; i<candidates.size(); ++i) {

        if(board.side==WHITE) {
            board.makeMove( candidates[i] );
            int64_t score = alphaBetaSearch(bestScore, beta, depth-1);
            board.unmakeMove( candidates[i] );

            if( score > bestScore ) {
                bestScore = score;
                bestMove = candidates[i];
                if ( score > beta ) {
                    return beta;
                }
            }
        }

        else if(board.side==BLACK) {
            board.makeMove( candidates[i] );
            int64_t score = alphaBetaSearch(alpha, bestScore, depth-1);
            board.unmakeMove( candidates[i] );

            if( score < bestScore ) {
                bestScore = score;
                bestMove = candidates[i];
                if ( score < alpha ) {
                    return alpha;
                }
            }
        }
    }

    //if we're at the root, bestMove should be set
    if( depth == depthLimit ) {
        this->bestMove = bestMove;
    }

    return bestScore;
}

void Chess::getAllMoves(vector<Move> &moves) {
    movegen.getAllMoves(this->board, moves);
}
