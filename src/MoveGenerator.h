#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include <array>
#include <unordered_map>
#include <vector>
#include <queue>
#include "Board.h"

using std::vector;
using std::priority_queue;

class MoveGenerator {
    public:
        MoveGenerator();
        void getAllMoves(Board& board, priority_queue<Move> &moves);

        //helpful methods which should be moved somewhere else
        bool isValid(Board& board, const Move& mv);
        bool isChecking(Board& board);

    private:
        void handle_piece(Board& board, priority_queue<Move> &moves, PIECE piece, uint64_t (MoveGenerator::*get_moves)(const Board& board, const uint8_t &position) );
        void handle_pawn_moves(Board& board, Move& mv, priority_queue<Move>& moves);

        // Bitboard operations: they return a 64-bit integer in which a bit is set if the piece on position can move there
        uint64_t getWhitePawnAttackMoves(const Board& board, const uint8_t& position);
        uint64_t getWhitePawnMoves(const Board& board, const uint8_t& position);
        uint64_t getBlackPawnAttackMoves(const Board& board, const uint8_t& position);
        uint64_t getBlackPawnMoves(const Board& board, const uint8_t& position);

        uint64_t getKnightMoves(const Board& board, const uint8_t& knight_position);
        uint64_t getBishopMoves(const Board& board, const uint8_t& bishop_position);
        uint64_t getBishopMovesUnfiltered(const uint64_t &occupancy, const uint8_t& bishop_position);
        uint64_t getRookMoves(const Board& board, const uint8_t& rook_position);
        uint64_t getRookMovesUnfiltered(const uint64_t &occupnacy, const uint8_t& rook_position);
        uint64_t getQueenMoves(const Board& board, const uint8_t& queen_position);
        uint64_t getKingMoves(const Board& board, const uint8_t& king_position);
        uint64_t getKingCastles(const Board& board, const uint8_t& king_position);

        uint64_t getBishopMovesSlow(const uint64_t& occupancy, const uint8_t& p_x, const uint8_t& p_y);
        uint64_t getRookMovesSlow(const uint64_t& occupancy, const uint8_t& p_x, const uint8_t& p_y);

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

        std::array<uint64_t, 64> rook_masks;
        std::array<uint64_t, 64> bishop_masks;

        // ~ Magic ~
        std::array<uint64_t, 64> rook_magic = { 0x5080009440008420, 0x200a0020410052, 0x4020000a80600400, 0x4558040004900900,
                                                0x4a08080002200400, 0x1600008410031a00, 0x880008500a68152, 0x2000141840a002d,
                                                0x60080020001000, 0x81800990001040, 0x300020610250440c, 0x180210c20200,
                                                0x840080046010c08, 0x22041001100202, 0xa101002505200, 0x1000200022014042,
                                                0x80000840102000, 0xa2045c200080100a, 0x2008031042001088, 0x101100410008,
                                                0x80811008401004, 0x100020010080481, 0xa10ac040208b0409, 0x1008020003842021,
                                                0x21a502a00106100, 0x9001120820004000, 0x84102200100, 0x20022801080010,
                                                0x404280020200118, 0x4004030032042420, 0x200120081500040, 0x46900010200a002,
                                                0x1880c40802100010, 0x428084000540020, 0x9000300828100004, 0x4820849200448,
                                                0x8010810020200d02, 0x8200920000802400, 0xc04080012c820002, 0x83000401c80040,
                                                0x2040008101002800, 0x8c00081020200, 0x31402068052000, 0x8022010041000,
                                                0x800402032c0400, 0xc004800e01010, 0x940400100051c84, 0x1000444010808,
                                                0x802a41080410100, 0x420004000080a08, 0x40045030000880, 0x9020340254200408,
                                                0x10480080021008, 0xa01040900008100, 0x8004208800501400, 0x804111a200,
                                                0x112101f022008046, 0x804210310494001, 0x4085100200125, 0x120028200402,
                                                0x40087902001022, 0x140c804002204c1, 0x42482820821004c4, 0x404a405450082 };
        uint64_t rook_lookup[64][4096];

        std::array<uint64_t, 64> bishop_magic = { 0x2000823004001080, 0x9044420845a000, 0x110100920160205, 0x4110104050400210,
                                                  0x1004002000040002, 0x100a1504000000, 0x8010280019200300, 0x80a100422000,
                                                  0x802400480801106, 0x430810288e0040, 0x401808101012c02, 0x40480203004012c,
                                                  0x220080c10400, 0x2000486950400004, 0x400093818421c, 0x1208002886002c08,
                                                  0x8a02220040a081, 0x8400088401104, 0x1010800040c10044, 0xa000200802000400,
                                                  0x2c3000202420480, 0x8180400809210400, 0x93848800d401200, 0x2200104812003080,
                                                  0x60054021bc80080, 0x10221186120103c0, 0x2060100000458081, 0x428180000a20020,
                                                  0x12088c0000802000, 0x8400204041a40200, 0x4020900804004310, 0x502009320840014,
                                                  0x440401900100, 0x4200602882a02080, 0x4c002010100c010c, 0x4010140400080120,
                                                  0x80e0008400328020, 0x48411120290020, 0x4120920004005402, 0x400c840208104120,
                                                  0x2004202004001c00, 0xa095188820012210, 0x2010100120049c0, 0x129102c0802012,
                                                  0x2000028082000c00, 0x20c0492200852008, 0x13004420201100, 0xc000204a00110680,
                                                  0x1001c108941300c1, 0x3d00430490052000, 0x1280880a40a20902, 0x10000498060021,
                                                  0x2200241489205020, 0x2041100241a1180, 0xa2202202028006, 0x230020108933000,
                                                  0x2000021284d80a44, 0x2000820a0110480, 0x8200440462082056, 0x2000021040100410,
                                                  0xd80000030510040, 0x220020200240d064, 0x1007010050043, 0x40b080c0206c0420 };
        uint64_t bishop_lookup[64][512];
};

#endif
