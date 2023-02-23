#include <...>
using namespace std;

class vtable {
  public:
    size() { return size of 1D-array }
    push(string &) { add new vertex label }
    operator[](int) { return vertex label }

    begin() { return begin iterator }
    end() { return end iterator }

  private:
    1D-array vertex labels;
};

class adjtable {
  public:
    size() { return size of 2D-array }

    set_size(int) { set size of 2D-array }
    operator[](int) { return reference to 1D-array }

  private:
    2D-array edges;
};

void set_oldfriends(vtable &names, adjtable &friends, int M0, int M1) {
  int N = (int)name.size();
  friends.set_size(N);

  create do_know index list of names

  for (int i=0; i<N; i++)
    set random num friends: M0 <= M <= M1

    randomize first M entries of do_know list
    ensure that index i does not appear here

    update pairs of friends entries accordingly
  }
}

void set_newfriends(adjtable &friends, adjtable &new_friends) {
  int N = (int)friends.size();
  new_friends.set_size(N);

  for (int i=0; i<N; i++) {
    for (each adjacent friend: friends[i][j] == 1) {
      for (each adjacent friend-of-friend: friends[j][k] == 1) {
        if (k-is-not-i && k-is-not-friend-of-i)
          update corresponding pairs of new_friends entries
      }
    }
  }
}

void writetofile(const char *fname, vtable &name, adjtable &friends) {
  int N = (int)name.size();

  determine max name length

  open file

  for (int i=0; i<N; i++) {
    for (each adjacent friend: friends[i][j] == 1) {
      pretty-print name[i] and name[j] (see assignment) 
  }

  close file
}

int main(int argc, char *argv[]) {
  int seed = 0; // default seed value
  int M0 = 1;   // min number of friends
  int M1 = 2;   // max number of friends

  parse command-line arguments
  print usage message and exit if invalid

  seed random number generator

  vtable names;    // vertex labelA
  populate vtable with names read from stdin

  adjtable friends
  adjtable new_friends

  set_oldfriends(name, friends, M0, M1);
  writetofile("doknow1.txt", name, friends);

  set_newfriends(friends, new_friends);
  writetofile("mightknow1.txt", name, new_friends);
}
