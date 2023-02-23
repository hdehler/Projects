//Harrison Dehler
//COSC 302
//In this lab we use the LCS function to setup our cost and link matrix which we then use to create the diff function
//In the diff funciton we use counters to get values in which we will determine what needs to be changed/what is different
//in print edit.
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

template<typename T>
class matrix {
  // class for 2D array
  public:
    
  void assign(int i, int j){
    //resize each dimension
    cell.resize(i);
    for(int k = 0; k < cell[i].size(); k++){
      cell[k].resize(j);
    }
    
  }
  int size(){
      return cell.size();
  }
  //vector op
  vector<int> &operator[](int idx) { return cell[idx];}
  private:
    //2D array
    vector<vector<int>> cell;
};

class text_align {
  public:
  text_align() : DIAG(1), VERT(2), HORZ(4){}
 
  // ex = "-" + string(x); m = strlen(x);
  // need the dash "-"
	void load_data1(string fin1);
	void load_data2(string fin2);
	void compute_lcs();

	void print_diff();
  void printTrace();
  
  private:
	void print_edit(int, int, int, int);
	void print_diff(int,int);

  const int DIAG; 
  const int VERT;
  const int HORZ;
  
  //counters
  int currLineT1 = 0;
  int currLineT2 = 0;
  int insertCount = 0;
  int delCount = 0;


  //text buffers
  vector<string> text1, text2;

  matrix<int> cost;
  matrix<int> link;
  vector<char> trace;

  //may have to delete
  int DEL(string c) { return 1; }
  int INS(string c) { return 1; }
  int SUB(string c1, string c2) { return c1==c2 ? 0 : 100; }
};
void text_align::load_data1(string tmp){
  ifstream fin1;
  fin1.open(tmp);

  string read1;
 //string dash
 string add_dash = "-";
    //text1.pushback(dash)
    text1.push_back(add_dash);
    //while
    //open it up here
    while(getline(fin1, read1)){
      text1.push_back(read1);
    }
//might need to check if eof

}
void text_align::load_data2(string tmp1){
  ifstream fin2;
  fin2.open(tmp1);

  string read2;
 //string dash
 string add_dash = "-";
    //text1.pushback(dash)
    text2.push_back(add_dash);
    //while
    //open it up here
    while(getline(fin2, read2)){
      text2.push_back(read2);
    }
    
    //getline(fname)
//might need to check if eof

}
//compute LCS - from handout
void text_align::compute_lcs(){
  cost.assign(text1.size()+1, text2.size()+1);
  link.assign(text1.size()+1, text2.size()+1);
  cost[0][0] = 0;
  link[0][0] = 0;
  for (int i=1; i<text1.size(); i++) {
    cost[i][0] = cost[i-1][0] + DEL(text1[i]);
    link[i][0] = VERT;
  }
  for (int j=1; j<text2.size(); j++) {
    cost[0][j] = cost[0][j-1] + INS(text2[j]);
    link[0][j] = HORZ;
  }
  for (int i=1; i<text1.size(); i++) {
    for (int j=1; j<text2.size(); j++) {
      cost[i][j] = cost[i-1][j-1] + SUB(text1[i],text2[j]);
      link[i][j] = DIAG;
  
      int delcost = cost[i-1][j] + DEL(text1[i]);
      if (delcost < cost[i][j]) {
        cost[i][j] = delcost;
        link[i][j] = VERT;
      }
      int inscost = cost[i][j-1] + INS(text2[j]);
      if (inscost < cost[i][j]) {
        cost[i][j] = inscost;
        link[i][j] = HORZ;
      }
    }
  }
}

//wrapper
void text_align::print_diff(){
  print_diff(text1.size()-1, text2.size()-1);
}


void text_align::print_diff(int i,int j){
//reverse/recursive iteration through
  if (i==0 && j==0) {
    for(int k = (int)trace.size(); 0<=k; k--){
      
      if(trace[k] == VERT){
       delCount++;
       currLineT1++;
      }
      else{ 
         if(trace[k] == DIAG){
            //increment the current lines
            currLineT1++;
            currLineT2++;
          
            //call print edit with the counters
            print_edit(delCount, currLineT1, currLineT2, insertCount);
            //reset the counters to zero
            delCount = 0;
            insertCount = 0;
          }
          if(trace[k] == HORZ){
          //increment the insert and text2 line
          insertCount++;
          currLineT2++;
        }
         
      } 
    }
    print_edit(delCount, ++currLineT1, ++currLineT2, insertCount);
  return;
 }
 
 if (link[i][j] & DIAG) {
 trace.push_back(DIAG);
 print_diff(i-1, j-1);
 } else
 if (link[i][j] & VERT) {
 trace.push_back(VERT);
 print_diff(i-1, j); 
 } else
 if (link[i][j] & HORZ) {
 trace.push_back(HORZ);
 print_diff(i, j-1);
  }
}

