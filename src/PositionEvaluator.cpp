#include "PositionEvaluator.h"
#include "PSQT.h"
#include "Definitions.h"

int64_t PositionEvaluator::Evaluate(const Board& board) {
    int64_t PSQTscoreMid = PSQTevaluation(board, PSQT_MIDDLE);
    int64_t PSQTscoreEnd = PSQTevaluation(board, PSQT_END);
    int phase = __builtin_popcountll(board.color[WHITE] | board.color[BLACK]);
    int64_t PSQTscore = (phase/32.0)*(PSQTscoreMid-PSQTscoreEnd) + PSQTscoreEnd;

    int64_t score = materialEvaluation(board) + PSQTscore;

    if( board.side == WHITE ) {
        return score;
    }
    return -score;
}

int64_t PositionEvaluator::materialEvaluation(const Board& board) {

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

uint64_t PositionEvaluator::handleBlackPSQT(const Board& board, const PIECE& piece, const array<int64_t, 64>& table) {
    uint64_t score = 0;
    uint64_t pieces = board.pieces[piece] & board.color[BLACK];
    while( pieces != 0 ) {
        uint8_t position = __builtin_ffsll( pieces )-1;
        score -= table[ position ];
        pieces &= ~( uint64_t(1) << position );
    }
    return score;
}

uint64_t PositionEvaluator::handleWhitePSQT(const Board& board, const PIECE& piece, const array<int64_t, 64>& table) {
    uint64_t score = 0;
    uint64_t pieces = board.pieces[piece] & board.color[WHITE];
    while( pieces != 0 ) {
        uint8_t position = __builtin_ffsll( pieces )-1;
        score += table[ WHITE_TRANS[position] ];
        pieces &= ~( uint64_t(1) << position );
    }
    return score;
}

int64_t PositionEvaluator::PSQTevaluation(const Board& board, const array< array<int64_t, 64>, 6> &PSQT) {
    uint64_t score = 0;

    //pawns
    score += handleWhitePSQT(board, PAWN, PSQT[PAWN]);
    score += handleBlackPSQT(board, PAWN, PSQT[PAWN]);

    //knights
    score += handleWhitePSQT(board, KNIGHT, PSQT[KNIGHT]);
    score += handleBlackPSQT(board, KNIGHT, PSQT[KNIGHT]);

    //bishops
    score += handleWhitePSQT(board, BISHOP, PSQT[BISHOP]);
    score += handleBlackPSQT(board, BISHOP, PSQT[BISHOP]);

    //rooks
    score += handleWhitePSQT(board, ROOK, PSQT[ROOK]);
    score += handleBlackPSQT(board, ROOK, PSQT[ROOK]);

    //queens
    score += handleWhitePSQT(board, QUEEN, PSQT[QUEEN]);
    score += handleBlackPSQT(board, QUEEN, PSQT[QUEEN]);

    //kings
    score += handleWhitePSQT(board, KING, PSQT[KING]);
    score += handleBlackPSQT(board, KING, PSQT[KING]);

    return score;
}

