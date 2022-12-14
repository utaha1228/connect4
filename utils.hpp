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

#define col_mask ((1 << COL_SZ) - 1)
#define getColumn(board, idx) ((board >> (COL_SZ * idx)) & col_mask)
#define height(col) (32 - __builtin_clz(col) - 1)

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
    const int col = getColumn(board, col_idx);
    const int h = height(col);
    return h != BOARD_HEIGHT;
}

/*
 * Return the board after playing a move in column `col_idx`.
 */
inline long long nxtMove(long long board, int col_idx) {
    const int col = getColumn(board, col_idx);
    const int h = height(col);

    assert (h != BOARD_HEIGHT);
    /*
                          (len = height)
    original column: 0001 --------------
    new column:      0011 --------------
    */
    return board ^ (1LL << (COL_SZ * col_idx + h + 1));
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

inline long long removePadding(long long board) {
    for (int i = 0; i < BOARD_WIDTH; i++) {
        const int h = height(getColumn(board, i));
        board ^= (1LL << (i * COL_SZ + h)); // remove the padding bit
    }
    return board;
}

/*
 * Check if we are winning
 */

bool isWinning(long long board) {
    board = removePadding(board);

    const int nDir = 4;
    // the shifting amount of vertical, horizontal, two diagonals, respectively
    const long long shifts[] = {1, COL_SZ, COL_SZ - 1, COL_SZ + 1};
    for (int i = 0; i < nDir; i++) {
        const long long tmp = board & (board >> shifts[i]);
        if (tmp & (tmp >> (2 * shifts[i]))) return true;
    }

    return false;
}


/*
 * Count the number of disks on the board.
 */
inline int countMoves(long long board) {
    int ret = 0;
    for (int i = 0; i < BOARD_WIDTH; i++) {
        ret += height(getColumn(board, i));
    }
    return ret;
}

/*
 * Evaluate the potential of current board. Only used to decide search order.
 */

int evalScore(long long board) {
    long long vac = 0b0111111011111101111110111111011111101111110111111LL;


    for (int i = 0; i < BOARD_WIDTH; i++) {
        const int h = height(getColumn(board, i));
        vac ^= ((1LL << h) - 1) << (COL_SZ * i);
    }

    board = removePadding(board);
    long long r = (board << 1) & (board << 2) & (board << 3);

    for (int shift = COL_SZ - 1; shift <= COL_SZ + 1; shift++) {
        long long tmp = (board >> shift) & (board >> (2 * shift));
        r |= tmp & (board >> (3 * shift));
        r |= tmp & (board << shift);
        tmp = (board << shift) & (board << (2 * shift));
        r |= tmp & (board << (3 * shift));
        r |= tmp & (board >> shift);
    }
    return __builtin_popcountll(r & vac);
}