void text_align::print_edit(int, int, int, int){
  
  int text1Begin = currLineT1 - delCount;
  int text1End = currLineT1 - 1;
  int text2Begin = currLineT2 - insertCount;
  int text2End = currLineT2 - 1;

 
  //for printing d
  if(insertCount == 0 && delCount > 0){
    //when it is greater than one we print with the comma seperated so we need to check for it
    if(delCount > 1){
      cout << text1Begin << "," << text1End << "d" << text2End << endl;
      for(int i = text1Begin; i <= text1End; i++){
        cout << "< " << text1[i] << endl;
 cout << "here looking for seg fault" << endl;
      }
    }
      //when there is only one change we dont need anyhting extra because it is only one number
      //between the counter value and the letter operator --- this goes for all that are the same
      if(delCount == 1){
        cout << text1Begin << "d" << text2End << endl;
        cout << "> " << text1[text1Begin] << endl;
      }
  }
  //for printing a
  if(delCount == 0 && insertCount > 0){
    if(insertCount > 1){
      cout << text1End << "a" << text2Begin << "," << text2End << endl;
      for(int i = text2Begin; i <= text2End; i++){
        cout << "> " << text2[i] << endl;
      }
    }
    if(insertCount == 1){
      cout << text1Begin << "a" << text2Begin << endl;
      cout << "> " << text2[text2Begin] << endl;
    }

  }
  //for printing c
  //in each, when we switch from text1 to text2 we need the dashes to seperate
  //this is the same as above as we check for counter values to determine what is to be printed out
  //i.e comma seperated or not and we use the loops to iterate through from begin to end and print the line
  if(delCount > 0 && insertCount > 0){
    if(delCount > 1 && insertCount > 1){
      cout << text1Begin << "," << text1End << "c" << text2Begin << "," << text2End << endl;
       for(int i = text1Begin; i <= text1End; i++){
        cout << "< " << text1[i] << endl;
       }
       cout << "---" << endl;
        for(int i = text2Begin; i <= text2End; i++){
        cout << "> " << text2[i] << endl;
        }
    }
    if(delCount == 1 && insertCount == 1){
      cout << text1Begin << "c" << text2Begin << endl;
      cout << "< " << text1[text1Begin] << endl;
      cout << "---" << endl;
      cout << "> " << text2[text2Begin] << endl;

    }
    if(delCount > 1 && insertCount == 1){
      cout << text1Begin << "," << text1End << "c" << text2Begin << endl;
      for(int i = text1Begin; i <= text1End; i++){
        cout << "< " << text1[i] << endl;
      }
      cout << "---" << endl;
      cout << "> " << text2[text2Begin] << endl;
    }

    if(delCount == 1 && insertCount > 1){
      cout << text1Begin << "c" << text2Begin << "," << text2End << endl;
      cout << "< " << text1[text1Begin] << endl;
      cout << "---" << endl;
      for(int i = text2Begin; i <= text2End; i++){
        cout << "> " << text2[i] << endl;
      }
    }
  }
  if(insertCount == 0 && delCount == 0){
    ; //do nothing
  }
}

int main(int argc, char *argv[]) {
  // check that input files are specified on command line
  // ensure they can be opened and if not, report and exit

  text_align align;  // instantiate "LCS based diff" object
  fstream fin1;
  fstream fin2;

  fin1.open(argv[1]);
  fin2.open(argv[2]);
  if(argc != 3){
    cerr << "usage: ./Diff8 file1 file2" << endl;
    exit(1);
  }

  if(!fin1.is_open()){
    cerr << "cannot open " << argv[1] << endl;
    exit(1);
  }
  if(!fin2.is_open()){
    cerr << "cannot open " << argv[2] << endl;
    exit(1);
  }
  fin1.close();
  fin2.close();

  // read the text from file1 into text_align::text1 buffer
  
  // read the text from file2 into text_align::text2 buffer

  align.load_data1(argv[1]);
  align.load_data2(argv[2]);
  align.compute_lcs();
  align.print_diff();
}
