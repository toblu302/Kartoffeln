#include "Chess.h"

Chess::Chess()
{
    //Knight moves
    KNIGHT_MOVES[0] = uint64_t(132096);
    KNIGHT_MOVES[1] = uint64_t(329728);
    KNIGHT_MOVES[2] = uint64_t(659712);
    KNIGHT_MOVES[3] = uint64_t(1319424);
    KNIGHT_MOVES[4] = uint64_t(2638848);
    KNIGHT_MOVES[5] = uint64_t(5277696);
    KNIGHT_MOVES[6] = uint64_t(10489856);
    KNIGHT_MOVES[7] = uint64_t(4202496);
    KNIGHT_MOVES[8] = uint64_t(33816580);
    KNIGHT_MOVES[9] = uint64_t(84410376);
    KNIGHT_MOVES[10] = uint64_t(168886289);
    KNIGHT_MOVES[11] = uint64_t(337772578);
    KNIGHT_MOVES[12] = uint64_t(675545156);
    KNIGHT_MOVES[13] = uint64_t(1351090312);
    KNIGHT_MOVES[14] = uint64_t(2685403152);
    KNIGHT_MOVES[15] = uint64_t(1075839008);
    KNIGHT_MOVES[16] = uint64_t(8657044482);
    KNIGHT_MOVES[17] = uint64_t(21609056261);
    KNIGHT_MOVES[18] = uint64_t(43234889994);
    KNIGHT_MOVES[19] = uint64_t(86469779988);
    KNIGHT_MOVES[20] = uint64_t(172939559976);
    KNIGHT_MOVES[21] = uint64_t(345879119952);
    KNIGHT_MOVES[22] = uint64_t(687463207072);
    KNIGHT_MOVES[23] = uint64_t(275414786112);
    KNIGHT_MOVES[24] = uint64_t(2216203387392);
    KNIGHT_MOVES[25] = uint64_t(5531918402816);
    KNIGHT_MOVES[26] = uint64_t(11068131838464);
    KNIGHT_MOVES[27] = uint64_t(22136263676928);
    KNIGHT_MOVES[28] = uint64_t(44272527353856);
    KNIGHT_MOVES[29] = uint64_t(88545054707712);
    KNIGHT_MOVES[30] = uint64_t(175990581010432);
    KNIGHT_MOVES[31] = uint64_t(70506185244672);
    KNIGHT_MOVES[32] = uint64_t(567348067172352);
    KNIGHT_MOVES[33] = uint64_t(1416171111120896);
    KNIGHT_MOVES[34] = uint64_t(2833441750646784);
    KNIGHT_MOVES[35] = uint64_t(5666883501293568);
    KNIGHT_MOVES[36] = uint64_t(11333767002587136);
    KNIGHT_MOVES[37] = uint64_t(22667534005174272);
    KNIGHT_MOVES[38] = uint64_t(45053588738670592);
    KNIGHT_MOVES[39] = uint64_t(18049583422636032);
    KNIGHT_MOVES[40] = uint64_t(145241105196122112);
    KNIGHT_MOVES[41] = uint64_t(362539804446949376);
    KNIGHT_MOVES[42] = uint64_t(725361088165576704);
    KNIGHT_MOVES[43] = uint64_t(1450722176331153408);
    KNIGHT_MOVES[44] = uint64_t(2901444352662306816);
    KNIGHT_MOVES[45] = uint64_t(5802888705324613632);
    KNIGHT_MOVES[46] = uint64_t(11533718717099671552);
    KNIGHT_MOVES[47] = uint64_t(4620693356194824192);
    KNIGHT_MOVES[48] = uint64_t(288234782788157440);
    KNIGHT_MOVES[49] = uint64_t(576469569871282176);
    KNIGHT_MOVES[50] = uint64_t(1224997833292120064);
    KNIGHT_MOVES[51] = uint64_t(2449995666584240128);
    KNIGHT_MOVES[52] = uint64_t(4899991333168480256);
    KNIGHT_MOVES[53] = uint64_t(9799982666336960512);
    KNIGHT_MOVES[54] = uint64_t(1152939783987658752);
    KNIGHT_MOVES[55] = uint64_t(2305878468463689728);
    KNIGHT_MOVES[56] = uint64_t(1128098930098176);
    KNIGHT_MOVES[57] = uint64_t(2257297371824128);
    KNIGHT_MOVES[58] = uint64_t(4796069720358912);
    KNIGHT_MOVES[59] = uint64_t(9592139440717824);
    KNIGHT_MOVES[60] = uint64_t(19184278881435648);
    KNIGHT_MOVES[61] = uint64_t(38368557762871296);
    KNIGHT_MOVES[62] = uint64_t(4679521487814656);
    KNIGHT_MOVES[63] = uint64_t(9077567998918656);

    //King moves
    KING_MOVES[0] = uint64_t(770);
    KING_MOVES[1] = uint64_t(1797);
    KING_MOVES[2] = uint64_t(3594);
    KING_MOVES[3] = uint64_t(7188);
    KING_MOVES[4] = uint64_t(14376);
    KING_MOVES[5] = uint64_t(28752);
    KING_MOVES[6] = uint64_t(57504);
    KING_MOVES[7] = uint64_t(49216);
    KING_MOVES[8] = uint64_t(197123);
    KING_MOVES[9] = uint64_t(460039);
    KING_MOVES[10] = uint64_t(920078);
    KING_MOVES[11] = uint64_t(1840156);
    KING_MOVES[12] = uint64_t(3680312);
    KING_MOVES[13] = uint64_t(7360624);
    KING_MOVES[14] = uint64_t(14721248);
    KING_MOVES[15] = uint64_t(12599488);
    KING_MOVES[16] = uint64_t(50463488);
    KING_MOVES[17] = uint64_t(117769984);
    KING_MOVES[18] = uint64_t(235539968);
    KING_MOVES[19] = uint64_t(471079936);
    KING_MOVES[20] = uint64_t(942159872);
    KING_MOVES[21] = uint64_t(1884319744);
    KING_MOVES[22] = uint64_t(3768639488);
    KING_MOVES[23] = uint64_t(3225468928);
    KING_MOVES[24] = uint64_t(12918652928);
    KING_MOVES[25] = uint64_t(30149115904);
    KING_MOVES[26] = uint64_t(60298231808);
    KING_MOVES[27] = uint64_t(120596463616);
    KING_MOVES[28] = uint64_t(241192927232);
    KING_MOVES[29] = uint64_t(482385854464);
    KING_MOVES[30] = uint64_t(964771708928);
    KING_MOVES[31] = uint64_t(825720045568);
    KING_MOVES[32] = uint64_t(3307175149568);
    KING_MOVES[33] = uint64_t(7718173671424);
    KING_MOVES[34] = uint64_t(15436347342848);
    KING_MOVES[35] = uint64_t(30872694685696);
    KING_MOVES[36] = uint64_t(61745389371392);
    KING_MOVES[37] = uint64_t(123490778742784);
    KING_MOVES[38] = uint64_t(246981557485568);
    KING_MOVES[39] = uint64_t(211384331665408);
    KING_MOVES[40] = uint64_t(846636838289408);
    KING_MOVES[41] = uint64_t(1975852459884544);
    KING_MOVES[42] = uint64_t(3951704919769088);
    KING_MOVES[43] = uint64_t(7903409839538176);
    KING_MOVES[44] = uint64_t(15806819679076352);
    KING_MOVES[45] = uint64_t(31613639358152704);
    KING_MOVES[46] = uint64_t(63227278716305408);
    KING_MOVES[47] = uint64_t(54114388906344448);
    KING_MOVES[48] = uint64_t(216739030602088448);
    KING_MOVES[49] = uint64_t(505818229730443264);
    KING_MOVES[50] = uint64_t(1011636459460886528);
    KING_MOVES[51] = uint64_t(2023272918921773056);
    KING_MOVES[52] = uint64_t(4046545837843546112);
    KING_MOVES[53] = uint64_t(8093091675687092224);
    KING_MOVES[54] = uint64_t(16186183351374184448);
    KING_MOVES[55] = uint64_t(13853283560024178688);
    KING_MOVES[56] = uint64_t(144959613005987840);
    KING_MOVES[57] = uint64_t(362258295026614272);
    KING_MOVES[58] = uint64_t(724516590053228544);
    KING_MOVES[59] = uint64_t(1449033180106457088);
    KING_MOVES[60] = uint64_t(2898066360212914176);
    KING_MOVES[61] = uint64_t(5796132720425828352);
    KING_MOVES[62] = uint64_t(11592265440851656704);
    KING_MOVES[63] = uint64_t(4665729213955833856);


    //Calculate the possible sliding moves
    // The "column" is the column of the moving piece for which you want to calculate moves
    // The "occ" (occupancy) is a bitboard with the pieces which will block the moving piece
    // SLIDING_MOVES[column][occ] is a bitboard with the positions to which the piece can move
    for(int column=0; column<8; ++column) {
        for(int occ=0; occ < 256; ++occ) {

            uint8_t position = uint8_t(1) << column;
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
