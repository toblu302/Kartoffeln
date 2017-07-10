#include "Chess.h"

int64_t Chess::Evaluate() {
    return materialEvaluation() + positionEvaluation();
}

int64_t Chess::materialEvaluation() {

    int64_t score = 0;

    for(int x=0; x<64; ++x) {
        uint64_t shifted = uint64_t(1) << x;

        if( BOARD[PAWN] & shifted & BOARD[WHITE] ) {
            score += 10;
        }
        else if( BOARD[ KNIGHT ] & shifted & BOARD[WHITE] ) {
            score += 30;
        }
        else if( BOARD[ ROOK ] & shifted & BOARD[WHITE] ) {
            score += 50;
        }
        else if( BOARD[ BISHOP ] & shifted & BOARD[WHITE] ) {
            score += 30;
        }
        else if( BOARD[ QUEEN ] & shifted & BOARD[WHITE] ) {
            score += 90;
        }
        else if( BOARD[PAWN] & shifted & BOARD[BLACK] ) {
            score -= 10;
        }
        else if( BOARD[ KNIGHT ] & shifted & BOARD[BLACK] ) {
            score -= 30;
        }
        else if( BOARD[ ROOK ] & shifted & BOARD[BLACK] ) {
            score -= 50;
        }
        else if( BOARD[ BISHOP ] & shifted & BOARD[BLACK] ) {
            score -= 30;
        }
        else if( BOARD[ QUEEN ] & shifted & BOARD[BLACK] ) {
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
        if( (*it) & BOARD[WHITE] ) {
            score += 1;
        }
        if( (*it) & BOARD[BLACK] ) {
            score -= 1;
        }
    }

    return score;
}

