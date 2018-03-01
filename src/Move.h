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

    bool operator==(const Move& rhs) const {
        return ((color == rhs.color) &&
                (move_type == rhs.move_type) &&
                (from_bitmove == rhs.from_bitmove) &&
                (to_bitmove == rhs.to_bitmove) &&
                (previous_en_passant_square == rhs.previous_en_passant_square) &&
                (previous_castling_rights == rhs.previous_castling_rights) );
    }

    bool operator<(const Move& rhs) const {
        return (move_type!=CAPTURE && rhs.move_type==CAPTURE);
    }
};
typedef struct Move Move;

#endif // MOVE_H_INCLUDED
