//Harrison Dehler
//COSC302
//In this lab sort through cities from a text file and calculate the distance through the haversince function
//and calculation the travel time based on the city type. We then use dijkstras to find the shortest and fastest route
//between cities
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <string>
#include <ostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <math.h>
#include <limits>

using namespace std;

class city{
public:
//contructor
city(){cityTown = " "; cityType; longitude = 0; latitude = 0; population = 0;}

//overloaded input and output operators to facilitate the fin and fout city name
friend istream& operator>>(istream& fin, city &myCity);
friend ostream& operator<<(ostream& fout, city &myCity);
friend bool operator<(const city&, const city&);
//longitude set
void setName(string cname){
  city1 = cname;
}
string getName(){
  return city1;
}
void setLongitude(int lon){
  longitude = lon;
}
//longitude get
float getLongitude(){
  return longitude * (3.1415926535/180);
}

//latitude set
void setLatitude(int lat){
  latitude = lat;
}
//latitude get
float getLatitude(){
  return latitude * (3.1415926535/180);
}
//population
void setPopulation(int pop){
  population = pop;
}
int getPopulation(){
  return population;
}
void setType(string type){
  cityType = type;
}
string getType(){
  return cityType;
}

static int w;

void read_cityinfo(vector<city> &cI);

private:
  string city1;
  string cityTown;
  enum type{LOCAL, METRO};
  string cityType;
  float latitude;
  float longitude;
  int population;
};
int city::w = 0;

istream& operator>>(istream& fin, city &myCity){
  string cityState;
  fin >> myCity.cityTown >> cityState >> myCity.cityType >> myCity.latitude >> myCity.longitude >> myCity.population;
  myCity.city1 = myCity.cityTown + "_" + cityState;
  int citySize = myCity.city1.length();
  if(citySize > myCity.w){
    myCity.w = citySize;
  }
  
  return fin;
}
ostream& operator<<(ostream& fout, city &myCity){
  int size = myCity.w + 3;
  /////ask about the setw using static////////
  
  fout << left << fixed << setprecision(2) << setw(size) << setfill('.') << myCity.city1 << "  " << setfill(' ')
  << setw(8) << left << myCity.cityType << "  "
  << setw(8) << right << myCity.population<< "  " 
  << setw(7) << right << myCity.latitude << "  " 
  << setw(7) << right << myCity.longitude;
  return fout;

}

class smatrix{
  public:
  //setting size of matrix
  void setSize(int matrixR){
    matrix.resize(matrixR);
    for(auto i = 0; i < matrix.size();i++){
      matrix[i].resize(matrixR);
    }
  }

  float getValue(int i, int j){
    return matrix[i][j];
  }

  void setValue(int i, int j, float value){
    matrix[i][j] = value;
  }

  private:
  vector<vector<float>> matrix;
};

class costtable{

};

//compare populations
bool operator<(const city &c1, const city&c2){
  if(c1.population > c2.population){
    return true;
  }
  else{
    return false;
  }
}

void read_cityinfo(vector<city> &cI){
  city C;
  string tmp;
  ifstream fin("uscities.txt");
  
  // fin.open(fname);
  //reading the file
  if(fin.is_open()){
    while(getline(fin, tmp)){
      //making sure we dont read the first line
      if(tmp[0] == '#' || tmp.length() == 0){
      }
      else{
        istringstream tmpRead(tmp);
        tmpRead >> C;
        cI.push_back(C);
      }
   
    }
    //Append the state name to the city name to produce "Knoxville_TN" and so forth.
 sort(cI.begin(), cI.end());
    //pass in cI in main when calling read_cityinfo -- dereference read_cityinfo(vector<city> &cI)
  }
  fin.close();
}
void write_cityinfo(const char *fname, vector<city> &cI) {
  ofstream fout;
  fout.open("city_info2.txt");
  for(int i = 0; i < cI.size(); i++){
    fout << setw(4) << i << "  " << cI[i] << endl;
  }
}
int distanceFormula(float lat1, float lat2, float lon1, float lon2){
    //haversine formula
    int radius = 3982;
    float sinFunctLat = pow(sin((lat2-lat1)/2),2);
    float sinFunctLon = pow(sin((lon2 - lon1)/2),2);
    float sqrtFunct = (sinFunctLat + (cos(lat1) * cos(lat2)) * sinFunctLon);
    //distance function
  
    float distance = 2 * radius * asin(sqrt(sqrtFunct));
    
    //returning the distance rounded to 5.0
    return (5.0*round(distance/5.0)); 
}
void create_citydist(vector<city> &c, smatrix &dist_table){
    //getting the distance
    dist_table.setSize(c.size());
    for(int i = 0; i < c.size(); i++){
      for(int j = 0; j < c.size(); j++){
        
        dist_table.setValue(i, j, distanceFormula(c[i].getLatitude(), c[j].getLatitude(), c[i].getLongitude(), c[j].getLongitude()));
      }
    }
  }

