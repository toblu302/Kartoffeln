#ifndef CHESS_H
#define CHESS_H

#include <string>
#include <vector>
#include <stack>
#include <array>
#include <stdint.h>
#include <chrono>

#include "Move.h"
#include "Board.h"

using namespace std;
using namespace std::chrono;

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
        void makeMove(Move mv); //Chess.cpp
        void popMove(); //Chess.cpp
        uint64_t Perft(uint8_t depth); //Chess.cpp
        void PerftDivided(uint8_t depth); //Chess.cpp


        // Search
        string moveToString(Move mv); //Chess.cpp
        Move stringToMove(string mv); //Chess.cpp
        Move getBestMove(); //ChessSearch.cpp
        void getAllMoves(vector<Move> &moves); //ChessSearch.cpp

        bool depthOnly; // to handle "go depth x"
        uint8_t depthLimit;

        bool movetime; // to handle "go movetime"
        uint64_t movetime_ms;

        uint32_t wtime; //time control variable
        uint32_t btime; //time control variable
        uint32_t binc; //time control variable
        uint32_t winc; //time control variable

        uint32_t fullTimeMove; //current move

    private:
        //Board representation
        char turn;
        Board board;
        stack< Board > boardStateStack;

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
        uint64_t getSlidingAlongRank(uint8_t piece_position, uint64_t blockers);
        uint64_t getSlidingAlongFile(uint8_t piece_position, uint64_t blockers);

        uint64_t getSlidingAlongDiagonalA1H8(uint8_t piece_position, uint64_t blockers);
        uint64_t getSlidingAlongDiagonalA8H1(uint8_t piece_position, uint64_t blockers);

        // Helpful constants
        uint64_t FILE[8]; //column
        uint64_t RANK[8]; //rank
        const uint64_t FILE_A = (uint64_t(1) | uint64_t(1) << 8 | uint64_t(1) << 16 | uint64_t(1) << 24 | uint64_t(1) << 32 | uint64_t(1) << 40 | uint64_t(1) << 48 | uint64_t(1) << 56);
        const uint64_t FILE_H = (uint64_t(1) << 7 | uint64_t(1) << 15 | uint64_t(1) << 23 | uint64_t(1) << 31 | uint64_t(1) << 39 | uint64_t(1) << 47 | uint64_t(1) << 55 | uint64_t(1) << 63);

        uint64_t KNIGHT_MOVES[64];
        uint64_t KING_MOVES[64];
        uint64_t SLIDING_MOVES[8][256];

        // Helpful functions
        string bitToStringPosition(uint64_t bit); //Chess.cpp
        string bitPositionToStringPosition(uint8_t bit); //Chess.cpp
        uint64_t stringToBitPosition(string position); //Chess.cpp
        bool isChecking(); //Chess.cpp
        bool isValid(Move mv); //Chess.cpp

        //Search
        void setTimer(uint64_t time_ms); //ChessSearch.cpp
        bool hasTimeLeft(); //ChessSearch.cpp
        milliseconds allowedTime; //the amount of time for which the search is allowed to run
        time_point<steady_clock> startTime; //the start of the timer

        bool searchInterrupted;
        uint32_t timeToCheckTime;
        int64_t alphaBetaSearch(int64_t alpha, int64_t beta, uint8_t depth); //ChessSearch.cpp
        Move bestMove; //Stores the result of bestMoveSearch

        // Evaluation
        int64_t Evaluate(); //ChessEvaluation.cpp
        int64_t materialEvaluation(); //ChessEvaluation.cpp
        int64_t positionEvaluation(); //ChessEvaluation.cpp
};

#endif // CHESS_H
