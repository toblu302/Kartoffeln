#include "Chess.h"
#include "PSQT.h"

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

    //white pawns positions
    uint64_t white_pawns = BOARD[PAWN] & BOARD[WHITE];
    while( white_pawns != 0 ) {
        uint8_t position = __builtin_ffsll( white_pawns )-1;
        score += PAWN_PSQT[position];
        white_pawns &= ~( uint64_t(1) << position );
    }

    //black pawns positions
    uint64_t black_pawns = BOARD[PAWN] & BOARD[BLACK];
    while( black_pawns != 0 ) {
        uint8_t position = __builtin_ffsll( black_pawns )-1;
        score -= PAWN_PSQT[64-position];
        black_pawns &= ~( uint64_t(1) << position );
    }

    //white knight positions
    uint64_t white_knights = BOARD[KNIGHT] & BOARD[WHITE];
    while( white_knights != 0 ) {
        uint8_t position = __builtin_ffsll( white_knights )-1;
        score += KNIGHT_PSQT[position];
        white_knights &= ~( uint64_t(1) << position );
    }

    //black knight positions
    uint64_t black_knights = BOARD[KNIGHT] & BOARD[BLACK];
    while( black_knights != 0 ) {
        uint8_t position = __builtin_ffsll( black_knights )-1;
        score += KNIGHT_PSQT[64-position];
        black_knights &= ~( uint64_t(1) << position );
    }

    //white bishop positions
    uint64_t white_bishops = BOARD[BISHOP] & BOARD[WHITE];
    while( white_bishops != 0 ) {
        uint8_t position = __builtin_ffsll( white_bishops )-1;
        score += BISHOP_PSQT[position];
        white_bishops &= ~( uint64_t(1) << position );
    }

    //black bishop positions
    uint64_t black_bishops = BOARD[BISHOP] & BOARD[BLACK];
    while( black_knights != 0 ) {
        uint8_t position = __builtin_ffsll( black_bishops )-1;
        score += BISHOP_PSQT[64-position];
        black_bishops &= ~( uint64_t(1) << position );
    }

    return score;
}

