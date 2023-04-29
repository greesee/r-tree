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
    int nodeMBR[4];
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
    n->nodeMBR[0] = INF;
    n->nodeMBR[1] = INF;
    n->nodeMBR[2] = -INF;
    n->nodeMBR[3] = -INF;
    n->parent = NULL;
    n->cnt_keys = 0;
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

bool comp(rtree** A, rtree** B, int i, int j, int sortOnInd) {
    if ((A[i]->root->nodeMBR[sortOnInd] + A[i]->root->nodeMBR[sortOnInd + 2]) == (B[j]->root->nodeMBR[sortOnInd] + B[j]->root->nodeMBR[sortOnInd + 2])) {
        return (A[i]->root->nodeMBR[(sortOnInd) ^ 1] + A[i]->root->nodeMBR[(sortOnInd + 2) ^ 1]) < (B[j]->root->nodeMBR[(sortOnInd) ^ 1] + B[j]->root->nodeMBR[(sortOnInd + 2) ^ 1]);
    }
    return (A[i]->root->nodeMBR[sortOnInd] + A[i]->root->nodeMBR[sortOnInd + 2]) < (B[j]->root->nodeMBR[sortOnInd] + B[j]->root->nodeMBR[sortOnInd + 2]);
}

int min(int a, int b) {
    if(a < b) return a;
    return b;
}

int max(int a, int b) {
    if(a < b) return b;
    return a;
}

void mergeAuxIter(rtree** L1, int s1, int e1, rtree** L2, int s2, int e2, rtree** L3, int s3, int e3, int sortOnInd) {
    int i, j, k;
    i = s1; j = s2; k = s3;
    while (i <= e1 && j <= e2) {
        if (comp(L1, L2, i, j, sortOnInd)) {
            L3[k]->root = L1[i]->root;
            k++;
            i++;
        }
        else {
            L3[k]->root = L2[j]->root;
            k++;
            j++;
        }
    }
    while (i <= e1) {
        L3[k]->root = L1[i]->root;
        k++;
        i++;
    }
    while (j <= e2) {
        L3[k]->root = L2[j]->root;
        k++;
        j++;
    }
}

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

void mergeSort(rtree** A, int st, int en, int sortOnInd) {
    if(en <= st) return;
    int mid = st + (en - st)/2;
    mergeSort(A, st, mid, sortOnInd);
    mergeSort(A, mid + 1, en, sortOnInd);
    merge(A, st, mid, en, sortOnInd);
}

// ************************************************************************************

void dfs(node* root, int level) {
    printf("\n\nNode MBR (of all children) = ");
    for(int i = 0; i < 4; i++) {
        printf("%d, ", root->nodeMBR[i]);
    }
    printf("\nDFS Level: %d\nNode children = ", level);
    for(int i = 0; i < M; i++) {
        printf("\n");
        for(int j = 0; j < 4; j++) {
            printf("%d, ", root->MBR[i][j]);
        }
    }
    for(int i = 0; i < M; i++) {
        if(root->children[i] != NULL)
            dfs(root->children[i], level + 1);
    }
}

rtree** buildAux(rtree** curLevel, int numberOfMBRs, int levelNumber){

    int numberOfSlices = ceil(sqrt(numberOfMBRs / (float)M));
    int nodesInSlice = numberOfSlices * M;
    int numberOfNodes = ceil(numberOfMBRs / (float)M);
    
    mergeSort(curLevel, 0, numberOfMBRs - 1, 0);
    for(int i = 0; i < numberOfSlices; i++) {
        mergeSort(curLevel, i * nodesInSlice, min((i + 1) * nodesInSlice, numberOfMBRs) - 1, 1);
    }

    rtree** nextLevel = (rtree**)malloc(sizeof(rtree*) * numberOfNodes);

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
        if(levelNumber != 0) {
            curLevel[i]->root->parent = nextLevel[i / M]->root;
            nextLevel[i / M]->root->children[i % M] = curLevel[i]->root;
        }
        nextLevel[i / M]->root->cnt_keys++;
    }

    if(levelNumber == 0) {
        for(int i = 0; i < numberOfMBRs; i++) 
            free(curLevel[i]->root);
    }
    for(int i = 0; i < numberOfMBRs; i++) 
        free(curLevel[i]);
    free(curLevel);

    if(numberOfNodes == 1) 
        return nextLevel;

    return buildAux(nextLevel, numberOfNodes, levelNumber + 1);
}

rtree* build(rtree** MBR, int numberOfMBRs) {
    // check
    int numberOfLevels = ceil(log10(numberOfMBRs)/log10(M));
    int numberOfNodes = ceil(numberOfMBRs / (float)M);
    rtree** listOfSubTrees = buildAux(MBR, numberOfMBRs, 0);

    dfs(listOfSubTrees[0]->root, 0);
    return NULL;
}

int main() {
    FILE* f = fopen("large.txt", "r");
    int listSize = 10;
    rtree** pointList = (rtree**)malloc(sizeof(rtree*) * listSize);

    int curIndex = 0;
    while(!feof(f)) {
        if(curIndex >= listSize) {
            listSize *= 2;
            pointList = realloc(pointList, sizeof(rtree*) * listSize);
        }
        pointList[curIndex] = new_tree();
        pointList[curIndex]->root = new_node();
        fscanf(f, "%d %d", &pointList[curIndex]->root->nodeMBR[0], &pointList[curIndex]->root->nodeMBR[1]);
        pointList[curIndex]->root->nodeMBR[2] = pointList[curIndex]->root->nodeMBR[0];
        pointList[curIndex]->root->nodeMBR[3] = pointList[curIndex]->root->nodeMBR[1];
        ++curIndex;
    }

    fclose(f);
    rtree* temp = build(pointList, curIndex);
}