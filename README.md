# Spectral Test of PRNGs in 2D and 3D

This project visualizes the **spectral properties** of different pseudo-random number generators:

- Linear Congruential Generator (LCG)
- RANDU (infamous bad LCG)
- Xorshift32
- std::mt19937 (Mersenne Twister)

We generate long sequences in **C++** and analyze the pairs/triples:

- 2D: \((x_n, x_{n+1})\)
- 3D: \((x_n, x_{n+1}, x_{n+2})\)

to show:

- LCGs / RANDU → points lie on **lines/planes** (bad randomness).
- Xorshift / MT → points look more like a uniform cloud.

## How to run

```bash
cd cpp
g++ -O2 -std=c++17 generate_sequence.cpp -o generate_sequence

./generate_sequence randu      1 1000000 randu.csv
./generate_sequence lcg        1 1000000 lcg.csv
./generate_sequence xorshift32 1 1000000 xorshift32.csv
./generate_sequence mt19937    1 1000000 mt19937.csv
