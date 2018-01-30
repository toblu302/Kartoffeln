#include "Chess.h"

Chess::Chess()
{
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
