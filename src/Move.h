#ifndef MOVE_H_INCLUDED
#define MOVE_H_INCLUDED

#include <stdint.h>
#include "Definitions.h"


enum MOVETYPE {
    QUIET, DOUBLE_PAWN,
    CASTLING_KINGSIDE, CASTLING_QUEENSIDE,
    CAPTURE, EN_PASSANT_CAPTURE,
    PROMOTION_QUEEN, PROMOTION_KNIGHT, PROMOTION_ROOK, PROMOTION_BISHOP,
    PROMOTION_QUEEN_CAPTURE, PROMOTION_KNIGHT_CAPTURE, PROMOTION_ROOK_CAPTURE, PROMOTION_BISHOP_CAPTURE,
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

    int64_t get_captured_value() const {
        switch(captured_piece) {
            case PAWN:
                return 100;
            case ROOK:
                return 500;
            case QUEEN:
                return 900;
            case KNIGHT:
                return 300;
            case BISHOP:
                return 300;
            default:
                return 1000000;
        }
    }

    bool isCapture() const {
        return move_type == CAPTURE || move_type == PROMOTION_QUEEN_CAPTURE ||
               move_type == PROMOTION_KNIGHT_CAPTURE || move_type == PROMOTION_ROOK_CAPTURE ||
               move_type == PROMOTION_BISHOP_CAPTURE || move_type == EN_PASSANT_CAPTURE;
    }

    bool operator==(const Move& rhs) const {
        return ((color == rhs.color) &&
                (move_type == rhs.move_type) &&
                (from_bitmove == rhs.from_bitmove) &&
                (to_bitmove == rhs.to_bitmove) &&
                (previous_en_passant_square == rhs.previous_en_passant_square) &&
                (previous_castling_rights == rhs.previous_castling_rights) );
    }

    bool operator<(const Move& rhs) const {

        if( isCapture() && rhs.isCapture() ) {
            return (get_captured_value()-moving_piece) < (rhs.get_captured_value()-rhs.moving_piece);
        }
        return !isCapture() && rhs.isCapture();
    }
};
typedef struct Move Move;

#endif // MOVE_H_INCLUDED
