#include "MoveGenerator.h"

MoveGenerator::MoveGenerator() {
    //Setup constants describing files and ranks as bitboards, where RANK[0] is rank 1 and FILE[0] is file A
    for(int i=0; i<8; ++i) {
        FILE[i] = 0;
        RANK[i] = 0;
        for(int j=0; j<8; ++j) {
            FILE[i] |= uint64_t(1) << (i+8*j);
            RANK[i] |= uint64_t(1) << ((i*8)+j);
        }
    }

    //Calculate the knight moves
    for(int pos=0; pos<64; ++pos) {
        uint64_t shifted = uint64_t(1) << pos;
        uint64_t result = 0;
        result |= (shifted & ~FILE[0] & ~FILE[1] & ~RANK[7]) << 6; //left-up
        result |= (shifted & ~FILE[0] & ~FILE[1] & ~RANK[0]) >> 10; //left-down

        result |= (shifted & ~FILE[0] & ~RANK[6] & ~RANK[7]) << 15; //up-left
        result |= (shifted & ~FILE[7] & ~RANK[6] & ~RANK[7]) << 17; //up-right

        result |= (shifted & ~FILE[6] & ~FILE[7] & ~RANK[7]) << 10; //right-up
        result |= (shifted & ~FILE[6] & ~FILE[7] & ~RANK[0]) >> 6; //right-down

        result |= (shifted & ~FILE[7] & ~RANK[0] & ~RANK[1]) >> 15; //down-right
        result |= (shifted & ~FILE[0] & ~RANK[0] & ~RANK[1]) >> 17; //down-left

        KNIGHT_MOVES[pos] = result;
    }

    //Calculate the king moves
    for(int pos=0; pos<64; ++pos) {
        uint64_t shifted = uint64_t(1) << pos;
        uint64_t result = 0;

        result |= (shifted & ~FILE[0]) >> 1; //left
        result |= (shifted & ~FILE[7]) << 1; //right

        result |= (shifted & ~FILE[0] & ~RANK[0] ) >> 9; //down-left
        result |= (shifted & ~RANK[0] ) >> 8; //down
        result |= (shifted & ~FILE[7] & ~RANK[0] ) >> 7; //down-right

        result |= (shifted & ~FILE[0] & ~RANK[7] ) << 7; //up-left
        result |= (shifted & ~RANK[7] ) << 8; //up
        result |= (shifted & ~FILE[7] & ~RANK[7] ) << 9; //up-right

        KING_MOVES[pos] = result;
    }

    //Calculate the possible sliding moves
    // The "column" is the column of the moving piece for which you want to calculate moves
    // The "occ" (occupancy) is a bitboard with the pieces which will block the moving piece
    // SLIDING_MOVES[column][occ] is a bitboard with the positions to which the piece can move
    for(int column=0; column<8; ++column) {
        for(int occ=0; occ < 256; ++occ) {

            uint8_t result = 0;

            //increasing
            for(int curr=column+1; curr<8; ++curr) {
                result |= (uint8_t(1) << curr);

                if( (uint8_t(1) << curr) & occ ) {
                    break;
                }
            }

            //decreasing
            for(int curr=column-1; curr>=0; --curr) {
                result |= uint8_t(1) << curr;

                if( (uint8_t(1) << curr) & occ ) {
                    break;
                }
            }

            SLIDING_MOVES[column][occ] = result;
        }
    }
}

void MoveGenerator::getAllMoves(Board& board, priority_queue<Move> &moves) {

    handle_piece(board, moves, KNIGHT, &MoveGenerator::getKnightMoves);
    handle_piece(board, moves, BISHOP, &MoveGenerator::getBishopMoves);

    //handle pawns
    if( board.side == WHITE ) {
        handle_piece(board, moves, PAWN, &MoveGenerator::getWhitePawnMoves);
    }
    else {
        handle_piece(board, moves, PAWN, &MoveGenerator::getBlackPawnMoves);
    }

    handle_piece(board, moves, KING, &MoveGenerator::getKingMoves);
    handle_piece(board, moves, QUEEN, &MoveGenerator::getQueenMoves);
    handle_piece(board, moves, ROOK, &MoveGenerator::getRookMoves);
}

