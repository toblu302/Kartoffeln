#include "Chess.h"
#include "PSQT.h"

int64_t Chess::Evaluate() {
    int64_t score = materialEvaluation() + positionEvaluation();
    if( board.side == WHITE ) {
        return score;
    }
    return -score;
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

    bool endgame = __builtin_popcountll( board.color[WHITE] | board.color[BLACK] ) <= 13 ? true : false;
    array< array<int64_t, 64>, 6> PSQT = endgame ? PSQT_END : PSQT_MIDDLE;

    //pawns
    score += handleWhitePSQT(PAWN, PSQT[PAWN]);
    score += handleBlackPSQT(PAWN, PSQT[PAWN]);

    //knights
    score += handleWhitePSQT(KNIGHT, PSQT[KNIGHT]);
    score += handleBlackPSQT(KNIGHT, PSQT[KNIGHT]);

    //bishops
    score += handleWhitePSQT(BISHOP, PSQT[BISHOP]);
    score += handleBlackPSQT(BISHOP, PSQT[BISHOP]);

    //rooks
    score += handleWhitePSQT(ROOK, PSQT[ROOK]);
    score += handleBlackPSQT(ROOK, PSQT[ROOK]);

    //queens
    score += handleWhitePSQT(QUEEN, PSQT[QUEEN]);
    score += handleBlackPSQT(QUEEN, PSQT[QUEEN]);

    //kings
    score += handleWhitePSQT(KING, PSQT[KING]);
    score += handleBlackPSQT(KING, PSQT[KING]);

    return score;
}