void write_citydist(vector<city> &c, smatrix &dist_table){
  //output to a different text file
  ofstream fout;
  fout.open("city_dist.txt");
  //1 to start with LA
   for(int i = 1; i < c.size(); i++){
     for(int j = 0; j < i; j++){

          fout << setw(4) << i << "  " 
          << setw(c[i].w + 3) << left << setfill('.') << c[i].getName() << " to " << setfill(' ')
          << setw(c[j].w +3) << left << setfill('.') << c[j].getName() << "  " 
          << right << setfill(' ') << setw(6) << setprecision(1) << fixed << dist_table.getValue(i, j)
          << " miles" << endl;
        }
     fout << endl;
   }
  } 

// int create_citytime(){
//   //similar to dist but time instead
//   //need to use the distance between the places
// }
// void write_citytime() { }

void create_citygraph(vector<city> &c, vector<vector<int>> &adj_table, smatrix &dist_table, vector<int> &cityRow){
  ifstream fin("uscities.txt");

  for(int i = 0; i < c.size(); i++){
    if(c[i].getType() == "METRO"){
      if(c[i].getPopulation() >= 2000000){
        c[i].setType("NATIONAL");
      }
    else if(c[i].getPopulation() < 500000){
      c[i].setType("LOCAL");
    }
    else{
      c[i].setType("REGIONAL");
      }
    }
  }

  //demoting national citites
  for(int i = 0; i < c.size(); i++){
    if(c[i].getType() == "NATIONAL"){
      vector<int> cityList;
      for(int j = 0; j < c.size(); j++){
        if(c[j].getType() == "NATIONAL" && dist_table.getValue(i, j) <= 200.0){
          cityList.push_back(j);
        }
      }
      int tmp = 0;
      //using bubblesort to sort the vector by population size
      for(int p = 0; p < cityList.size(); p++){
        for(int q = p+1; q < cityList.size(); q++){
          if(c[cityList[q]].getPopulation() > c[cityList[p]].getPopulation()){
            tmp = cityList[p];
            cityList[p] = cityList[q];
            cityList[q] = tmp;
          }
        }
      }
      for(int k = 1; k < cityList.size(); k++){
        c[cityList[k]].setType("REGIONAL");
      }
      }
    }

  //demoting regional cities
  for(int i = 0; i < c.size(); i++){
    if(c[i].getType() == "REGIONAL"){
      vector<int> cityList;
      for(int j = 0; j < c.size(); j++){
        if(c[j].getType() == "REGIONAL" && dist_table.getValue(i, j) < 100.0){
          cityList.push_back(j);
        }
      }
      int tmp = 0;
      //using bubblesort to sort the vector by population size
      for(int p = 0; p < cityList.size(); p++){
        for(int q = p+1; q < cityList.size(); q++){
          if(c[cityList[q]].getPopulation() > c[cityList[p]].getPopulation()){
            tmp = cityList[p];
            cityList[p] = cityList[q];
            cityList[q] = tmp;
          }
        }
      }
      for(int k = 1; k < cityList.size(); k++){
        c[cityList[k]].setType("LOCAL");
      }
      }
    }

    //connection the edges (National Cities)
  adj_table.resize(c.size());
  cityRow.resize(c.size());
  int count = 0;
  for(int i = 0; i < c.size(); i++){
    if(c[i].getType() == "NATIONAL"){
      for(int j = 0; j < c.size(); j++){
        if(c[j].getType() == "NATIONAL" && i != j){
          adj_table[i].push_back(j);
          count++;
          //cout << c[adj_table[i][0]].getName() << endl;
        }
      }
      cityRow[i] = count;
    }
  }
  
    //connection regional cities
  count = 0;  
  for(int i = 0; i < c.size(); i++){
      if(c[i].getType() == "REGIONAL"){
      vector<int> cityList;
      for(int j = 0; j < c.size(); j++){
        if(c[j].getType() != "LOCAL" && i != j){
          cityList.push_back(j);
        }
      }
      int tmp = 0;
      //using bubblesort to sort the vector by increasing distance
      for(int p = 0; p < cityList.size(); p++){
        for(int q = p+1; q < cityList.size(); q++){
          if(dist_table.getValue(i, cityList[p]) > dist_table.getValue(i, cityList[q])){
            tmp = cityList[p];
            cityList[p] = cityList[q];
            cityList[q] = tmp;
          }
        }
      }
      //adding regional connections
      for(int k = 0; k < 3; k++){
        adj_table[i].push_back(cityList[k]);
        adj_table[cityList[k]].push_back(i);
        count++;
        cityRow[cityList[k]]++;
      }
      cityRow[i] += count;
      cityList.clear();
  }
}

  //connecting local cities
  count = 0;
  for(int i = 0; i < c.size(); i++){
      if(c[i].getType() == "LOCAL"){
      vector<int> cityList;
      for(int j = 0; j < c.size(); j++){
        if(c[j].getType() != "LOCAL" && i != j){
          cityList.push_back(j);
        }
      }
      int tmp = 0;
      //using bubblesort to sort the vector by increasing distance
      for(int p = 0; p < cityList.size(); p++){
        for(int q = p+1; q < cityList.size(); q++){
          if(dist_table.getValue(i, cityList[p]) > dist_table.getValue(i, cityList[q])){
            tmp = cityList[p];
            cityList[p] = cityList[q];
            cityList[q] = tmp;
          }
        }
      }
      //adding local - regional connections
      for(int k = 0; k < 2; k++){
        adj_table[i].push_back(cityList[k]);
        adj_table[cityList[k]].push_back(i);
        count++;
        cityRow[cityList[k]]++;
      }
      cityRow[i] += count;
      cityList.clear();
  }
}

  //creating local - local connections
  count = 0;
  for(int i = 0; i < c.size(); i++){
      if(c[i].getType() == "LOCAL"){
      vector<int> cityList;
      for(int j = 0; j < c.size(); j++){
        if(c[j].getType() == "LOCAL" && i != j){
          cityList.push_back(j);
        }
      }
      int tmp = 0;
      //using bubblesort to sort the vector by increasing distance
      for(int p = 0; p < cityList.size(); p++){
        for(int q = p+1; q < cityList.size(); q++){
          if(dist_table.getValue(i, cityList[p]) > dist_table.getValue(i, cityList[q])){
            tmp = cityList[p];
            cityList[p] = cityList[q];
            cityList[q] = tmp;
          }
        }
      }
      //adding regional connections
      int k = 0;
      for(int k = 0; k < cityList.size(); k++)
      {
        if(dist_table.getValue(i, cityList[k]) <= 75.0){
          adj_table[i].push_back(cityList[k]);
          adj_table[cityList[k]].push_back(i);
          count++;
          cityRow[cityList[k]]++;
        }
      }
        cityRow[i] += count;
        cityList.clear();
    }
  }
  for(int i = 0; i < c.size(); i++){
    sort(adj_table[i].begin(), adj_table[i].end());
    adj_table[i].erase(unique(adj_table[i].begin(), adj_table[i].end()), adj_table[i].end());
  }

}

