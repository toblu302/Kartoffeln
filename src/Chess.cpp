#include <iostream>
#include <algorithm>
#include "Chess.h"

using namespace std;

void Chess::printBoard() {
    cout << "White:" << endl;
    printBitBoard(this->board.color[WHITE]);
    cout << "Black:" << endl;
    printBitBoard(this->board.color[BLACK]);
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
    boardStateStack.push( board );

    //Regardless of movetype, a piece will always move from one square to another
    //This thus deals with quiet moves, which is why they are not included in the switch
    board.pieces[ mv.moving_piece ] ^= mv.from_bitmove | mv.to_bitmove;
    board.color[ mv.color ] ^= mv.from_bitmove | mv.to_bitmove;

    COLOR opposite_color = (mv.color == WHITE) ? BLACK : WHITE;

    //store the current en passant square and then reset it
    uint64_t en_passant_target = board.EN_PASSANT_SQUARE >> 8;

    if( turn == 'b' ) {
        en_passant_target = board.EN_PASSANT_SQUARE << 8;
    }
    board.EN_PASSANT_SQUARE = 0;

    //change castling rights, if needed
    if( mv.moving_piece == KING && turn == 'w' ) {
        board.CASTLE_RIGHTS &= ~(0xFF);
    }
    if( mv.moving_piece == KING && turn == 'b' ) {
        board.CASTLE_RIGHTS &= ~( uint64_t(0xFF) << 56 );
    }
    if( (mv.from_bitmove|mv.to_bitmove) & (1<<7) )  {
        board.CASTLE_RIGHTS &= ~(1<<6);
    }
    if( (mv.from_bitmove|mv.to_bitmove) & 1 )  {
        board.CASTLE_RIGHTS &= ~(1<<2);
    }
    if( (mv.from_bitmove|mv.to_bitmove) & (uint64_t(1)<<56) )  {
        board.CASTLE_RIGHTS &= ~(uint64_t(1)<<58);
    }
    if( (mv.from_bitmove|mv.to_bitmove) & (uint64_t(1)<<63) )  {
        board.CASTLE_RIGHTS &= ~(uint64_t(1)<<62);
    }


    switch( mv.move_type ) {
        case CAPTURE:
            board.pieces[ mv.captured_piece ] ^= mv.to_bitmove;
            board.color[ opposite_color ] ^= mv.to_bitmove;
            break;

        case EN_PASSANT_CAPTURE:
            board.pieces[ PAWN ] ^= en_passant_target;
            board.color[ opposite_color ] ^= en_passant_target;
            break;

        //set the en passant sqaure if we made a double push with pawn
        case DOUBLE_PAWN:
            if( turn == 'w' ) {
                board.EN_PASSANT_SQUARE = mv.to_bitmove >> 8;
            }
            else {
                board.EN_PASSANT_SQUARE = mv.to_bitmove << 8;
            }
            break;

        //castling
        case CASTLING_KINGSIDE:
            board.pieces[ ROOK ] ^= (uint64_t(mv.from_bitmove) << 3) | (uint64_t(mv.from_bitmove) << 1);
            board.color[ mv.color ] ^= (uint64_t(mv.from_bitmove) << 3) | (uint64_t(mv.from_bitmove) << 1);
            break;

        case CASTLING_QUEENSIDE:
            board.pieces[ ROOK ] ^= (uint64_t(mv.from_bitmove) >> 4) | (uint64_t(mv.from_bitmove) >> 1);
            board.color[ mv.color ] ^= (uint64_t(mv.from_bitmove) >> 4) | (uint64_t(mv.from_bitmove) >> 1);
            break;

        //promotions
        case PROMOTION_KNIGHT_CAPTURE:
            board.pieces[ mv.captured_piece ] ^= mv.to_bitmove;
            board.color[ opposite_color ] ^= mv.to_bitmove;
        case PROMOTION_KNIGHT:
            board.pieces[ PAWN ] ^= mv.to_bitmove;
            board.pieces[ KNIGHT ] ^= mv.to_bitmove;
            break;

        case PROMOTION_QUEEN_CAPTURE:
            board.pieces[ mv.captured_piece ] ^= mv.to_bitmove;
            board.color[ opposite_color ] ^= mv.to_bitmove;
        case PROMOTION_QUEEN:
            board.pieces[ PAWN ] ^= mv.to_bitmove;
            board.pieces[ QUEEN ] ^= mv.to_bitmove;
            break;

        case PROMOTION_ROOK_CAPTURE:
            board.pieces[ mv.captured_piece ] ^= mv.to_bitmove;
            board.color[ opposite_color ] ^= mv.to_bitmove;
        case PROMOTION_ROOK:
            board.pieces[ PAWN ] ^= mv.to_bitmove;
            board.pieces[ ROOK ] ^= mv.to_bitmove;
            break;

        case PROMOTION_BISHOP_CAPTURE:
            board.pieces[ mv.captured_piece ] ^= mv.to_bitmove;
            board.color[ opposite_color ] ^= mv.to_bitmove;
        case PROMOTION_BISHOP:
            board.pieces[ PAWN ] ^= mv.to_bitmove;
            board.pieces[ BISHOP ] ^= mv.to_bitmove;
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
    board = boardStateStack.top();
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
    uint64_t this_side_pieces = (turn=='w') ? board.color[WHITE] : board.color[BLACK];

    uint64_t blockers = (turn=='w') ? board.color[BLACK] : board.color[WHITE]; //get the other players pieces
    uint32_t king_position = __builtin_ffsll( board.pieces[KING] & blockers )-1; //get the other players king

    uint64_t diagonal_sliders = getSlidingAlongDiagonalA1H8(king_position, blockers) | getSlidingAlongDiagonalA8H1(king_position, blockers);
    uint64_t vertical_sliders = getSlidingAlongFile(king_position, blockers) | getSlidingAlongRank(king_position, blockers);

    uint64_t bitMoves = uint64_t(0);
    bitMoves |= getKnightMoves(king_position, 0) & board.pieces[KNIGHT];
    bitMoves |= getKingMoves(king_position, blockers) & board.pieces[KING];
    bitMoves |= diagonal_sliders & board.pieces[BISHOP];
    bitMoves |= diagonal_sliders & board.pieces[QUEEN];
    bitMoves |= vertical_sliders & board.pieces[ROOK];
    bitMoves |= vertical_sliders & board.pieces[QUEEN];

    if( turn == 'w' ) {
        bitMoves |= getBlackPawnAttackMoves( uint64_t(1) << king_position ) & board.pieces[PAWN];
    }
    else {
        bitMoves |= getWhitePawnAttackMoves( uint64_t(1) << king_position ) & board.pieces[PAWN];
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

Move Chess::stringToMove(string mv) {
    Move return_move;
    return_move.from_bitmove = stringToBitPosition(mv.substr(0,2));
    return_move.to_bitmove = stringToBitPosition(mv.substr(2,2));

    return_move.color = (turn == 'w') ? WHITE : BLACK;
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
