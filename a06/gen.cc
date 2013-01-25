#include <iostream>
#include <vector>
#include <cstdlib>
#include <sstream>
using namespace std;

vector<vector<int> *> garbage;

string printMoney(const int value){
	string s = "";
	switch(value){
		case 20: s="loonie";break;
		case 5: s="quarter";break;
		case 2: s="dime"; break; 
		case 1: s="nickel"; break;
	}
	return s;
} 

int search(stringstream &ss, const int size, const int val[], const int &max,int &numCombo, vector<int> *lst = NULL, const int acc = 0, const int adder = 0){
	int newval = adder + acc;
	if (!lst){
		lst = new vector<int>();
		garbage.push_back(lst);
	}
	if (newval < max){
		for (int i = 0; i < size; i++){
			vector<int> *v = new vector<int>(*lst);
			garbage.push_back(v);
			v->push_back(adder);
			search(ss, size, val, max, numCombo, v, newval , val[i]);
		}
//ss << acc << " " << printMoney(adder) << " " << newval << endl;

	} else if (newval == max){
//ss << acc << " " << printMoney(adder) << " " << newval << endl;
#if 1
		for (int i = 1, localacc = (*lst)[i-1], next, cur; i <= lst->size(); i++){
			numCombo++;
			cur = i == lst->size() ? adder : (*lst)[i];
			next = cur + localacc;
			ss << localacc <<" " <<printMoney(cur) << " "<< next << endl;;
			localacc += cur;
		}
		//cout << printMoney(adder) <<endl;
#endif
	}
}

inline float normalize(int v){
	return (float) v / 20;
}

int main(){
	int max = 25;	
	int val[] = {20,5,2,1};	
	int size = 4;
	int numCombo = 0;
	stringstream ss;
	search(ss, size, val, max, numCombo);
#if 0
	cout << size << endl;
	for (int i = 0; i < size; i++){
		cout << printMoney(val[i]) << endl;
	}
	
	cout << max + 1 << endl;
	for (int i = 0; i <= max; i++){
		cout << i << endl;
	}
	// start
	cout << 0 << endl;

	// end
	cout << 1 << endl << max << endl;

#if 0
	for (int i = 0; i < max; i++){
		cout << normalize(max)<< endl;
	}
#endif

	cout << numCombo << endl;
#endif
	cout << ss.str();

	for (int i =0; i< garbage.size(); i++){
		delete garbage[i];
	}
#if 0
	int *b[3];
	int k[2]= {3,3};
	b[0]= k;
	b[1]=k;
#endif
}
