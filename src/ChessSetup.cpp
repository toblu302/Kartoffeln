#include <sstream>
#include <iostream>
#include "Chess.h"

using namespace std;

void Chess::setup() {
    this->setup("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void Chess::setup(string fenString) {

    //reset stuff
    BOARD[ALL_WHITE] = 0;
    BOARD[WHITE_PAWN] = 0;
    BOARD[WHITE_ROOK] = 0;
    BOARD[WHITE_KNIGHT] = 0;
    BOARD[WHITE_BISHOP] = 0;
    BOARD[WHITE_QUEEN] = 0;
    BOARD[WHITE_KING] = 0;
    BOARD[ALL_BLACK] = 0;
    BOARD[BLACK_PAWN] = 0;
    BOARD[BLACK_ROOK] = 0;
    BOARD[BLACK_KNIGHT] = 0;
    BOARD[BLACK_BISHOP] = 0;
    BOARD[BLACK_QUEEN] = 0;
    BOARD[BLACK_KING] = 0;

    BOARD[EN_PASSANT] = 0;
    BOARD[WHITE_CASTLE_RIGHTS] = 0;
    BOARD[BLACK_CASTLE_RIGHTS] = 0;

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
                    BOARD[WHITE_PAWN] |= bit;
                    BOARD[ALL_WHITE] |= bit;
                    break;
                case 'p':
                    BOARD[BLACK_PAWN] |= bit;
                    BOARD[ALL_BLACK] |= bit;
                    break;
                case 'R':
                    BOARD[WHITE_ROOK] |= bit;
                    BOARD[ALL_WHITE] |= bit;
                    break;
                case 'r':
                    BOARD[BLACK_ROOK] |= bit;
                    BOARD[ALL_BLACK] |= bit;
                    break;
                case 'N':
                    BOARD[WHITE_KNIGHT] |= bit;
                    BOARD[ALL_WHITE] |= bit;
                    break;
                case 'n':
                    BOARD[BLACK_KNIGHT] |= bit;
                    BOARD[ALL_BLACK] |= bit;
                    break;
                case 'B':
                    BOARD[WHITE_BISHOP] |= bit;
                    BOARD[ALL_WHITE] |= bit;
                    break;
                case 'b':
                    BOARD[BLACK_BISHOP] |= bit;
                    BOARD[ALL_BLACK] |= bit;
                    break;
                case 'Q':
                    BOARD[WHITE_QUEEN] |= bit;
                    BOARD[ALL_WHITE] |= bit;
                    break;
                case 'q':
                    BOARD[BLACK_QUEEN] |= bit;
                    BOARD[ALL_BLACK] |= bit;
                    break;
                case 'K':
                    BOARD[WHITE_KING] |= bit;
                    BOARD[ALL_WHITE] |= bit;
                    break;
                case 'k':
                    BOARD[BLACK_KING] |= bit;
                    BOARD[ALL_BLACK] |= bit;
                    break;
            }

            currentX += 1;
        }
    }

    this->turn = tokens[1][0];

    for(auto character: tokens[2]) {
        switch(character) {
            case 'K':
                BOARD[WHITE_CASTLE_RIGHTS] |= (1<<6);
                break;
            case 'Q':
                BOARD[WHITE_CASTLE_RIGHTS] |= (1<<2);
                break;
            case 'k':
                BOARD[BLACK_CASTLE_RIGHTS] |= (uint64_t(1)<<62);
                break;
            case 'q':
                BOARD[BLACK_CASTLE_RIGHTS] |= (uint64_t(1)<<58);
                break;

            case '-':
            default:
                break;
        }
    }

    if( tokens[3][0] != '-' ) {
        uint8_t enPassantX = tokens[3][0] - 'a';
        uint8_t enPassantY = tokens[3][1] - '1';
        BOARD[EN_PASSANT] = uint64_t(1) << ( enPassantY*8 + enPassantX );
    }

    if( tokens.size() >= 5 ) {
        fullTimeMove = atoi( tokens[5].c_str() );
    }

}

void Chess::UCIgetReady() {
    this->depthOnly = false;
    this->bestMove = "0000";
    this->depthLimit = 4;
    this->searchInterrupted = false;
}
