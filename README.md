## R-Tree using STR Bulk Loading
R-trees are tree data structures used for spatial access methods, i.e., for indexing multi-dimensional data.
This program implements the sort-tile-recursive (STR) bulk loading technique that reads all rectangles from a file in order to construct an R-tree (in memory) for them. This is an implementation of the paper [*STR: A Simple and Efficient Algorithm for R-Tree Packing, by Scott T. Leutenegger, J. M. Edgington, and Mario A. López*](https://apps.dtic.mil/sti/pdfs/ADA324493.pdf)

### Running the program
This program accepts dataset file as an argument. To pass the file, first compile the code, and pass it as an additional argument while executing the binary.

```
gcc -o rtree rtree.c #compiling the binary
./rtree data.txt #assuming that the file is named data.txt
```
### Visualization
Visualization requires formatted input in the form of a CSV file (4 columns, in the order `minx, miny, maxx, maxy`). Store the CSV file in the same folder as the python file. This can then be simply run by:

```
python visualize.py #Enter the name of the CSV file after this
```

### Results
* For data.txt (21 data points):
![](https://github.com/greesee/r-tree/blob/master/Figure_2.png?raw=true)


* For large.txt (150000 data points):
![](https://github.com/greesee/r-tree/blob/master/Figure_1.png?raw=true)
