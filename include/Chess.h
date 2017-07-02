#ifndef CHESS_H
#define CHESS_H

#include <string>
#include <vector>
#include <stack>
#include <array>
#include <ctime>
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

        // Setup
        void setup(); //ChessSetup.cpp
        void setup(string fenstring); //ChessSetup.cpp
        void UCIgetReady(); //ChessSetup.cpp, resets stuff to prepare for UCI

        // Printing
        void printBoard(); //Chess.cpp
        void printBitBoard(uint64_t board); //Chess.cpp

        // Moves
        void pushMove(string move); //Chess.cpp
        void popMove(); //Chess.cpp
        uint64_t Perft(uint8_t depth); //Chess.cpp
        uint64_t PerftDivided(uint8_t depth); //Chess.cpp

        // Search
        string getBestMove(); //ChessSearch.cpp
        void getAllMoves(vector< string > & moves); //ChessSearch.cpp

        bool depthOnly; // to handle "go depth x"
        uint8_t depthLimit;

        uint32_t wtime; //time control variable
        uint32_t btime; //time control variable
        uint32_t binc; //time control variable
        uint32_t winc; //time control variable

        uint32_t fullTimeMove; //current move

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

        array<uint64_t, 4> CENTER_SQARES { { (uint64_t(1) << 27), (uint64_t(1) << 28), (uint64_t(1) << 35), (uint64_t(1) << 36) } };

        uint64_t KNIGHT_MOVES[64];
        uint64_t KING_MOVES[64];
        uint64_t OCCUPANCY[8][256];

        // Helpful functions
        string bitToStringPosition(uint64_t bit); //Chess.cpp
        string bitPositionToStringPosition(uint8_t bit); //Chess.cpp
        uint64_t stringToBitPosition(string position); //Chess.cpp
        bool isChecking(); //Chess.cpp

        //Search
        void setTimer(uint64_t time_ms); //ChessSearch.cpp
        bool hasTimeLeft(); //ChessSearch.cpp
        uint64_t allowedTime; //the amount of time for which the search is allowed to run
        clock_t startTime; //the start of the timer

        bool searchInterrupted;
        int64_t bestMoveSearch(uint8_t depth); //ChessSearch.cpp
        string bestMove; //Stores the result of bestMoveSearch

        // Evaluation
        int64_t Evaluate(); //ChessEvaluation.cpp
        int64_t materialEvaluation(); //ChessEvaluation.cpp
        int64_t positionEvaluation(); //ChessEvaluation.cpp
};

#endif // CHESS_H
