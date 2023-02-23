
#include <cstdio>
#include <ctime>
#include <cstdlib>



// add constructor, operator overloads as needed
struct cell { 
  int i,j;

  cell(){
    i = 0;
    j = 0;
  }

  cell(int intOne, int intTwo){
    i = intOne;
    j = intTwo;
  }
  cell(const cell& oneC){
    i = oneC.i;
    j = oneC.j;
  }

  bool operator!=(cell one){
    if(i == one.i && j == one.j){
      return false;
    }
    else{
      return true;
    }
  }
};

enum color_t { WHITE, BLACK };
  

bool empty(int &NS){
  return NS == 0;
}
void resize(int stackResize, int &NSmax, cell *&S){
  cell *temp = NULL;
  temp = new cell [stackResize];
  for(int i = 0; i < NSmax; i++){//define up top
    temp[i] = S[i]; //stack above
  }
  NSmax = stackResize;
  delete[] S;
  S = temp;
}
void push(cell c, int &NS, int &NSmax, cell *&S){
  //recieve cell, add to the top of the stack

  if(NS == NSmax){
    resize(2*NSmax, NSmax, S);
  }
  S[NS] = c;
  NS = NS+1;
}

void pop(int &NS){
  //if stack is 0/empty
  //if stack isnt 0/empty
  if(NS == 0){
    return;
  }
  else{
    NS--;
  }
}
cell top(cell *&S, int &NS){
  return S[NS-1];
}

