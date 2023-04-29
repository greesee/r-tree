/*  Implemented by:
    Anish Ashish Kasegaonkar
    Abhinav Sahay
    Jay Prakash Mundhra
    Harsh Pokarna
    Srijan Khatri
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define m 2                          /* Minimum children */
#define M 4                          /* Maximum children */
#define INF 1e9

// ************************************************************************************
//                              STRUCTS AND OBJECT CREATION

/*  Node of RTree, that contains a list of children nodes and their respective MBRs.
    Contains MBR of all children nodes' MBRs for efficient sorting.
*/
typedef struct node {
    int MBR[M][4];
    struct node* children[M];
    struct node* parent;
    int cnt_keys;
    int nodeMBR[4];
} node;

/*  RTree that contains a pointer to the root node.
*/
typedef struct rtree {
    node* root;
} rtree;

/*  Construct a new node.
*/
node* new_node(){
    node* n = (node *)malloc(sizeof(node));
    for(int i = 0; i < M; i++) {
        for(int j = 0; j < 4; j++) {
            n->MBR[i][j] = 0;
        }
        n->children[i] = NULL;
    }
    n->nodeMBR[0] = INF;
    n->nodeMBR[1] = INF;
    n->nodeMBR[2] = -INF;
    n->nodeMBR[3] = -INF;
    n->parent = NULL;
    n->cnt_keys = 0;
    return n;
}

/*  Construct a new tree.
*/
rtree* new_tree(){
    rtree* t = (rtree*)malloc(sizeof(rtree));
    t->root = NULL;
    return t;
}

// ************************************************************************************
//                                  MERGE SORT

/*  Comparator for merge sort
    Compares based on sum of (minX + maxX) or (minY + maxY) based on sortOnInd, which is essentially comparing based on midpoint of MBR.
    Checks with respect to opposite coordinate in equality condition.
*/
bool comp(rtree** A, rtree** B, int i, int j, int sortOnInd) {
    if ((A[i]->root->nodeMBR[sortOnInd] + A[i]->root->nodeMBR[sortOnInd + 2]) == (B[j]->root->nodeMBR[sortOnInd] + B[j]->root->nodeMBR[sortOnInd + 2])) {
        return (A[i]->root->nodeMBR[(sortOnInd) ^ 1] + A[i]->root->nodeMBR[(sortOnInd + 2) ^ 1]) < (B[j]->root->nodeMBR[(sortOnInd) ^ 1] + B[j]->root->nodeMBR[(sortOnInd + 2) ^ 1]);
    }
    return (A[i]->root->nodeMBR[sortOnInd] + A[i]->root->nodeMBR[sortOnInd + 2]) < (B[j]->root->nodeMBR[sortOnInd] + B[j]->root->nodeMBR[sortOnInd + 2]);
}

/*  Minimum of two integers
*/
int min(int a, int b) {
    if(a < b) return a;
    return b;
}

/*  Maximum of two integers
*/
int max(int a, int b) {
    if(a < b) return b;
    return a;
}

/*  Iteratively merging two list of rtree pointers (same as standard merge sort).
    Shallow copy used since we do not alter the tree underlying the root node.
*/
void mergeAuxIter(rtree** L1, int s1, int e1, rtree** L2, int s2, int e2, rtree** L3, int s3, int e3, int sortOnInd) {
    int i, j, k;
    i = s1; j = s2; k = s3;
    while (i <= e1 && j <= e2) {
        if (comp(L1, L2, i, j, sortOnInd)) {
            L3[k++]->root = L1[i++]->root;
        }
        else {
            L3[k++]->root = L2[j++]->root;
        }
    }
    while (i <= e1) {
        L3[k++]->root = L1[i++]->root;
    }
    while (j <= e2) {
        L3[k++]->root = L2[j++]->root;
    }
}

/*  Creates a temporary list of rtree* for merging operation, frees it once utilized (same as standard merge sort).
*/
void merge(rtree** A, int s, int mid, int e, int sortOnInd) {
    rtree** C = (rtree**)malloc(sizeof(rtree*) * (e - s + 1));
    for(int i = 0; i < e - s + 1; i++) {
        C[i] = new_tree();
    }
    mergeAuxIter(A, s, mid, A, mid + 1, e, C, 0, e-s, sortOnInd);
    for(int i = 0; i < e - s + 1; i++) {
        A[s + i]->root = C[i]->root;
    }
    for(int i = 0; i < e - s + 1; i++) {
        free(C[i]);
    }
    free(C); 
}

/*  Standard merge sort function that recursively solves smaller arrays, uses it to solve a larger array of rtree*
*/
void mergeSort(rtree** A, int st, int en, int sortOnInd) {
    if(en <= st) return;
    int mid = st + (en - st)/2;
    mergeSort(A, st, mid, sortOnInd);
    mergeSort(A, mid + 1, en, sortOnInd);
    merge(A, st, mid, en, sortOnInd);
}

// ************************************************************************************
//                          PRE-ORDER TRAVERSAL

/*  Pre-order traversal (DFS) that prints current node before printing its children.
    Prints the DFS level, i.e. the depth of the recursion call.
*/
void dfs(node* curNode, int level) {

    // Prints the MBR of the entire root node (all its children).
    if(level == 0) {
        printf("\nMBR of Root Node: ");
        for(int i = 0; i < 4; i++) {
            printf("%d ", curNode->nodeMBR[i]);
        }
    }

    // For each call, prints the DFS depth as well as all children associated with the current node.
    printf("\n\nDFS Level: %d\nNode children = ", level);
    for(int i = 0; i < curNode->cnt_keys; i++) {
        printf("[");
        for(int j = 0; j < 4; j++) {
            printf("%d", curNode->MBR[i][j]);
            if(j != 3) printf(", ");
        }
        printf("]");
        if(i != curNode->cnt_keys - 1) printf(", ");
    }

    // Recursive DFS call to visit children of the current node.
    for(int i = 0; i < M; i++) {
        if(curNode->children[i] != NULL)
            dfs(curNode->children[i], level + 1);
    }
}

