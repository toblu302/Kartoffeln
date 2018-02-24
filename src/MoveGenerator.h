#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include <vector>
#include "Board.h"

using std::vector;

class MoveGenerator {
    public:
        MoveGenerator();
        void getAllMoves(Board& board, vector<Move> &moves);

        //helpful methods which should be moved somewhere else
        bool isValid(Board& board, const Move& mv);
        bool isChecking(Board& board);

    private:
        void handle_piece(Board& board, vector<Move> &moves, PIECE piece, uint64_t (MoveGenerator::*get_moves)(const Board& board, const uint8_t &position) );
        void handle_pawn_moves(Board& board, Move& mv, vector<Move>& moves);

        // Bitboard operations: they return a 64-bit integer in which a bit is set if the piece on position can move there
        uint64_t getWhitePawnAttackMoves(const Board& board, const uint8_t& position);
        uint64_t getWhitePawnMoves(const Board& board, const uint8_t& position);
        uint64_t getBlackPawnAttackMoves(const Board& board, const uint8_t& position);
        uint64_t getBlackPawnMoves(const Board& board, const uint8_t& position);

        uint64_t getKnightMoves(const Board& board, const uint8_t& knight_position);
        uint64_t getBishopMoves(const Board& board, const uint8_t& bishop_position);
        uint64_t getRookMoves(const Board& board, const uint8_t& rook_position);
        uint64_t getQueenMoves(const Board& board, const uint8_t& queen_position);

        uint64_t getKingMoves(const Board& board, const uint8_t& king_position);
        uint64_t getKingCastles(const Board& board, const uint8_t& king_position);

        // Bitboard operations for sliding pieces
        uint64_t getSlidingMovesFromOccupancy(const Board& board, const uint8_t& column, const uint8_t& occupancy);
        uint64_t getSlidingAlongRank(const Board& board, const uint8_t& piece_position, const uint64_t& blockers);
        uint64_t getSlidingAlongFile(const Board& board, const uint8_t& piece_position, const uint64_t& blockers);
        uint64_t getSlidingAlongDiagonalA1H8(const Board& board, const uint8_t& piece_position, const uint64_t& blockers);
        uint64_t getSlidingAlongDiagonalA8H1(const Board& board, const uint8_t& piece_position, const uint64_t& blockers);


        // Helpful methods
        uint64_t getWhiteAttacking(const Board& board);
        uint64_t getBlackAttacking(const Board& board);

        uint64_t get_next_bitboard(uint64_t &bitfield);
        uint8_t get_next_value(uint64_t &bitfield);
        bool isCapture(const Board& board, const Move& mv);

        // Helpful constants
        uint64_t FILE[8]; //column
        uint64_t RANK[8]; //rank

        uint64_t KNIGHT_MOVES[64];
        uint64_t KING_MOVES[64];
        uint64_t SLIDING_MOVES[8][256];
};

#endif
