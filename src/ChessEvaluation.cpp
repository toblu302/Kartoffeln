#include "Chess.h"

int64_t Chess::Evaluate() {
    return materialEvaluation() + positionEvaluation();
}

int64_t Chess::materialEvaluation() {

    int64_t score = 0;

    score += 10*( __builtin_popcountll( BOARD[PAWN] & BOARD[WHITE] ) );
    score += 30*( __builtin_popcountll( BOARD[KNIGHT] & BOARD[WHITE] ) );
    score += 30*( __builtin_popcountll( BOARD[BISHOP] & BOARD[WHITE] ) );
    score += 50*( __builtin_popcountll( BOARD[ROOK] & BOARD[WHITE] ) );
    score += 90*( __builtin_popcountll( BOARD[QUEEN] & BOARD[WHITE] ) );

    score -= 10*( __builtin_popcountll( BOARD[PAWN] & BOARD[BLACK] ) );
    score -= 30*( __builtin_popcountll( BOARD[KNIGHT] & BOARD[BLACK] ) );
    score -= 30*( __builtin_popcountll( BOARD[BISHOP] & BOARD[BLACK] ) );
    score -= 50*( __builtin_popcountll( BOARD[ROOK] & BOARD[BLACK] ) );
    score -= 90*( __builtin_popcountll( BOARD[QUEEN] & BOARD[BLACK] ) );

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

