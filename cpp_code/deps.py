import numpy as np
import networkx as nx

from network2tikz import plot
from matplotlib import pyplot as plt

import os

dependencies = nx.DiGraph()

src_files = os.listdir('src')

print(src_files)

for path in src_files:
    if not path.endswith(".h"):
        continue

    with open('src/' + path) as f:
        lines = f.readlines()

        for l in lines:
            if l.startswith('#include "'):
                dependencies.add_edge(path, l.split('"')[1])


nx.draw_spring(dependencies, with_labels=True)
plt.show()
