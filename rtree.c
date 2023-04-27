#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define m 2                          /*minimum children; sizeof child index*/
#define M 4                          /*maximum children; sizeof child index*/
#define INF 1e9

typedef struct node {
    int MBR[M][4];
    struct node* children[M];
    struct node* parent;
    int cnt_keys;
    bool isLeaf;
} node;

typedef struct rtree {
    node *root;
} rtree;

node* new_node(){
    node* n = (node *)malloc(sizeof(node));
    for(int i = 0; i < M; i++) {
        for(int j = 0; j < 4; j++) {
            n->MBR[i][j] = 0;
        }
        n->children[i] = NULL;
    }
    n->parent = NULL;
    n->cnt_keys = 0;
    n->isLeaf = true;
    return n;
}

rtree* new_tree(){
    rtree* t = (rtree*)malloc(sizeof(rtree));
    t->root = NULL;
    return t;
}

int* new_MBR(){
    int* MBR = (int*)malloc(sizeof(int) * 4);
    MBR[0] = INF;
    MBR[1] = INF;
    MBR[2] = -INF;
    MBR[3] = -INF;
    return MBR;
}

// ************************************************************************************

/* sortOnInd = 0 -> X
   sortOnInd = 1 -> Y
*/

bool comp(int** A, int** B, int i, int j, int sortOnInd) {
    return (A[i][sortOnInd] + A[i][sortOnInd + 2]) < (B[j][sortOnInd] + B[j][sortOnInd + 2]);
}

int min(int a, int b) {
    if(a < b) return a;
    return b;
}

int max(int a, int b) {
    if(a < b) return b;
    return a;
}

void mergeAuxIter(int** L1, int s1, int e1, int** L2, int s2, int e2, int** L3, int s3, int e3, int sortOnInd) {
    int i, j, k;
    i = s1; j = s2; k = s3;
    while (i <= e1 && j <= e2) {
        if (comp(L1, L2, i, j, sortOnInd)) {
            for(int it = 0; it < 4; it++)
                L3[k][it] = L1[i][it];
            k++;
            i++;
        }
        else {
            for(int it = 0; it < 4; it++)
                L3[k][it] = L2[j][it];
            k++;
            j++;
        }
    }
    while (i <= e1) {
        for(int it = 0; it < 4; it++)
            L3[k][it] = L1[i][it];
        k++;
        i++;
    }
    while (j <= e2) {
        for(int it = 0; it < 4; it++)
            L3[k][it] = L2[j][it];
        k++;
        j++;
    }
}

void merge(int** A, int s, int mid, int e, int sortOnInd) {
    int** C = (int**)malloc(sizeof(int*) * (e - s + 1));
    for(int i = 0; i < e - s + 1; i++) {
        C[i] = (int*)malloc(sizeof(int) * 4);
    }
    mergeAuxIter(A, s, mid, A, mid + 1, e, C, 0, e-s, sortOnInd);
    for(int i = 0; i < e - s + 1; i++) {
        for(int j = 0; j < 4; j++) {
            A[s + i][j] = C[i][j];
        }
        free(C[i]);
    }
    free(C); 
}

void mergeSort(int** A, int st, int en, int sortOnInd) {
    if(en <= st) return;
    int mid = st + (en - st)/2;
    mergeSort(A, st, mid, sortOnInd);
    mergeSort(A, mid + 1, en, sortOnInd);
    merge(A, st, mid, en, sortOnInd);
}

// ************************************************************************************


// rtree* build(int ** pointList,int size){
//     sort_x();
//     int num_nodes = size/m;
//     int nodes_in_slice = ceil(sqrt(num_nodes));
//     int slices = ceil(sqrt(num_nodes));

