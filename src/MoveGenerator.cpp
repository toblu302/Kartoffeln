#include "MoveGenerator.h"

uint64_t MoveGenerator::getRookMovesSlow(const uint64_t& occupancy, const uint8_t& p_x, const uint8_t& p_y) {
    uint64_t answer = 0;

    // Right
    for(int x=p_x+1; x<8; ++x) {
        uint64_t mask = uint64_t(1) << (p_y*8+x);
        answer |= mask;
        if(mask & occupancy) break;
    }

    // Left
    for(int x=p_x-1; x>=0; --x) {
        uint64_t mask = uint64_t(1) << (p_y*8+x);
        answer |= mask;
        if(mask & occupancy) break;
    }

    // Up
    for(int y=p_y+1; y<8; ++y) {
        uint64_t mask = uint64_t(1) << (y*8+p_x);
        answer |= mask;
        if(mask & occupancy) break;
    }

    // Down
    for(int y=p_y-1; y>=0; --y) {
        uint64_t mask = uint64_t(1) << (y*8+p_x);
        answer |= mask;
        if(mask & occupancy) break;
    }

    return answer;
}

uint64_t MoveGenerator::getBishopMovesSlow(const uint64_t& occupancy, const uint8_t& p_x, const uint8_t& p_y) {
    uint64_t answer = 0;

    // Up, Right
    for(int x=p_x+1, y=p_y+1; x<8 && y<8; ++x, ++y) {
        uint64_t mask = uint64_t(1) << (y*8+x);
        answer |= mask;
        if(mask & occupancy) break;
    }

    // Up, Left
    for(int x=p_x-1, y=p_y+1; x>=0 && y<8; --x, ++y) {
        uint64_t mask = uint64_t(1) << (y*8+x);
        answer |= mask;
        if(mask & occupancy) break;
    }

    // Down, Right
    for(int x=p_x+1, y=p_y-1; x<8 && y>=0; ++x, --y) {
        uint64_t mask = uint64_t(1) << (y*8+x);
        answer |= mask;
        if(mask & occupancy) break;
    }

    // Down, Left
    for(int x=p_x-1, y=p_y-1; x>=0 && y>=0; --x, --y) {
        uint64_t mask = uint64_t(1) << (y*8+x);
        answer |= mask;
        if(mask & occupancy) break;
    }

    return answer;
}

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

    // Calculate rook bitmasks
    for(uint8_t px=0; px<8; ++px) {
        for(uint8_t py=0; py<8; ++py) {
            uint64_t occupancy_mask = 0;
            for(int j=1; j<7; ++j) {
                uint8_t p1 = py*8+j;
                uint8_t p2 = j*8+px;
                if(j != px) occupancy_mask |= uint64_t(1) << p1;
                if(j != py) occupancy_mask |= uint64_t(1) << p2;
            }
            rook_masks[py*8+px] = occupancy_mask;
        }
    }

    // Calculate bishop bitmasks
    for(uint8_t px=0; px<8; ++px) {
        for(uint8_t py=0; py<8; ++py) {
            uint64_t occupancy_mask = 0;
            for(int x=1, y=(py-px)+1; x<7 && y<7; ++x, ++y) {
                if(y>0) occupancy_mask |= uint64_t(1) << (y*8+x);
            }
            for(int x=1, y=(px+py)-1; x<7 && y>0; ++x, --y) {
                if(y<7) occupancy_mask |= uint64_t(1) << (y*8+x);
            }
            occupancy_mask &= ~(uint64_t(1) << (py*8+px));
            bishop_masks[py*8+px] = occupancy_mask;
        }
    }

    // Calculate rook moves
    for(uint8_t square=0; square<64; ++square) {
        uint8_t p_x = square%8;
        uint8_t p_y = square/8;

        for(uint16_t i=0; i<4096; ++i) {
            uint64_t occupancy_mask = 0;

            for(int j=1; j<7; ++j) {
                uint8_t x = p_y*8+j;
                uint8_t y = j*8+p_x;

                uint64_t value1 = (i >> (j-1)) & 1;
                uint64_t value2 = (i >> (j+5)) & 1;

                occupancy_mask |= value1 << x;
                occupancy_mask |= value2 << y;
            }

            occupancy_mask &= ~(uint64_t(1) << square);


            uint16_t hash = (occupancy_mask*rook_magic[square]) >> 52;
            uint64_t answer = getRookMovesSlow(occupancy_mask, square%8, square/8);

            rook_lookup[square][hash] = answer;
        }
    }

    // Calculate bishop moves
    for(uint8_t square=0; square<64; ++square) {
        uint8_t p_x = square%8;
        uint8_t p_y = square/8;

        for(uint16_t i=0; i<4096; ++i) {
            uint64_t occupancy_mask = 0;

            int j=0;
            for(int x=1, y=(p_y-p_x)+1; x<7 && y<7; ++x, ++y) {
                if(y>0) {
                    uint8_t a = y*8+x;
                    uint64_t value1 = (i >> j) & 1;
                    occupancy_mask |= value1 << a;
                    ++j;
                }
            }
            for(int x=1, y=(p_x+p_y)-1; x<7 && y>0; ++x, --y) {
                if(y<7) {
                    uint8_t a = y*8+x;
                    uint64_t value1 = (i >> j) & 1;
                    occupancy_mask |= value1 << a;
                    ++j;
                }
            }
            
            occupancy_mask &= ~(uint64_t(1) << square);

            uint16_t hash = (occupancy_mask*bishop_magic[square]) >> 55;
            uint64_t answer = getBishopMovesSlow(occupancy_mask, square%8, square/8);

            bishop_lookup[square][hash] = answer;
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

    uint64_t occupancy = (board.color[WHITE] | board.color[BLACK]) & bishop_masks[king_position];
    uint16_t hash = (occupancy*bishop_magic[king_position]) >> 55;
    uint64_t diagonal_sliders = bishop_lookup[king_position][hash] & ~blockers;

    occupancy = (board.color[WHITE] | board.color[BLACK]) & rook_masks[king_position];
    hash = (occupancy*rook_magic[king_position]) >> 52;
    uint64_t vertical_sliders = rook_lookup[king_position][hash] & ~blockers;

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
    return getBishopMovesUnfiltered(board.color[WHITE] | board.color[BLACK], bishop_position) & ~(board.color[board.side]);
}

uint64_t MoveGenerator::getBishopMovesUnfiltered(const uint64_t &occupancy, const uint8_t &bishop_position) {
    uint64_t filtered = occupancy & bishop_masks[bishop_position];
    uint16_t hash = (filtered*bishop_magic[bishop_position]) >> 55;
    return bishop_lookup[bishop_position][hash];
}

uint64_t MoveGenerator::getRookMoves(const Board& board, const uint8_t &rook_position) {
    return getRookMovesUnfiltered(board.color[WHITE] | board.color[BLACK], rook_position) & ~(board.color[board.side]);
}

uint64_t MoveGenerator::getRookMovesUnfiltered(const uint64_t &occupancy, const uint8_t &rook_position) {
    uint64_t filteded = occupancy & rook_masks[rook_position];
    uint16_t hash = (filteded*rook_magic[rook_position]) >> 52;
    return rook_lookup[rook_position][hash];
}

uint64_t MoveGenerator::getQueenMoves(const Board& board, const uint8_t &queen_position) {
    uint64_t bitMoves = getRookMoves(board, queen_position);
    bitMoves |= getBishopMoves(board, queen_position);
    return bitMoves;
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
            bitMoves |= getRookMovesUnfiltered(board.color[WHITE] | board.color[BLACK], x) & ~blockers;
        }
        else if( board.pieces[ BISHOP ] & shifted & board.color[WHITE] ) {
            bitMoves |= getBishopMovesUnfiltered(board.color[WHITE] | board.color[BLACK], x) & ~blockers;
        }
        else if( board.pieces[ QUEEN ] & shifted & board.color[WHITE] ) {
            bitMoves |= getBishopMovesUnfiltered(board.color[WHITE] | board.color[BLACK], x) & ~blockers;
            bitMoves |= getRookMovesUnfiltered(board.color[WHITE] | board.color[BLACK], x) & ~blockers;
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
            bitMoves |= getRookMovesUnfiltered(board.color[WHITE] | board.color[BLACK], x) & ~blockers;
        }
        else if( board.pieces[ BISHOP ] & shifted & board.color[BLACK] ) {
            bitMoves |= getBishopMovesUnfiltered(board.color[WHITE] | board.color[BLACK], x) & ~blockers;
        }
        else if( board.pieces[ QUEEN ] & shifted & board.color[BLACK] ) {
            bitMoves |= getBishopMovesUnfiltered(board.color[WHITE] | board.color[BLACK], x) & ~blockers;
            bitMoves |= getRookMovesUnfiltered(board.color[WHITE] | board.color[BLACK], x) & ~blockers;
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
