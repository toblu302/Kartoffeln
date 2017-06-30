#ifndef CHESS_H
#define CHESS_H

#include <string>
#include <vector>
#include <stack>
#include <array>
#include <stdint.h>
using namespace std;

#define BITBOARD_COUNT 17
enum BITBOARDS {
    WHITE_PAWN=0, WHITE_ROOK=1, WHITE_KNIGHT=2, WHITE_BISHOP=3, WHITE_QUEEN=4, WHITE_KING=5,
    BLACK_PAWN=6, BLACK_ROOK=7, BLACK_KNIGHT=8, BLACK_BISHOP=9, BLACK_QUEEN=10, BLACK_KING=11,
    ALL_WHITE=12, ALL_BLACK=13,
    EN_PASSANT=14,
    WHITE_CASTLE_RIGHTS=15, BLACK_CASTLE_RIGHTS=16
};

class Chess
{
    public:
        Chess();

        void setup(); //ChessSetup.cpp
        void setup(string fenstring); //ChessSetup.cpp

        void printBoard(); //Chess.cpp
        void printBitBoard(uint64_t board); //Chess.cpp

        void pushMove(string move); //Chess.cpp
        void popMove(); //Chess.cpp
        uint64_t Perft(uint8_t depth); //Chess.cpp
        uint64_t PerftDivided(uint8_t depth); //Chess.cpp

        int64_t getBestMove(uint8_t depth); //ChessSearch.cpp
        string bestMove; //stores the result of getBestMove()

        void getAllMoves(vector< string > & moves); //ChessSearch.cpp

    private:
        //Board representation
        char turn;
        array<uint64_t, BITBOARD_COUNT> BOARD;
        stack< array<uint64_t, BITBOARD_COUNT> > boardStateStack;

        //Move generators (ChessMoveGenerator.cpp)
        uint64_t getWhitePawnAttackMoves(uint64_t WHITE_PAWNS);
        uint64_t getWhitePawnMoves(uint64_t PAWNS);
        uint64_t getBlackPawnAttackMoves(uint64_t WHITE_PAWNS);
        uint64_t getBlackPawnMoves(uint64_t PAWNS);
        uint64_t getKnightMoves(uint8_t knight_position, uint64_t blockers);
        uint64_t getKingMoves(uint8_t king_position, uint64_t blockers);
        uint64_t getKingCastles(uint8_t king_position);

        uint64_t getWhiteAttacking();
        uint64_t getBlackAttacking();

        uint64_t getSlidingMovesFromOccupancy(uint8_t column, uint8_t occupancy);
        uint64_t getSlindingAlongRank(uint8_t piece_position, uint64_t blockers);
        uint64_t getSlindingAlongFile(uint8_t piece_position, uint64_t blockers);

        uint64_t getSlindingAlongDiagonalA1H8(uint8_t piece_position, uint64_t blockers);
        uint64_t getSlindingAlongDiagonalA8H1(uint8_t piece_position, uint64_t blockers);

        // Helpful constants
        const uint64_t FILE_A = (uint64_t(1) | uint64_t(1) << 8 | uint64_t(1) << 16 | uint64_t(1) << 24 | uint64_t(1) << 32 | uint64_t(1) << 40 | uint64_t(1) << 48 | uint64_t(1) << 56);
        const uint64_t FILE_H = (uint64_t(7) | uint64_t(1) << 15 | uint64_t(1) << 23 | uint64_t(1) << 31 | uint64_t(1) << 39 | uint64_t(1) << 47 | uint64_t(1) << 55 | uint64_t(1) << 63);

        uint64_t KNIGHT_MOVES[64];
        uint64_t KING_MOVES[64];
        uint64_t OCCUPANCY[8][256];

        // Helpful functions
        string bitToStringPosition(uint64_t bit); //Chess.cpp
        string bitPositionToStringPosition(uint8_t bit); //Chess.cpp
        uint64_t stringToBitPosition(string position); //Chess.cpp
        bool isChecking(); //Chess.cpp

        // Evaluation
        int64_t Evaluate(); //ChessEvaluation.cpp
};

#endif // CHESS_H
