#include <chrono>
#include <vector>
#include "utils.hpp"
#include "solver.hpp"

#define EMPTY_BOARD 0b1000000100000010000001000000100000010000001LL

void test(std::string moves, int exp_score, bool isWeak) {
    long long board = EMPTY_BOARD;
    long long flp = flip(board);
    {
        for (char c : moves) {
            int idx = c - '1';
            nxtMove(board, flp, idx);
            long long tmp = board;
            board = flp;
            flp = tmp;
        }
    }
    auto res = isWeak ? weakSolve(board) : solve(board);
    int score = res.first;

    if (isWeak) {
        if (exp_score < 0) exp_score = -1;
        if (exp_score > 0) exp_score = 1;
        if (score < 0) score = -1;
        if (score > 0) score = 1;
    }

    if (score != exp_score) {
        printf("Wrong score for %s\n", moves.c_str());
        printBoard(board);
        printf("Expected score %d, but got %d.\n", exp_score, score);
        printf("Solver claims the best move is cloumn %d\n", res.second + 1);
        exit(0);
    }
}


int main(int argc, char *argv[]) {
    bool isWeak = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--weak") == 0 || strcmp(argv[i], "-w") == 0) {
            isWeak = true;
        }
    }

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
        test(boards[i], scores[i], isWeak);
        clean_cache();
    }

    double time = double(clock() - start) / CLOCKS_PER_SEC;
    printf("# boards: %d\n", testcase_cnt);
    printf("Average time spent: %lf\n", time / testcase_cnt);
    printf("Average # positions searched per board: %lf\n", ((double) SEARCH_COUNT) / testcase_cnt);
    printf("# boards per ms: %lf\n", ((double) SEARCH_COUNT) / time / 1000.);

    return 0;
}
