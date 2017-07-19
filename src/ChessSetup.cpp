#include <sstream>
#include <iostream>
#include "Chess.h"

using namespace std;

void Chess::setup() {
    this->setup("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void Chess::setup(string fenString) {

    //reset stuff
    BOARD[WHITE] = 0;
    BOARD[BLACK] = 0;
    BOARD[PAWN] = 0;
    BOARD[ROOK] = 0;
    BOARD[KNIGHT] = 0;
    BOARD[BISHOP] = 0;
    BOARD[QUEEN] = 0;
    BOARD[KING] = 0;

    BOARD[EN_PASSANT_SQUARE] = 0;
    BOARD[CASTLE_RIGHTS] = 0;

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
    for(int i=0; i<boardString.size(); ++i)
    {
        if( boardString[i] >= '1' && boardString[i] <= '8')
        {
            currentX += boardString[i]-'0';
        }
        else if( boardString[i] == '/' )
        {
            currentY -= 1;
            currentX = 0;
        }
        else
        {
            char piece = boardString[i];
            uint64_t bit = uint64_t(1) << ((currentY*8)+currentX);
            switch( piece ) {
                case 'P':
                    BOARD[PAWN] |= bit;
                    BOARD[WHITE] |= bit;
                    break;
                case 'p':
                    BOARD[PAWN] |= bit;
                    BOARD[BLACK] |= bit;
                    break;
                case 'R':
                    BOARD[ROOK] |= bit;
                    BOARD[WHITE] |= bit;
                    break;
                case 'r':
                    BOARD[ROOK] |= bit;
                    BOARD[BLACK] |= bit;
                    break;
                case 'N':
                    BOARD[KNIGHT] |= bit;
                    BOARD[WHITE] |= bit;
                    break;
                case 'n':
                    BOARD[KNIGHT] |= bit;
                    BOARD[BLACK] |= bit;
                    break;
                case 'B':
                    BOARD[BISHOP] |= bit;
                    BOARD[WHITE] |= bit;
                    break;
                case 'b':
                    BOARD[BISHOP] |= bit;
                    BOARD[BLACK] |= bit;
                    break;
                case 'Q':
                    BOARD[QUEEN] |= bit;
                    BOARD[WHITE] |= bit;
                    break;
                case 'q':
                    BOARD[QUEEN] |= bit;
                    BOARD[BLACK] |= bit;
                    break;
                case 'K':
                    BOARD[KING] |= bit;
                    BOARD[WHITE] |= bit;
                    break;
                case 'k':
                    BOARD[KING] |= bit;
                    BOARD[BLACK] |= bit;
                    break;
            }

            currentX += 1;
        }
    }

    this->turn = tokens[1][0];

    for(auto character: tokens[2]) {
        switch(character) {
            case 'K':
                BOARD[CASTLE_RIGHTS] |= (1<<6);
                break;
            case 'Q':
                BOARD[CASTLE_RIGHTS] |= (1<<2);
                break;
            case 'k':
                BOARD[CASTLE_RIGHTS] |= (uint64_t(1)<<62);
                break;
            case 'q':
                BOARD[CASTLE_RIGHTS] |= (uint64_t(1)<<58);
                break;

            case '-':
            default:
                break;
        }
    }

    if( tokens[3][0] != '-' ) {
        uint8_t enPassantX = tokens[3][0] - 'a';
        uint8_t enPassantY = tokens[3][1] - '1';
        BOARD[EN_PASSANT_SQUARE] = uint64_t(1) << ( enPassantY*8 + enPassantX );
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
