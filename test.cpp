#include <fstream>
#include <chrono>
#include <vector>
#include "utils.hpp"
#include "solver.hpp"

#define EMPTY_BOARD 0b1000000100000010000001000000100000010000001LL

void test(std::string moves, int exp_score) {
    long long board = EMPTY_BOARD;
    {
        for (char c : moves) {
            int idx = c - '1';
            board = nxtMove(board, idx);
            board = flip(board);
        }
    }
    auto res = solve(board, -BOARD_SZ, BOARD_SZ);
    int score = res.first;
    if (score > 0) {
        score = (score + 1) / 2;
    }
    else if (score < 0) {
        score = (score - 1) / 2;
    }

    if (score != exp_score) {
        printf("Wrong score for %s\n", moves.c_str());
        printBoard(board);
        printf("Expected score %d, but got %d.", exp_score, score);
        printf("Solve claims the best move is cloumn %d\n", res.second + 1);
        exit(0);
    }
}


int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: ./test <testcase filename>");
        return 0;
    }

    std::ifstream f;
    f.open(argv[1], std::fstream::in);
    if (!f.good()) {
        printf("Failed to open file %s\n", argv[1]);
    }


    std::vector<std::string> boards;
    std::vector<int> scores;
    {
        std::string moves;
        int score;
        while (f >> moves >> score) {
            boards.push_back(moves);
            scores.push_back(score);
        }
    }

    const int testcase_cnt = boards.size();
    clock_t start = clock();

    int progress = 1;
    const int prog_bar_len = 100;

    for (int i = 0; i < testcase_cnt; i++){
        test(boards[i], scores[i]);
        clean_cache();

        while (i + 1 >= testcase_cnt * progress / prog_bar_len) {
            printf(".");
            if (progress % 10 == 0) {
                printf("\n");
            }
            progress ++;
        }
    }

    double time = double(clock() - start) / CLOCKS_PER_SEC;
    printf("Testing against file %s with %d boards\n", argv[1], testcase_cnt);
    printf("Average time spent: %f\n", time / testcase_cnt);
    printf("Average board searched: %f\n", 1. * SEARCH_COUNT / testcase_cnt);

    return 0;
}