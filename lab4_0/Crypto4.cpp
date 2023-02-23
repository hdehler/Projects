//Harrison Dehler
//10/11/2022
//In this lab we are encoding ascii text into a ppm file.
#include <iostream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <numeric>
#include <functional>
#include <stdio.h>
using namespace std;

#include "PPM.h"
#include "Rnumgen.h"
#include "PPM.cpp"
#include "Rnumgen.cpp"

const char EOT = 0x4;	// ASCII: end-of-transmission

void set_pixel_list(PPM &img, vector<int> &indicies, int n) {
	int size = img.get_Npixel();
	int arr[6] = {1, 1, 2, 3, 5, 8};
	int i = 0;
	while(n < size){
		indicies.push_back(n);
		n = n + arr[i%6];
		i++;
	}
}
//fibonacci sequence


void perturb_pixel_list(PPM &img, vector<int> &indicies, int seed) {
	//  write this
	vector<int> hist;
	//histogram
	//produce array of these pixels
	hist.assign(32768, 0);
	//making histogram
	for(int i = 0; i < img.get_Npixel(); i++){
		//read image
		//bits 6 through 2
		int x =
			((img[i].R & 0b01111100) >> 2) +
			((img[i].G & 0b01111100) << (5-2)) +
			((img[i].B & 0b01111100) << (10 - 2));

		hist[x]++;

	}

	//iterate through all indicies and shifting them randomly
	//handout 9
	rnumgen(seed, hist);
	//double size of int
	for (int64_t i=(int64_t)indicies.size()-1; i>0; --i) {
		int64_t r1_15 = rand();
		int64_t r2_15 = rand();
		int64_t r_30 = (r1_15 << 15) | r2_15;
		//test
		// if(r_30 % (i+1) >= indicies.size()){
		//   cout << "here" << endl;
		//   cout << r_30 % (i+1) << " " << indicies.size() << endl; 
		// }
		swap(indicies[i], indicies[r_30 % (i+1)]);
	}
}


void encode(PPM &img, vector<int> &indicies, string key) {
	string en;
	char b;
	int index = 0;
	getline(cin,en);
	int lengthmess=en.length();
	if(!key.empty()){
		string een;
		int i = 0;
		//encoding text using the key
		for(int j = 0; j < lengthmess; j++){
			een = een + char ((en[j] ^ key[i]));
			i = (i+1)%key.length();
		}
		en = een;
	}
	en.at(1);
	for(int j = 0; j < lengthmess; j++){
		b = en.at(j);
		for(int i = 0; i < 7; i++){
			// cout << int(b) << " " << int(img[index/3].R) << " " << int(img[index/3].G) << " " << int(img[index/3].B) << "=="; 
			if(index % 3 ==0){
				if(b &0x1){
					img[index].R |= 0x1;
				}
				else{
					img[index].R &= ~0x1;
				}
			}
			else if(index % 3 ==1){
				if(b &0x1){
					img[index].G |= 0x1;
				}
				else{
					img[index].G &= ~0x1;
				}
			}
			else if(index % 3 ==2){
				if(b &0x1){
					img[index].B |= 0x1;
				}
				else{
					img[index].B &= ~0x1;
				}
			}
			b = b >> 1;
			// color &= 0xFE;
			// color |= (b& 0x1);

			index++;
			//if(flag == 1) break;
		}
	}
	b = EOT;
	for(int i = 0; i < 7; i++){
		//  b= b >> i;

		if(index % 3 ==0){
			if(b &0x1){
				img[index].R |= 0x1;
			}
			else{
				img[index].R &= ~0x1;
			}
		}
		else if(index % 3 ==1){
			if(b &0x1){
				img[index].G |= 0x1;
			}
			else{
				img[index].G &= ~0x1;
			}
		}
		else if(index % 3 ==2){
			if(b &0x1){
				img[index].B |= 0x1;
			}
			else{
				img[index].B &= ~0x1;
			}
		}
		b = b >> 1;
		index++; 
	}
}




void decode(PPM &img, vector<int> &indicies, string key){
	string s;
	char dec = 0;
	char b;
	int index = 0;
	while(1){
		dec = 0;
		for(int i = 0; i < 7; i++){
			// RGB *p = &img[indicies[index]/img.numRows * indicies[index]%img.numCols];
			if(index % 3 ==0){
				b = img[index].R;
			}
			else if(index % 3 ==1){
				b = img[index].G;
			}
			else if(index % 3 ==2){
				b = img[index].B;
			}
			dec |= ((b&0x1)<<i);
			index++;
		}
		if(dec != EOT){
			// cout << dec;
			s = s + dec;
		}
		else if(dec == EOT){
			// cout << endl;
			break;
			/* code */


		}
	}
	if(!key.empty()){
		string een;
		int i = 0;
		//encoding text using the key
		for(int j = 0; j < s.length(); j++){
			een = een + char ((s[j] ^ key[i]));
			i = (i+1)%key.length();
		}
		s = een;
		cout << s;
		cout << endl;
	} 
	else{
		cout << s << endl;
	}

}

int main(int argc, char *argv[]) {
	// parse command line arguments
	string enco = "-encode";
	string deco = "-decode";

	if(argc < 3){
		cerr << "Too few commands" << endl;
		return 1;
	}

	if(argv[1] != enco && argv[1] != deco){
		cerr << "-encode|-decode" << endl;
		return 1;
	}
	string fname;
	if(argc == 3){
		fname = argv[2];
	}

	int m = 0;
	int seed = 0;
	string key;

	if(argc >= 4){
		fname = argv[argc-1];
		string s;
		s = argv[2];

		if(s.substr(0, 3) == "-m="){
			//m command line
			s = s.substr(3, s.size());
			//string to integer
			m = stoi(s);
		}
		else if(s.substr(0, 6) == "-seed="){
			//seed command line
			s = s.substr(6, s.size());
			//string to integer

			seed = stoi(s);

		}
		if(argc == 5){
			key = argv[3];
			key = key.substr(5, key.size());
			if(key[0] == '"'){
				key.erase(0, 1);
			}
			if(key[key.size()-1] == '"'){
				key.erase(key.size()-1, 1);
			}
		}

	}

	PPM img;
	img.read(fname);
	// declare pixel_list
	vector<int> indicies;

	set_pixel_list(img, indicies, m);
	// if condition met

	perturb_pixel_list(img, indicies, seed);

	if(argv[1] == enco) {
		encode(img, indicies, key);
		img.write(fname); 
	}
	else if(argv[1] == deco) {
		decode(img, indicies, key);
	}   
}
