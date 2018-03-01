#include "Board.h"
#include "Move.h"
#include "MoveGenerator.h"

void Board::makeMove(const Move& mv) {
    seen_positions[ tt.getHashValue() ] += 1;

    //Update hash values
    tt.saveHashValue();
    uint8_t from_square = __builtin_ffsll(mv.from_bitmove)-1;
    uint8_t to_square = __builtin_ffsll(mv.to_bitmove)-1;
    tt.updateHashValue(mv.color, mv.moving_piece, from_square);
    tt.updateHashValue(mv.color, mv.moving_piece, to_square);

    //Regardless of movetype, a piece will always move from one square to another
    //This thus deals with quiet moves, which is why they are not included in the switch
    pieces[ mv.moving_piece ] ^= mv.from_bitmove | mv.to_bitmove;
    color[ mv.color ] ^= mv.from_bitmove | mv.to_bitmove;

    COLOR opposite_color = (mv.color == WHITE) ? BLACK : WHITE;

    //store the current en passant square and then reset it
    uint64_t en_passant_target = EN_PASSANT_SQUARE >> 8;

    if( side == BLACK ) {
        en_passant_target = EN_PASSANT_SQUARE << 8;
    }
    if( EN_PASSANT_SQUARE != 0 ) {
        tt.updateEnPassantSquare( __builtin_ffsll( EN_PASSANT_SQUARE ) );
    }
    EN_PASSANT_SQUARE = 0;

    //change castling rights, if needed
    if( mv.moving_piece == KING && side == WHITE ) {
        if( CASTLE_RIGHTS & (1<<6) ) tt.updateCastlingRights(mv.color, 1);
        if( CASTLE_RIGHTS & (1<<2) ) tt.updateCastlingRights(mv.color, 0);
        CASTLE_RIGHTS &= ~(0xFF);
    }
    if( mv.moving_piece == KING && side == BLACK ) {
        if( CASTLE_RIGHTS & (uint64_t(1)<<58) ) tt.updateCastlingRights(mv.color, 0);
        if( CASTLE_RIGHTS & (uint64_t(1)<<62) ) tt.updateCastlingRights(mv.color, 1);
        CASTLE_RIGHTS &= ~( uint64_t(0xFF) << 56 );
    }
    if( (mv.from_bitmove|mv.to_bitmove) & (1<<7) )  {
        if( CASTLE_RIGHTS & (uint64_t(1)<<6) ) tt.updateCastlingRights(mv.color, 1);
        CASTLE_RIGHTS &= ~(1<<6);
    }
    if( (mv.from_bitmove|mv.to_bitmove) & 1 )  {
        if( CASTLE_RIGHTS & (uint64_t(1)<<2) ) tt.updateCastlingRights(mv.color, 0);
        CASTLE_RIGHTS &= ~(1<<2);
    }
    if( (mv.from_bitmove|mv.to_bitmove) & (uint64_t(1)<<56) )  {
        if( CASTLE_RIGHTS & (uint64_t(1)<<58) ) tt.updateCastlingRights(mv.color, 0);
        CASTLE_RIGHTS &= ~(uint64_t(1)<<58);
    }
    if( (mv.from_bitmove|mv.to_bitmove) & (uint64_t(1)<<63) )  {
        if( CASTLE_RIGHTS & (uint64_t(1)<<62) ) tt.updateCastlingRights(mv.color, 1);
        CASTLE_RIGHTS &= ~(uint64_t(1)<<62);
    }


    switch( mv.move_type ) {
        case CAPTURE:
            pieces[ mv.captured_piece ] ^= mv.to_bitmove;
            color[ opposite_color ] ^= mv.to_bitmove;
            tt.updateHashValue(opposite_color, mv.captured_piece, to_square);
            break;

        case EN_PASSANT_CAPTURE:
            pieces[ PAWN ] ^= en_passant_target;
            color[ opposite_color ] ^= en_passant_target;
            break;

        //set the en passant sqaure if we made a double push with pawn
        case DOUBLE_PAWN:
            if( side == WHITE ) {
                EN_PASSANT_SQUARE = mv.to_bitmove >> 8;
            }
            else {
                EN_PASSANT_SQUARE = mv.to_bitmove << 8;
            }
            break;

        //castling
        case CASTLING_KINGSIDE:
            pieces[ ROOK ] ^= (uint64_t(mv.from_bitmove) << 3) | (uint64_t(mv.from_bitmove) << 1);
            color[ mv.color ] ^= (uint64_t(mv.from_bitmove) << 3) | (uint64_t(mv.from_bitmove) << 1);
            break;

        case CASTLING_QUEENSIDE:
            pieces[ ROOK ] ^= (uint64_t(mv.from_bitmove) >> 4) | (uint64_t(mv.from_bitmove) >> 1);
            color[ mv.color ] ^= (uint64_t(mv.from_bitmove) >> 4) | (uint64_t(mv.from_bitmove) >> 1);
            break;

        //promotions
        case PROMOTION_KNIGHT_CAPTURE:
            pieces[ mv.captured_piece ] ^= mv.to_bitmove;
            color[ opposite_color ] ^= mv.to_bitmove;
        case PROMOTION_KNIGHT:
            pieces[ PAWN ] ^= mv.to_bitmove;
            pieces[ KNIGHT ] ^= mv.to_bitmove;
            break;

        case PROMOTION_QUEEN_CAPTURE:
            pieces[ mv.captured_piece ] ^= mv.to_bitmove;
            color[ opposite_color ] ^= mv.to_bitmove;
        case PROMOTION_QUEEN:
            pieces[ PAWN ] ^= mv.to_bitmove;
            pieces[ QUEEN ] ^= mv.to_bitmove;
            break;

        case PROMOTION_ROOK_CAPTURE:
            pieces[ mv.captured_piece ] ^= mv.to_bitmove;
            color[ opposite_color ] ^= mv.to_bitmove;
        case PROMOTION_ROOK:
            pieces[ PAWN ] ^= mv.to_bitmove;
            pieces[ ROOK ] ^= mv.to_bitmove;
            break;

        case PROMOTION_BISHOP_CAPTURE:
            pieces[ mv.captured_piece ] ^= mv.to_bitmove;
            color[ opposite_color ] ^= mv.to_bitmove;
        case PROMOTION_BISHOP:
            pieces[ PAWN ] ^= mv.to_bitmove;
            pieces[ BISHOP ] ^= mv.to_bitmove;
            break;

        default:
            break;
    }

    if( side == WHITE ) {
        side = BLACK;
    }
    else {
        side = WHITE;
    }
    halftimeMove += 1;
}

