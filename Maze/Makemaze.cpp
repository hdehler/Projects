//Harrison Dehler
//CS 302
#include <stdio.h>
#include <ctime>
#include <stdlib.h>
#include <cstdlib>
#include <cstring>

class dset {
    struct node {
    node() { rank=0, parent=-1; }
    int rank;
    int parent;
  };
  public:
    dset(int N);
    int size() { return Nsets; }
    int merge(int, int);
    int find(int);
  private:
    int Nsets;
    node *S;
};

// add constructors as needed
struct cell { int i, j; };
struct cell_pair { cell c1, c2; };


// optional function
void swapElements(int element1, int element2, cell_pair* adjlist){
  cell_pair tmp;
  tmp = adjlist[element1];
  adjlist[element1] = adjlist[element2];
  adjlist[element2] = tmp;
}

//CHANGE TODO 
dset::dset(int N) {
  S = new node[N];
  // for(int i = 0; i < N; i++){
  //   S[i] = node();
  // }
  Nsets = N;
}

int dset::merge(int i, int j) {
  i = find(i);
  j = find(j);
  if (i != j) {
    node &Si = S[i];
    node &Sj = S[j];
    // merge (union) by rank
    if (Si.rank > Sj.rank)      Sj.parent = i;
    else if (Si.rank < Sj.rank) Si.parent = j; 
    else { Sj.parent = i; Si.rank += 1; }
    Nsets -= 1;
  }
  return find(i);
}

int dset::find(int i) {
  if (S[i].parent == -1)
    return i;
  // recursive path compression
  S[i].parent = find(S[i].parent);
  return S[i].parent;
}

int main(int argc, char *argv[]) {
  // process command line arguments
  // ./Makemaze Nrows Ncols maze.txt 
 
  if(argc != 4){
    fprintf(stderr, "Usage: %s N M maze.txt\n", argv[0]);
    exit(1);
  }

  srand(time(NULL));

  // create list of cells (i,j) and their neighbors 
  // below (i+1,j) and to the right (i,j+1) while 
  // excluding neighbors outside grid
  // int Nrows = strtol(argv[1], NULL, 10);
  // int Ncols = strtol(argv[2], NULL, 10);
  int Nrows = atoi(argv[1]);
  int Ncols = atoi(argv[2]);
  int Ncells = Nrows * Ncols;
  cell_pair adjlist[2*Ncells];
  
  char *fname = argv[3];
  FILE *fo;
  fo = fopen(fname, "w");
  
  //checking cells to see if theyre in the maze
  // populate adjlist[]: (i,j) loop adding entries
  int count = 0;
  cell cell1;
  cell cell2;
  cell cell3; 
  cell_pair pair1;
  cell_pair pair2;
  for(int i = 0; i < Nrows; i++){
    for(int j = 0; j < Ncols; j++){
        cell1.i = i;
        cell1.j = j;
        if(i + 1 < Nrows){
          cell2.i = i + 1;
          cell2.j = j;
          // cell3.i = j+1;
          pair1.c1 = cell1;
          pair1.c2 = cell2;

          adjlist[count] = pair1;

          count++;
      }
      if(j + 1 < Ncols){
        //creating a cell pair and adding pair
        cell2.i = i;
        cell2.j = j + 1;

        pair2.c1 = cell1;
        pair2.c2 = cell2;
        adjlist[count] = pair2;
        count++;
      }
    }
  }

  // randomize content (optional: write swap function)
  for(int i = 0; i < count; i++){
    int randNum = 0;
    randNum = rand() % count;
    swapElements(i, randNum, adjlist);

  }
  

  // construct maze using kruskal's mst algorithm
  //
  // place each cell in own set, then iteratively
  // merge neighboring cells if in different sets
  // otherwise, print cell pair (ie wall) to file 

  // note: when all cells belong to same set, the
  // merging stops and you merely print cell pairs
 
  // write MAZE header to file

  dset DS(count);
  //dset DS(count)

  fprintf(fo, "MAZE %d %d \n", Nrows, Ncols);
  //TO FIX
  for(int p = 0; p < count; p++){

    int i = adjlist[p].c1.i * Ncols + adjlist[p].c1.j;
    int j = adjlist[p].c2.i * Ncols + adjlist[p].c2.j;
  
    if(DS.find(i) != DS.find(j)){
      fprintf(fo, " %3d %3d %3d %3d\n", adjlist[p].c1.i, adjlist[p].c1.j, adjlist[p].c2.i, adjlist[p].c2.j);
      DS.merge(i, j);
    }
  }

  // while(1 < DS.size()) {
  //   for(int i = 0; i < count; i++){
  //   //get next cell pair
	//     cell c1 = adjlist[i].c1;
  //     cell c2 = adjlist[i].c2;
      
  //   if cells c1,c2 belong to different disjoint sets, 
	// merge sets and write indices i1,j1,i2,j2 to file
    }
  

