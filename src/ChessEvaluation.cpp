#include "Chess.h"

int64_t Chess::Evaluate() {
    return materialEvaluation() + positionEvaluation();
}

int64_t Chess::materialEvaluation() {

    int64_t score = 0;

    for(int x=0; x<64; ++x) {
        uint64_t shifted = uint64_t(1) << x;

        if( BOARD[WHITE_PAWN] & shifted ) {
            score += 10;
        }
        else if( BOARD[ WHITE_KNIGHT ] & shifted ) {
            score += 30;
        }
        else if( BOARD[ WHITE_ROOK ] & shifted ) {
            score += 50;
        }
        else if( BOARD[ WHITE_BISHOP ] & shifted ) {
            score += 30;
        }
        else if( BOARD[ WHITE_QUEEN ] & shifted ) {
            score += 90;
        }
        else if( BOARD[BLACK_PAWN] & shifted ) {
            score -= 10;
        }
        else if( BOARD[ BLACK_KNIGHT ] & shifted ) {
            score -= 30;
        }
        else if( BOARD[ BLACK_ROOK ] & shifted ) {
            score -= 50;
        }
        else if( BOARD[ BLACK_BISHOP ] & shifted ) {
            score -= 30;
        }
        else if( BOARD[ BLACK_QUEEN ] & shifted ) {
            score -= 90;
        }
    }

    return score;
}

int64_t Chess::positionEvaluation() {
    uint64_t score = 0;

    uint64_t whiteAttacking = getWhiteAttacking();
    uint64_t blackAttacking = getBlackAttacking();

    for( array<uint64_t, 4>::iterator it = CENTER_SQARES.begin(); it!=CENTER_SQARES.end(); ++it) {
        if( (*it) & whiteAttacking ) {
            score += 1;
        }
        if( (*it) & blackAttacking ) {
            score -= 1;
        }
        if( (*it) & BOARD[ALL_WHITE] ) {
            score += 1;
        }
        if( (*it) & BOARD[ALL_BLACK] ) {
            score -= 1;
        }
    }

    return score;
}

