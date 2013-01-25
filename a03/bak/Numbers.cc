#include <iostream>

using namespace std;

int main() {
  while(true) {
    int i;
    cin >> i;
    if(cin.fail()) break;
    cout << i << endl;
  }
}