bool MoveGenerator::isValid(Board& board, const Move& mv) {
    board.makeMove( mv );
    bool retValue = (isChecking(board) == false);
    board.unmakeMove( mv );
    return retValue;
}

bool MoveGenerator::isChecking(Board& board) {
    uint64_t this_side_pieces = (board.side == WHITE) ? board.color[WHITE] : board.color[BLACK];

    uint64_t blockers = (board.side == WHITE) ? board.color[BLACK] : board.color[WHITE]; //get the other players pieces
    uint32_t king_position = __builtin_ffsll( board.pieces[KING] & blockers )-1; //get the other players king

    uint64_t diagonal_sliders = getSlidingAlongDiagonalA1H8(board, king_position, blockers) | getSlidingAlongDiagonalA8H1(board, king_position, blockers);
    uint64_t vertical_sliders = getSlidingAlongFile(board, king_position, blockers) | getSlidingAlongRank(board, king_position, blockers);

    uint64_t bitMoves = uint64_t(0);
    bitMoves |= KNIGHT_MOVES[king_position] & ~blockers & board.pieces[KNIGHT];
    bitMoves |= KING_MOVES[king_position] & ~blockers & board.pieces[KING];
    bitMoves |= diagonal_sliders & board.pieces[BISHOP];
    bitMoves |= diagonal_sliders & board.pieces[QUEEN];
    bitMoves |= vertical_sliders & board.pieces[ROOK];
    bitMoves |= vertical_sliders & board.pieces[QUEEN];

    if(board.side == WHITE) {
        bitMoves |= getBlackPawnAttackMoves(board, king_position) & board.pieces[PAWN];
    }
    else {
        bitMoves |= getWhitePawnAttackMoves(board, king_position) & board.pieces[PAWN];
    }

    return (bitMoves & this_side_pieces) != 0;
}

void MoveGenerator::handle_piece(Board& board, priority_queue<Move> &moves, PIECE piece, uint64_t (MoveGenerator::*get_moves)(const Board& board, const uint8_t &position) ) {
    Move mv;
    mv.color = board.side;
    mv.previous_castling_rights = board.CASTLE_RIGHTS;
    mv.previous_en_passant_square = board.EN_PASSANT_SQUARE;
    mv.moving_piece = piece;

    uint64_t pieces = board.getPiecesOfColor(board.side, piece);

    while( pieces != 0 ) {
        uint8_t position = get_next_value(pieces);
        uint64_t shifted = uint64_t(1) << position;
        mv.from_bitmove = shifted;
        uint64_t bitmoves = (this->*get_moves)( board, position );
        while( (mv.to_bitmove = get_next_bitboard(bitmoves)) != 0 ) {
            mv.move_type = QUIET;

            //Handle captures
            if( isCapture(board, mv) ) {
                mv.move_type = CAPTURE;
                for( int captured_board=0; captured_board<NUM_PIECES; ++captured_board ) {
                    if( board.pieces[ captured_board ] & mv.to_bitmove ) {
                        mv.captured_piece = PIECE(captured_board);
                    }
                }
            }

            if(piece == KING) {
                if( (mv.to_bitmove >> 2) == mv.from_bitmove ) {
                    mv.move_type = CASTLING_KINGSIDE;
                }
                else if( (mv.to_bitmove << 2) == mv.from_bitmove ) {
                    mv.move_type = CASTLING_QUEENSIDE;
                }
            }
            if(piece == PAWN) {
                handle_pawn_moves(board, mv, moves);
            }
            else {
                if(isValid(board, mv)) moves.emplace( mv );
            }
        }
    }
}

