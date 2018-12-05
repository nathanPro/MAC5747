import numpy as np

m = int(input())
n = int(input())
for i in range(m):
    alpha = (i / m) * np.pi + .5 * np.pi
    print(np.cos(alpha), np.sin(alpha))

for i in range(n):
    print(1 * (n - i)/n, 0)
