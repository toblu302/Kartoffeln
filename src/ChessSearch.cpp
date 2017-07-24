#include <iostream>
#include <utility>
#include <limits.h>
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

Move Chess::getBestMove() {
    //if we're only doing depth, do a simple alpha-beta search
    if( depthOnly ) {
        setTimer( -1 );
        alphaBetaSearch(INT64_MIN, INT64_MAX, depthLimit);
        return this->bestMove;
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
        Move lastMove;
        while( hasTimeLeft() ) {
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

    }

    return this->bestMove;
}


int64_t Chess::alphaBetaSearch(int64_t alpha, int64_t beta, uint8_t depth) {

    if( !hasTimeLeft() ) {
        this->searchInterrupted = true;
        return 0;
    }

    if( depth == 0 ) {
        return Evaluate();
    }

    vector< Move > candidates;
    getAllMoves(candidates);

    if( candidates.size() == 0 ) {
        return (turn == 'w') ? INT64_MIN+1 : INT64_MAX-1;
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
    if( this->turn == 'w' ) {
        blockers = BOARD[WHITE];
    }
    else {
        blockers = BOARD[BLACK];
    }

    Move mv;
    mv.color = (turn == 'w') ? WHITE : BLACK;

    //iterate over all positions on the board
    for(int x=0; x<64; ++x) {
        uint64_t shifted = uint64_t(1) << x;
        uint64_t bitMoves = 0;

        if( turn == 'w' && (BOARD[PAWN] & shifted & BOARD[WHITE]) ) {
            bitMoves = getWhitePawnMoves( shifted );

            mv.from_bitboard = PAWN;
            mv.from_bitmove = shifted;
        }
        else if( turn == 'b' && (BOARD[PAWN] & shifted & BOARD[BLACK]) ) {
            bitMoves = getBlackPawnMoves( shifted );

            mv.from_bitboard = PAWN;
            mv.from_bitmove = shifted;
        }

        if( BOARD[KING] & shifted & BOARD[mv.color] ) {
            bitMoves = getKingMoves( x, blockers );
            bitMoves |= getKingCastles(x);

            mv.from_bitboard = KING;
            mv.from_bitmove = shifted;
        }
        else if( BOARD[ KNIGHT ] & shifted & BOARD[mv.color] ) {
            bitMoves = getKnightMoves( x, blockers );

            mv.from_bitboard = KNIGHT;
            mv.from_bitmove = shifted;
        }
        else if( BOARD[ ROOK ] & shifted & BOARD[mv.color] ) {
            bitMoves = getSlindingAlongRank( x, blockers );
            bitMoves |= getSlindingAlongFile( x, blockers );

            mv.from_bitboard = ROOK;
            mv.from_bitmove = shifted;
        }
        else if( BOARD[ BISHOP ] & shifted & BOARD[mv.color] ) {
            bitMoves = getSlindingAlongDiagonalA1H8( x, blockers );
            bitMoves |= getSlindingAlongDiagonalA8H1( x, blockers );

            mv.from_bitboard = BISHOP;
            mv.from_bitmove = shifted;
        }
        else if( BOARD[ QUEEN ] & shifted & BOARD[mv.color] ) {
            bitMoves = getSlindingAlongDiagonalA1H8( x, blockers );
            bitMoves |= getSlindingAlongDiagonalA8H1( x, blockers );
            bitMoves |= getSlindingAlongRank( x, blockers );
            bitMoves |= getSlindingAlongFile( x, blockers );

            mv.from_bitboard = QUEEN;
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
            if ( (turn == 'b' && (mv.to_bitmove & BOARD[WHITE])) ||
                 (turn == 'w' && (mv.to_bitmove & BOARD[BLACK])) ) {

                mv.move_type = CAPTURE;

                for( int board=0; board<BITBOARD_PIECES; ++board ) {
                    if( BOARD[ board ] & mv.to_bitmove ) {
                        mv.captured_bitboard = BITBOARDS(board);
                    }
                }
            }

            //handle promotions
            if( mv.from_bitboard == PAWN && ((mv.to_bitmove & 0xFF) | (mv.to_bitmove & (uint64_t(0xFF) << 56))) ) {
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
            else if ( (mv.from_bitboard == PAWN) &&
                       ( ((mv.to_bitmove >> 16) == mv.from_bitmove) ||
                         ((mv.to_bitmove << 16) == mv.from_bitmove)) ) {
                mv.move_type = DOUBLE_PAWN;
            }

            //handle en passant captures
            else if( (mv.to_bitmove & BOARD[EN_PASSANT_SQUARE]) && (mv.from_bitboard == PAWN) ) {
                mv.move_type = EN_PASSANT_CAPTURE;
            }

            //handle castling
            else if (mv.from_bitboard == KING) {
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
