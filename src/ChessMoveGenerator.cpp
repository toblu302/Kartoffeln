#include "Chess.h"
#include <iostream>

uint64_t Chess::getWhitePawnAttackMoves(uint64_t WHITE_PAWNS) {
    uint64_t bitwiseMove = ((WHITE_PAWNS & ~FILE_A ) << 7); //attacking forward left
    bitwiseMove |= ((WHITE_PAWNS & ~FILE_H ) << 9); //attacking forward right
    return bitwiseMove;
}

uint64_t Chess::getWhitePawnMoves(uint64_t WHITE_PAWNS) {
    uint64_t bitwiseMove = (WHITE_PAWNS << 8) & ~(board.color[BLACK] | board.color[WHITE]); //moving forward 1 step

    uint64_t unmovedPawns = (uint64_t(0xFF) << 8) & WHITE_PAWNS;
    unmovedPawns = (unmovedPawns << 8) & ~(board.color[BLACK] | board.color[WHITE]); //try to move forward 1 step
    unmovedPawns = (unmovedPawns << 8) & ~(board.color[BLACK] | board.color[WHITE]); //try to move forward another step
    bitwiseMove |= unmovedPawns;


    bitwiseMove |= getWhitePawnAttackMoves( WHITE_PAWNS ) & (board.color[BLACK] | board.EN_PASSANT_SQUARE);

    return bitwiseMove;
}

uint64_t Chess::getBlackPawnAttackMoves(uint64_t BLACK_PAWNS) {

    uint64_t bitwiseMove = ((BLACK_PAWNS & ~FILE_H ) >> 7); //attacking forward left
    bitwiseMove |= ((BLACK_PAWNS & ~FILE_A ) >> 9); //attacking forward right

    return bitwiseMove;
}

uint64_t Chess::getBlackPawnMoves(uint64_t BLACK_PAWNS) {

    uint64_t bitwiseMove = (BLACK_PAWNS >> 8) & ~(board.color[BLACK] | board.color[WHITE]); //moving forward 1 step

    uint64_t unmovedPawns = (uint64_t(0xFF) << 48 ) & BLACK_PAWNS;
    unmovedPawns = (unmovedPawns >> 8) & ~(board.color[BLACK] | board.color[WHITE]); //try to move forward 1 step
    unmovedPawns = (unmovedPawns >> 8) & ~(board.color[BLACK] | board.color[WHITE]); //try to move forward another step
    bitwiseMove |= unmovedPawns;

    bitwiseMove |= getBlackPawnAttackMoves( BLACK_PAWNS ) & (board.color[WHITE] | board.EN_PASSANT_SQUARE);
    return bitwiseMove;
}

uint64_t Chess::getKnightMoves(uint8_t knight_position, uint64_t blockers) {
    return KNIGHT_MOVES[ knight_position ] & ~blockers;
}

uint64_t Chess::getKingMoves(uint8_t king_position, uint64_t blockers) {
    return KING_MOVES[ king_position ] & ~blockers;
}

