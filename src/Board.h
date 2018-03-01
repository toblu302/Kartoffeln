#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include <unordered_map>
#include <stdint.h>
#include "Move.h"
#include "Zobrist.h"
#include "Definitions.h"

using std::unordered_map;

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

    bool hasBeenSeen();
    void reset();

    //TODO: hashing
    Zobrist tt;
    bool operator==(const Board& rhs) const;
    unordered_map<uint64_t, int> seen_positions;
};
typedef struct Board Board;

#endif // BOARD_H_INCLUDED
