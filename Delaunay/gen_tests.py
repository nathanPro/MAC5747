import random

LO = -1_000_000
HI = 1_000_000

def gen_cood():
    return random.randint(LO, HI)

n = int(input())
print(n)
for i in range(n):
    print(gen_cood(), gen_cood())