void MoveGenerator::handle_pawn_moves(Board& board, Move& mv, priority_queue<Move>& moves) {
    //handle promotions
    if( ((mv.to_bitmove & 0xFF) | (mv.to_bitmove & (uint64_t(0xFF) << 56))) ) {
        if( mv.move_type == CAPTURE ) {
            mv.move_type = PROMOTION_QUEEN_CAPTURE;
            if( isValid(board, mv) ) moves.emplace(mv);
            mv.move_type = PROMOTION_BISHOP_CAPTURE;
            if( isValid(board, mv) ) moves.emplace(mv);
            mv.move_type = PROMOTION_KNIGHT_CAPTURE;
            if( isValid(board, mv) ) moves.emplace(mv);
            mv.move_type = PROMOTION_ROOK_CAPTURE;
            if( isValid(board, mv) ) moves.emplace(mv);
            return;
        }
        mv.move_type = PROMOTION_QUEEN;
        if( isValid(board, mv) ) moves.emplace(mv);
        mv.move_type = PROMOTION_BISHOP;
        if( isValid(board, mv) ) moves.emplace(mv);
        mv.move_type = PROMOTION_KNIGHT;
        if( isValid(board, mv) ) moves.emplace(mv);
        mv.move_type = PROMOTION_ROOK;
        if( isValid(board, mv) ) moves.emplace(mv);
        return;
    }

    //handle double pushes
    else if ( ((mv.to_bitmove >> 16) == mv.from_bitmove) ||
              ((mv.to_bitmove << 16) == mv.from_bitmove)) {
        mv.move_type = DOUBLE_PAWN;
    }

    //handle en passant captures
    else if( (mv.to_bitmove & board.EN_PASSANT_SQUARE) && (mv.moving_piece == PAWN) ) {
        mv.move_type = EN_PASSANT_CAPTURE;
    }

    if( isValid(board, mv) ) moves.emplace(mv);
}

/*************************************
*    bitboard operations - pieces
**************************************/

uint64_t MoveGenerator::getWhitePawnAttackMoves(const Board &board, const uint8_t &position) {
    uint64_t pawn = (uint64_t(1) << position);
    uint64_t bitwiseMove = ((pawn & ~FILE[0] ) << 7); //attacking forward left
    bitwiseMove |= ((pawn & ~FILE[7] ) << 9); //attacking forward right
    return bitwiseMove;
}

uint64_t MoveGenerator::getWhitePawnMoves(const Board &board, const uint8_t &position) {
    uint64_t pawn = (uint64_t(1) << position);
    uint64_t bitwiseMove = (pawn << 8) & ~(board.color[BLACK] | board.color[WHITE]); //moving forward 1 step

    uint64_t unmovedPawns = (uint64_t(0xFF) << 8) & pawn;
    unmovedPawns = (unmovedPawns << 8) & ~(board.color[BLACK] | board.color[WHITE]); //try to move forward 1 step
    unmovedPawns = (unmovedPawns << 8) & ~(board.color[BLACK] | board.color[WHITE]); //try to move forward another step
    bitwiseMove |= unmovedPawns;

    bitwiseMove |= getWhitePawnAttackMoves(board, position) & (board.color[BLACK] | board.EN_PASSANT_SQUARE);

    return bitwiseMove;
}

uint64_t MoveGenerator::getBlackPawnAttackMoves(const Board &board, const uint8_t &position) {
    uint64_t pawn = (uint64_t(1) << position);
    uint64_t bitwiseMove = ((pawn & ~FILE[7] ) >> 7); //attacking forward left
    bitwiseMove |= ((pawn & ~FILE[0] ) >> 9); //attacking forward right

    return bitwiseMove;
}

