#ifndef CHESS_H
#define CHESS_H

#include <string>
#include <vector>
#include <stack>
#include <array>
#include <ctime>
#include <stdint.h>
using namespace std;

#define BITBOARD_COUNT 10
#define BITBOARD_PIECES 6
enum BITBOARDS {
    PAWN=0, ROOK=1, KNIGHT=2, BISHOP=3, QUEEN=4, KING=5,
    WHITE=6, BLACK=7,
    EN_PASSANT_SQUARE=8,
    CASTLE_RIGHTS=9
};

enum MOVETYPE {
    QUIET, CAPTURE, EN_PASSANT_CAPTURE,
    PROMOTION_QUEEN, PROMOTION_KNIGHT, PROMOTION_ROOK, PROMOTION_BISHOP,
    PROMOTION_QUEEN_CAPTURE, PROMOTION_KNIGHT_CAPTURE, PROMOTION_ROOK_CAPTURE, PROMOTION_BISHOP_CAPTURE,
    CASTLING_KINGSIDE, CASTLING_QUEENSIDE,
    DOUBLE_PAWN,
    NONE
};

struct Move {
    MOVETYPE move_type;

    //regular moves
    uint64_t from_bitmove;
    uint64_t to_bitmove;
    BITBOARDS from_bitboard;

    //captures
    BITBOARDS captured_bitboard;

    BITBOARDS color;
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
        void makeMove(Move mv); //Chess.cpp
        void popMove(); //Chess.cpp
        uint64_t Perft(uint8_t depth); //Chess.cpp
        void PerftDivided(uint8_t depth); //Chess.cpp


        // Search
        string moveToString(Move mv); //Chess.cpp
        Move getBestMove(); //ChessSearch.cpp
        void getAllMoves(vector<Move> &moves); //ChessSearch.cpp

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
        const uint64_t FILE_H = (uint64_t(1) << 7 | uint64_t(1) << 15 | uint64_t(1) << 23 | uint64_t(1) << 31 | uint64_t(1) << 39 | uint64_t(1) << 47 | uint64_t(1) << 55 | uint64_t(1) << 63);

        uint64_t KNIGHT_MOVES[64];
        uint64_t KING_MOVES[64];
        uint64_t OCCUPANCY[8][256];

        // Helpful functions
        string bitToStringPosition(uint64_t bit); //Chess.cpp
        string bitPositionToStringPosition(uint8_t bit); //Chess.cpp
        uint64_t stringToBitPosition(string position); //Chess.cpp
        bool isChecking(); //Chess.cpp
        bool isValid(Move mv); //Chess.cpp

        //Search
        void setTimer(uint64_t time_ms); //ChessSearch.cpp
        bool hasTimeLeft(); //ChessSearch.cpp
        uint64_t allowedTime; //the amount of time for which the search is allowed to run
        clock_t startTime; //the start of the timer

        bool searchInterrupted;
        int64_t alphaBetaSearch(int64_t alpha, int64_t beta, uint8_t depth); //ChessSearch.cpp
        Move bestMove; //Stores the result of bestMoveSearch

        // Evaluation
        int64_t Evaluate(); //ChessEvaluation.cpp
        int64_t materialEvaluation(); //ChessEvaluation.cpp
        int64_t positionEvaluation(); //ChessEvaluation.cpp


        //Piece-Square Tables
        int64_t PAWN_PSQT[64] = {  0, 0, 0, 0, 0, 0, 0, 0,
                                   3, 3,-5,-5,-5,-5, 3, 3,
                                   0, 0, 0, 0, 0, 0, 0, 0,
                                   1, 1, 1, 8, 8, 1, 1, 1,
                                   5, 0, 0, 8, 8, 0, 0, 5,
                                   3, 3, 3, 5, 5, 3, 3, 3,
                                   9, 9, 9, 9, 9, 9, 9, 9,
                                   0, 0, 0, 0, 0, 0, 0, 0 };


        int64_t BISHOP_PSQT[64] = { -6,-5,-5,-5,-5,-5,-5,-6,
                                    -5, 0, 0, 0, 0, 0, 0,-5,
                                    -5, 6, 6, 6, 6, 6, 6,-5,
                                    -5, 0, 6, 6, 6, 6, 0,-5,
                                    -5, 0, 6, 6, 6, 6, 0,-5,
                                    -5, 0, 4, 4, 4, 4, 0,-5,
                                    -5, 0, 0, 0, 0, 0, 0,-5,
                                    -6,-5,-5,-5,-5,-5,-5,-6 };

        int64_t KNIGHT_PSQT[64] = { -6,-5,-5,-5,-5,-5,-5,-6,
                                    -5, 0, 0, 5, 5, 0, 0,-5,
                                    -5, 0, 6, 6, 6, 6, 0,-5,
                                    -5, 0, 6, 7, 7, 6, 0,-5,
                                    -5, 0, 6, 7, 7, 6, 0,-5,
                                    -5, 0, 6, 6, 6, 6, 0,-5,
                                    -5, 0, 0, 0, 0, 0, 0,-5,
                                    -6,-5,-5,-5,-5,-5,-5,-6 };
};

#endif // CHESS_H
