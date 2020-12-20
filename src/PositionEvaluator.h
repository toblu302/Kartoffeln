#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "Board.h"
#include "MoveGenerator.h"
#include <array>

using std::array;

class PositionEvaluator {
    public:
        int64_t Evaluate(Board& board);

    private:
        int64_t PSQTevaluation(const Board& board, const array< array<int64_t, 64>, 6> &PSQT);
        uint64_t handleBlackPSQT(const Board& board, const PIECE& piece, const array<int64_t, 64>& table);
        uint64_t handleWhitePSQT(const Board& board, const PIECE& piece, const array<int64_t, 64>& table);

        int64_t materialEvaluation(const Board& board);
        MoveGenerator movegen;
};

#endif