uint64_t MoveGenerator::getBlackPawnMoves(const Board &board, const uint8_t &position) {
    uint64_t pawn = (uint64_t(1) << position);
    uint64_t bitwiseMove = (pawn >> 8) & ~(board.color[BLACK] | board.color[WHITE]); //moving forward 1 step

    uint64_t unmovedPawns = (uint64_t(0xFF) << 48 ) & pawn;
    unmovedPawns = (unmovedPawns >> 8) & ~(board.color[BLACK] | board.color[WHITE]); //try to move forward 1 step
    unmovedPawns = (unmovedPawns >> 8) & ~(board.color[BLACK] | board.color[WHITE]); //try to move forward another step
    bitwiseMove |= unmovedPawns;

    bitwiseMove |= getBlackPawnAttackMoves(board, position) & (board.color[WHITE] | board.EN_PASSANT_SQUARE);
    return bitwiseMove;
}

uint64_t MoveGenerator::getKnightMoves(const Board& board, const uint8_t &knight_position) {
    return KNIGHT_MOVES[ knight_position ] & ~board.color[board.side];
}

uint64_t MoveGenerator::getKingMoves(const Board& board, const uint8_t &king_position) {
    uint64_t bitMoves = KING_MOVES[ king_position ] & ~board.color[board.side];
    bitMoves |= getKingCastles(board, king_position);
    return bitMoves;
}

uint64_t MoveGenerator::getKingCastles(const Board& board, const uint8_t &king_position) {
    uint64_t attackingMoves = 0;
    uint64_t king = uint64_t(1) << king_position;
    uint64_t blockers = board.color[WHITE] | board.color[BLACK];
    uint64_t castle_rights = 0;

    if( board.side == WHITE ) {
        attackingMoves = getBlackAttacking(board);
        castle_rights = board.CASTLE_RIGHTS;
    }
    else {
        attackingMoves = getWhiteAttacking(board);
        castle_rights = board.CASTLE_RIGHTS;
    }

    uint64_t bitMoves = 0;


    uint64_t kingSide = (king << 2) & castle_rights;
    uint64_t queenSide = (king >> 2) & castle_rights;

    uint64_t kingPassingSquares = king | (king << 1) | (king << 2);
    uint64_t queenPassingSquares = king | (king >> 1) | (king >> 2);

    uint64_t kingEmptySquares = (king << 1) | (king << 2);
    uint64_t queenEmptySquares = (king >> 1) | (king >> 2) | (king >> 3);

    if( kingSide && (kingPassingSquares & attackingMoves) == 0 && (kingEmptySquares & blockers) == 0 ){
        bitMoves |= kingSide;
    }
    if( queenSide && (queenPassingSquares & attackingMoves) == 0 && (queenEmptySquares & blockers) == 0 ){
        bitMoves |= queenSide;
    }

    return bitMoves;
}

uint64_t MoveGenerator::getBishopMoves(const Board& board, const uint8_t &bishop_position) {
    uint64_t bitMoves = getSlidingAlongDiagonalA1H8( board, bishop_position, board.color[board.side] );
    bitMoves |= getSlidingAlongDiagonalA8H1( board, bishop_position, board.color[board.side] );
    return bitMoves;
}

uint64_t MoveGenerator::getRookMoves(const Board& board, const uint8_t &rook_position) {
    uint64_t bitMoves = getSlidingAlongRank( board, rook_position, board.color[board.side] );
    bitMoves |= getSlidingAlongFile( board, rook_position, board.color[board.side] );
    return bitMoves;
}

uint64_t MoveGenerator::getQueenMoves(const Board& board, const uint8_t &queen_position) {
    uint64_t bitMoves = getRookMoves(board, queen_position);
    bitMoves |= getBishopMoves(board, queen_position);
    return bitMoves;
}

/*************************************
*    bitboard operations - sliding
**************************************/

uint64_t MoveGenerator::getSlidingMovesFromOccupancy(const Board& board, const uint8_t& column, const uint8_t& occupancy) {
    uint64_t retValue = SLIDING_MOVES[column][occupancy];
    return retValue;
}