int main(int argc, char *argv[]) {
  // process command line arguments
  // ./Findpath maze.txt path.txt
  
  if(argc != 3){
    printf("usage: ./Findpath [i1 j1 i2 j2] maze1.txt path1.txt\n", argv[0]);
    exit(1);
  }
  
  char *Mazefname = argv[1];
  
  FILE *fo;
  fo = fopen(Mazefname, "r");

  char *Pathfname = argv[2];
  FILE *Pfo;
  Pfo = fopen(Pathfname, "w");

  if(fo == NULL){
    printf("open error: %s\n", argv[1]);
    exit(1);
  }
  if(Pfo == NULL){
    printf("File failure\n");
    exit(1);
  }



  // obtain maze size from maze file header

  int Nrows;
  int Ncols;
    
  char *mTmp = new char[4];
  fscanf(fo, "%s %d %d", mTmp, &Nrows, &Ncols);
  delete[] mTmp;

  bool ***adjtable = new bool**[Nrows];
    // create 3D adjacency table: allocate memory to access
    for (int i = 0; i < Nrows; i++) {
        adjtable[i] = new bool*[Ncols];

        for (int j = 0; j < Ncols; j++) {
            adjtable[i][j] = new bool[4];
        }
    }

    //assign false to every value
    for(int i = 0; i < Nrows; i++){
      for(int j = 0; j < Ncols; j++){
        for(int k = 0; k < 4; k++){
          adjtable[i][j][k] = false;
        }
      }
    }
    //reads in each cell pair, adds true to relevant index's
    //for loop - iterate through maze1.txt
      //retreive next cell pair from file
      //change value of first cell in pair from false to true
      //change value in second cell in pair from false to true
    cell cell1;
    cell cell2;
    //populate adj table
    //if feof, break
    while(1){
      fscanf(fo, "%d %d %d %d", &cell1.i, &cell1.j, &cell2.i, &cell2.j);
      // adjtable[i][j][0]: cell(i,j) adjacent to cell(i-1,j)
      if(feof(fo)){
        break;
      }
      else if(cell2.i == cell1.i){
        adjtable[cell1.i][cell1.j][3] = true;
        adjtable[cell2.i][cell2.j][2] = true;
      }
  // adjtable[i][j][3]: cell(i,j) adjacent to cell(i,j+1)
      else if(cell2.j == cell1.j){
        adjtable[cell1.i][cell1.j][0] = true;
        adjtable[cell2.i][cell2.j][1] = true;
      }
    }
  fclose(fo);

  // find path using DFS source-to-sink traversal of graph
  //allocate memory
  cell source(0,0);
  cell sink(Nrows-1, Ncols-1);

  color_t **vcolor = new color_t* [Nrows];

  // mark all cells WHITE, set all links to cells themselves
  for(int i = 0; i < Nrows; i++){
    vcolor[i] = new color_t[Ncols];
  }

  for(int i = 0; i < Nrows; i++){
    for(int j = 0; j < Ncols; j++){
      vcolor[i][j] = WHITE;
    }
  }

  int NS = 0;       // stack size: initially 0
  int NSmax = 100;    // stack capacity: initially 100
  cell *S = new cell [NSmax];
  cell **link;
  //link cells to themselves
  link = new cell* [Nrows];
  for(int i = 0; i < Nrows; i++){
    link[i] = new cell[Ncols];
  }
  for(int i = 0; i < Nrows; i++){
    for(int j = 0; j < Ncols; j++){
      cell cell1;
      cell1.i = i;
      cell1.j = j;
      link[i][j] = cell1;
    }
  }
  //iterate through the four adjacent cells
  //selects the first connected cell in the maze, which is not black, and not on the stack
  //color in cell
  //dfs
  push(source, NS, NSmax, S);
  while(!empty(NS)){
  //find cell2
  cell c2 = top(S, NS);
  cell pushCell;
  pushCell.i = 0;
  pushCell.j = 0;
  pop(NS);

  
  if(vcolor[c2.i][c2.j] == BLACK){
    continue;
  }
    vcolor[c2.i][c2.j] = BLACK;
  
  for(int k = 0; k < 4; k++){
    if(adjtable[c2.i][c2.j][k] == true){
      switch(k){
        case 0: {
            pushCell.i = c2.i + 1;
            pushCell.j = c2.j;
            link[pushCell.i][pushCell.j] = c2;
            push(pushCell, NS, NSmax, S);
        break;
        }
        case 1: {
            pushCell.i = c2.i - 1;
            pushCell.j = c2.j;
            link[pushCell.i][pushCell.j] = c2;
            push(pushCell, NS, NSmax, S);          
        break;
        }
        case 2: {
            pushCell.i = c2.i;
            pushCell.j = c2.j - 1;
            link[pushCell.i][pushCell.j] = c2;
            push(pushCell, NS, NSmax, S);
        break;
        }
        case 3: {
            pushCell.i = c2.i;
            pushCell.j = c2.j + 1;
            link[pushCell.i][pushCell.j] = c2;
            push(pushCell, NS, NSmax, S);
        break;
        }
      }
      if(vcolor[pushCell.i][pushCell.j] != BLACK){
      }
    }
  }
  
  if(c2.i == sink.i && c2.j == sink.j){
    break;
    }
  }

  // write header to file
  cell tmp;
  fprintf(Pfo, "PATH %d %d\n", Nrows, Ncols);
  fprintf(Pfo, "SOURCE %d %d\n", source.i, source.j);
  fprintf(Pfo, "SINK %d %d\n", sink.i, sink.j);
  //print the paths
  tmp = link[source.i][source.j];
    fprintf(Pfo, "%3d %3d\n", source.i, source.j);
    fprintf(Pfo, "%3d %3d\n", tmp.i, tmp.j);
  while(tmp.i != sink.i || tmp.j != sink.j){
    tmp = link[tmp.i][tmp.j];
    fprintf(Pfo, "%3d %3d\n", tmp.i, tmp.j);
  }
  fclose(Pfo);
  
  // close file
  
  // deallocate memory for stack and 3D adjtable
  
  for(int i = 0; i < Nrows; i++){
    delete[] vcolor[i];
  }
  delete[] vcolor;
  for(int i = 0; i < Nrows; i++){
    delete[] link[i];
  }
  delete[] link;

  for (int i = 0; i < Nrows; i++) {
        for (int j = 0; j < Ncols; j++) {
            delete[] adjtable[i][j];
        }
    delete[] adjtable[i];
    }
  delete[] adjtable;

  delete[] S;
  

}
