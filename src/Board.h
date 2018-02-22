#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include <stdint.h>

#define NUM_PIECES 6
enum PIECE {
    PAWN=0, ROOK=1, KNIGHT=2, BISHOP=3, QUEEN=4, KING=5
};

#define NUM_COLORS 2
enum COLOR {
    WHITE=0, BLACK=1
};

struct Board {
    uint64_t pieces[NUM_PIECES];
    uint64_t color[NUM_COLORS];

    uint64_t EN_PASSANT_SQUARE;
    uint64_t CASTLE_RIGHTS;

    uint32_t halftimeMove;
    COLOR side;
};
typedef struct Board Board;

#endif // BOARD_H_INCLUDED