uint64_t MoveGenerator::getSlidingAlongRank(const Board& board, const uint8_t& piece_position, const uint64_t& blockers) {
    //convert a rank to a 8 bit number representing where the pieces on the rank are at
    uint8_t column = piece_position%8;
    uint8_t row = (piece_position-column)/8;

    uint8_t occupancy = ( (board.color[WHITE] | board.color[BLACK])  >> (row*8)) & 255;

    return (getSlidingMovesFromOccupancy(board, column, occupancy) << (row*8)) & ~blockers;
}

uint64_t MoveGenerator::getSlidingAlongFile(const Board& board, const uint8_t& piece_position, const uint64_t& blockers) {

    uint8_t column = piece_position%8;
    uint8_t row = (piece_position-column)/8;

    //convert a rank to a 8 bit number representing where the pieces on the column are at
    uint64_t ALL_PIECES = (board.color[WHITE] | board.color[BLACK]) >> column;
    uint8_t occupancy = 0;
    for(int i=0; i<8; ++i) {
        occupancy |= ( (ALL_PIECES >> i*8) << i) & (1<<i);
    }

    //convert back into a column
    uint64_t sliding_moves = getSlidingMovesFromOccupancy(board, row, occupancy);
    uint64_t bitMoves = 0;
    for(int i=0; i<8; ++i) {
        bitMoves |= ((sliding_moves & (1<<i)) >> i) << (i*8);
    }

    //return to be about the column and return
    return (bitMoves << column) & ~blockers;
}

uint64_t MoveGenerator::getSlidingAlongDiagonalA1H8(const Board& board, const uint8_t& piece_position, const uint64_t& blockers) {

    uint64_t ALL_PIECES = (board.color[WHITE] | board.color[BLACK]);

    uint8_t column = piece_position%8;
    uint8_t row = (piece_position-column)/8;

    uint8_t occupancy = 0;
    int8_t r = row-column;
    for(uint8_t c=0; c<8; ++c) {
        if(r>=0 && r<=7) {
            occupancy |= (((ALL_PIECES >> (r*8+c)) & 1) << c);
        }

        ++r;
    }

    //convert back into a column
    uint64_t sliding_moves = getSlidingMovesFromOccupancy(board, column, occupancy);
    uint64_t bitMoves = 0;

    r = row-column;
    for(uint8_t c=0; c<8; ++c) {
        if(r>=0 && r<=7) {
            bitMoves |= ((sliding_moves>>c)&1) << (r*8+c);
        }

        ++r;
    }

    //return to be about the column and return
    return bitMoves & ~blockers;
}

uint64_t MoveGenerator::getSlidingAlongDiagonalA8H1(const Board& board, const uint8_t& piece_position, const uint64_t& blockers) {

    uint64_t ALL_PIECES = (board.color[WHITE] | board.color[BLACK]);

    uint8_t column = piece_position%8;
    uint8_t row = (piece_position-column)/8;

    uint8_t occupancy = 0;
    int8_t r = row+column;
    for(uint8_t c=0; c<8; ++c) {
        if(r>=0 && r<=7) {
            occupancy |= (((ALL_PIECES >> (r*8+c)) & 1) << c);
        }

        --r;
    }

    //convert back into a column
    uint64_t sliding_moves = getSlidingMovesFromOccupancy(board, column, occupancy);
    uint64_t bitMoves = 0;

    r = row+column;
    for(uint8_t c=0; c<8; ++c) {
        if(r>=0 && r<=7) {
            bitMoves |= ((sliding_moves>>c)&1) << (r*8+c);
        }

        --r;
    }

    //return to be about the column and return
    return bitMoves & ~blockers;
}

/***********************
*    helpful methods
************************/

