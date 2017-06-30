#include <iostream>
#include <algorithm>
#include "Chess.h"

using namespace std;

void Chess::printBoard() {
    //Implement this
}

void Chess::printBitBoard(uint64_t board) {
    string theString = "";
    for(int y=0; y<8; ++y) {
        string line = "";
        for(int x=0; x<8; ++x) {
            int b = y*8+x;
            uint8_t shifted = (board >> b) & 1;
            if( shifted ) {
                line += "1";
            }
            else {
                line += "0";
            }
        }

        theString = line + "\n" + theString;
    }

    cout << theString << endl;
}

uint64_t Chess::PerftDivided(uint8_t depth)
{
    vector<string> moves;
    this->getAllMoves(moves);
    sort(moves.begin(), moves.end());
    uint64_t totalNodes = 0;
    for (int i = 0; i < moves.size(); i++) {
        cout << moves[i] << ": ";
        pushMove(moves[i]);
        uint64_t nodes = Perft(depth - 1);
        totalNodes += nodes;
        cout << nodes << endl;
        popMove();
    }
    cout << "TOTAL NODES: " << totalNodes << endl;
}

uint64_t Chess::Perft(uint8_t depth)
{
    if (depth == 0) return 1;
    vector<string> moves;
    this->getAllMoves(moves);
    if (depth == 1) return moves.size();

    uint64_t nodes = 0;

    for (int i = 0; i < moves.size(); i++) {
        pushMove(moves[i]);
        nodes += Perft(depth - 1);
        popMove();
    }
    return nodes;
}