float timeMath(float distance, string cityType1, string cityType2){
  float time = 0;
  if(cityType1 == "NATIONAL" && cityType2 == "NATIONAL"){
    time = distance/500.0; 
  }
  else{
    time = distance/55.0;
    
  }
  
  return time;
}

void create_citytime(vector<city> &c, smatrix &time_table, smatrix &dist_table){
  time_table.setSize(c.size());
    for(int i = 0; i < c.size(); i++){
      for(int j = 0; j < c.size(); j++){
        
        time_table.setValue(i, j, timeMath(dist_table.getValue(i, j), c[i].getType(), c[j].getType()));
      }
    }
  }

void write_cityTime(vector<city> c, smatrix time_table){
  ofstream fout;
  fout.open("city_time.txt");
    for(int i = 1; i < c.size(); i++){
     for(int j = 0; j < i; j++){
          fout << setw(4) << i << "  " 
          << setw(c[i].w + 3) << left << setfill('.') << c[i].getName() << " to " << setfill(' ')
          << setw(c[j].w +3) << left << setfill('.') << c[j].getName() << "  " 
          << right << setfill(' ') << setw(6)  << fixed << setprecision(1) << time_table.getValue(i, j)
          << " hours" << endl;
        }
     fout << endl;
   }
}
  void write_citygraph(vector<city> &c, smatrix &time_table, smatrix &dist_table , vector<vector<int>> &adj_table, vector<int> &cityRow){
    ofstream fout;
    fout.open("city_graph2.txt");
    
    for(int i = 0; i < c.size(); i++){
      //cout << "INTHISLOOP" << endl;
      fout << setw(4) << right << i << " " << c[i].getName() << endl;
     for(int j = 0; j < adj_table[i].size(); j++){
      //for(int j = 0; j < c[i].size(); j++){
       //cout << "INLOOP" << endl;
         fout << setw(6) << adj_table[i][j] << " "
          << setw(c[i].w + 3) << left << setfill('.') << c[adj_table[i][j]].getName() << "  "
          << right << setfill(' ') << setw(6) << setprecision(1) << fixed << dist_table.getValue(i, adj_table[i][j]) << " miles"
          << right << setfill(' ') << setw(5) << setprecision(1) << fixed << time_table.getValue(i, adj_table[i][j])
          << " hours" << endl;
        }
        fout << endl; 
  }
}
enum vcolorType {WHITE, BLACK};
vector<int> dijkstra_route(int source, int sink, smatrix &weight, vector<city> &V, vector<vector<int>> &E){
  //TODO: modify to make work without template format, change to floats
  vector<vcolorType> vcolor;
  vector<float> vdist;
  vector<float> vlink;
  vcolor.assign(V.size(), WHITE);
  vdist.assign(V.size(), numeric_limits<float>::max());
  vdist[source] = 0;
  vlink.assign(V.size(), -1);
  vlink[source] = source;
  
  vector<int> lastCities(V.size(), source);
  vector<int> cityOrder;
  while (1) {
    int next_i = -1;
    float mindist = numeric_limits<float>::max();
    for (int i=0; i<(int)vcolor.size(); i++) {
      if (vcolor[i] == WHITE && mindist > vdist[i]) {
        next_i = i;
        mindist = vdist[i];
      }
    }
    int i = next_i;
    if (i == -1)
      return cityOrder;
    vcolor[i] = BLACK;
    if (i == sink)
      break;
    
    for (int k : E[i]) {
      float wij = weight.getValue(i, k);
      if (vcolor[k] == BLACK)
        continue;
      if (vdist[k] > vdist[i] + wij) {
        vdist[k] = vdist[i] + wij;
        vlink[k] = i;
        lastCities[k] = i;
      }
    }
  }

  while(sink != source){
    // cout << source << endl;
    cityOrder.push_back(sink);
    sink = lastCities[sink];
  }

  cityOrder.push_back(source);

  reverse(cityOrder.begin(), cityOrder.end());
  return cityOrder;
} 
void write_dijkstra(int sink, int source, string runMode, vector<city> &cI, smatrix &time_table, smatrix &dist_table , vector<vector<int>> &adj_table){
  string c1;
  string c2;
  vector<int> dijkstra;
  
 //allowing input when the runmode is going to be dist or time, info should not be included
  if(runMode == "dist" || runMode == "time"){
     cout << "Enter> ";
     while(cin >> c1){
       cin >> c2;
       int flag = -1;

       for(int i = 0; i < cI.size(); i++){
         if(cI[i].getName() == c1){
           source = i;
         }
         else if(cI[i].getName() == c2){
           sink = i;
         }
       }
       if(source != flag && sink != flag){
         if(runMode == "dist"){
           dijkstra = dijkstra_route(source, sink, dist_table, cI, adj_table);
         }
         else if(runMode == "time"){
           dijkstra = dijkstra_route(source, sink, time_table, cI, adj_table);
         }
          //location
         cout << setw(7) << right << fixed << setprecision(2) << dist_table.getValue(source, source) << " miles "
         << setw(5) << fixed << setprecision(2) << right << time_table.getValue(source, source) << " hours  "
         << setw(3) << right << source << "  " 
         << setfill('.') << left << setw(cI[0].w + 3) << cI[source].getName() << "  "
         << setw(8) << setfill(' ') << right << cI[source].getPopulation() << endl;

         float addDistTOT = 0.00;
         // NTS : needed 0.00 in order to have hours and miles added correct RML -- did not work prior
         float addTimeTOT = 0.00;
        //Adding the total mileage and time after each iteration/each city
         for(int i = 1; i < dijkstra.size(); i++){
           addDistTOT += dist_table.getValue(source, dijkstra[i]);
           addTimeTOT += time_table.getValue(source, dijkstra[i]);
                                                                  //TODO: Make this so they add mileage currently not adding
          //output routes
           cout << setw(7) << right << fixed << setprecision(2) << addDistTOT << " miles "
           << setw(5) << fixed << setprecision(2) << right << addTimeTOT << " hours  "
           << setw(3) << right << dijkstra[i] << "  "
            << setfill('.') << left << setw(cI[0].w + 3) << cI[dijkstra[i]].getName() << "  "
          << setw(8) << setfill(' ') << right << cI[dijkstra[i]].getPopulation() << " "
          << setw(7) << fixed << setprecision(2) << right << dist_table.getValue(source, dijkstra[i]) << " miles "
          << setw(5) << right << time_table.getValue(source, dijkstra[i]) << " hours " << endl;
          source = dijkstra[i];
         }
       }
       
       else{
         if(source != flag){
           cout << c1 << ": not found" << endl;
         }
         if(sink == flag){
           cout << c2 << ": not found" << endl;
         }
       }
       cout << endl << "Enter> ";
     }
   } 
}
int main(int argc, char *argv[])
{
  vector<int> cityRow;
  vector<city> cI;
  vector<vector<int>> adj_table; 
  smatrix dist_table;
  smatrix time_table;

  string c1;
  string c2;
  vector<int> dijkstra;
  int source;
  int sink;

  string tmp = argv[1];
  string runMode;

//setting runmodes, cmd line
 if(argc != 3){
   cerr << "Usage: " << argv[0] <<  " -info|dist|time file.txt" << endl;
   exit(1);
 }
  if(tmp == "-info"){
    runMode = "info";
  }
  if(tmp == "-dist"){
    runMode = "dist";
  }
  if(tmp == "-time"){
    runMode = "time";
  }
  if(tmp != "-time" && tmp != "-dist" && tmp != "-info"){
    cerr << "Usage: " << argv[0] << " -info|dist|time file.txt" << endl;
    exit(1);
  }
  read_cityinfo(cI);
  create_citydist(cI, dist_table);
  create_citygraph(cI, adj_table, dist_table, cityRow);
  create_citytime(cI, time_table, dist_table);
  if(runMode == "info"){ 
    write_cityinfo("city_info.txt", cI);
    write_citydist(cI, dist_table);
    write_citygraph(cI, time_table, dist_table, adj_table, cityRow);
   
    write_cityTime(cI, time_table);
  }
  
  
  write_dijkstra(sink, source, runMode, cI, time_table, dist_table, adj_table);

//   commandline option decoding

//   city graph declarations

//   read_cityinfo()

//   set up costtables

//   create_citygraph()

//   if (mode == graphinfo) {
//     write_cityinfo()

//     write_distancetable()
//     write_timetable()

// 	write_citygraph()

// 	return from program
//   }

//   while (not done) {
// 	enter> from, to cities
//     dijkstra_route(costmode,from,to) 
//   }
}