uint64_t MoveGenerator::getWhiteAttacking(const Board& board) {
    uint64_t blockers = board.color[WHITE];
    uint64_t bitMoves = 0;
    for(int x=0; x<64; ++x) {
        uint64_t shifted = uint64_t(1) << x;
        if( board.pieces[PAWN] & shifted & board.color[WHITE] ) {
            bitMoves |= getWhitePawnAttackMoves( board, x );
        }
        else if( board.pieces[KING] & shifted & board.color[WHITE] ) {
            bitMoves |= KING_MOVES[x] & ~blockers;
        }
        else if( board.pieces[ KNIGHT ] & shifted & board.color[WHITE] ) {
            bitMoves |= KNIGHT_MOVES[x] & ~blockers;
        }
        else if( board.pieces[ ROOK ] & shifted & board.color[WHITE] ) {
            bitMoves |= getSlidingAlongRank( board, x, blockers );
            bitMoves |= getSlidingAlongFile( board, x, blockers );
        }
        else if( board.pieces[ BISHOP ] & shifted & board.color[WHITE] ) {
            bitMoves |= getSlidingAlongDiagonalA1H8( board, x, blockers );
            bitMoves |= getSlidingAlongDiagonalA8H1( board, x, blockers );
        }
        else if( board.pieces[ QUEEN ] & shifted & board.color[WHITE] ) {
            bitMoves |= getSlidingAlongDiagonalA1H8( board, x, blockers );
            bitMoves |= getSlidingAlongDiagonalA8H1( board, x, blockers );
            bitMoves |= getSlidingAlongRank( board, x, blockers );
            bitMoves |= getSlidingAlongFile( board, x, blockers );
        }
    }
    return bitMoves;
}

uint64_t MoveGenerator::getBlackAttacking(const Board& board) {
    uint64_t blockers = board.color[BLACK];
    uint64_t bitMoves = 0;
    for(int x=0; x<64; ++x) {
        uint64_t shifted = uint64_t(1) << x;

        if( board.pieces[PAWN] & shifted & board.color[BLACK] ) {
            bitMoves |= getBlackPawnAttackMoves( board, x );
        }
        else if( board.pieces[KING] & shifted & board.color[BLACK] ) {
            bitMoves |= KING_MOVES[x] & ~blockers;
        }
        else if( board.pieces[ KNIGHT ] & shifted & board.color[BLACK] ) {
            bitMoves |= KNIGHT_MOVES[x] & ~blockers;
        }
        else if( board.pieces[ ROOK ] & shifted & board.color[BLACK] ) {
            bitMoves |= getSlidingAlongRank( board, x, blockers );
            bitMoves |= getSlidingAlongFile( board, x, blockers );
        }
        else if( board.pieces[ BISHOP ] & shifted & board.color[BLACK] ) {
            bitMoves |= getSlidingAlongDiagonalA1H8( board, x, blockers );
            bitMoves |= getSlidingAlongDiagonalA8H1( board, x, blockers );
        }
        else if( board.pieces[ QUEEN ] & shifted & board.color[BLACK] ) {
            bitMoves |= getSlidingAlongDiagonalA1H8( board, x, blockers );
            bitMoves |= getSlidingAlongDiagonalA8H1( board, x, blockers );
            bitMoves |= getSlidingAlongRank( board, x, blockers );
            bitMoves |= getSlidingAlongFile( board, x, blockers );
        }
    }
    return bitMoves;
}

uint64_t MoveGenerator::get_next_bitboard(uint64_t &bitfield) {
    if( bitfield == 0 ) {
        return 0;
    }
    uint64_t s = (uint64_t(1) << (__builtin_ffsll(bitfield)-1));
    bitfield &= ~s;
    return s;
}

uint8_t MoveGenerator::get_next_value(uint64_t &bitfield) {
    if( bitfield == 0 ) {
        return 0;
    }
    uint8_t retval = __builtin_ffsll(bitfield)-1;
    bitfield &= ~(uint64_t(1) << retval);
    return retval;
}

bool MoveGenerator::isCapture(const Board& board, const Move& mv) {
    return ( ((board.side==BLACK) && (mv.to_bitmove & board.color[WHITE])) ||
             ((board.side==WHITE) && (mv.to_bitmove & board.color[BLACK])) );
}
