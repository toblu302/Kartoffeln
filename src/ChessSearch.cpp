#include <iostream>
#include <utility>
#include "Chess.h"
#include <stdlib.h>
#include <time.h>

using namespace std;

int64_t Chess::getBestMove(uint8_t depth) {

    if( depth == 0 ) {
        return Evaluate();
    }

    vector< string > candidates;
    getAllMoves(candidates);

    int64_t bestScore = (turn == 'w') ? -100000 : 100000;
    string bestMove = "0000";

    for(uint32_t i=0; i<candidates.size(); ++i) {
            pushMove( candidates[i] );
            int64_t score = getBestMove(depth-1);
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

    this->bestMove = bestMove;
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
