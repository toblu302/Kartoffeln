#include "Chess.h"
#include "PSQT.h"

int64_t Chess::Evaluate() {
    return materialEvaluation() + positionEvaluation();
}

int64_t Chess::materialEvaluation() {

    int64_t score = 0;

    score += 10*( __builtin_popcountll( board.pieces[PAWN] & board.color[WHITE] ) );
    score += 30*( __builtin_popcountll( board.pieces[KNIGHT] & board.color[WHITE] ) );
    score += 30*( __builtin_popcountll( board.pieces[BISHOP] & board.color[WHITE] ) );
    score += 50*( __builtin_popcountll( board.pieces[ROOK] & board.color[WHITE] ) );
    score += 90*( __builtin_popcountll( board.pieces[QUEEN] & board.color[WHITE] ) );

    score -= 10*( __builtin_popcountll( board.pieces[PAWN] & board.color[BLACK] ) );
    score -= 30*( __builtin_popcountll( board.pieces[KNIGHT] & board.color[BLACK] ) );
    score -= 30*( __builtin_popcountll( board.pieces[BISHOP] & board.color[BLACK] ) );
    score -= 50*( __builtin_popcountll( board.pieces[ROOK] & board.color[BLACK] ) );
    score -= 90*( __builtin_popcountll( board.pieces[QUEEN] & board.color[BLACK] ) );

    return score;
}

int64_t Chess::positionEvaluation() {
    uint64_t score = 0;

    //white pawns positions
    uint64_t white_pawns = board.pieces[PAWN] & board.color[WHITE];
    while( white_pawns != 0 ) {
        uint8_t position = __builtin_ffsll( white_pawns )-1;
        score += PAWN_PSQT[position];
        white_pawns &= ~( uint64_t(1) << position );
    }

    //black pawns positions
    uint64_t black_pawns = board.pieces[PAWN] & board.color[BLACK];
    while( black_pawns != 0 ) {
        uint8_t position = __builtin_ffsll( black_pawns )-1;
        score -= PAWN_PSQT[64-position];
        black_pawns &= ~( uint64_t(1) << position );
    }

    //white knight positions
    uint64_t white_knights = board.pieces[KNIGHT] & board.color[WHITE];
    while( white_knights != 0 ) {
        uint8_t position = __builtin_ffsll( white_knights )-1;
        score += KNIGHT_PSQT[position];
        white_knights &= ~( uint64_t(1) << position );
    }

    //black knight positions
    uint64_t black_knights = board.pieces[KNIGHT] & board.color[BLACK];
    while( black_knights != 0 ) {
        uint8_t position = __builtin_ffsll( black_knights )-1;
        score += KNIGHT_PSQT[64-position];
        black_knights &= ~( uint64_t(1) << position );
    }

    //white bishop positions
    uint64_t white_bishops = board.pieces[BISHOP] & board.color[WHITE];
    while( white_bishops != 0 ) {
        uint8_t position = __builtin_ffsll( white_bishops )-1;
        score += BISHOP_PSQT[position];
        white_bishops &= ~( uint64_t(1) << position );
    }

    //black bishop positions
    uint64_t black_bishops = board.pieces[BISHOP] & board.color[BLACK];
    while( black_knights != 0 ) {
        uint8_t position = __builtin_ffsll( black_bishops )-1;
        score += BISHOP_PSQT[64-position];
        black_bishops &= ~( uint64_t(1) << position );
    }

    return score;
}

