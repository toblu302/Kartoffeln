#include <random>
#include <limits>
#include <iostream>
#include <string>

#include "Zobrist.h"

Zobrist::Zobrist() {
    reset();
}

void Zobrist::reset() {
    std::random_device rd;
    std::string seed = "hello everyone and thank you for checking out the source code of my engine. i hope you enjoy it";
    std::seed_seq seed1 (seed.begin(),seed.end());
    std::mt19937 engine(seed1);
    std::uniform_int_distribution<uint64_t> generator(0, std::numeric_limits<uint64_t>::max() );

    currentHashValue = generator(engine);

    //generate random values for each piece on each board position
    for(int square=0; square<64; ++square) {
        for(int color=0; color<NUM_COLORS; ++color) {
            for(int piece=0; piece<NUM_PIECES; ++piece) {
                this->randomNumbers[square][color][piece] = generator(engine);
            }
        }
    }

    this->randomWhiteToMove = generator(engine);

    for(int color=0; color<NUM_COLORS; ++color) {
        this->randomCastleRights[color][0] = generator(engine);
        this->randomCastleRights[color][1] = generator(engine);
    }

    for(int i=0; i<64; ++i) {
        this->randomEnPassantSquare[i] = generator(engine);
    }

    while( this->hashValueStack.size() != 0 ) {
        this->hashValueStack.pop();
    }
}

uint64_t Zobrist::getHashValue() {
    return this->currentHashValue;
}

void Zobrist::updateHashValue(COLOR color, PIECE piece, uint8_t position) {
    currentHashValue ^= this->randomNumbers[position][color][piece];
}

void Zobrist::updateCastlingRights(COLOR color, int side) {
    currentHashValue ^= randomCastleRights[color][side];
}

void Zobrist::updateEnPassantSquare(int square) {
    currentHashValue ^= randomEnPassantSquare[square-1];
}

void Zobrist::saveHashValue() {
    hashValueStack.push( currentHashValue );
    currentHashValue ^= this->randomWhiteToMove;
}

void Zobrist::revertHashValue() {
    this->currentHashValue = hashValueStack.top();
    hashValueStack.pop();
}
