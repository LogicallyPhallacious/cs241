#include <iostream>
#include <sstream>

using namespace std;

int main() {

stringstream s;
  while(true) {
    int i;
    cin >> i;
    if(cin.fail()) break;
    s << i << endl;
  }

string t;
t = s.str();
cout << t << t;
}