uint64_t Chess::getKingCastles(uint8_t king_position) {
    uint64_t attackingMoves = 0;
    uint64_t king = uint64_t(1) << king_position;
    uint64_t blockers = board.color[WHITE] | board.color[BLACK];
    uint64_t castle_rights = 0;

    if(board.side == WHITE) {
        attackingMoves = getBlackAttacking();
        castle_rights = board.CASTLE_RIGHTS;
    }
    else {
        attackingMoves = getWhiteAttacking();
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

uint64_t Chess::getSlidingMovesFromOccupancy(uint8_t column, uint8_t occupancy) {
    uint64_t retValue = SLIDING_MOVES[column][occupancy];
    return retValue;
}

uint64_t Chess::getSlidingAlongRank(uint8_t piece_position, uint64_t blockers) {
    //convert a rank to a 8 bit number representing where the pieces on the rank are at
    uint8_t column = piece_position%8;
    uint8_t row = (piece_position-column)/8;

    uint8_t occupancy = ( (board.color[WHITE] | board.color[BLACK])  >> (row*8)) & 255;

    return (getSlidingMovesFromOccupancy(column, occupancy) << (row*8)) & ~blockers;
}

uint64_t Chess::getSlidingAlongFile(uint8_t piece_position, uint64_t blockers) {

    uint8_t column = piece_position%8;
    uint8_t row = (piece_position-column)/8;

    //convert a rank to a 8 bit number representing where the pieces on the column are at
    uint64_t ALL_PIECES = (board.color[WHITE] | board.color[BLACK]) >> column;
    uint8_t occupancy = 0;
    for(int i=0; i<8; ++i) {
        occupancy |= ( (ALL_PIECES >> i*8) << i) & (1<<i);
    }

    //convert back into a column
    uint64_t sliding_moves = getSlidingMovesFromOccupancy(row, occupancy);
    uint64_t bitMoves = 0;
    for(int i=0; i<8; ++i) {
        bitMoves |= ((sliding_moves & (1<<i)) >> i) << (i*8);
    }

    //return to be about the column and return
    return (bitMoves << column) & ~blockers;
}

uint64_t Chess::getSlidingAlongDiagonalA1H8(uint8_t piece_position, uint64_t blockers) {

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
    uint64_t sliding_moves = getSlidingMovesFromOccupancy(column, occupancy);
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

uint64_t Chess::getSlidingAlongDiagonalA8H1(uint8_t piece_position, uint64_t blockers) {

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
    uint64_t sliding_moves = getSlidingMovesFromOccupancy(column, occupancy);
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


uint64_t Chess::getWhiteAttacking()
{
    uint64_t blockers = board.color[WHITE];
    uint64_t bitMoves = 0;
    for(int x=0; x<64; ++x) {
        uint64_t shifted = uint64_t(1) << x;
        if( board.pieces[PAWN] & shifted & board.color[WHITE] ) {
            bitMoves |= getWhitePawnAttackMoves( shifted );
        }
        else if( board.pieces[KING] & shifted & board.color[WHITE] ) {
            bitMoves |= getKingMoves( x, blockers );
        }
        else if( board.pieces[ KNIGHT ] & shifted & board.color[WHITE] ) {
            bitMoves |= getKnightMoves( x, blockers );
        }
        else if( board.pieces[ ROOK ] & shifted & board.color[WHITE] ) {
            bitMoves |= getSlidingAlongRank( x, blockers );
            bitMoves |= getSlidingAlongFile( x, blockers );
        }
        else if( board.pieces[ BISHOP ] & shifted & board.color[WHITE] ) {
            bitMoves |= getSlidingAlongDiagonalA1H8( x, blockers );
            bitMoves |= getSlidingAlongDiagonalA8H1( x, blockers );
        }
        else if( board.pieces[ QUEEN ] & shifted & board.color[WHITE] ) {
            bitMoves |= getSlidingAlongDiagonalA1H8( x, blockers );
            bitMoves |= getSlidingAlongDiagonalA8H1( x, blockers );
            bitMoves |= getSlidingAlongRank( x, blockers );
            bitMoves |= getSlidingAlongFile( x, blockers );
        }
    }
    return bitMoves;
}

uint64_t Chess::getBlackAttacking()
{
    uint64_t blockers = board.color[BLACK];
    uint64_t bitMoves = 0;
    for(int x=0; x<64; ++x) {
        uint64_t shifted = uint64_t(1) << x;

        if( board.pieces[PAWN] & shifted & board.color[BLACK] ) {
            bitMoves |= getBlackPawnAttackMoves( shifted );
        }
        else if( board.pieces[KING] & shifted & board.color[BLACK] ) {
            bitMoves |= getKingMoves( x, blockers );
        }
        else if( board.pieces[ KNIGHT ] & shifted & board.color[BLACK] ) {
            bitMoves |= getKnightMoves( x, blockers );
        }
        else if( board.pieces[ ROOK ] & shifted & board.color[BLACK] ) {
            bitMoves |= getSlidingAlongRank( x, blockers );
            bitMoves |= getSlidingAlongFile( x, blockers );
        }
        else if( board.pieces[ BISHOP ] & shifted & board.color[BLACK] ) {
            bitMoves |= getSlidingAlongDiagonalA1H8( x, blockers );
            bitMoves |= getSlidingAlongDiagonalA8H1( x, blockers );
        }
        else if( board.pieces[ QUEEN ] & shifted & board.color[BLACK] ) {
            bitMoves |= getSlidingAlongDiagonalA1H8( x, blockers );
            bitMoves |= getSlidingAlongDiagonalA8H1( x, blockers );
            bitMoves |= getSlidingAlongRank( x, blockers );
            bitMoves |= getSlidingAlongFile( x, blockers );
        }
    }
    return bitMoves;
}
