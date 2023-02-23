#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

#include "PPM.h"

void PPM::read(string fname) { 
  //write this
  //double check for EOF
  //use istream::gcount()
  ifstream fin;
  fin.open(fname);
  string s;
  
  if(fin.is_open()){
        fin >> val >> numRows >> numCols >> pixelNum;
        img  = new RGB[numRows * numCols];
        if(val != "P6"){
          cerr << "file is not P6 PMM format" << endl;
          exit(1);
        }
        if(pixelNum != 255){
          cerr << "Wrong number of pixels" << endl;
          exit(1);
        }
        fin.ignore();
        fin.read((char*) img, numRows * numCols * 3);
        if(fin.gcount() != numRows * numCols * 3){
          cerr << "Too few bytes" << endl;
          // exit(1);
        }
        fin.read((char*) img, 1);
        if(fin.gcount() != 0){
          cerr << "Too many bytes" << endl;
          exit(1);
        }
       
      } 
      else{
        //actual error statement
          cerr << "File not open read" << endl;
          exit(1);
        }
       
      fin.close();
    }


void PPM::write(string fname) { 
  //write this
  ofstream fout;
    string tmp = "_wmsg.ppm";
    
    fname.erase(fname.end()-4, fname.end()-0);
    fname = fname + tmp;
    // exit(1);
    fout.open(fname);
    if(fout.is_open()){
      fout << val << "\n" << numRows << " " << numCols << "\n" << pixelNum << "\n";
    fout.write((char*) img, numRows * numCols * 3);  
    }
      else{
        //error statement
          cerr << "File not open write" << endl;
          exit(1);
        }
        fout.close();
}
