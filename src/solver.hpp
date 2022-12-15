#pragma once
#include <utility>
#include <cstring>
#include <vector>
#include "utils.hpp"

/*
 * Memorize the upper bound of the score. A board `b` is stored in 
 * `b % SZ`th bucket in the format: 
 *     <board><result>
 * where the board is 49-bit and the result is 7 bit.
 *
 * The entry can be initialized to 0 because no board is labelled 0.
 */
#define DB_SIZE 1048583
long long db[DB_SIZE];
#define RESULT_LEN 7

void clean_cache() {
    memset(db, 0, DB_SIZE * sizeof(long long));
}

/* 
 * Find the best move of the current board. Return a tuple of
 * the score and the move.
 */

int SEARCH_COUNT = 0;
const int search_order[] = {3, 4, 2, 5, 1, 6, 0};

std::pair<int, int> abSearch(long long board, int alpha, int beta, int moves) {
    if (moves == BOARD_SZ) {
        return std::make_pair(0, -1);
    }

    // check table
    const long long entry = db[board % DB_SIZE];
    if ((entry >> RESULT_LEN) == board) {
        const int mx = (entry & ((1 << RESULT_LEN) - 1)) - BOARD_SZ;
        if (beta > mx) {
            beta = mx;
        }
        if (alpha >= beta) return std::make_pair(beta, -1);
    }

    SEARCH_COUNT ++;

    // check if I can win in next move
    for (int i = 0; i < BOARD_WIDTH; i++) {
        if (!isPlayable(board, i))
            continue;

        const long long nxtBoard = nxtMove(board, i);
        
        if (isWinning(nxtBoard)) {
            return std::make_pair(BOARD_SZ - (moves + 1) + 1, i);
        }
    }
    
    // if winning in next move is still not good enough, prune this branch
    const int optimalScore = BOARD_SZ - (moves + 1) + 1;
    if (optimalScore <= alpha) {
        return std::make_pair(optimalScore, -1);
    }

    {
        // check if the opponent is threatening to win
        int must_play = -1;
        long long flipped = flip(board);

        for (int i = 0; i < BOARD_WIDTH; i++) {
            if (!isPlayable(flipped, i))
                continue;

            const long long nxtBoard = nxtMove(flipped, i);
            
            if (isWinning(nxtBoard)) {
                if (must_play != -1) {
                    // we have to stop at least two things, which means we are losing in next move
                    return std::make_pair(-(BOARD_SZ - (moves + 2) + 1), must_play);
                }
                must_play = i;
            }
        }

        if (must_play != -1) {
            const long long nxtBoard = nxtMove(board, must_play);
            std::pair<int, int> result = abSearch(flip(nxtBoard), -beta, -alpha, moves + 1);
            return std::make_pair(-result.first, must_play);
        }
    }

    // decide the search order
    std::pair<int,int> order[BOARD_WIDTH];
    int cnt = 0;
    for (int i = 0; i < BOARD_WIDTH; i++) {
        const int idx = search_order[i];
        if (!isPlayable(board, idx))
            continue;

        int score = evalScore(nxtMove(board, idx));
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

        if (!isPlayable(board, idx))
            continue;

        const long long nxtBoard = nxtMove(board, idx);
        
        std::pair<int, int> result = abSearch(flip(nxtBoard), -beta, -alpha, moves + 1);
        const int score = -result.first;
        if (score >= beta) {
            return std::make_pair(score, idx);
        }
        if (score > alpha) {
            alpha = score;
            bestMove = idx;
        }
    }
    db[board % DB_SIZE] = (board << RESULT_LEN) ^ (BOARD_SZ + alpha);
    return std::make_pair(alpha, bestMove);
}

std::pair<int, int> solve(long long board) {
    const int moves = countMoves(board);

    if (isWinning(board)) {
        return std::make_pair(BOARD_SZ - moves + 1, -1);
    }

    return abSearch(board, -BOARD_SZ, BOARD_SZ, moves);
} 