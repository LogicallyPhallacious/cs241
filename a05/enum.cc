#include <iostream>
#include <cstdlib>
using namespace std;

int main(int c, char **a){
	if (c != 3) {
		cerr << "Usage: " << a[0] << " from to"<<endl;
		exit(1);
	}
	int v1 = atoi(a[1]);
	int v2 = atoi(a[2]);
	int max = v1 > v2? v1 :v2;
	for (int i = v1 < v2? v1 : v2; i <= max; i++)
		cout << i << endl;
}
