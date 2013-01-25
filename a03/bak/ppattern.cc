#include <iostream>
using namespace std;

int main(){
	for (int i = -0x8000; i < 0x8000 ; i++)
		cout << "beq $3, $3, " << i << endl;
}
