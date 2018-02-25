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
    pv.clear();

    //if we're only doing depth, do a simple alpha-beta search
    if( depthOnly ) {
        setTimer( -1 );
        timeToCheckTime=-1;
        alphaBetaSearch(INT64_MIN, INT64_MAX, depthLimit, pv);
        return pv[0];
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
    vector<Move> temp_pv;
    while( !searchInterrupted && hasTimeLeft() ) {
        timeToCheckTime=30;
        alphaBetaSearch(INT64_MIN, INT64_MAX, depthLimit, temp_pv);

        //If the search was interrupted, set the best move the the best move from the last search
        if( searchInterrupted ) {
            break;
        }

        //send info using UCI
        pv.clear();
        cout << "info depth " << int(this->depthLimit) << " pv ";
        for(auto &mv: temp_pv) {
            cout << moveToString(mv) << " ";
            pv.push_back(mv);
        }
        cout << endl;

        depthLimit += 1;
    }


    return pv[0];
}


int64_t Chess::alphaBetaSearch(int64_t alpha, int64_t beta, uint8_t depth, vector<Move>& this_level_pv) {
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
    vector<Move> next_level_pv;

    for(uint32_t i=0; i<candidates.size(); ++i) {
        next_level_pv.clear();

        if(board.side==WHITE) {
            board.makeMove( candidates[i] );
            int64_t score = alphaBetaSearch(bestScore, beta, depth-1, next_level_pv);
            board.unmakeMove( candidates[i] );

            if( score > bestScore ) {
                bestScore = score;
                this_level_pv.clear();
                this_level_pv.push_back(candidates[i]);
                this_level_pv.insert(this_level_pv.end(), next_level_pv.begin(), next_level_pv.end());
                if ( score > beta ) {
                    return beta;
                }
            }
        }

        else if(board.side==BLACK) {
            board.makeMove( candidates[i] );
            int64_t score = alphaBetaSearch(alpha, bestScore, depth-1, next_level_pv);
            board.unmakeMove( candidates[i] );

            if( score < bestScore ) {
                bestScore = score;
                this_level_pv.clear();
                this_level_pv.push_back(candidates[i]);
                this_level_pv.insert(this_level_pv.end(), next_level_pv.begin(), next_level_pv.end());
                if ( score < alpha ) {
                    return alpha;
                }
            }
        }
    }

    return bestScore;
}

void Chess::getAllMoves(vector<Move> &moves) {
    movegen.getAllMoves(this->board, moves);
}
