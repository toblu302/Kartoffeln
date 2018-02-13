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
    uint64_t inc = (turn=='w') ? winc : binc;
    uint64_t time = (turn=='w') ? wtime : btime;
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
        turn = ('w'+'b')-turn;
        if( isChecking() ) {
            retval = (turn == 'w') ? INT64_MAX-1 : INT64_MIN+1;
        }
        turn = ('w'+'b')-turn;
        return retval;
    }

    int64_t bestScore = (turn == 'w') ? alpha : beta;
    Move bestMove;

    for(uint32_t i=0; i<candidates.size(); ++i) {

        if( turn=='w' ) {
            makeMove( candidates[i] );
            int64_t score = alphaBetaSearch(bestScore, beta, depth-1);
            popMove();

            if( score > bestScore ) {
                bestScore = score;
                bestMove = candidates[i];
                if ( score > beta ) {
                    return beta;
                }
            }
        }

        else if( turn=='b' ) {
            makeMove( candidates[i] );
            int64_t score = alphaBetaSearch(alpha, bestScore, depth-1);
            popMove();

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

    uint64_t blockers = 0;
    if( turn == 'w' ) {
        blockers = board.color[WHITE];
    }
    else {
        blockers = board.color[BLACK];
    }

    Move mv;
    mv.color = (turn == 'w') ? WHITE : BLACK;

    //iterate over all positions on the board
    for(int x=0; x<64; ++x) {
        uint64_t shifted = uint64_t(1) << x;
        uint64_t bitMoves = 0;

        if( turn == 'w' && (board.pieces[PAWN] & shifted & board.color[WHITE]) ) {
            bitMoves = getWhitePawnMoves( shifted );

            mv.moving_piece = PAWN;
            mv.from_bitmove = shifted;
        }
        else if( turn == 'b' && (board.pieces[PAWN] & shifted & board.color[BLACK]) ) {
            bitMoves = getBlackPawnMoves( shifted );

            mv.moving_piece = PAWN;
            mv.from_bitmove = shifted;
        }

        if( board.pieces[KING] & shifted & board.color[mv.color] ) {
            bitMoves = getKingMoves( x, blockers );
            bitMoves |= getKingCastles(x);

            mv.moving_piece = KING;
            mv.from_bitmove = shifted;
        }
        else if( board.pieces[ KNIGHT ] & shifted & board.color[mv.color] ) {
            bitMoves = getKnightMoves( x, blockers );

            mv.moving_piece = KNIGHT;
            mv.from_bitmove = shifted;
        }
        else if( board.pieces[ ROOK ] & shifted & board.color[mv.color] ) {
            bitMoves = getSlidingAlongRank( x, blockers );
            bitMoves |= getSlidingAlongFile( x, blockers );

            mv.moving_piece = ROOK;
            mv.from_bitmove = shifted;
        }
        else if( board.pieces[ BISHOP ] & shifted & board.color[mv.color] ) {
            bitMoves = getSlidingAlongDiagonalA1H8( x, blockers );
            bitMoves |= getSlidingAlongDiagonalA8H1( x, blockers );

            mv.moving_piece = BISHOP;
            mv.from_bitmove = shifted;
        }
        else if( board.pieces[ QUEEN ] & shifted & board.color[mv.color] ) {
            bitMoves = getSlidingAlongDiagonalA1H8( x, blockers );
            bitMoves |= getSlidingAlongDiagonalA8H1( x, blockers );
            bitMoves |= getSlidingAlongRank( x, blockers );
            bitMoves |= getSlidingAlongFile( x, blockers );

            mv.moving_piece = QUEEN;
            mv.from_bitmove = shifted;
        }


        //if the position has some moves, find where they're leading
        while( bitMoves != 0 ) {
            int stopPos = __builtin_ffsll( bitMoves )-1;

            uint64_t shifted = uint64_t(1) << stopPos;
            bitMoves &= ~(shifted);

            mv.to_bitmove = shifted;
            mv.move_type = QUIET;

            //handle captures
            if ( (turn == 'b' && (mv.to_bitmove & board.color[WHITE])) ||
                 (turn == 'w' && (mv.to_bitmove & board.color[BLACK])) ) {

                mv.move_type = CAPTURE;

                for( int captured_board=0; captured_board<NUM_PIECES; ++captured_board ) {
                    if( board.pieces[ captured_board ] & mv.to_bitmove ) {
                        mv.captured_piece = PIECE(captured_board);
                    }
                }
            }

            //handle promotions
            if( mv.moving_piece == PAWN && ((mv.to_bitmove & 0xFF) | (mv.to_bitmove & (uint64_t(0xFF) << 56))) ) {
                if( mv.move_type == CAPTURE ) {
                    mv.move_type = PROMOTION_QUEEN_CAPTURE;
                    if( isValid(mv) ) moves.push_back(mv);
                    mv.move_type = PROMOTION_BISHOP_CAPTURE;
                    if( isValid(mv) ) moves.push_back(mv);
                    mv.move_type = PROMOTION_KNIGHT_CAPTURE;
                    if( isValid(mv) ) moves.push_back(mv);
                    mv.move_type = PROMOTION_ROOK_CAPTURE;
                    if( isValid(mv) ) moves.push_back(mv);
                    continue;
                }
                mv.move_type = PROMOTION_QUEEN;
                if( isValid(mv) ) moves.push_back(mv);
                mv.move_type = PROMOTION_BISHOP;
                if( isValid(mv) ) moves.push_back(mv);
                mv.move_type = PROMOTION_KNIGHT;
                if( isValid(mv) ) moves.push_back(mv);
                mv.move_type = PROMOTION_ROOK;
                if( isValid(mv) ) moves.push_back(mv);
                continue;
            }

            //handle double pushes
            else if ( (mv.moving_piece == PAWN) &&
                       ( ((mv.to_bitmove >> 16) == mv.from_bitmove) ||
                         ((mv.to_bitmove << 16) == mv.from_bitmove)) ) {
                mv.move_type = DOUBLE_PAWN;
            }

            //handle en passant captures
            else if( (mv.to_bitmove & board.EN_PASSANT_SQUARE) && (mv.moving_piece == PAWN) ) {
                mv.move_type = EN_PASSANT_CAPTURE;
            }

            //handle castling
            else if (mv.moving_piece == KING) {
                if( (mv.to_bitmove >> 2) == mv.from_bitmove ) {
                    mv.move_type = CASTLING_KINGSIDE;
                }
                else if( (mv.to_bitmove << 2) == mv.from_bitmove ) {
                    mv.move_type = CASTLING_QUEENSIDE;
                }
            }

            if( isValid(mv) ) {
                moves.push_back(mv);
            }
        }

    }
}
