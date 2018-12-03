import os
import sys

with open(sys.argv[1], "r") as f:
    lines = [s.strip() for s in f.readlines()]
funcs = ["GameAndFitness", "main", "crossover", "master", "slave", "manage_slaves"]

with open(sys.argv[2], "r") as fsec:
    lines2= [s.strip() for s in fsec.readlines()]

ll = list(filter(lambda s: "main" in s, lines))
ls = list(filter(lambda s: "main" in s, lines2))
if not ls:
    ls = list(filter(lambda s: "master" in s, lines2))
tseq = int( ll[0].split(": ")[1]) 
tpar = int(ls[0].split(": ")[1])
print(tpar)
print(tseq) 
print(tseq/tpar)
   # if ll:
   #     pr:int(fn, sum([int(l[l.index(":")+1:]) for l in ll]))
