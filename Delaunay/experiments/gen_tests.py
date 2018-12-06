import numpy as np

c = int(input())
cs = [int(s) for s in input().split()]
t = int(input())

print(c)
print(t)
for n in cs:
    print(2 * n)

    for i in range(n):
        alpha = (i / n) * np.pi + .5 * np.pi
        print(np.cos(alpha), np.sin(alpha))

    for i in range(n):
        print(1 * (n - i)/n, 0)
