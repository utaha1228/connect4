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
    auto res = solve(board);
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
        printf("Expected score %d, but got %d.\n", exp_score, score);
        printf("Solver claims the best move is cloumn %d\n", res.second + 1);
        exit(0);
    }
}


int main() {
    std::vector<std::string> boards;
    std::vector<int> scores;
    {
        char moves[BOARD_SZ + 1];
        int score;

        while (scanf("%s %d\n", moves, &score) == 2) {
            boards.push_back(std::string(moves));
            scores.push_back(score);
        }
    }

    const int testcase_cnt = boards.size();
    clock_t start = clock();

    for (int i = 0; i < testcase_cnt; i++){
        test(boards[i], scores[i]);
        clean_cache();
    }

    double time = double(clock() - start) / CLOCKS_PER_SEC;
    printf("# boards: %d\n", testcase_cnt);
    printf("Average time spent: %f\n", time / testcase_cnt);
    printf("Average # positions searched per board: %f\n", 1. * SEARCH_COUNT / testcase_cnt);
    printf("# boards per ms: %f\n", SEARCH_COUNT / time / 1000.);

    return 0;
}
