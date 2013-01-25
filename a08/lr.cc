#include <stdio.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <new>
#include <vector>
using namespace std;

enum Action {reduce, shift};

template <class T>
void printVector(vector<T> v){
	int i = 0, size = v.size();
	for (; i < size ; i++)
		cout << v[i] << " ";
	cout << endl;
}

template <class T>
inline void deleteVectorOfPointers(vector<T> v){
	for (int i = 0; i < v.size(); i++)
		delete v[i];
}

class Pair{
	public:
	int x, y;
	Pair(int x, int y){
		this->x = x;
		this->y = y;
	}
};

ostream &operator<<(ostream &out, Pair p){
	out << '(' << p.x << ' ' << p.y << ')';
	return out;
}

/**
 * Skip the grammar part of the input.
 *
 * @param in the file pointer for reading input
 */
void skipLine(FILE* in) {
	char buf[256];
	fgets(buf, 256, in);
}

char* trim(char* str) {
	int i, len;
	char* result;

	len = strlen(str);
	if (str[len-1] == '\n'){
		str[--len] = '\0';
	}

	for (i=len-1; i>=0 && ' '==str[i]; --i)
		len--;
	str[i+1] = '\0';

	result = str;
	for (i=0; i<len && ' '==str[i]; ++i) {
		result = result+1;
	}
	
	return result;
}

enum SpecialStates{START, NUL, WHITESPACE, NUM, SS_SIZE};

class DFA{
private:
	int cur, numToken, res, tokenTemp, strAt;
	vector <vector<int> > delta;
	vector <bool> finish;
	vector <string> token;
	vector<int> emptyVec;
	vector<int> prevStrAt;

	int newToken(int index = -1){
		index = index == -1 ? cur : index;
		delta.push_back(emptyVec);
		return index;
	}

	void setState(int tokenId){
		int length = token[tokenId].length(), j = 0, current = START;
		for (int next = NUL; j < length - 1; j++){
			next = delta[current][token[tokenId][j]];
			if (next != NUL) goto setStatecont;
			next = newToken();
			delta[current][token[tokenId][j]] = next;
			cur++;

setStatecont:
			current = next;
		}
		delta[current][token[tokenId][j]] = tokenId + SS_SIZE;
	}

public:
	DFA(){
		res = SS_SIZE;
		strAt = cur = numToken = 0;
		delta.reserve(res);
		for (int i = 0; i < 256; emptyVec.push_back(NUL), i++);
		for (int i = 0; i < SS_SIZE; newToken(i), i++);
		// setup numbers
		for (int i = '0'; i < '9'; delta[NUM][i] = NUM, i++);
	}
	
	~DFA(){
	//		delete [][] delta;
	}

	void pushToken(string s){
		for (int i = 0; i< token.size(); i++)
			if (token[i] == s) return;
		token.push_back(s);
		newToken(numToken + SS_SIZE);
		numToken++;
	}

	void complete(){
		int res = SS_SIZE + numToken - 1;
		delta.reserve(res);
		for (int i = 0; i < res; delta[i].reserve(256), i++);
		cur = SS_SIZE + numToken;
		for (int i = 0; i < numToken; setState(i), i++);
	}
	
	bool inLanguage(string s, int starting = 0, bool pop = 1){
		int length = s.length(), current = START, i = starting;
		for (int next; i < length; i++){
			next = delta[current][s[i]];
			if (s[i] == ' ') {
				// find the first instant of non-whitespace character
				for (; i < length && s[i] == ' '; i++); 
//cout  << "s[i]: "<<s[i] << " "<<i <<" "<< (s[i] == ' ') <<endl;
				break;
			} else if (next == NUL) {
				current = next;
				break;
			}
			current = next;
		}
		tokenTemp = current;
		if (i <= length && pop){
			prevStrAt.push_back(strAt);
			strAt = i;
		}
		return isFinish(current);
	}