void Chess::pushMove(string mv) {
    boardStateStack.push( BOARD );

    uint64_t from = stringToBitPosition( mv.substr(0,2) );
    uint64_t to = stringToBitPosition( mv.substr(2,2) );
    uint64_t bitMove = from | to;

    // if we're attacking a pawn using en passant, remove the attacked pawn
    if( (to & BOARD[EN_PASSANT]) && (from & BOARD[WHITE_PAWN]) ) {
        BOARD[BLACK_PAWN] &= ~(BOARD[EN_PASSANT] >> 8);
        BOARD[ALL_BLACK] &= ~(BOARD[EN_PASSANT] >> 8);
    }
    else if( (to & BOARD[EN_PASSANT]) && (from & BOARD[BLACK_PAWN]) ) {
        BOARD[WHITE_PAWN] &= ~(BOARD[EN_PASSANT] << 8);
        BOARD[ALL_WHITE] &= ~(BOARD[EN_PASSANT] << 8);
    }

    //clear the en passant target square
    BOARD[EN_PASSANT] = 0;

    // if we're moving forward two steps, en passant targets should be set
    if( from & BOARD[WHITE_PAWN] ) {
        if( (from<<16) == to ) {
            BOARD[EN_PASSANT] = from << 8;
        }
    }
    else if( from & BOARD[BLACK_PAWN] ) {
        if( (from>>16) == to ) {
            BOARD[EN_PASSANT] = from >> 8;
        }
    }

    //if we're castling, move the rooks
    if( (from & BOARD[WHITE_KING]) && ((from >> 2) == to) ) {
        BOARD[WHITE_ROOK] ^= 1 | (1<<3);
        BOARD[ALL_WHITE] ^= 1 | (1<<3);
    }
    else if( (from & BOARD[WHITE_KING]) && ((from << 2) == to) ) {
        BOARD[WHITE_ROOK] ^= (1<<7) | (1<<5);
        BOARD[ALL_WHITE] ^= (1<<7) | (1<<5);
    }
    else if( (from & BOARD[BLACK_KING]) && ((from >> 2) == to) ) {
        BOARD[BLACK_ROOK] ^= (uint64_t(1) << 56) | (uint64_t(1) << 59);
        BOARD[ALL_BLACK] ^= (uint64_t(1) << 56) | (uint64_t(1) << 59);
    }
    else if( (from & BOARD[BLACK_KING]) && ((from << 2) == to) ) {
        BOARD[BLACK_ROOK] ^= (uint64_t(1) << 61) | (uint64_t(1) << 63);
        BOARD[ALL_BLACK] ^= (uint64_t(1) << 61) | (uint64_t(1) << 63);
    }

    //if we're moving rooks/kings, remove castling rights
    if( (from & BOARD[WHITE_KING])) {
        BOARD[WHITE_CASTLE_RIGHTS] = 0;
    }
    if( (from & BOARD[BLACK_KING])) {
        BOARD[BLACK_CASTLE_RIGHTS] = 0;
    }
    if( (from|to) & (1<<7) )  {
        BOARD[WHITE_CASTLE_RIGHTS] &= ~(1<<6);
    }
    if( (from|to) & 1 )  {
        BOARD[WHITE_CASTLE_RIGHTS] &= ~(1<<2);
    }
    if( (from|to) & (uint64_t(1)<<56) )  {
        BOARD[BLACK_CASTLE_RIGHTS] &= ~(uint64_t(1)<<58);
    }
    if( (from|to) & (uint64_t(1)<<63) )  {
        BOARD[BLACK_CASTLE_RIGHTS] &= ~(uint64_t(1)<<62);
    }


    //find the relevant board and perform the move on them
    for(int i=0; i<BITBOARD_COUNT; ++i) {
        if( to & BOARD[i] ) {
            BOARD[i] &= ~to;
        }
        else if( from & BOARD[i] ) {
            BOARD[i] ^= bitMove;
        }
    }

    //if the pawn is getting promoted
    if(mv.length() == 5) {
        char promo = mv[4];
        switch( promo ) {
            case 'q':
                if( turn == 'w') {
                    BOARD[WHITE_QUEEN] |= to;
                    BOARD[WHITE_PAWN] &= ~to;
                    BOARD[ALL_WHITE] |= to;
                }
                else {
                    BOARD[BLACK_QUEEN] |= to;
                    BOARD[BLACK_PAWN] &= ~to;
                    BOARD[ALL_BLACK] |= to;
                }
                break;
            case 'n':
                if( turn == 'w') {
                    BOARD[WHITE_KNIGHT] |= to;
                    BOARD[WHITE_PAWN] &= ~to;
                    BOARD[ALL_WHITE] |= to;
                }
                else {
                    BOARD[BLACK_KNIGHT] |= to;
                    BOARD[BLACK_PAWN] &= ~to;
                    BOARD[ALL_BLACK] |= to;
                }
                break;
            case 'b':
                if( turn == 'w') {
                    BOARD[WHITE_BISHOP] |= to;
                    BOARD[WHITE_PAWN] &= ~to;
                    BOARD[ALL_WHITE] |= to;
                }
                else {
                    BOARD[BLACK_BISHOP] |= to;
                    BOARD[BLACK_PAWN] &= ~to;
                    BOARD[ALL_BLACK] |= to;
                }
                break;
            case 'r':
                if( turn == 'w') {
                    BOARD[WHITE_ROOK] |= to;
                    BOARD[WHITE_PAWN] &= ~to;
                    BOARD[ALL_WHITE] |= to;
                }
                else {
                    BOARD[BLACK_ROOK] |= to;
                    BOARD[BLACK_PAWN] &= ~to;
                    BOARD[ALL_BLACK] |= to;
                }
                break;
        }
    }


    turn = ('b' + 'w') - turn;
}

void Chess::popMove() {
    BOARD = boardStateStack.top();
    boardStateStack.pop();

    turn = ('b' + 'w') - turn;
}

//converting a bitboard with a single bit to a position on the board as a string
string Chess::bitToStringPosition(uint64_t bit) {
    uint64_t x;
    for(x=0; x<64; ++x) {
        bit = bit >> 1;
        if( bit == 0 ) {
            break;
        }
    }

    uint8_t column = x%8;
    uint8_t row = (x-column)/8;

    string retString;
    retString += (column+'a');
    retString += (row+'1');
    return retString;
}

string Chess::bitPositionToStringPosition(uint8_t bit) {
    uint64_t x;

    uint8_t column = bit%8;
    uint8_t row = (bit-column)/8;

    string retString;
    retString += (column+'a');
    retString += (row+'1');
    return retString;
}

uint64_t Chess::stringToBitPosition(string position) {
    uint64_t x = position[0]-'a';
    uint64_t y = position[1]-'1';
    return uint64_t(1) << (y*8)+x;
}

bool Chess::isChecking() {
    uint64_t bitMoves;
    if( turn == 'w' ) {
        bitMoves = getWhiteAttacking();
        return (bitMoves & BOARD[BLACK_KING]) != 0;
    }
    bitMoves = getBlackAttacking();
    return (bitMoves & BOARD[WHITE_KING]) != 0;
}
