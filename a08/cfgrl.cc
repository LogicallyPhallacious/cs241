/*
   CFGrl Version 1 -- converts .cfg-r file to equivalent .cfg file

   NOTE: NO MEMORY MANAGEMENT IS DONE
   
   Author:   Adam Roegiest
   Version:  1.0

   Input:  .cfg-r file with a single derivation
   Output: .cfg file with abbreviated forward leftmost version of the derivation

   Literal translation of CFGrl Version 2 by Ondrej Lhotak. Available (currently) at:

     http://www.student.cs.uwaterloo.ca/~cs241/a08/CFGrl.java

*/


#include <set>
#include <string>
#include <iostream>
#include <list>
#include <sstream>
#include <stack>
using namespace std;

set<string> terms;
set<string> nonterms;
set<string> prods;
string start;

struct Tree {
    string rule;
    list<Tree*> children;
};

void readsyms(set<string> &t) {
	int n;
	string temp;
	getline(cin,temp);
	istringstream iss(temp);
	iss >> n;
    for(int i = 0; i < n; i++) {
		getline(cin,temp);
		t.insert(temp);
    }
}

void traverse(Tree *t, int d) {
    for(int i = 0; i < d; i++) cout << " ";
	cout << t->rule << endl; // print root
    for(list<Tree*>::iterator it=(t->children).begin(); it != (t->children).end(); it++) {  // print all subtrees
        traverse(*it, d+1);
	}
}

void dump(set<string> &h) {
    cout << h.size() << endl;
    for(set<string>::iterator it=h.begin(); it != h.end(); it++) {
        cout << *it << endl;
    }
}

void popper(stack<Tree *> &myStack, list<string> &rhs, string rule) {
	Tree *n = new Tree();
    n->rule = rule;
    for(list<string>::iterator it=rhs.begin(); it != rhs.end(); it++){
		Tree *tmp = myStack.top();
        n->children.push_front(tmp);
		myStack.pop();
    }
    myStack.push(n);
}

Tree* lrdo() {
	stack<Tree*> myStack;
	string l; // lhs symbol
	do {
		string f;
		getline(cin,f);
		list<string> r; // rhs symbols
		istringstream iss(f);
		iss >> l; // lhs symbol
		string s;
		while(iss >> s) {
			if(nonterms.count(s) > 0) r.push_back(s); // only non-terminals
		}
		popper(myStack, r, f); // reduce rule
	} while(start != l);
	return myStack.top();
}

int main(){
	readsyms(terms); // read terminals
	readsyms(nonterms); // read nonterminals
	getline(cin,start); // read start symbol
	readsyms(prods); // read production rules
	
	Tree *parsetree = lrdo(); // read reverse rightmost derivation
	dump(terms); // write terminals in .cfg format
	dump(nonterms); // write nonterminals
	cout << start << endl; // write start symbol
	dump(prods); // write production rules
	traverse(parsetree, 0); // write forward leftmost derivation
}
