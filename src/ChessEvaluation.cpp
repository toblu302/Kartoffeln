#include "Chess.h"
#include "PSQT.h"

int64_t Chess::Evaluate() {
    return materialEvaluation() + positionEvaluation();
}

int64_t Chess::materialEvaluation() {

    int64_t score = 0;

    score += 100*( __builtin_popcountll( board.pieces[PAWN] & board.color[WHITE] ) );
    score += 300*( __builtin_popcountll( board.pieces[KNIGHT] & board.color[WHITE] ) );
    score += 300*( __builtin_popcountll( board.pieces[BISHOP] & board.color[WHITE] ) );
    score += 500*( __builtin_popcountll( board.pieces[ROOK] & board.color[WHITE] ) );
    score += 900*( __builtin_popcountll( board.pieces[QUEEN] & board.color[WHITE] ) );

    score -= 100*( __builtin_popcountll( board.pieces[PAWN] & board.color[BLACK] ) );
    score -= 300*( __builtin_popcountll( board.pieces[KNIGHT] & board.color[BLACK] ) );
    score -= 300*( __builtin_popcountll( board.pieces[BISHOP] & board.color[BLACK] ) );
    score -= 500*( __builtin_popcountll( board.pieces[ROOK] & board.color[BLACK] ) );
    score -= 900*( __builtin_popcountll( board.pieces[QUEEN] & board.color[BLACK] ) );

    return score;
}

uint64_t Chess::handleBlackPSQT(const PIECE& piece, const array<int64_t, 64>& table) {
    uint64_t score = 0;
    uint64_t pieces = board.pieces[piece] & board.color[BLACK];
    while( pieces != 0 ) {
        uint8_t position = __builtin_ffsll( pieces )-1;
        score -= table[ position ];
        pieces &= ~( uint64_t(1) << position );
    }
    return score;
}

uint64_t Chess::handleWhitePSQT(const PIECE& piece, const array<int64_t, 64>& table) {
    uint64_t score = 0;
    uint64_t pieces = board.pieces[piece] & board.color[WHITE];
    while( pieces != 0 ) {
        uint8_t position = __builtin_ffsll( pieces )-1;
        score += table[ WHITE_TRANS[position] ];
        pieces &= ~( uint64_t(1) << position );
    }
    return score;
}

int64_t Chess::positionEvaluation() {
    uint64_t score = 0;

    //pawns
    score += handleWhitePSQT(PAWN, PAWN_PSQT);
    score += handleBlackPSQT(PAWN, PAWN_PSQT);

    //knights
    score += handleWhitePSQT(KNIGHT, KNIGHT_PSQT);
    score += handleBlackPSQT(KNIGHT, KNIGHT_PSQT);

    //bishops
    score += handleWhitePSQT(BISHOP, BISHOP_PSQT);
    score += handleBlackPSQT(BISHOP, BISHOP_PSQT);

    return score;
}

