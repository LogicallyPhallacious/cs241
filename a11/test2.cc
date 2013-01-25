#include<iostream>
using namespace std;

void println(int k){
	cout << k<<endl;
}

int wain(int p1, int p2) {
	//----------------------------------------------------
	// define more variables than registers	
	//----------------------------------------------------
	int counter = 0;
	int ret = 0;
	int loopEnd = 100;
	//----------------------------------------------------
	int v0 = 0;
	int v1 = 1;
	int v2 = 2;
	int v3 = 3;
	int v4 = 4;
	int v5 = 5;
	int v6 = 6;
	int v7 = 7;
	int v8 = 8;
	int v9 = 9;
	int v10 = 0;
	int v11 = 0;
	int v12 = 0;
	int v13 = 0;
	int v14 = 0;
	int v15 = 0;
	int v16 = 0;
	int v17 = 0;
	int v18 = 0;
	int v19 = 0;
	int v20 = 20;
	int v21 = 21;
	int v22 = 22;
	int v23 = 23;
	int v24 = 24;
	int v25 = 25;
	int v26 = 26;
	int v27 = 27;
	int v28 = 28;
	int v29 = 29;
	int v30 = 30;
	int v31 = 31;


	//----------------------------------------------------
	// force calculations of v10 -> v19
	// use a combination of variables and literals in
	// the calculations.
	//----------------------------------------------------
	v10 = (v1+(v1+(v1+(v1+(v1+(v1+(v1+(v1+(v1+v1)))))))));
	v11 = ((v2+v2)+(2+2)+(v2+v1));
	v12 = (v6*2);
	v13 = 1+((((((((((((1+1)+1)+1)+1)+1)+1)+1)+1)+1)+1)+1));
	v14 = v7 * 2;
	v15 = v5 * (v3 * v1);
	v16 = v8 + (v2 * (v2 * (v4 / 2)));
	v17 = (v31-v15) + 1;
	v18 = (v10 * (v28-v10)) / v10;
	v19 = v17 + (v10 % 4);

	//----------------------------------------------------
	// loop that prints all numbers between
	// (counter -> loopEnd) where (n mod p1) == 0
	//----------------------------------------------------
	// if p2 >= 0:
	// 		ret incremented by 1 when (n mod p1) != 0
	// if p2 < 0:
	//		ret decremented by 1 when (n mod p1) != 0 
	//----------------------------------------------------
	while(counter < loopEnd) {
		counter = counter + v1;
		
		if ( (counter % p1) == v0 ) {
			println(counter);
		} else {
			if ( p2 >= v0 ) {
				ret = ret + v1;
			} else {
				ret = ret - 1; 
			}
		}
	}

	println(ret);
	return ret;
}

int main(int argc, char **argv){
//int a[21]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21};
cout << wain(7,10)<<endl;;
 return 0;
}
