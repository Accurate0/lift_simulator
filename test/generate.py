#!/usr/bin/env python3

# JUST A SMALL SCRIPT GENERATE SOME TEST FILES OF VARYING SIZE

# n = 50-100
# m = 20
import random

floor_combos = [ (x, y) for x in range(1, 21) for y in range(1, 21) if x != y ]

n = 1
for r in range(50, 101):
    with open(f'inputs/test{n}.txt', 'w') as f:
        for t in random.sample(floor_combos, r):
            f.write(f'{t[0]} {t[1]}\n')
    n += 1
