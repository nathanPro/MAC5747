import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns

def dist_plots(d, lo = 4, hi = 10):
    for c in range(lo, hi + 1):
        plt.figure()
        n = 2**c
        curr = d[d['size'] == n].flips
        x = np.random.normal(np.mean(curr), np.std(curr), 256)
        print(n, np.mean(curr)/n, np.std(curr)/n)
        sns.distplot(x)
        sns.distplot(curr)

def flips_linear_scale(d, cmps = None):
    if cmps is None:
        cmps = [.5, 2., 4., 9.]

    plt.figure()
    plt.title('Flips in linear scale')
    plt.ylabel('Flips')
    plt.xlabel('Input size')
    x = np.arange(0, 1030, 0.5)
    plt.xlim(0, 1030)
    plt.ylim(0, 4100)
    plt.scatter(d['size'], d['flips'])

    for c in cmps:
        plt.plot(x, c * x)


d = pd.read_csv('data.csv')

sns.catplot(x = 'size', y = 'flips', data=d, kind="box")
sns.catplot(x = 'size', y = 'time', data=d, kind="box")


dist_plots(d, 9, 10)
flips_linear_scale(d, [.5, 2, 4])
plt.show()
