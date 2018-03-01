#include <iostream>
#include <algorithm>
#include "Chess.h"

using namespace std;

void Chess::printBoard() {
    cout << "White:" << board.color[WHITE] << endl;
    printBitBoard(this->board.color[WHITE]);
    cout << "Black:" << board.color[BLACK] << endl;
    printBitBoard(this->board.color[BLACK]);
    cout << "Current side: " << board.side << endl;
    cout << "Current hash: " << board.tt.getHashValue() << endl;
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

void Chess::PerftDivided(uint8_t depth)
{
    vector<Move> moves;
    this->getAllMoves(moves);

    uint64_t totalNodes = 0;
    for (auto &move: moves) {
        cout << moveToString(move) << ": ";
        board.makeMove(move);
        uint64_t nodes = Perft(depth - 1);
        totalNodes += nodes;
        cout << nodes << endl;
        board.unmakeMove(move);
    }
    cout << "TOTAL: " << totalNodes << endl;
}

uint64_t Chess::Perft(uint8_t depth)
{
    if (depth == 0) return 1;
    vector<Move> moves;
    this->getAllMoves(moves);
    if (depth == 1) return moves.size();

    uint64_t nodes = 0;

    for (auto &move: moves) {
        board.makeMove(move);
        nodes += Perft(depth - 1);
        board.unmakeMove(move);
    }
    return nodes;
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
    return uint64_t(1) << ((y*8)+x);
}

string Chess::moveToString(Move mv) {
    string retstring = bitToStringPosition(mv.from_bitmove);
    retstring += bitToStringPosition(mv.to_bitmove);
    switch( mv.move_type ) {
        case PROMOTION_BISHOP_CAPTURE:
        case PROMOTION_BISHOP:
            retstring += 'b';
            break;
        case PROMOTION_ROOK_CAPTURE:
        case PROMOTION_ROOK:
            retstring += 'r';
            break;
        case PROMOTION_QUEEN_CAPTURE:
        case PROMOTION_QUEEN:
            retstring += 'q';
            break;
        case PROMOTION_KNIGHT_CAPTURE:
        case PROMOTION_KNIGHT:
            retstring += 'n';
            break;
        default:
            break;
    }
    return retstring;
}

Move Chess::stringToMove(string mv) {
    Move return_move;
    return_move.from_bitmove = stringToBitPosition(mv.substr(0,2));
    return_move.to_bitmove = stringToBitPosition(mv.substr(2,2));

    return_move.color = board.side;
    return_move.move_type = QUIET;

    bool capture = false;

    //get the moving piece and, if applicable, the captured piece
    for(int piece=0; piece<NUM_PIECES; ++piece) {
        if( return_move.from_bitmove & this->board.pieces[piece] ) {
            return_move.moving_piece = PIECE(piece);
        }
        if( return_move.to_bitmove & this->board.pieces[piece] ) {
            return_move.captured_piece = PIECE(piece);
            return_move.move_type = CAPTURE;
            capture = true;
        }
    }

    //handle promotions
    if( mv.size() == 5 ) {
        switch(mv[4]) {
            case 'q':
                return_move.move_type = PROMOTION_QUEEN;
                if( capture ) {
                    return_move.move_type = PROMOTION_QUEEN_CAPTURE;
                }
                break;
            case 'r':
                return_move.move_type = PROMOTION_ROOK;
                if( capture ) {
                    return_move.move_type = PROMOTION_ROOK_CAPTURE;
                }
                break;
            case 'b':
                return_move.move_type = PROMOTION_BISHOP;
                if( capture ) {
                    return_move.move_type = PROMOTION_BISHOP_CAPTURE;
                }
                break;
            case 'n':
                return_move.move_type = PROMOTION_KNIGHT;
                if( capture ) {
                    return_move.move_type = PROMOTION_KNIGHT_CAPTURE;
                }
                break;
        }
    }

    //handle double pushes
    else if ( (return_move.moving_piece == PAWN) &&
               ( ((return_move.to_bitmove >> 16) == return_move.from_bitmove) ||
                 ((return_move.to_bitmove << 16) == return_move.from_bitmove)) ) {
        return_move.move_type = DOUBLE_PAWN;
    }

    //handle en passant captures
    else if( (return_move.to_bitmove & board.EN_PASSANT_SQUARE) && (return_move.moving_piece == PAWN) ) {
        return_move.move_type = EN_PASSANT_CAPTURE;
    }

    //handle castling
    else if (return_move.moving_piece == KING) {
        if( (return_move.to_bitmove >> 2) == return_move.from_bitmove ) {
            return_move.move_type = CASTLING_KINGSIDE;
        }
        else if( (return_move.to_bitmove << 2) == return_move.from_bitmove ) {
            return_move.move_type = CASTLING_QUEENSIDE;
        }
    }

    if(return_move.move_type == QUIET && capture) {
        return_move.move_type = CAPTURE;
    }

    return return_move;
}
