#pragma once
#include <stdio.h>
#include <cassert>
#include <string>
#include <vector>

#define BOARD_HEIGHT 6
#define BOARD_WIDTH 7
#define COL_SZ (BOARD_HEIGHT + 1)
#define BOARD_SZ (BOARD_HEIGHT * BOARD_WIDTH)
#define N 4

/*
 * Using alpha-beta search to find the optimal move of a Connect-4 game.
 *
 * The board is compressed to a 49-bit integer of form:
 *        7 bit           7 bit      ...
 *    <first column> <second column> ...
 *
 * Each column will have 0 ~ 6 disks, we map current player's disks to 
 * bit 1, and other player's disks to bit 0. In order to distinguish 
 * no disks and a single other player's disk, we pad some zeros 
 * and a single one in front until it's a 7-bit integer. For example,
 * 
 * (O is out disk, X is their disk, . means empty)
 * ..OOXX => 0011100
 * OXXOOX => 1100110
 * ...... => 0000001
 *
 * The score is 0 if it's a draw, postive if we are winning, and negative 
 * if we are losing. The absolute value is the number of empty space left 
 * on the board plus one when someone win.
 */

#define col_mask ((1LL << COL_SZ) - 1)
#define getColumn(board, idx) ((board >> (COL_SZ * idx)) & col_mask)
#define height(col) (32 - __builtin_clz(col) - 1)

const long long topBit[] = {
    1LL << (COL_SZ - 1),
    1LL << (COL_SZ * 2 - 1),
    1LL << (COL_SZ * 3 - 1),
    1LL << (COL_SZ * 4 - 1),
    1LL << (COL_SZ * 5 - 1),
    1LL << (COL_SZ * 6 - 1),
    1LL << (COL_SZ * 7 - 1),
};

const long long colPos[] = {
    col_mask << (COL_SZ * 0),
    col_mask << (COL_SZ * 1),
    col_mask << (COL_SZ * 2),
    col_mask << (COL_SZ * 3),
    col_mask << (COL_SZ * 4),
    col_mask << (COL_SZ * 5),
    col_mask << (COL_SZ * 6),
};

long long encodeBoard(std::vector<std::string> board) {
    long long ret = 0;
    for (int i = 0; i < BOARD_WIDTH; i++) {
        int j;
        long long col = 0;
        for (j = 0; j < BOARD_HEIGHT; j++) {
            const char c = board[BOARD_HEIGHT - 1 - j][i];
            if (c == '.') break;

            if (c == 'O') col ^= (1 << j);
        }
        col ^= (1 << j);

        ret ^= (col << (COL_SZ * i));
    }
    return ret;
}

std::vector<std::string> decodeBoard(long long board) {
    std::vector<std::string> ret;
    ret.resize(BOARD_HEIGHT);
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        ret[i] = std::string(BOARD_WIDTH, '.');
    }

    for (int i = 0; i < BOARD_WIDTH; i++) {
        const int col = getColumn(board, i);
        const int h = height(col);
        for (int j = 0; j < h; j++) {
            if (col & (1 << j)) {
                ret[BOARD_HEIGHT - 1 - j][i] = 'O';
            }
            else {
                ret[BOARD_HEIGHT - 1 - j][i] = 'X';
            }
        }
    }
    return ret;
}

void printBoard(long long board) {
    std::vector<std::string> v = decodeBoard(board);
    for (std::string s : v) {
        printf("%s\n", s.c_str());
    }
}

/*
 * Checking if a column is full.
 */
inline bool isPlayable(long long board, int col_idx) {
    return !(board & topBit[col_idx]);
}

/*
 * Return the board after playing a move in column `col_idx`.
 */
inline void nxtMove(long long &board, long long &flp, int col_idx) {
    const long long top = board & flp & colPos[col_idx];
    /*
                          (len = height)
    original column: 0001 --------------
    new column:      0011 --------------
    */

    board ^= (top << 1);
    flp ^= (top << 1) ^ top;
}

/*
 * Flip the perspective – "we" become the opposing team. Return the resulting board.
 */

inline long long flip(long long board) {
    long long ret = 0;
    for (int i = 0; i < BOARD_WIDTH; i++) {
        const int col = getColumn(board, i);
        const int h = height(col);
        const long long new_col = ((~col) & ((1 << h) - 1)) ^ (1 << h);
        ret |= (new_col << (COL_SZ * i));
    }
    return ret;
}

/*
 * Remove padding bits of a board.
 */

inline long long removePadding(long long board, long long flp) {
    return board ^ (board & flp);
}

/*
 * Check if we are winning
 */

bool isWinning(long long board, long long flp) {
    board = removePadding(board, flp);

    {
        const long long tmp = board & (board >> 1);
        if (tmp & (tmp >> (2 * 1))) return true;
    }
    {
        const long long tmp = board & (board >> COL_SZ);
        if (tmp & (tmp >> (2 * COL_SZ))) return true;
    }
    {
        const long long tmp = board & (board >> (COL_SZ - 1));
        if (tmp & (tmp >> (2 * (COL_SZ - 1)))) return true;
    }
    {
        const long long tmp = board & (board >> (COL_SZ + 1));
        if (tmp & (tmp >> (2 * (COL_SZ + 1)))) return true;
    }
    return false;
}


/*
 * Count the number of disks on the board.
 */
inline int countMoves(long long board, long long flp) {
    return __builtin_popcountll(board ^ flp);
}

inline long long winningSpot(long long board, long long flp) {
    const long long playable = 0b0111111011111101111110111111011111101111110111111LL;
    long long empty = ~(board ^ flp);
    board = removePadding(board, flp);

    long long r = (board << 1) & (board << 2) & (board << 3);

    {
        long long tmp = (board >> COL_SZ) & (board >> (2 * COL_SZ));
        r |= tmp & (board >> (3 * COL_SZ));
        r |= tmp & (board << COL_SZ);
        tmp = (board << COL_SZ) & (board << (2 * COL_SZ));
        r |= tmp & (board << (3 * COL_SZ));
        r |= tmp & (board >> COL_SZ);
    }
    {
        long long tmp = (board >> (COL_SZ + 1)) & (board >> (2 * (COL_SZ + 1)));
        r |= tmp & (board >> (3 * (COL_SZ + 1)));
        r |= tmp & (board << (COL_SZ + 1));
        tmp = (board << (COL_SZ + 1)) & (board << (2 * (COL_SZ + 1)));
        r |= tmp & (board << (3 * (COL_SZ + 1)));
        r |= tmp & (board >> (COL_SZ + 1));
    }
    {
        long long tmp = (board >> (COL_SZ - 1)) & (board >> (2 * (COL_SZ - 1)));
        r |= tmp & (board >> (3 * (COL_SZ - 1)));
        r |= tmp & (board << (COL_SZ - 1));
        tmp = (board << (COL_SZ - 1)) & (board << (2 * (COL_SZ - 1)));
        r |= tmp & (board << (3 * (COL_SZ - 1)));
        r |= tmp & (board >> (COL_SZ - 1));
    }
    return r & playable & empty;
}

/*
 * Evaluate the potential of current board. Only used to decide search order.
 */

int evalScore(long long board, long long flp) {
    const long long ws = winningSpot(board, flp);
    /*
     * Besides having many winning spots, having two consecutive winning spots in 
     * the same column is also strong, and imply that the game won't last too long.
     * So it's worth to search it first.
     */
    return __builtin_popcountll(ws) + 4 * __builtin_popcountll(ws & (ws >> 1));
}