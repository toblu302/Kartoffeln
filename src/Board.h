#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include <stdint.h>
#include "Move.h"
#include "Zobrist.h"
#include "Definitions.h"

struct Board {
    uint64_t pieces[NUM_PIECES];
    uint64_t color[NUM_COLORS];

    uint64_t EN_PASSANT_SQUARE;
    uint64_t CASTLE_RIGHTS;

    uint32_t halftimeMove;
    COLOR side;

    //methods
    void makeMove(const Move& mv);
    void unmakeMove(const Move& mv);
    uint64_t getPiecesOfColor(const COLOR& color, const PIECE& piece);

    //TODO: hashing
    Zobrist tt;
    bool operator==(const Board& rhs) const;
};
typedef struct Board Board;

#endif // BOARD_H_INCLUDED