	inline bool isFinish(int state){
		return state >= SS_SIZE && state < numToken + SS_SIZE;
	}

	inline int getTokenId(string s, int starting = 0,bool pop=1){
		return inLanguage(s, starting,pop) ? tokenTemp : -1;
	}

	void restorePreStrAt(){
		int size = prevStrAt.size();
		strAt = prevStrAt[size-1];
		prevStrAt.pop_back();
	}

	inline int getStrAt(){
		return strAt;
	}

	inline string symToStr(int sym){
		return token[sym - SS_SIZE];
	}

	inline int T(int s0, int c){
		return delta[s0][c];
	}

	void printTokens(){
		printVector<string>(token);
	}

	void newIteration(){
		strAt = 0;
	}
} dfa;

class Grammar{
public:
	class Rule{
	public:
		int sym;
		vector<int> RHS;
		Rule(int sym){
			this->sym = sym;
		}
		void addToRHS(int r){
			RHS.push_back(r);
		}
	};

	class Action{
	public:
		int sym, nextStateOrRule;
		char opType;
		Action(int sym, char opType, int nextStateOrRule){
			this->sym = sym;
			this->opType = opType;
			this->nextStateOrRule = nextStateOrRule;
		}
	};

	int numTerm, numNonTerm, numRules, numStates, numActions, initAction, start;
	vector<Rule>rules;
	vector<vector<Action> > action;

	Grammar(){
		initAction =0;
	}

	void addRule(string s){
		dfa.newIteration();
		int len = s.length(), sym = dfa.getTokenId(s, dfa.getStrAt());
		Rule r = Rule(sym);
		for(;dfa.getStrAt() < len;){
			sym = dfa.getTokenId(s, dfa.getStrAt());
			r.addToRHS(sym);
		}
		rules.push_back(r);
	}

	void setStart(int s){
		start = s;
	}

	void printRule(int i){
		cout << dfa.symToStr(rules[i].sym) << " ";
		for (int j = 0; j < rules[i].RHS.size(); j++){
			cout <<  dfa.symToStr(rules[i].RHS[j]) << " ";
		}
		cout << endl;
	}

	void printAllRule(){
		int i = 0, size = rules.size();
		for (; i < size ; i++){
			printRule(i);
		}
	}

	void setAttributes(int numTerm, int numNonTerm, int numRules, int numStates, int numActions){
		this->numTerm = numTerm;
		this->numNonTerm = numNonTerm;
		this->numRules = numRules;
		this->numStates = numStates;
		this->numActions = numActions;
	}

	void addAction(int state, string sym, char opType, int nextStateOrRule){
		if(!initAction){
			action.reserve(numStates);
			vector<Action> vact;
			for (int i = 0; i < numStates;i++){
				action.push_back(vact);
			}
			initAction = 1;
		}
		Action act = Action(dfa.getTokenId(sym), opType, nextStateOrRule);
		action[state].push_back(act);
	}

	int predict(int state, int sym){
		for (int j = 0 ; j < action[state].size(); j++){
			if (action[state][j].sym == sym) return j;
		}
		return -1;
	}

#if 0
	void reduce(int state, int prev, vector<Pair> &symStack, int ruleNum){
		if (ruleNum == -1) return;
		size = rules[ruleNum].RHS.size();
		for (int i = 0; i < size; i++){
			symStack.pop_back();	
		}
		int sym = rules[ruleNum].sym;		
		

		for (int i = 0; i < rules.size(); i++){
			if (rules[i].RHS.size() != 1) continue;
			if (rules[i].RHS.[0] == sym){
				sym = rules[i].sym;
			}
		}

		prediction = predict(prev, rules[ruleNum].sym);


		symStack.push_back(Pair(prev, prediction));


		nextAction = action[prev][prediction];
		prev = symStack[symStack.size()-2].x;
		state = nextAction.nextStateOrRule;
	}
#endif

