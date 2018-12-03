import os
import sys

with open(sys.argv[1], "r") as f:
    lines = [s.strip() for s in f.readlines()]
funcs = ["GameAndFitness", "main", "crossover", "master", "slave", "manage_slaves"]

for fn in funcs:
    ll = list(filter(lambda s: fn in s, lines))
    if ll:
        print(fn, sum([int(l[l.index(":")+1:]) for l in ll]))


