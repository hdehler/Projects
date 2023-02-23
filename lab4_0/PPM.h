#ifndef __PPM_H__
#define __PPM_H__

#include <iostream>

struct RGB {
  //set everything to 0 
  RGB() {R = 0; G = 0; B = 0;}

  unsigned char R, G, B;
};

class PPM { 
  //all data members must be private
  //default constructor
  public:
    PPM() {pixelNum = 0; val = ""; numCols = 0; numRows = 0; img = nullptr;}
    ~PPM() {
      if(img){
        delete[] img;
      }
    }

	void read(string fname);
	void write(string fname);

	RGB &operator[](int i) {return img[i];}

	int get_Npixel() {return numRows * numCols;}

  private:
  string val; 
  int numRows;
  int numCols;
  int pixelNum;
  
  // data members

	RGB *img;
};

#endif