void Board::unmakeMove(const Move& mv) {
    tt.revertHashValue();

    EN_PASSANT_SQUARE = mv.previous_en_passant_square;
    CASTLE_RIGHTS = mv.previous_castling_rights;

    uint64_t en_passant_target = EN_PASSANT_SQUARE >> 8;
    if( mv.color == BLACK ) {
        en_passant_target = EN_PASSANT_SQUARE << 8;
    }

    //Regardless of movetype, a piece will always move from one square to another
    //This thus deals with quiet moves, which is why they are not included in the switch
    pieces[ mv.moving_piece ] ^= mv.from_bitmove | mv.to_bitmove;
    color[ mv.color ] ^= mv.from_bitmove | mv.to_bitmove;

    COLOR opposite_color = (mv.color == WHITE) ? BLACK : WHITE;

    switch( mv.move_type ) {
        case CAPTURE:
            pieces[ mv.captured_piece ] ^= mv.to_bitmove;
            color[ opposite_color ] ^= mv.to_bitmove;
            break;

        case EN_PASSANT_CAPTURE:
            pieces[ PAWN ] ^= en_passant_target;
            color[ opposite_color ] ^= en_passant_target;
            break;

        case DOUBLE_PAWN:
            break;

        //castling
        case CASTLING_KINGSIDE:
            pieces[ ROOK ] ^= (uint64_t(mv.from_bitmove) << 3) | (uint64_t(mv.from_bitmove) << 1);
            color[ mv.color ] ^= (uint64_t(mv.from_bitmove) << 3) | (uint64_t(mv.from_bitmove) << 1);
            break;

        case CASTLING_QUEENSIDE:
            pieces[ ROOK ] ^= (uint64_t(mv.from_bitmove) >> 4) | (uint64_t(mv.from_bitmove) >> 1);
            color[ mv.color ] ^= (uint64_t(mv.from_bitmove) >> 4) | (uint64_t(mv.from_bitmove) >> 1);
            break;

        //promotions
        case PROMOTION_KNIGHT_CAPTURE:
            pieces[ mv.captured_piece ] ^= mv.to_bitmove;
            color[ opposite_color ] ^= mv.to_bitmove;
        case PROMOTION_KNIGHT:
            pieces[ PAWN ] ^= mv.to_bitmove;
            pieces[ KNIGHT ] ^= mv.to_bitmove;
            break;

        case PROMOTION_QUEEN_CAPTURE:
            pieces[ mv.captured_piece ] ^= mv.to_bitmove;
            color[ opposite_color ] ^= mv.to_bitmove;
        case PROMOTION_QUEEN:
            pieces[ PAWN ] ^= mv.to_bitmove;
            pieces[ QUEEN ] ^= mv.to_bitmove;
            break;

        case PROMOTION_ROOK_CAPTURE:
            pieces[ mv.captured_piece ] ^= mv.to_bitmove;
            color[ opposite_color ] ^= mv.to_bitmove;
        case PROMOTION_ROOK:
            pieces[ PAWN ] ^= mv.to_bitmove;
            pieces[ ROOK ] ^= mv.to_bitmove;
            break;

        case PROMOTION_BISHOP_CAPTURE:
            pieces[ mv.captured_piece ] ^= mv.to_bitmove;
            color[ opposite_color ] ^= mv.to_bitmove;
        case PROMOTION_BISHOP:
            pieces[ PAWN ] ^= mv.to_bitmove;
            pieces[ BISHOP ] ^= mv.to_bitmove;
            break;

        default:
            break;
    }

    if( side == WHITE ) {
        side = BLACK;
    }
    else {
        side = WHITE;
    }
    halftimeMove -= 1;
    seen_positions[tt.getHashValue()] -= 1;
    if( seen_positions[tt.getHashValue()] <= 0 ) {
        seen_positions.erase(tt.getHashValue());
    }
}


uint64_t Board::getPiecesOfColor(const COLOR& color, const PIECE& piece) {
    return this->color[color] & this->pieces[piece];
}

void Board::reset() {
    tt.reset();
    seen_positions.clear();
}

bool Board::hasBeenSeen()  {
    uint64_t val = tt.getHashValue();
    return seen_positions[val] != 0;
}

bool Board::operator==(const Board& rhs) const {
    return (pieces[0] == rhs.pieces[0] &&
           pieces[1] == rhs.pieces[1] &&
           pieces[2] == rhs.pieces[2] &&
           pieces[3] == rhs.pieces[3] &&
           pieces[4] == rhs.pieces[4] &&
           pieces[5] == rhs.pieces[5] &&
           color[0] == rhs.color[0] &&
           color[1] == rhs.color[1] &&
           EN_PASSANT_SQUARE == rhs.EN_PASSANT_SQUARE &&
           CASTLE_RIGHTS == rhs.CASTLE_RIGHTS);
}
