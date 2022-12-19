#pragma once
#include <utility>
#include <cstring>
#include <vector>
#include "utils.hpp"

/*
 * Memorize the upper bound of the score. A board `b` is stored in 
 * `b % SZ`th bucket in the format: 
 *     <board><result>
 * where the board is 49-bit (but only store 26 low-bit) and the result is 6 bit.
 * By CRT, there's no collition if DB_SIZE is odd and DB_SIZE * (2^26) > 2^49
 * The entry can be initialized to 0 because no board is labelled 0.
 */

// Note: big DB_SIZE slows down the speed for easy-to-solve board
#define DB_SIZE 8388617
uint32_t db[DB_SIZE];
#define RESULT_LEN 6
#define LOW_MASK ((1LL << 26) - 1)

void clean_cache() {
    memset(db, 0, DB_SIZE * sizeof(uint32_t));
}

/* 
 * Find the best move of the current board. Return a tuple of
 * the score and the move.
 */

long long SEARCH_COUNT = 0;
const int search_order[] = {3, 4, 2, 5, 1, 6, 0};

std::pair<int, int> abSearch(long long board, long long flp, int alpha, int beta, int moves) {
    if (moves == BOARD_SZ) {
        return std::make_pair(0, -1);
    }

    // check table
    const uint32_t entry = db[board % DB_SIZE];
    if ((entry >> RESULT_LEN) == (board & LOW_MASK)) {
        const int mx = (entry & ((1 << RESULT_LEN) - 1)) - BOARD_SZ / 2;
        if (beta > mx) {
            beta = mx;
        }
        if (alpha >= beta) return std::make_pair(beta, -1);
    }

    SEARCH_COUNT ++;

    long long nxtBoard[BOARD_WIDTH];
    long long nxtBoardFlip[BOARD_WIDTH];
    long long canPlay[BOARD_WIDTH];

    // Initialize the future boards and check if we can win in one move.
    for (int i = 0; i < BOARD_WIDTH; i++) {
        if (canPlay[i] = isPlayable(board, i)) {
            nxtBoard[i] = board;
            nxtBoardFlip[i] = flp;
            nxtMove(nxtBoard[i], nxtBoardFlip[i], i);

            // check if we can win in one move
            if (isWinning(nxtBoard[i], nxtBoardFlip[i])) {
                const int score = (BOARD_SZ - (moves + 1)) / 2 + 1;
                return std::make_pair(score, i);
            }
        }
    }
    
    // if winning in the move after next move is still not good enough, prune this branch
    const int optimalScore = (BOARD_SZ - (moves + 3)) / 2 + 1;
    if (optimalScore <= alpha) {
        return std::make_pair(optimalScore, -1);
    }

    {
        // check if the opponent is threatening to win
        int must_play = -1;

        for (int i = 0; i < BOARD_WIDTH; i++) {
            if (!canPlay[i])
                continue;

            long long nxtBoard = flp;
            long long nxtBoardFlp = board;
            nxtMove(nxtBoard, nxtBoardFlp, i);
            
            if (isWinning(nxtBoard, nxtBoardFlp)) {
                if (must_play != -1) {
                    // we have to stop at least two things, which means we are losing in next move
                    return std::make_pair(-((BOARD_SZ - (moves + 2)) / 2 + 1), must_play);
                }
                must_play = i;
            }
        }

        if (must_play != -1) {
            auto result = abSearch(nxtBoardFlip[must_play], nxtBoard[must_play], -beta, -alpha, moves + 1);
            return std::make_pair(-result.first, must_play);
        }
    }

    // decide the search order
    std::pair<int,int> order[BOARD_WIDTH];
    int cnt = 0;
    for (int i = 0; i < BOARD_WIDTH; i++) {
        const int idx = search_order[i];
        if (!canPlay[idx])
            continue;

        int score = evalScore(nxtBoard[idx], nxtBoardFlip[idx]);
        int position;
        for (position = 0; position < cnt; position++) {
            if (score > order[position].second) break;
        }
        for (int j = cnt; j > position; j--) {
            order[j] = order[j - 1];
        }
        order[position] = std::make_pair(idx, score);
        cnt ++;
    }

    // actual searching
    int bestMove = -1;
    for (int i = 0; i < cnt; i++) {
        const int idx = order[i].first;

        if (!canPlay[idx])
            continue;
        
        std::pair<int, int> result = abSearch(nxtBoardFlip[idx], nxtBoard[idx], -beta, -alpha, moves + 1);
        const int score = -result.first;
        if (score >= beta) {
            return std::make_pair(score, idx);
        }
        if (score > alpha) {
            alpha = score;
            bestMove = idx;
        }
    }
    db[board % DB_SIZE] = ((board & LOW_MASK) << RESULT_LEN) ^ (BOARD_SZ / 2 + alpha);
    return std::make_pair(alpha, bestMove);
}

std::pair<int, int> solve(long long board) {
    const long long flp = flip(board);
    const int moves = countMoves(board, flp);

    if (isWinning(board, flp)) {
        return std::make_pair(BOARD_SZ - moves + 1, -1);
    }

    return abSearch(board, flp, -BOARD_SZ / 2, BOARD_SZ / 2, moves);
} 

std::pair<int, int> weakSolve(long long board) {
    const long long flp = flip(board);
    const int moves = countMoves(board, flp);

    if (isWinning(board, flp)) {
        return std::make_pair(1, -1);
    }
    if (isWinning(flp, board)) {
        return std::make_pair(-1, -1);
    }

    return abSearch(board, flp, -1, 1, moves);
} 