	void parse(FILE *in){
			char line[256];
			fgets(line, 256, in);
			char* str = trim(line);
			int len = strlen(str), state = 0, sym, prediction, size, prev = 0, ruleNum;
			vector <Pair> symStack;
			symStack.reserve(len);

			dfa.newIteration();
		for(int count = 1;;){
			sym = dfa.getTokenId(str, dfa.getStrAt(), 0);
			if (sym == -1) break;
			prediction = predict(state, sym);
			if (prediction == -1){
				throw(count);
			}

			Action nextAction = action[state][prediction];
			if (nextAction.opType == 'r'){
// continue to reduce
contToReduce:
				ruleNum = nextAction.nextStateOrRule;
				size = rules[ruleNum].RHS.size();
				for (int i = 0; i < size-1; i++){
					symStack.pop_back();	
				}
				Pair prevP =symStack[symStack.size()-1];
				symStack.pop_back();
				prev = prevP.x;
				int symReduced = rules[ruleNum].sym;		
				printRule(nextAction.nextStateOrRule);

				prediction = predict(prev, symReduced);
				symStack.push_back(Pair(prev, prediction));
				nextAction = action[prev][prediction];
				state=prev;
				if (nextAction.nextStateOrRule == 'r') goto contToReduce;
				//dfa.restorePreStrAt();
			} else {
				symStack.push_back(Pair(state, prediction));
				dfa.getTokenId(str, dfa.getStrAt());
				count++;
			}
			prev = state;
			state = nextAction.nextStateOrRule;
//			printVector<Pair>(symStack);
//for (int i = 0; i < symStack.size();i++){
//	cout << dfa.symToStr(action[symStack[i].x][symStack[i].y].sym) <<" ";
//} cout << endl;
//			cout << state << endl;
		}
		for (int i = 0; i < rules.size(); i++){
			if (rules[i].sym == start){
				printRule(i);
				break;
			}
		}
//			printVector<Pair>(symStack);
	}
} G;

void createGrammar(FILE* in) {
	int i, numTerm, numNonTerm, numRules, numStates, numActions;
	char line[256];
	char start[256];

	// read the number of terminals and move to the next line
	fscanf(in, "%d", &numTerm);
	skipLine(in);
	
	// push the lines containing the terminals to the DFA
	for (i = 0; i < numTerm; i++) {
		fgets(line, 256, in);
		dfa.pushToken(trim(line));
	}
	
	// read the number of non-terminals and move to the next line
	fscanf(in, "%d", &numNonTerm);
	skipLine(in);
	
	// skip the lines containing the non-terminals
	for (i = 0; i < numNonTerm; i++) {
		fgets(line, 256, in);
		dfa.pushToken(trim(line));
	}
	dfa.complete(); // finish the dfa
	
	// skip the line containing the start symbol
	fscanf(in, "%s", start);
	skipLine(in);
	G.setStart(dfa.getTokenId(start));

	// read the number of rules and move to the next line
	fscanf(in, "%d", &numRules);
	skipLine(in);
	
	// skip the lines containing the production rules
	for (i = 0; i < numRules; i++) {
		fgets(line, 256, in);
		G.addRule(trim(line));
	}

	fscanf(in, "%d", &numStates);
	skipLine(in);

	fscanf(in, "%d", &numActions);
	skipLine(in);

	G.setAttributes(numTerm, numNonTerm, numRules, numStates, numActions);

	int state, nextStateOrRule;
	char sym[256], opType[256];
	for (i = 0; i < numActions; i++) {
		fscanf(in, "%d %s %s %d", &state, sym, opType, &nextStateOrRule);
		skipLine(in);
		G.addAction(state,sym,opType[0],nextStateOrRule);
	}
}

/**
 * Reads a .cfg file and prints the left-canonical
 * derivation without leading or trailing spaces.
 */
int main(int argc, char** argv) {
	createGrammar(stdin);
	try{
		G.parse(stdin);
	} catch (int e){
		cerr << "ERROR at " << e << endl;
	}
	return 0;
}
