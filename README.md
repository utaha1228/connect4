# Connect4 solver

The solver is inspired by the tutorial http://blog.gamesolver.org/. We evaluate the solver by the same benchmark from the website. The program is run on a Mac 2017 with Intel i5 CPU.

# Evaluation

| Test file  | # disks on board | # remaining moves |
| ---------- | ---------------- | ----------------- |
| Test_L3_R1 | 29~42            | 0~13              |
| Test_L2_R1 | 15~28            | 0~13              |
| Test_L2_R2 | 15~28            | 14~27             |
| Test_L1_R1 | 0~14             | 0~13              |
| Test_L1_R2 | 0~14             | 14~27             |
| Test_L1_R3 | 0~14             | 28~42             |

## Strong solver

| Test file  | Average time | Board explored | board/ms |
| ---------- | ------------ | -------------- | -------- |
| Test_L3_R1 | 1.18 ms      | 53.37          | 45.14    |
| Test_L2_R1 | 1.56 ms      | 2,233          | 1,428    |
| Test_L2_R2 | 8.48 ms      | 70,663         | 8,335    |
| Test_L1_R1 | 15.61 ms     | 138,417        | 8,864    |
| Test_L1_R2 | 0.37 s       | 3,425,816      | 9,271    |
| Test_L1_R3 | N/A          | N/A            | N/A      |

## Weak solver

| Test file  | Average time | Board explored | board/ms |
| ---------- | ------------ | -------------- | -------- |
| Test_L3_R1 | 1.14 ms      | 38             | 33       |
| Test_L2_R1 | 1.25 ms      | 883            | 703      |
| Test_L2_R2 | 4.67 ms      | 31,581         | 6,759    |
| Test_L1_R1 | 5.30 ms      | 41,353         | 7,798    |
| Test_L1_R2 | 0.10 s       | 960,612        | 9,435    |
| Test_L1_R3 | 6.12 s       | 58,822,066     | 9,607    |