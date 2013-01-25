#include <iostream>
#include <cstdlib>
#include <sstream>
using namespace std;

int main(int c, char **v){
	if (c != 2){
		cerr << v[0] << ": num_of_repetition"<< endl;
		exit(1);
	}

	int num_rep = atoi(v[1]);
	stringstream ss;

	for (string s;;){
		getline(cin,s);
		if(cin.fail()) break;
		ss << s << endl;
	}

	string s = ss.str();
	for (int i =0; i< num_rep;i++){
		cout << s;
	}
}
