import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns

d = pd.read_csv('data.csv')
sns.catplot(x = 'size', y = 'flips', data=d)
plt.show()
