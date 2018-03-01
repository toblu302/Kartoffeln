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
#include "MoveGenerator.h"

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
        void unmakeMove(Move mv);
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

        Board board; //Board representation
        MoveGenerator movegen;

    private:
        // Helpful functions
        string bitToStringPosition(uint64_t bit); //Chess.cpp
        string bitPositionToStringPosition(uint8_t bit); //Chess.cpp
        uint64_t stringToBitPosition(string position); //Chess.cpp

        //Search
        void setTimer(uint64_t time_ms); //ChessSearch.cpp
        bool hasTimeLeft(); //ChessSearch.cpp
        milliseconds allowedTime; //the amount of time for which the search is allowed to run
        time_point<steady_clock> startTime; //the start of the timer
        uint32_t searched_nodes;

        bool searchInterrupted;
        uint32_t timeToCheckTime;
        int64_t alphaBetaSearch(int64_t alpha, int64_t beta, uint8_t depth, vector<Move>& this_level_pv); //ChessSearch.cpp
        vector<Move> pv;

        // Evaluation
        int64_t Evaluate(); //ChessEvaluation.cpp
        int64_t materialEvaluation(); //ChessEvaluation.cpp
        int64_t positionEvaluation(); //ChessEvaluation.cpp
        uint64_t handleBlackPSQT(const PIECE& piece, const array<int64_t, 64>& table); //ChessEvaluation.cpp
        uint64_t handleWhitePSQT(const PIECE& piece, const array<int64_t, 64>& table); //
};

#endif // CHESS_H
