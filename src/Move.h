#ifndef MOVE_H_INCLUDED
#define MOVE_H_INCLUDED

#include <stdint.h>

#include "Pieces.h"

enum MOVETYPE {
    QUIET, CAPTURE, EN_PASSANT_CAPTURE,
    PROMOTION_QUEEN, PROMOTION_KNIGHT, PROMOTION_ROOK, PROMOTION_BISHOP,
    PROMOTION_QUEEN_CAPTURE, PROMOTION_KNIGHT_CAPTURE, PROMOTION_ROOK_CAPTURE, PROMOTION_BISHOP_CAPTURE,
    CASTLING_KINGSIDE, CASTLING_QUEENSIDE,
    DOUBLE_PAWN,
    NONE
};

struct Move {
    MOVETYPE move_type;

    //regular moves
    uint64_t from_bitmove;
    uint64_t to_bitmove;
    BITBOARDS from_bitboard;

    //captures
    BITBOARDS captured_bitboard;

    BITBOARDS color;
};

#endif // MOVE_H_INCLUDED
