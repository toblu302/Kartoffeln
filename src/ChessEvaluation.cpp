#include "Chess.h"

int64_t Chess::Evaluate() {
    int64_t score = 0;

    //Material evaulation
    for(int x=0; x<64; ++x) {
        uint64_t shifted = uint64_t(1) << x;

        if( BOARD[WHITE_PAWN] & shifted ) {
            score += 1;
        }
        else if( BOARD[ WHITE_KNIGHT ] & shifted ) {
            score += 3;
        }
        else if( BOARD[ WHITE_ROOK ] & shifted ) {
            score += 5;
        }
        else if( BOARD[ WHITE_BISHOP ] & shifted ) {
            score += 3;
        }
        else if( BOARD[ WHITE_QUEEN ] & shifted ) {
            score += 8;
        }
        else if( BOARD[BLACK_PAWN] & shifted ) {
            score -= 1;
        }
        else if( BOARD[ BLACK_KNIGHT ] & shifted ) {
            score -= 3;
        }
        else if( BOARD[ BLACK_ROOK ] & shifted ) {
            score -= 5;
        }
        else if( BOARD[ BLACK_BISHOP ] & shifted ) {
            score -= 3;
        }
        else if( BOARD[ BLACK_QUEEN ] & shifted ) {
            score -= 8;
        }
    }

    return score;
}