//     for(int i=0;i<slices;i++){
//         sort_y();//nodes_in_slice
//     }
//     int ** new_list; //malloc 
//     int k = 0;
//     for(int i=0;i<num_nodes;i++){
//         for(int j=k;j<k+m,k<size;j++){
//             //allocate each to a separate node "n" and set mbr and also set the parent of the children of the nodes in j=k to k+m as n;
//         }
//         k+=m;
//     }
//     build(new_list,num_nodes);
// }

node*** buildAux(int** MBR, node*** nodeList, int numberOfMBRs, int curLevel){
    mergeSort(MBR, 0, numberOfMBRs - 1, 0);
    int numberOfNodes = ceil(numberOfMBRs / (float)m);
    int nodesInSlice = ceil(sqrt(numberOfMBRs * m));
    int numberOfSlices = ceil(sqrt(numberOfMBRs / (float)m));
    for(int i = 0; i < numberOfSlices; i++) {
        mergeSort(MBR, i * nodesInSlice, min((i + 1) * nodesInSlice, numberOfMBRs) - 1, 1);
    }

    nodeList[curLevel] = (node**)malloc(sizeof(node*) * numberOfNodes);
    int** newMBR = (int**)malloc(sizeof(int*) * numberOfNodes);
    for(int i = 0; i < numberOfMBRs; i++) {
        if(i % m == 0) {
            nodeList[curLevel][i / m] = new_node();
            newMBR[i / m] = new_MBR();
        }
        for(int j = 0; j < 4; j++) {
            nodeList[curLevel][i / m]->MBR[i % m][j] = MBR[i][j];
            if(j < 2) newMBR[i / m][j] = min(newMBR[i / m][j], MBR[i][j]);
            else newMBR[i / m][j] = max(newMBR[i / m][j], MBR[i][j]);
        }
        nodeList[curLevel][i / m]->cnt_keys++;
    }

    for(int i = 0; i < numberOfMBRs; i++) 
        free(MBR[i]);
    free(MBR);

    if(numberOfNodes == 1) return nodeList;
    return buildAux(newMBR, nodeList, numberOfNodes, curLevel + 1);
}

rtree* build(int** MBR, int numberOfMBRs) {
    rtree* t = new_tree();
    int numberOfLevels = ceil(log10(numberOfMBRs)/log10(m));
    node*** nodeList = (node***)malloc(sizeof(node**) * numberOfLevels);
    nodeList = buildAux(MBR, nodeList, numberOfMBRs, 0);

    for(int i = 0; i < numberOfLevels; i++) {
        printf("Level #%d:\n", i);
        int comp = ceil(numberOfMBRs/(float)m);
        for(int j = 0; j < i; j++) comp = ceil(comp/(float)m);
        for(int j = 0; j < comp; j++) {
            printf("Node #%d:\n", j);
            for(int l = 0; l < nodeList[i][j]->cnt_keys; l++) {
                printf("MBR #%d:", l);
                for(int k = 0; k < 4; k++) {
                    printf(" %d", nodeList[i][j]->MBR[l][k]);
                }
                printf(", ");
            }
            printf("\n");
        }
        printf("\n");
    }

    return NULL;
}

int main() {
    FILE* f = fopen("selfgen.txt", "r");
    int listSize = 100;
    int** pointList = (int**)malloc(sizeof(int*) * listSize);
    int curIndex = 0;
    while(!feof(f)) {
        if(curIndex >= listSize) {
            listSize *= 2;
            pointList = realloc(pointList, sizeof(int*) * listSize);
        }
        pointList[curIndex] = new_MBR();
        fscanf(f, "%d %d", &pointList[curIndex][0], &pointList[curIndex][1]);
        pointList[curIndex][2] = pointList[curIndex][0];
        pointList[curIndex][3] = pointList[curIndex][1];
        ++curIndex;
    }
    fclose(f);
    rtree* temp = build(pointList, curIndex);
    printf("%d\n", curIndex);
    // for(int i = 0; i < curIndex; i++) {
    //     printf("%d %d\n", pointList[i][0], pointList[i][1]);
    // }
}