// ************************************************************************************
//                          SORT-TILE-RECURSIVE BUILD

/*  Implementation of Sort-Tile-Recursive algorithm that efficiently bulk-loads data into the RTree.
    Time complexity = O(N * (log(N))^2), N is the number of points/objects.
*/
rtree** buildSTR(rtree** curLevel, int numberOfMBRs, int levelNumber){

    // Calculations for number of slices required, number of nodes in each slice, and total number of nodes in current level.
    int numberOfSlices = ceil(sqrt(numberOfMBRs / (float)M));
    int nodesInSlice = numberOfSlices * M;
    int numberOfNodes = ceil(numberOfMBRs / (float)M);
    
    // Sorts entire list on basis of X-coordinate of the midpoint.
    mergeSort(curLevel, 0, numberOfMBRs - 1, 0);

    // Sorts each slice on basis of Y-coordinate of the midpoint.
    for(int i = 0; i < numberOfSlices; i++) {
        mergeSort(curLevel, i * nodesInSlice, min((i + 1) * nodesInSlice, numberOfMBRs) - 1, 1);
    }

    // Creates a list of rtree* for the upper level.
    // Note that level here is not the same as DFS level, as it is from bottom to top.
    rtree** nextLevel = (rtree**)malloc(sizeof(rtree*) * numberOfNodes);

    /*  This loop populates the nextLevel's children's MBRs, which is basically the MBR of the current node (i.e. nodeMBR for each child).
        Also populates the nodeMBRs for the nextLevel, using these children's MBRs.
        Uses modular arithmetic to simplify the code.
    */
    for(int i = 0; i < numberOfMBRs; i++) {
        if(i % M == 0) {
            nextLevel[i / M] = new_tree();
            nextLevel[i / M]->root = new_node();
        }
        for(int j = 0; j < 4; j++) {
            nextLevel[i / M]->root->MBR[i % M][j] = curLevel[i]->root->nodeMBR[j];
            if(j < 2) nextLevel[i / M]->root->nodeMBR[j] = min(nextLevel[i / M]->root->nodeMBR[j], curLevel[i]->root->nodeMBR[j]);
            else nextLevel[i / M]->root->nodeMBR[j] = max(nextLevel[i / M]->root->nodeMBR[j], curLevel[i]->root->nodeMBR[j]);
        }
        // Special case to remove the first level, because the first level has nodes with only one MBR each, and should not be included in final RTree.
        if(levelNumber != 0) {
            curLevel[i]->root->parent = nextLevel[i / M]->root;
            nextLevel[i / M]->root->children[i % M] = curLevel[i]->root;
        }
        nextLevel[i / M]->root->cnt_keys++;
    }

    // Special case to remove the first level.
    if(levelNumber == 0) {
        for(int i = 0; i < numberOfMBRs; i++) 
            free(curLevel[i]->root);
    }

    // Free up waste memory, list of rtree* as well as the rtree's allocated.
    // Note that the nodes are not freed, as they are a part of our final RTree
    for(int i = 0; i < numberOfMBRs; i++) 
        free(curLevel[i]);
    free(curLevel);

    // When the number of nodes is 1, return the final list as we have reached the root node.
    if(numberOfNodes == 1) 
        return nextLevel;

    // Recursive call for the upper level of MBRs.
    return buildSTR(nextLevel, numberOfNodes, levelNumber + 1);
}

// ************************************************************************************
//                              MAIN FUNCTION

int main() {
    // Initialises file pointer.
    FILE* f = fopen("data.txt", "r");
    int listSize = 10;

    // Initial list of rtree* that would store the input, in the root of every tree.
    // Uses rtree** instead of int** so that the mergeSort function can be reused for the lowest level as well.
    rtree** pointList = (rtree**)malloc(sizeof(rtree*) * listSize);

    // Size of input.
    int inputSize = 0;

    // Reads until end of file.
    while(!feof(f)) {
        
        // Every time the size of input reaches the list size limit, we reallocate the list with double the memory.
        // Since this reallocation would be done approximately ceil(log(N)) times, N is input size, there is no efficiency loss.
        // Similar to "vector" in vector.h library of cpp.
        if(inputSize >= listSize) {
            listSize *= 2;
            pointList = realloc(pointList, sizeof(rtree*) * listSize);
        }

        // Creates a tree, and a root for the tree in which each point is stored in the nodeMBR of a unique node.
        pointList[inputSize] = new_tree();
        pointList[inputSize]->root = new_node();

        fscanf(f, "%d %d", &pointList[inputSize]->root->nodeMBR[0], &pointList[inputSize]->root->nodeMBR[1]);
        pointList[inputSize]->root->nodeMBR[2] = pointList[inputSize]->root->nodeMBR[0];
        pointList[inputSize]->root->nodeMBR[3] = pointList[inputSize]->root->nodeMBR[1];
        ++inputSize;
    }

    // Closes file pointer.
    fclose(f);

    // Calls sort-tile-recursive build function which returns a pointer to the tree.
    rtree** pointerToTree = buildSTR(pointList, inputSize, 0);
    rtree* finalTree = pointerToTree[0];
    free(pointerToTree);

    // Runs a pre-order traversal of the tree.
    dfs(finalTree->root, 0);
    return 0;
}