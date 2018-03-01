#ifndef HASHTABLE_H_INCLUDED
#define HASHTABLE_H_INCLUDED

#include <stdint.h>
#include <stack>
#include "Definitions.h"
using std::stack;

class Zobrist {
    public:
        Zobrist();
        void reset();

        void saveHashValue();

        void updateHashValue(COLOR color, PIECE piece, uint8_t position );
        void updateCastlingRights(COLOR color, int side);
        void updateEnPassantSquare(int square);

        void revertHashValue();
        uint64_t getHashValue();

        stack<uint64_t> hashValueStack;

    private:
        uint64_t currentHashValue;

        uint64_t randomNumbers[64][NUM_COLORS][NUM_PIECES]; //random numbers for each position
        uint64_t randomWhiteToMove; //random number indicating if it is whites turn to move
        uint64_t randomEnPassantSquare[64];
        uint64_t randomCastleRights[NUM_COLORS][2];
};

#endif // HASHTABLE_H_INCLUDED
