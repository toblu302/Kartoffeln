#include <sstream>
#include <iostream>
#include "Chess.h"

using namespace std;

void Chess::setup() {
    this->setup("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void Chess::setup(string fenString) {

    //reset stuff
    board.color[WHITE] = 0;
    board.color[BLACK] = 0;
    board.pieces[PAWN] = 0;
    board.pieces[ROOK] = 0;
    board.pieces[KNIGHT] = 0;
    board.pieces[BISHOP] = 0;
    board.pieces[QUEEN] = 0;
    board.pieces[KING] = 0;

    board.EN_PASSANT_SQUARE = 0;
    board.CASTLE_RIGHTS = 0;

    winc = 0;
    wtime = -1;
    binc = 0;
    btime = -1;

    fullTimeMove = 0;


    istringstream command(fenString);
    string token;
    vector<string> tokens;
    while(command >> token) {
        tokens.push_back(token);
    }

    string boardString = tokens[0];
    int currentY = 7;
    int currentX = 0;
    for(char &character: boardString)
    {
        if( character >= '1' && character <= '8')
        {
            currentX += character-'0';
        }
        else if( character == '/' )
        {
            currentY -= 1;
            currentX = 0;
        }
        else
        {
            char piece = character;
            uint64_t bit = uint64_t(1) << ((currentY*8)+currentX);
            switch( piece ) {
                case 'P':
                    board.pieces[PAWN] |= bit;
                    board.color[WHITE] |= bit;
                    break;
                case 'p':
                    board.pieces[PAWN] |= bit;
                    board.color[BLACK] |= bit;
                    break;
                case 'R':
                    board.pieces[ROOK] |= bit;
                    board.color[WHITE] |= bit;
                    break;
                case 'r':
                    board.pieces[ROOK] |= bit;
                    board.color[BLACK] |= bit;
                    break;
                case 'N':
                    board.pieces[KNIGHT] |= bit;
                    board.color[WHITE] |= bit;
                    break;
                case 'n':
                    board.pieces[KNIGHT] |= bit;
                    board.color[BLACK] |= bit;
                    break;
                case 'B':
                    board.pieces[BISHOP] |= bit;
                    board.color[WHITE] |= bit;
                    break;
                case 'b':
                    board.pieces[BISHOP] |= bit;
                    board.color[BLACK] |= bit;
                    break;
                case 'Q':
                    board.pieces[QUEEN] |= bit;
                    board.color[WHITE] |= bit;
                    break;
                case 'q':
                    board.pieces[QUEEN] |= bit;
                    board.color[BLACK] |= bit;
                    break;
                case 'K':
                    board.pieces[KING] |= bit;
                    board.color[WHITE] |= bit;
                    break;
                case 'k':
                    board.pieces[KING] |= bit;
                    board.color[BLACK] |= bit;
                    break;
            }

            currentX += 1;
        }
    }

    turn = tokens[1][0];

    for(auto &character: tokens[2]) {
        switch(character) {
            case 'K':
                board.CASTLE_RIGHTS |= (1<<6);
                break;
            case 'Q':
                board.CASTLE_RIGHTS |= (1<<2);
                break;
            case 'k':
                board.CASTLE_RIGHTS |= (uint64_t(1)<<62);
                break;
            case 'q':
                board.CASTLE_RIGHTS |= (uint64_t(1)<<58);
                break;

            case '-':
            default:
                break;
        }
    }

    if( tokens[3][0] != '-' ) {
        uint8_t enPassantX = tokens[3][0] - 'a';
        uint8_t enPassantY = tokens[3][1] - '1';
        board.EN_PASSANT_SQUARE = uint64_t(1) << ( enPassantY*8 + enPassantX );
    }

    if( tokens.size() >= 5 ) {
        fullTimeMove = atoi( tokens[5].c_str() );
    }

}

void Chess::UCIgetReady() {
    this->depthOnly = false;
    this->depthLimit = 4;
    this->searchInterrupted = false;
}
