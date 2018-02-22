#ifndef MOVE_H_INCLUDED
#define MOVE_H_INCLUDED

#include <stdint.h>
#include "Definitions.h"

enum MOVETYPE {
    QUIET, CAPTURE, EN_PASSANT_CAPTURE,
    PROMOTION_QUEEN, PROMOTION_KNIGHT, PROMOTION_ROOK, PROMOTION_BISHOP,
    PROMOTION_QUEEN_CAPTURE, PROMOTION_KNIGHT_CAPTURE, PROMOTION_ROOK_CAPTURE, PROMOTION_BISHOP_CAPTURE,
    CASTLING_KINGSIDE, CASTLING_QUEENSIDE,
    DOUBLE_PAWN,
    NONE
};

struct Move {
    COLOR color;
    MOVETYPE move_type;

    //regular moves
    uint64_t from_bitmove;
    uint64_t to_bitmove;
    PIECE moving_piece;

    //captures
    PIECE captured_piece;

    //history
    uint64_t previous_en_passant_square;
    uint64_t previous_castling_rights;
};
typedef struct Move Move;

#endif // MOVE_H_INCLUDED
