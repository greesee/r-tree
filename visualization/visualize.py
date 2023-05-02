import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
import pandas as pd

fig, ax = plt.subplots()

fname = str(input("Enter the name of the formatted CSV file: "))
print('\n')
df = pd.read_csv(fname)
for i in range(0, len(df)):
  minx = df["minx"][i]
  miny = df["miny"][i]
  maxx = df["maxx"][i]
  maxy = df["maxy"][i]
  width = maxx - minx
  height = maxy - miny
  if (maxx == minx) and (maxy==miny):
    ax.scatter(minx, miny, s=10)
  else:
    rect = Rectangle((minx, miny), width, height, edgecolor = 'black', fill = False)
    ax.add_patch(rect)

plt.show()
