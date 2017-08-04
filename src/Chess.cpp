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

void Chess::PerftDivided(uint8_t depth)
{
    vector<Move> moves;
    this->getAllMoves(moves);

    uint64_t totalNodes = 0;
    for (int i = 0; i < moves.size(); i++) {
        cout << moveToString(moves[i]) << ": ";
        makeMove(moves[i]);
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
    vector<Move> moves;
    this->getAllMoves(moves);
    if (depth == 1) return moves.size();

    uint64_t nodes = 0;

    for (int i = 0; i < moves.size(); i++) {
        makeMove(moves[i]);
        nodes += Perft(depth - 1);
        popMove();
    }
    return nodes;
}

bool Chess::isValid(Move mv) {
    makeMove( mv );
    bool retValue = (isChecking() == false);
    popMove();
    return retValue;
}

void Chess::makeMove(Move mv) {
    boardStateStack.push( BOARD );

    //Regardless of movetype, a piece will always move from one square to another
    //This thus deals with quiet moves, which is why they are not included in the switch
    BOARD[ mv.from_bitboard ] ^= mv.from_bitmove | mv.to_bitmove;
    BOARD[ mv.color ] ^= mv.from_bitmove | mv.to_bitmove;

    BITBOARDS opposite_color = (mv.color == WHITE) ? BLACK : WHITE;

    //store the current en passant square and then reset it
    uint64_t en_passant_target = BOARD[EN_PASSANT_SQUARE] >> 8;
    if( turn == 'b' ) {
        en_passant_target = BOARD[EN_PASSANT_SQUARE] << 8;
    }
    BOARD[EN_PASSANT_SQUARE] = 0;

    //change castling rights, if needed
    if( mv.from_bitboard == KING && turn == 'w' ) {
        BOARD[CASTLE_RIGHTS] &= ~(0xFF);
    }
    if( mv.from_bitboard == KING && turn == 'b' ) {
        BOARD[CASTLE_RIGHTS] &= ~( uint64_t(0xFF) << 56 );
    }
    if( (mv.from_bitmove|mv.to_bitmove) & (1<<7) )  {
        BOARD[CASTLE_RIGHTS] &= ~(1<<6);
    }
    if( (mv.from_bitmove|mv.to_bitmove) & 1 )  {
        BOARD[CASTLE_RIGHTS] &= ~(1<<2);
    }
    if( (mv.from_bitmove|mv.to_bitmove) & (uint64_t(1)<<56) )  {
        BOARD[CASTLE_RIGHTS] &= ~(uint64_t(1)<<58);
    }
    if( (mv.from_bitmove|mv.to_bitmove) & (uint64_t(1)<<63) )  {
        BOARD[CASTLE_RIGHTS] &= ~(uint64_t(1)<<62);
    }


    switch( mv.move_type ) {
        case CAPTURE:
            BOARD[ mv.captured_bitboard ] ^= mv.to_bitmove;
            BOARD[ opposite_color ] ^= mv.to_bitmove;
            break;

        case EN_PASSANT_CAPTURE:
            BOARD[ PAWN ] ^= en_passant_target;
            BOARD[ opposite_color ] ^= en_passant_target;
            break;

        //set the en passant sqaure if we made a double push with pawn
        case DOUBLE_PAWN:
            if( turn == 'w' ) {
                BOARD[EN_PASSANT_SQUARE] = mv.to_bitmove >> 8;
            }
            else {
                BOARD[EN_PASSANT_SQUARE] = mv.to_bitmove << 8;
            }
            break;

        //castling
        case CASTLING_KINGSIDE:
            BOARD[ ROOK ] ^= (uint64_t(mv.from_bitmove) << 3) | (uint64_t(mv.from_bitmove) << 1);
            BOARD[ mv.color ] ^= (uint64_t(mv.from_bitmove) << 3) | (uint64_t(mv.from_bitmove) << 1);
            break;

        case CASTLING_QUEENSIDE:
            BOARD[ ROOK ] ^= (uint64_t(mv.from_bitmove) >> 4) | (uint64_t(mv.from_bitmove) >> 1);
            BOARD[ mv.color ] ^= (uint64_t(mv.from_bitmove) >> 4) | (uint64_t(mv.from_bitmove) >> 1);
            break;

        //promotions
        case PROMOTION_KNIGHT_CAPTURE:
            BOARD[ mv.captured_bitboard ] ^= mv.to_bitmove;
            BOARD[ opposite_color ] ^= mv.to_bitmove;
        case PROMOTION_KNIGHT:
            BOARD[ PAWN ] ^= mv.to_bitmove;
            BOARD[ KNIGHT ] ^= mv.to_bitmove;
            break;

        case PROMOTION_QUEEN_CAPTURE:
            BOARD[ mv.captured_bitboard ] ^= mv.to_bitmove;
            BOARD[ opposite_color ] ^= mv.to_bitmove;
        case PROMOTION_QUEEN:
            BOARD[ PAWN ] ^= mv.to_bitmove;
            BOARD[ QUEEN ] ^= mv.to_bitmove;
            break;

        case PROMOTION_ROOK_CAPTURE:
            BOARD[ mv.captured_bitboard ] ^= mv.to_bitmove;
            BOARD[ opposite_color ] ^= mv.to_bitmove;
        case PROMOTION_ROOK:
            BOARD[ PAWN ] ^= mv.to_bitmove;
            BOARD[ ROOK ] ^= mv.to_bitmove;
            break;

        case PROMOTION_BISHOP_CAPTURE:
            BOARD[ mv.captured_bitboard ] ^= mv.to_bitmove;
            BOARD[ opposite_color ] ^= mv.to_bitmove;
        case PROMOTION_BISHOP:
            BOARD[ PAWN ] ^= mv.to_bitmove;
            BOARD[ BISHOP ] ^= mv.to_bitmove;
            break;

        default:
            break;
    }

    if( turn == 'b' ) {
        fullTimeMove += 1;
    }
    turn = ('b' + 'w') - turn;
}

void Chess::popMove() {
    BOARD = boardStateStack.top();
    boardStateStack.pop();

    if( turn == 'w' ) {
        fullTimeMove -= 1;
    }
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
    uint64_t this_side_pieces = (turn=='w') ? BOARD[WHITE] : BOARD[BLACK];

    uint64_t blockers = (turn=='w') ? BOARD[BLACK] : BOARD[WHITE]; //get the other players pieces
    uint32_t king_position = __builtin_ffsll( BOARD[KING] & blockers )-1; //get the other players king

    uint64_t diagonal_sliders = getSlidingAlongDiagonalA1H8(king_position, blockers) | getSlidingAlongDiagonalA8H1(king_position, blockers);
    uint64_t vertical_sliders = getSlidingAlongFile(king_position, blockers) | getSlidingAlongRank(king_position, blockers);

    uint64_t bitMoves = uint64_t(0);
    bitMoves |= getKnightMoves(king_position, 0) & BOARD[KNIGHT];
    bitMoves |= getKingMoves(king_position, blockers) & BOARD[KING];
    bitMoves |= diagonal_sliders & BOARD[BISHOP];
    bitMoves |= diagonal_sliders & BOARD[QUEEN];
    bitMoves |= vertical_sliders & BOARD[ROOK];
    bitMoves |= vertical_sliders & BOARD[QUEEN];

    if( turn == 'w' ) {
        bitMoves |= getBlackPawnAttackMoves( uint64_t(1) << king_position ) & BOARD[PAWN];
    }
    else {
        bitMoves |= getWhitePawnAttackMoves( uint64_t(1) << king_position ) & BOARD[PAWN];
    }

    return (bitMoves & this_side_pieces) != 0;
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
