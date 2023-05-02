## R-Tree using STR Bulk Loading
R-trees are tree data structures used for spatial access methods, i.e., for indexing multi-dimensional data.
This program implements the sort-tile-recursive (STR) bulk loading technique that reads all rectangles from a file in order to construct an R-tree (in memory) for them. This is an implementation of the paper [*STR: A Simple and Efficient Algorithm for R-Tree Packing, by Scott T. Leutenegger, J. M. Edgington, and Mario A. López*](https://apps.dtic.mil/sti/pdfs/ADA324493.pdf)

### Running the program
This program accepts a text file `(.txt)` as an argument. The file should be passed as an additional argument while executing the binary (the file should be in the same folder as the compiled binary).

```
gcc -o rtree rtree.c -lm    # compiling the binary
./rtree data.txt            # assuming that the file is named data.txt
```
### Visualization
Visualization requires formatted input in the form of a CSV file (4 columns, in the order `minx, miny, maxx, maxy`). Store the CSV file in the same folder as the python file. This can then be simply run by:

```
python visualize.py     # Enter the name of the CSV file after this
```

### Results
* For `large.txt` (105000 data points):

![](https://github.com/greesee/r-tree/blob/master/results/Figure_1.png?raw=True)

* For `data.txt` (21 data points):

![](https://github.com/greesee/r-tree/blob/master/results/Figure_2.png?raw=True)

