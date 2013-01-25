#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <sstream>
#include <cstdio>
using namespace std;

//void error(int e);
//void error(string s);

void skipLine(istream &in) {
	in.ignore(256, '\n');
}

template<class T>
ostream &operator<<(ostream &out, vector<T> const &v) {
	for (int i = 0, size = v.size();;) {
		out << v[i];
		i++;
		if (i == size)
			break;
		out << " ";
	}
	return out;
}

template<class T1, class T2>
struct Pair {
	T1 x;
	T2 y;
	Pair(T1 x, T2 y) {
		this->x = x;
		this->y = y;
	}
};

template<class T1, class T2, class T3>
struct Triple {
	T1 x;
	T2 y;
	T3 z;
	Triple(T1 x, T2 y, T3 z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

template<class T>
void printVector(vector<T> v) {
	for (int i = 0; i < v.size(); i++)
		cout << "\"" << v[i] << "\"," << endl;
}
template<class T1, class T2>
ostream &operator<<(ostream &out, Pair<T1, T2> const &p) {
	out << '(' << p.x << ' ' << p.y << ')';
	return out;
}

enum DataType {
	TYPE_INT, TYPE_INTSTAR
};
const string DataTypeStr[] = { "int", "int*" };

enum SpecialStates {
	START, NUL, SS_SIZE
};

enum Tokens {
	T_AMP = SS_SIZE,
	T_BECOMES,
	T_BOF,
	T_COMMA,
	T_DELETE,
	T_ELSE,
	T_EOF,
	T_EQ,
	T_GE,
	T_GT,
	T_ID,
	T_IF,
	T_INT,
	T_LBRACE,
	T_LBRACK,
	T_LE,
	T_LPAREN,
	T_LT,
	T_MINUS,
	T_NE,
	T_NEW,
	T_NULL,
	T_NUM,
	T_PCT,
	T_PLUS,
	T_PRINTLN,
	T_RBRACE,
	T_RBRACK,
	T_RETURN,
	T_RPAREN,
	T_S,
	T_SEMI,
	T_SLASH,
	T_STAR,
	T_WAIN,
	T_WHILE,
	T_dcl,
	T_dcls,
	T_expr,
	T_factor,
	T_lvalue,
	T_procedure,
	T_statement,
	T_statements,
	T_term,
	T_test,
	T_type
};

bool const isTerminal[47] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0 };

enum{SIZE_PRINTLN = 60};

string const PRINTLN[SIZE_PRINTLN] ={"print:",
"sw $1, -4($30)",
"sw $2, -8($30)",
"sw $3, -12($30)",
"sw $4, -16($30)",
"sw $5, -20($30)",
"sw $6, -24($30)",
"sw $7, -28($30)",
"sw $8, -32($30)",
"sw $9, -36($30)",
"sw $10, -40($30)",
"lis $3",
".word -40",
"add $30, $30, $3",
"lis $3",
".word 0xffff000c",
"lis $4",
".word 10",
"lis $5",
".word 4",
"add $6, $1, $0",
"slt $7, $1, $0",
"beq $7, $0, IfDone",
"lis $8",
".word 0x0000002d",
"sw $8, 0($3)",
"sub $6, $0, $6",
"IfDone:",
"add $9, $30, $0",
"Loop:",
"divu $6, $4",
"mfhi $10",
"sw $10, -4($9)",
"mflo $6",
"sub $9, $9, $5",
"slt $10, $0, $6",
"bne $10, $0, Loop",
"lis $7",
".word 48",
"Loop2:",
"lw $8, 0($9)",
"add $8, $8, $7",
"sw $8, 0($3)",
"add $9, $9, $5",
"bne $9, $30, Loop2",
"sw $4, 0($3)",
"lis $3",
".word 40",
"add $30, $30, $3",
"lw $1, -4($30)",
"lw $2, -8($30)",
"lw $3, -12($30)",
"lw $4, -16($30)",
"lw $5, -20($30)",
"lw $6, -24($30)",
"lw $7, -28($30)",
"lw $8, -32($30)",
"lw $9, -36($30)",
"lw $10, -40($30)",
"jr $31"};

class DFA {

	int cur, numToken, tokenTemp, i;
	vector<vector<int> > delta;
	vector<string> token;
	vector<int> emptyVec;

	int newToken(int index = -1) {
		index = index == -1 ? cur : index;
		delta.push_back(emptyVec);
		return index;
	}

	void setState(int tokenId) {
		int length = token[tokenId].length(), j = 0, current = START;
		for (int next = NUL; j < length - 1; j++) {
			next = delta[current][token[tokenId][j]];

			if (next != NUL) // token exists in DFA
				goto setStatecont;

			next = newToken();
			delta[current][token[tokenId][j]] = next;
			cur++;

			setStatecont: current = next;
		}
		delta[current][token[tokenId][j]] = tokenId + SS_SIZE; // setup finish state
	}

public:
	DFA() {
		cur = numToken = 0;
		token.reserve(400);
		delta.reserve(SS_SIZE);
		for (i = 0; i < 256; i++)
			emptyVec.push_back(NUL);
		for (i = 0; i < SS_SIZE; i++)
			newToken();
	}

	void pushToken(string const &s) {
		for (int i = 0; i < token.size(); i++)
			if (token[i] == s)
				return;
		token.push_back(s);
		newToken(numToken + SS_SIZE);
		numToken++;
	}

	void complete() {
		int res = SS_SIZE + numToken - 1;
		sort(token.begin(), token.end());
		delta.reserve(res);
		for (i = 0; i < res; i++)
			delta[i].reserve(256);

		cur = SS_SIZE + numToken;

		for (i = 0; i < numToken; i++)
			setState(i);
	}

	bool inLanguage(string const &s) {
		int length = s.length(), current = START;
		for (int i = 0, next; i < length; i++) {
			next = delta[current][s[i]];
			current = next;
			if (next == NUL) {
				break;
			}
		}
		tokenTemp = current;
		return isToken(current);
	}

	inline bool isToken(int state) const {
		return state >= SS_SIZE && state < numToken + SS_SIZE;
	}

	inline int getTokenId(string const &s) {
		return inLanguage(s) ? tokenTemp : -1;
	}

	inline int getTokenIdNormalized(string const &s){
		return  getTokenId(s) - SS_SIZE;
	}

	void printAllTokens(){
		for(int i = 0; i < token.size(); i++){
		cout <<token[i]<< " ";
		}
		cout << endl;
	}

	inline string &symToStr(int sym) {
		return token[sym - SS_SIZE];
	}
} dfa, varDfa;

class Grammar {
	class Rule {
		friend ostream &operator<<(ostream &out, Rule &r);
		int RHScounter;
	public:
		int sym;
		vector<int> RHS;

		Rule(int sym) {
			this->sym = sym;
			RHS.reserve(10);
			RHScounter = 0;
		}

		inline int sizeRHS() const {
			return RHScounter;
		}

		inline void addToRHS(int r) {
			RHS.push_back(r);
			RHScounter++;
		}
	};
	friend ostream &operator<<(ostream &out, Rule &r);

	struct Action {
		int sym, nextStateOrRule;
		char opType;
		Action(int sym, char opType, int nextStateOrRule) {
			this->sym = sym;
			this->opType = opType;
			this->nextStateOrRule = nextStateOrRule;
		}
	};

	struct TreeNode {
		int id;
		string lexeme;
		vector<int> RHS;
		list<TreeNode *> children;

		TreeNode(int id, vector<int> &RHS) {
			this->id = id;
			this->RHS = RHS;
			this->lexeme = "";
		}

		TreeNode(int id, string &lexeme) {
			this->id = id;
			this->lexeme = lexeme;
		}

		TreeNode(int id, vector<int> &RHS, string &lexeme) {
			this->id = id;
			this->RHS = RHS;
			this->lexeme = lexeme;
		}

		~TreeNode() {
			for (; !children.empty();) {
				delete children.back();
				children.pop_back();
			}
		}
	private:
		friend ostream &operator<<(ostream &out, TreeNode *t);
	};
	friend ostream &operator<<(ostream &out, Grammar::TreeNode *t);

	inline void setStart(int s) {
		start = s;
	}

	inline void setNumTerm(int i) {
		numTerm = i;
	}
	inline void setNonTerm(int i) {
		numNonTerm = i;
	}
	inline void setNumRules(int i) {
		numRules = i;
	}
	inline void setNumStates(int i) {
		numStates = i;
		action.reserve(numStates);
		vector<Action> vact;
		vact.reserve(10);
		for (int i = 0; i < numStates; i++) {
			action.push_back(vact);
		}
		vector<int> neg;
		neg.reserve(256);
		for (int i = 0; i < 256; i++) {
			neg.push_back(-1);
		}
		predictTable.reserve(numStates);
		for (int i = 0; i < numStates; i++)
			predictTable.push_back(neg);
	}
	inline void setNumActions(int i) {
		numActions = i;
	}

	TreeNode *buildTree(int id, int ruleId, string lexeme, list<int> &derSteps,
			list<Pair<int, string> > &lstToken) {
		TreeNode * t;
		if (ruleId == -1)
			return new TreeNode(id, lexeme);
		t = new TreeNode(id, rules[ruleId].RHS);
		string s;

		for (int i = rules[ruleId].sizeRHS() - 1, nextId, nextRule; i >= 0; i--) {
			nextId = rules[ruleId].RHS[i];
			if (!derSteps.empty() && nextId == rules[derSteps.back()].sym) {
				nextRule = derSteps.back();
				derSteps.pop_back();
				t->children.push_front(buildTree(nextId, nextRule, "",
						derSteps, lstToken));
			} else { // all read have been read; lexeme remaining
				s = lstToken.back().y;
				lstToken.pop_back();
				t->children.push_front(buildTree(nextId, -1, s, derSteps,
						lstToken));
			}
		}

		return t;
	}

	bool existInSymTable(string &s) {
		list<Pair<string, DataType> >::iterator it;
		for (it = symbolTable.begin(); it != symbolTable.end(); it++)
			if (it->x == s)
				return 1;
		return 0;
	}

	// Compute symbols defined in t.
	void genSymbols(TreeNode *t) {
		int size = 0;
		DataType type;
		if (t->id == T_dcl) {
			string lexeme = t->children.back()->lexeme;
			if (existInSymTable(lexeme))
				throw("duplicate identifier: " + lexeme);
			type = (t->children.front()->children.size() == 1) ? TYPE_INT
					: TYPE_INTSTAR;
			symbolTable.push_back(Pair<string, DataType> (lexeme, type));
			varDfa.pushToken(lexeme);
		} else if (t->id == T_ID) {
			if (!existInSymTable(t->lexeme))
				throw("identifier used before declaration: " + t->lexeme);
		} else {
			list<TreeNode*>::iterator it;
			for (it = t->children.begin(); it != t->children.end(); it++) {
				genSymbols(*it);
			}
		}
	}

	TreeNode *parseTree;
	int numTerm, numNonTerm, numRules, numStates, numActions, start,
			startingRuleId, labelCounter;
	bool includedPrintln;
	vector<Rule> rules;
	vector<vector<Action> > action;
	vector<vector<int> > predictTable;
//	vector<int>varValue;
	list<Pair<string, DataType> > symbolTable;
	string buf, buf2;

public:
	Grammar() { // initialise values for safety
		parseTree = NULL;
		numTerm = numNonTerm = numRules = numStates = numActions = start
				= startingRuleId = labelCounter=includedPrintln=0;
	}

	void addRule(string &s) {
		stringstream ss(stringstream::in | stringstream::out);
		ss << s;
		ss >> buf;
		int LHS = dfa.getTokenId(buf), sym;
		Rule r = Rule(LHS);
		for (;;) {
			ss >> buf;
			if (ss.fail())
				break;
			sym = dfa.getTokenId(buf);
			r.addToRHS(sym);
		}
		rules.push_back(r);
		if (LHS == start)
			startingRuleId = rules.size() - 1;
	}

	void addAction(int state, string &sym, char opType, int nextStateOrRule) {
		int id = dfa.getTokenId(sym);
		predictTable[state][id] = action[state].size();
		action[state].push_back(Action(id, opType, nextStateOrRule));
	}

	void parse(istream &in) {
		int i, j, state = 0, sym, prediction, size, prev = 0, ruleNum, count =
				0, symReduced;
		list<Pair<int, int> > lstAction;
		list<int> derSteps;
		list<Pair<int, string> > lstToken;

		for (bool pop = 1, bof = 1, eof = 0, end;;) {
			end = in.peek() == EOF;
			if (pop) {
				if (bof) {
					buf = buf2 = "BOF";
					bof = 0;
				} else if (end && !eof) {
					buf = buf2 = "EOF";
					eof = 1;
				} else if (end) {
					break;
				} else {
					in >> buf >> buf2;
					skipLine(in);
				}
				sym = dfa.getTokenId(buf);

				lstToken.push_back(Pair<int, string> (sym, buf2));
				pop = 0;
			}

			prediction = predictTable[state][sym];
			if (prediction == -1) {
				if (in.fail())
					break;
				throw(count);
			}
			Action nextAction = action[state][prediction];
			if (nextAction.opType == 'r') {
				// continue to reduce
				contToReduce: derSteps.push_back(nextAction.nextStateOrRule);

				ruleNum = nextAction.nextStateOrRule;
				size = rules[ruleNum].sizeRHS();
				if (size != 0) {
					for (i = 0; i < size - 1; i++) {
						lstAction.pop_back();
					}

					prev = lstAction.back().x;
					lstAction.pop_back();
				} else {
					prev = state;
				}
				symReduced = rules[ruleNum].sym;

				prediction = predictTable[prev][symReduced];
				lstAction.push_back(Pair<int, int> (prev, prediction));
				nextAction = action[prev][prediction];
				state = prev;

				if (nextAction.nextStateOrRule == 'r') {
					goto contToReduce;
				}
			} else {
				lstAction.push_back(Pair<int, int> (state, prediction));
				count++;
				pop = 1;
			}
			prev = state;
			state = nextAction.nextStateOrRule;
		}

		size = lstAction.size();
		Pair<int, int> *p;
		if (rules[startingRuleId].sizeRHS() != size)
			throw(count);
		for (j = 0; !lstAction.empty(); j++) {
			p = &(lstAction.front());
			if (rules[startingRuleId].RHS[j] != action[p->x][p->y].sym) {
				throw(count);
			}
			lstAction.pop_front();
		}

		parseTree = buildTree(start, startingRuleId, dfa.symToStr(start),
				derSteps, lstToken);
		genSymbols(parseTree);
		varDfa.complete();
	}

	void printParseTreeUnformatted(TreeNode *t) {
		cout << t << endl;

		list<TreeNode*>::iterator it = t->children.begin();
		for (; it != t->children.end(); it++) {
			printParseTreeUnformatted(*it);
		}
	}

	void printParseTree() {
		printParseTree(parseTree);
	}

	void printParseTree(TreeNode *t, int depth = 0) {
		for (int i = 0; i < depth; i++)
			cout << '	';
		cout << t << endl;

		list<TreeNode*>::iterator it = t->children.begin();
		for (; it != t->children.end(); it++) {
			printParseTree(*it, depth + 1);
		}
	}

	void completeWLPPGrammar() {
		int i, numTerm, numNonTerm, numRules, numStates, numActions;
		string line, start;

		const string terminal[] = { "AMP", "BECOMES", "BOF", "COMMA", "DELETE",
				"ELSE", "EOF", "EQ", "GE", "GT", "ID", "IF", "INT", "LBRACE",
				"LBRACK", "LE", "LPAREN", "LT", "MINUS", "NE", "NEW", "NULL",
				"NUM", "PCT", "PLUS", "PRINTLN", "RBRACE", "RBRACK", "RETURN",
				"RPAREN", "SEMI", "SLASH", "STAR", "WAIN", "WHILE" };

		const string nonTerm[] = { "S", "dcl", "dcls", "expr", "factor",
				"lvalue", "procedure", "statement", "statements", "term",
				"test", "type" };

		// read the number of terminals and move to the next line
		numTerm = 35;
		setNumTerm(numTerm);

		// push the lines containing the terminals to the DFA
		for (i = 0; i < numTerm; i++) {
			dfa.pushToken(terminal[i]);
		}

		// read the number of non-terminals and move to the next line
		numNonTerm = 12;
		setNonTerm(numNonTerm);

		// skip the lines containing the non-terminals
		for (i = 0; i < numNonTerm; i++) {
			dfa.pushToken(nonTerm[i]);
		}
		dfa.complete(); // finish the dfa
return;

		start = nonTerm[0];
		numRules = 38;

		setStart(dfa.getTokenId(nonTerm[0]));
		setNumRules(numRules);

		string
				proRules[] = {
						"S BOF procedure EOF",
						"procedure INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE",
						"type INT",
						"type INT STAR",
						"dcls",
						"dcls dcls dcl BECOMES NUM SEMI",
						"dcls dcls dcl BECOMES NULL SEMI",
						"dcl type ID",
						"statements",
						"statements statements statement",
						"statement lvalue BECOMES expr SEMI",
						"statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE",
						"statement WHILE LPAREN test RPAREN LBRACE statements RBRACE",
						"statement PRINTLN LPAREN expr RPAREN SEMI",
						"statement DELETE LBRACK RBRACK expr SEMI",
						"test expr EQ expr", "test expr NE expr",
						"test expr LT expr", "test expr LE expr",
						"test expr GE expr", "test expr GT expr", "expr term",
						"expr expr PLUS term", "expr expr MINUS term",
						"term factor", "term term STAR factor",
						"term term SLASH factor", "term term PCT factor",
						"factor ID", "factor NUM", "factor NULL",
						"factor LPAREN expr RPAREN", "factor AMP lvalue",
						"factor STAR factor",
						"factor NEW INT LBRACK expr RBRACK", "lvalue ID",
						"lvalue STAR factor", "lvalue LPAREN lvalue RPAREN" };

		// skip the lines containing the production rules
		for (i = 0; i < numRules; i++) {
			addRule(proRules[i]);
		}

		numStates = 107;
		numActions = 710;

		setNumStates(numStates);
		setNumActions(numActions);

		string actions[] = { "6 NE reduce 33", "90 NE reduce 32",
				"46 NUM shift 1", "54 NUM shift 1", "32 NUM shift 1",
				"36 NE reduce 31", "80 NE reduce 34", "80 MINUS reduce 34",
				"30 STAR shift 2", "103 RETURN reduce 8", "59 EQ reduce 36",
				"12 NE reduce 28", "1 NE reduce 29", "11 NE reduce 30",
				"53 EQ reduce 35", "47 NEW shift 3", "48 NEW shift 3",
				"12 MINUS reduce 28", "1 MINUS reduce 29",
				"11 MINUS reduce 30", "6 MINUS reduce 33",
				"90 MINUS reduce 32", "45 EQ reduce 37", "36 MINUS reduce 31",
				"101 RETURN reduce 8", "100 RETURN reduce 8",
				"34 SEMI shift 4", "51 SEMI shift 5", "4 WHILE reduce 6",
				"5 WHILE reduce 5", "85 STAR reduce 4", "67 PRINTLN reduce 8",
				"14 factor shift 6", "6 RBRACK reduce 33",
				"90 RBRACK reduce 32", "23 GT shift 7", "71 RBRACE shift 8",
				"72 RBRACE shift 9", "66 RETURN reduce 9",
				"36 RBRACK reduce 31", "80 RBRACK reduce 34",
				"73 RBRACE shift 10", "12 RBRACK reduce 28",
				"1 RBRACK reduce 29", "11 RBRACK reduce 30", "4 INT reduce 6",
				"5 INT reduce 5", "83 NULL shift 11", "13 NULL shift 11",
				"7 ID shift 12", "16 ID shift 12", "24 ID shift 12",
				"21 ID shift 12", "37 ID shift 12", "17 ID shift 12",
				"81 BECOMES reduce 7", "83 LPAREN shift 13",
				"46 STAR shift 14", "54 STAR shift 14", "32 STAR shift 14",
				"83 factor shift 15", "45 GT reduce 37", "13 factor shift 15",
				"59 GT reduce 36", "13 LPAREN shift 13", "53 GT reduce 35",
				"23 GE shift 16", "53 GE reduce 35", "59 GE reduce 36",
				"85 PRINTLN reduce 4", "45 GE reduce 37", "23 EQ shift 17",
				"68 BECOMES shift 18", "19 RBRACK reduce 21",
				"78 RBRACK reduce 22", "79 RBRACK reduce 23", "49 NUM shift 1",
				"18 NUM shift 1", "2 LPAREN shift 13", "53 PCT reduce 35",
				"27 NUM shift 1", "2 STAR shift 14", "84 term shift 19",
				"59 PCT reduce 36", "45 PCT reduce 37", "56 RPAREN shift 20",
				"23 LT shift 21", "95 PLUS shift 22", "94 PLUS shift 22",
				"93 PLUS shift 22", "92 PLUS shift 22", "91 PLUS shift 22",
				"96 PLUS shift 22", "83 NUM shift 1", "47 expr shift 23",
				"48 expr shift 23", "59 BECOMES reduce 36",
				"53 BECOMES reduce 35", "45 BECOMES reduce 37",
				"84 ID shift 12", "47 NULL shift 11", "48 NULL shift 11",
				"14 LPAREN shift 13", "80 SEMI reduce 34", "23 LE shift 24",
				"89 MINUS shift 25", "36 SEMI reduce 31", "88 MINUS shift 25",
				"13 STAR shift 14", "84 STAR shift 14", "13 NUM shift 1",
				"83 STAR shift 14", "27 term shift 19", "49 term shift 19",
				"18 term shift 19", "25 AMP shift 26", "22 AMP shift 26",
				"67 RETURN reduce 8", "10 DELETE reduce 11",
				"105 RBRACE reduce 10", "7 STAR shift 14", "16 STAR shift 14",
				"24 STAR shift 14", "21 STAR shift 14", "37 STAR shift 14",
				"17 STAR shift 14", "74 RBRACE reduce 13",
				"106 RBRACE reduce 14", "66 WHILE reduce 9",
				"9 DELETE reduce 12", "9 RBRACE reduce 12",
				"104 RBRACK shift 27", "6 SEMI reduce 33", "6 STAR reduce 33",
				"90 SEMI reduce 32", "90 STAR reduce 32",
				"10 RBRACE reduce 11", "12 SEMI reduce 28",
				"12 STAR reduce 28", "1 SEMI reduce 29", "1 STAR reduce 29",
				"11 SEMI reduce 30", "11 STAR reduce 30", "53 LE reduce 35",
				"36 STAR reduce 31", "80 STAR reduce 34", "64 SEMI shift 28",
				"74 PRINTLN reduce 13", "106 PRINTLN reduce 14",
				"105 PRINTLN reduce 10", "40 PLUS reduce 25",
				"41 PLUS reduce 26", "42 PLUS reduce 27",
				"9 PRINTLN reduce 12", "2 NUM shift 1", "15 PLUS reduce 24",
				"10 PRINTLN reduce 11", "27 LPAREN shift 13",
				"88 PLUS shift 22", "7 NULL shift 11", "16 NULL shift 11",
				"24 NULL shift 11", "21 NULL shift 11", "37 NULL shift 11",
				"17 NULL shift 11", "89 PLUS shift 22", "7 NUM shift 1",
				"16 NUM shift 1", "24 NUM shift 1", "21 NUM shift 1",
				"37 NUM shift 1", "17 NUM shift 1", "49 LPAREN shift 13",
				"18 LPAREN shift 13", "101 STAR reduce 8", "100 STAR reduce 8",
				"67 STAR reduce 8", "15 SEMI reduce 24", "103 STAR reduce 8",
				"39 STAR shift 29", "40 SEMI reduce 25", "41 SEMI reduce 26",
				"42 SEMI reduce 27", "80 RPAREN reduce 34",
				"67 WHILE reduce 8", "105 DELETE reduce 10",
				"74 DELETE reduce 13", "106 DELETE reduce 14",
				"12 RPAREN reduce 28", "1 RPAREN reduce 29",
				"11 RPAREN reduce 30", "6 RPAREN reduce 33",
				"90 RPAREN reduce 32", "47 STAR shift 14", "48 STAR shift 14",
				"36 RPAREN reduce 31", "19 LT reduce 21", "45 PLUS reduce 37",
				"97 LPAREN shift 30", "78 LT reduce 22", "79 LT reduce 23",
				"53 PLUS reduce 35", "59 PLUS reduce 36", "15 NE reduce 24",
				"64 PLUS shift 22", "40 NE reduce 25", "41 NE reduce 26",
				"42 NE reduce 27", "28 RBRACE shift 31", "39 ID reduce 2",
				"14 NEW shift 3", "30 LPAREN shift 30", "80 EQ reduce 34",
				"12 GT reduce 28", "1 GT reduce 29", "11 GT reduce 30",
				"36 GT reduce 31", "6 GT reduce 33", "90 GT reduce 32",
				"78 RPAREN reduce 22", "79 RPAREN reduce 23",
				"19 RPAREN reduce 21", "36 GE reduce 31", "80 GE reduce 34",
				"67 ID reduce 8", "10 WHILE reduce 11", "67 IF reduce 8",
				"66 RBRACE reduce 9", "12 GE reduce 28", "1 GE reduce 29",
				"11 GE reduce 30", "6 GE reduce 33", "90 GE reduce 32",
				"78 NE reduce 22", "79 NE reduce 23", "19 NE reduce 21",
				"4 STAR reduce 6", "5 STAR reduce 5", "9 WHILE reduce 12",
				"25 NEW shift 3", "22 NEW shift 3", "74 WHILE reduce 13",
				"106 WHILE reduce 14", "105 WHILE reduce 10",
				"101 IF reduce 8", "100 IF reduce 8", "103 ID reduce 8",
				"73 STAR shift 2", "103 IF reduce 8", "79 PCT shift 32",
				"78 PCT shift 32", "53 RBRACK reduce 35",
				"59 RBRACK reduce 36", "45 RBRACK reduce 37",
				"101 ID reduce 8", "100 ID reduce 8", "3 INT shift 33",
				"71 STAR shift 2", "72 STAR shift 2", "85 LPAREN reduce 4",
				"46 ID shift 12", "54 ID shift 12", "32 ID shift 12",
				"66 LPAREN reduce 9", "80 GT reduce 34", "85 IF reduce 4",
				"43 NULL shift 34", "85 ID reduce 4", "8 ELSE shift 35",
				"66 DELETE reduce 9", "89 RPAREN shift 36", "23 NE shift 37",
				"97 WHILE shift 38", "19 LE reduce 21", "14 AMP shift 26",
				"98 INT shift 39", "70 INT shift 39", "71 LPAREN shift 30",
				"72 LPAREN shift 30", "46 factor shift 40",
				"54 factor shift 41", "32 factor shift 42",
				"81 RPAREN reduce 7", "13 term shift 19", "78 LE reduce 22",
				"79 LE reduce 23", "83 term shift 19", "73 LPAREN shift 30",
				"58 BECOMES shift 43", "80 PCT reduce 34", "12 PCT reduce 28",
				"1 PCT reduce 29", "11 PCT reduce 30", "6 PCT reduce 33",
				"90 PCT reduce 32", "36 PCT reduce 31", "40 MINUS reduce 25",
				"41 MINUS reduce 26", "42 MINUS reduce 27",
				"15 MINUS reduce 24", "99 INT shift 44", "2 AMP shift 26",
				"15 STAR reduce 24", "45 RPAREN reduce 37",
				"7 factor shift 15", "16 factor shift 15",
				"24 factor shift 15", "21 factor shift 15",
				"37 factor shift 15", "17 factor shift 15",
				"9 LPAREN reduce 12", "105 LPAREN reduce 10",
				"74 LPAREN reduce 13", "106 LPAREN reduce 14",
				"53 RPAREN reduce 35", "69 RPAREN shift 45",
				"59 RPAREN reduce 36", "84 NUM shift 1", "10 LPAREN reduce 11",
				"19 STAR shift 46", "79 STAR shift 46", "78 STAR shift 46",
				"47 LPAREN shift 13", "48 LPAREN shift 13",
				"52 LPAREN shift 47", "38 LPAREN shift 48",
				"82 LPAREN shift 49", "19 MINUS reduce 21",
				"99 procedure shift 50", "78 MINUS reduce 22",
				"79 MINUS reduce 23", "36 LT reduce 31", "12 LT reduce 28",
				"1 LT reduce 29", "11 LT reduce 30", "67 INT shift 39",
				"6 LT reduce 33", "90 LT reduce 32", "80 LT reduce 34",
				"78 GE reduce 22", "79 GE reduce 23", "43 NUM shift 51",
				"97 IF shift 52", "19 GE reduce 21", "15 RBRACK reduce 24",
				"36 LE reduce 31", "6 LE reduce 33", "90 LE reduce 32",
				"23 PLUS shift 22", "40 RBRACK reduce 25",
				"41 RBRACK reduce 26", "42 RBRACK reduce 27",
				"12 LE reduce 28", "1 LE reduce 29", "11 LE reduce 30",
				"80 LE reduce 34", "97 ID shift 53", "47 ID shift 12",
				"48 ID shift 12", "78 GT reduce 22", "79 GT reduce 23",
				"19 SLASH shift 54", "19 GT reduce 21", "79 SLASH shift 54",
				"78 SLASH shift 54", "84 NULL shift 11", "13 AMP shift 26",
				"31 EOF reduce 1", "83 AMP shift 26", "40 STAR reduce 25",
				"41 STAR reduce 26", "42 STAR reduce 27", "67 RBRACE reduce 8",
				"15 EQ reduce 24", "40 EQ reduce 25", "41 EQ reduce 26",
				"42 EQ reduce 27", "27 expr shift 55", "49 expr shift 56",
				"18 expr shift 57", "103 RBRACE reduce 8", "67 dcl shift 58",
				"13 ID shift 12", "46 AMP shift 26", "54 AMP shift 26",
				"32 AMP shift 26", "83 ID shift 12", "2 factor shift 59",
				"4 PRINTLN reduce 6", "5 PRINTLN reduce 5", "98 dcl shift 60",
				"97 STAR shift 2", "71 DELETE shift 61", "72 DELETE shift 61",
				"80 SLASH reduce 34", "70 dcl shift 62", "27 ID shift 12",
				"49 ID shift 12", "18 ID shift 12", "67 type shift 63",
				"36 PLUS reduce 31", "6 SLASH reduce 33", "90 SLASH reduce 32",
				"36 SLASH reduce 31", "80 PLUS reduce 34",
				"67 LPAREN reduce 8", "12 SLASH reduce 28",
				"1 SLASH reduce 29", "11 SLASH reduce 30", "12 PLUS reduce 28",
				"1 PLUS reduce 29", "11 PLUS reduce 30", "6 PLUS reduce 33",
				"90 PLUS reduce 32", "15 GT reduce 24", "40 GT reduce 25",
				"41 GT reduce 26", "42 GT reduce 27", "26 LPAREN shift 30",
				"70 type shift 63", "98 type shift 63", "23 MINUS shift 25",
				"15 GE reduce 24", "84 expr shift 64", "40 GE reduce 25",
				"41 GE reduce 26", "42 GE reduce 27", "45 STAR reduce 37",
				"59 STAR reduce 36", "53 STAR reduce 35", "73 DELETE shift 61",
				"103 WHILE reduce 8", "85 DELETE reduce 4",
				"60 RPAREN shift 65", "101 RBRACE reduce 8",
				"100 RBRACE reduce 8", "71 statement shift 66",
				"72 statement shift 66", "49 factor shift 15",
				"18 factor shift 15", "7 AMP shift 26", "16 AMP shift 26",
				"24 AMP shift 26", "21 AMP shift 26", "37 AMP shift 26",
				"17 AMP shift 26", "27 factor shift 15",
				"73 statement shift 66", "101 WHILE reduce 8",
				"100 WHILE reduce 8", "85 dcls shift 67", "97 lvalue shift 68",
				"85 WHILE reduce 4", "84 NEW shift 3", "85 RETURN reduce 4",
				"66 ID reduce 9", "66 IF reduce 9", "2 NEW shift 3",
				"30 lvalue shift 69", "75 LPAREN shift 70",
				"47 factor shift 15", "48 factor shift 15", "13 NEW shift 3",
				"101 statements shift 71", "100 statements shift 72",
				"83 NEW shift 3", "103 statements shift 73", "27 NEW shift 3",
				"49 NEW shift 3", "18 NEW shift 3", "25 ID shift 12",
				"22 ID shift 12", "26 STAR shift 2", "2 ID shift 12",
				"66 STAR reduce 9", "103 PRINTLN reduce 8", "47 AMP shift 26",
				"48 AMP shift 26", "14 ID shift 12", "101 PRINTLN reduce 8",
				"100 PRINTLN reduce 8", "19 PCT shift 32", "4 LPAREN reduce 6",
				"5 LPAREN reduce 5", "71 lvalue shift 68",
				"72 lvalue shift 68", "73 lvalue shift 68", "36 EQ reduce 31",
				"20 SEMI shift 74", "12 EQ reduce 28", "1 EQ reduce 29",
				"11 EQ reduce 30", "49 NULL shift 11", "18 NULL shift 11",
				"6 EQ reduce 33", "90 EQ reduce 32", "27 NULL shift 11",
				"40 LT reduce 25", "41 LT reduce 26", "42 LT reduce 27",
				"15 LT reduce 24", "81 COMMA reduce 7", "10 RETURN reduce 11",
				"15 LE reduce 24", "97 statement shift 66", "44 WAIN shift 75",
				"59 MINUS reduce 36", "53 MINUS reduce 35",
				"15 RPAREN reduce 24", "45 MINUS reduce 37", "40 LE reduce 25",
				"41 LE reduce 26", "42 LE reduce 27", "105 RETURN reduce 10",
				"40 RPAREN reduce 25", "41 RPAREN reduce 26",
				"42 RPAREN reduce 27", "7 term shift 19", "16 term shift 19",
				"24 term shift 19", "21 term shift 19", "37 term shift 19",
				"17 term shift 19", "74 RETURN reduce 13",
				"106 RETURN reduce 14", "7 NEW shift 3", "16 NEW shift 3",
				"24 NEW shift 3", "21 NEW shift 3", "37 NEW shift 3",
				"17 NEW shift 3", "9 RETURN reduce 12", "66 PRINTLN reduce 9",
				"53 LT reduce 35", "59 LT reduce 36", "15 PCT reduce 24",
				"47 test shift 76", "48 test shift 77", "59 LE reduce 36",
				"45 LE reduce 37", "9 STAR reduce 12", "22 term shift 78",
				"25 term shift 79", "40 PCT reduce 25", "41 PCT reduce 26",
				"42 PCT reduce 27", "10 STAR reduce 11", "101 LPAREN reduce 8",
				"100 LPAREN reduce 8", "88 RBRACK shift 80",
				"74 STAR reduce 13", "106 STAR reduce 14",
				"103 LPAREN reduce 8", "45 LT reduce 37", "105 STAR reduce 10",
				"97 DELETE shift 61", "103 DELETE reduce 8", "63 ID shift 81",
				"64 MINUS shift 25", "101 DELETE reduce 8",
				"100 DELETE reduce 8", "47 term shift 19", "48 term shift 19",
				"97 PRINTLN shift 82", "71 ID shift 53", "72 ID shift 53",
				"45 NE reduce 37", "25 NUM shift 1", "22 NUM shift 1",
				"71 IF shift 52", "72 IF shift 52", "59 NE reduce 36",
				"53 NE reduce 35", "73 ID shift 53", "73 IF shift 52",
				"30 ID shift 53", "7 LPAREN shift 13", "16 LPAREN shift 13",
				"24 LPAREN shift 13", "21 LPAREN shift 13",
				"37 LPAREN shift 13", "17 LPAREN shift 13", "25 STAR shift 14",
				"22 STAR shift 14", "57 PLUS shift 22", "56 PLUS shift 22",
				"55 PLUS shift 22", "40 SLASH reduce 25", "41 SLASH reduce 26",
				"42 SLASH reduce 27", "15 SLASH reduce 24",
				"33 LBRACK shift 83", "78 PLUS reduce 22", "79 PLUS reduce 23",
				"97 RETURN shift 84", "55 MINUS shift 25", "57 MINUS shift 25",
				"56 MINUS shift 25", "19 PLUS reduce 21", "67 DELETE reduce 8",
				"46 LPAREN shift 13", "54 LPAREN shift 13",
				"32 LPAREN shift 13", "45 SEMI reduce 37", "59 SEMI reduce 36",
				"53 SEMI reduce 35", "49 AMP shift 26", "18 AMP shift 26",
				"27 AMP shift 26", "95 MINUS shift 25", "94 MINUS shift 25",
				"93 MINUS shift 25", "92 MINUS shift 25", "91 MINUS shift 25",
				"96 MINUS shift 25", "65 LBRACE shift 85", "71 WHILE shift 38",
				"72 WHILE shift 38", "4 DELETE reduce 6", "5 DELETE reduce 5",
				"73 WHILE shift 38", "22 factor shift 15",
				"25 factor shift 15", "47 NUM shift 1", "48 NUM shift 1",
				"76 RPAREN shift 86", "77 RPAREN shift 87", "2 NULL shift 11",
				"26 ID shift 53", "46 NEW shift 3", "54 NEW shift 3",
				"32 NEW shift 3", "19 SEMI reduce 21", "83 expr shift 88",
				"13 expr shift 89", "4 IF reduce 6", "5 IF reduce 5",
				"78 SEMI reduce 22", "79 SEMI reduce 23", "14 STAR shift 14",
				"26 lvalue shift 90", "4 ID reduce 6", "5 ID reduce 5",
				"36 BECOMES reduce 31", "25 LPAREN shift 13",
				"49 STAR shift 14", "18 STAR shift 14", "22 LPAREN shift 13",
				"6 BECOMES reduce 33", "90 BECOMES reduce 32",
				"12 BECOMES reduce 28", "1 BECOMES reduce 29",
				"11 BECOMES reduce 30", "27 STAR shift 14", "29 ID reduce 3",
				"10 IF reduce 11", "10 ID reduce 11", "9 IF reduce 12",
				"105 IF reduce 10", "74 IF reduce 13", "106 IF reduce 14",
				"9 ID reduce 12", "14 NULL shift 11", "80 BECOMES reduce 34",
				"95 RPAREN reduce 16", "94 RPAREN reduce 17",
				"93 RPAREN reduce 18", "92 RPAREN reduce 19",
				"91 RPAREN reduce 20", "96 RPAREN reduce 15",
				"46 NULL shift 11", "54 NULL shift 11", "32 NULL shift 11",
				"14 NUM shift 1", "7 expr shift 91", "16 expr shift 92",
				"24 expr shift 93", "21 expr shift 94", "37 expr shift 95",
				"17 expr shift 96", "4 RETURN reduce 6", "5 RETURN reduce 5",
				"67 statements shift 97", "74 ID reduce 13",
				"106 ID reduce 14", "105 ID reduce 10", "62 COMMA shift 98",
				"0 BOF shift 99", "19 EQ reduce 21", "78 EQ reduce 22",
				"79 EQ reduce 23", "45 SLASH reduce 37", "87 LBRACE shift 100",
				"86 LBRACE shift 101", "50 EOF shift 102",
				"59 SLASH reduce 36", "84 LPAREN shift 13",
				"35 LBRACE shift 103", "53 SLASH reduce 35",
				"61 LBRACK shift 104", "73 PRINTLN shift 82",
				"57 SEMI shift 105", "85 INT reduce 4", "55 SEMI shift 106",
				"84 AMP shift 26", "71 PRINTLN shift 82",
				"72 PRINTLN shift 82", "25 NULL shift 11",
				"84 factor shift 15", "22 NULL shift 11" };

		int state, nextStateOrRule;
		string sym, opType;

		stringstream ss(stringstream::in | stringstream::out);

		for (i = 0; i < numActions; i++) {
			ss << actions[i];
			ss >> state >> sym >> opType >> nextStateOrRule;
			ss.flush();
			ss.clear();
			addAction(state, sym, opType[0], nextStateOrRule);
		}
	}

	void printSymbolTable(ostream &out) {
		list<Pair<string, DataType> >::iterator it;
		for (it = symbolTable.begin(); it != symbolTable.end(); it++)
			out << it->x << " " << (it->y == TYPE_INT ? "int" : "int*") << endl;
	}

	~Grammar() {
		delete parseTree;
	}

	void readParse(istream &in) {
		parseTree = buildTree(in);
		genSymbols(parseTree);
		varDfa.complete();
		contextSensitiveCheck(parseTree);
	}

	DataType type(TreeNode *t) {
		stringstream ss;

		switch (t->id) {
		case T_NUM:
			return TYPE_INT;
		case T_NULL:
			return TYPE_INTSTAR;
		case T_ID:
			list<Pair<string, DataType> >::iterator it;
			for (it = symbolTable.begin(); it != symbolTable.end(); it++) {
				if (it->x == t->lexeme) {
					return it->y;
				}
			}
		}

		list<TreeNode*>::iterator it = t->children.begin();

		int size = t->RHS.size();
		if (size == 1)
			return type(t->children.back());

		advance(it, 1);
		TreeNode *middle = *it;

		if (size == 2) {
			if (t->id == T_factor) {
				if (t->RHS[0] == T_AMP && type(middle) == TYPE_INT)
					return TYPE_INTSTAR;
				if (t->RHS[0] == T_STAR && type(middle) == TYPE_INTSTAR)
					return TYPE_INT;
				ss << "parsing factor with two tokens";
				throw(ss.str());
			} else if (t->id == T_lvalue) {
				if (t->RHS[0] == T_STAR && type(middle) == TYPE_INTSTAR)
					return TYPE_INT;
				ss << "parsing lvalue with two tokens";
				throw(ss.str());
			}
		}

		if (size == 5) { // new int (E);
			advance(it, 2);
			if (type(*it) == TYPE_INT)
				return TYPE_INTSTAR;
			ss << "parsing dynamic allocation; require expr to be INT";
			throw(ss.str());
		}

		// size == 3
		if (t->RHS[0] == T_LPAREN && t->RHS[2] == T_RPAREN) {
			it = t->children.begin();
			advance(it, 1);
			return type(*it);
		}
		it = t->children.begin();
		DataType T1 = type(*it);
		advance(it, 2);
		DataType T2 = type(*it);
		if (t->id == T_expr) {
			if (T1 == T2 && (T1 == TYPE_INT || T1 == TYPE_INTSTAR && middle->id
					== T_MINUS))
				return TYPE_INT;
			if (T1 == TYPE_INTSTAR && T2 == TYPE_INT)
				return TYPE_INTSTAR;
			if (T1 == TYPE_INT && T2 == TYPE_INTSTAR && t->RHS[1] == T_PLUS)
				return TYPE_INTSTAR;
			ss << "parsing expr; Got: " << DataTypeStr[T1] << " "
					<< dfa.symToStr(middle->id) << " " << DataTypeStr[T2];
			throw(ss.str());
		} else if (t->id == T_term) {
			if (T1 == TYPE_INT && T1 == T2)
				return TYPE_INT;
			ss << "parsing term; require int, but found int*; Got: "
					<< DataTypeStr[T1] << " " << dfa.symToStr(middle->id)
					<< " " << DataTypeStr[T2];
			throw(ss.str());
		}
		throw((string) "do some debugging please");
	}

	DataType getDclType(TreeNode *t) {
		list<TreeNode*>::iterator it = t->children.begin();
		int size = (*it)->RHS.size();
		return size == 1 ? TYPE_INT : TYPE_INTSTAR;
	}

	void wellTyped(TreeNode *t) {
//cout << t<<endl;

		list<TreeNode*>::iterator it = t->children.begin();
		int size = t->RHS.size();
if (size > 0)
//cout <<'	'<< *it<<endl;

		if (t->id == T_statement) {
			int typeE1, typeE2;
			if ((*it)->id == T_lvalue) { //assignment
				typeE1 = type(*it);
				advance(it, 2);
				typeE2 = type(*it);
				if (typeE1 != typeE2)
					throw((string) "unmatched type in assignment");
			} else if ((*it)->id == T_IF) { // if else
				advance(it, 2);
				wellTyped(*it);
				advance(it, 3);
				wellTyped(*it);
				advance(it, 4);
				wellTyped(*it);
			} else if ((*it)->id == T_WHILE) { // while
				advance(it, 2);
				wellTyped(*it);
				advance(it, 3);
				wellTyped(*it);
			} else if ((*it)->id == T_PRINTLN) { // println
				advance(it, 2);
				if (type(*it) != TYPE_INT)
					throw((string) "println requires int");
			} else { //delete
				advance(it, 3);
				if (type(*it) != TYPE_INTSTAR)
					throw((string) "delete requires int*");
			}
		} else if (t->id == T_statements) {
			if (size == 0)
				return;
			wellTyped(*it);
			advance(it, 1);
			wellTyped(*it);
		} else if (t->id == T_test) {
			int typeE1 = type(*it), typeE2;
			advance(it, 2);
			typeE2 = type(*it);
			if (typeE1 != typeE2)
				throw((string) "unmatched type in test");
		} else if (t->id == T_dcls) {
			if (size == 0)
				return;
			wellTyped(*it);
			advance(it, 1);
			int type = getDclType(*it);
			advance(it, 2);
			int RHS = (*it)->id;
			if (type == TYPE_INT && RHS == T_NULL || type == TYPE_INTSTAR
					&& RHS == T_NUM)
				throw((string) "right-hand side doesn't match declaration");
		} else if (t->id == T_procedure) {
			advance(it, 5);
			if (getDclType(*it) != TYPE_INT)
				throw((string) "require INT in the second parameter");
			advance(it, 3);
			wellTyped(*it);
			advance(it, 1);
			wellTyped(*it);
			advance(it, 2);
			if (type(*it) != TYPE_INT)
				throw((string) "expected an INT in the return statement");
		}

	}

	void contextSensitiveCheck(TreeNode *t) {
		list<TreeNode*>::iterator it = t->children.begin();
		advance(it, 1); // advance to procedure in BOF procedure EOF
		wellTyped(*it);
	}

	TreeNode *buildTree(istream &in, int nextTarget = -1) {
		if (in.fail())
			throw("Unexpected end of file."); //should not happen
		stringstream ss;
		int id, RHStemp = -1, RHScount = 0;
		string line;
		vector<int> RHS;
		TreeNode * t;

		getline(in, line); // read a line
		// build LHS
		ss << line;
		ss >> buf;
		id = dfa.getTokenId(buf);

		if (id == T_EOF || id == T_BOF) {
			return new TreeNode(id, buf);
		} else if (ss.peek() == EOF) {
			buf = "";
			return new TreeNode(id, buf);
		} else if (isTerminal[id - SS_SIZE]) {
			ss >> buf;
			return new TreeNode(id, buf);
		}

		// build RHS
		for (; ss.peek() != EOF; RHScount++) {
			ss >> buf;
			RHStemp = dfa.getTokenId(buf);
			if (RHStemp == -1) // fact: only two tokens in the parse text
				return new TreeNode(id, buf);
			RHS.push_back(RHStemp);
		}
		t = new TreeNode(id, RHS);
		for (int i = 0; i < RHScount; i++)
			t->children.push_back(buildTree(in, RHS[i]));
		return t;
	}

	int getVar(TreeNode *t){
/*
string s = "v2";
varDfa.printAllTokens();
string td = "c";
string r = "d";
string v = "v11";
string p = "v1";
cout << t<<" "<<varDfa.getTokenIdNormalized(td)<< " " << varDfa.getTokenIdNormalized(r)<< " " << varDfa.getTokenIdNormalized(p)<< " " << varDfa.getTokenIdNormalized(v)<< " " << varDfa.getTokenIdNormalized(s)<<endl;
*/
		return (varDfa.getTokenIdNormalized(t->lexeme) + 1) << 2;
	}

	int getLValue(TreeNode *t){
		int size = t->children.size();
		list<TreeNode*>::iterator it = t->children.begin();
		if(size == 1){ // ID
			return getVar(*it);
		} else if (size == 2){ // *factor
			return 0;
		} else if (size == 3){ // (lvalue)
			advance(it, 1);
			return getLValue(*it);
		}
	}

	void pushToStack(){
		cout << "sw $3, -4($30)"<<endl;
		cout << "sub $30, $30, $4"<<endl;
	}

	void getFromStack(){
			cout << "add $30, $30, $4"<<endl;
			cout << "lw $8, -4($30)"<<endl;
	}

	int getLabelId(){
		labelCounter++;
		return labelCounter;
	}

	void genTerm(TreeNode *t){
		int size = t->RHS.size();
		list<TreeNode*>::iterator it = t->children.begin();
		switch(size){
			case 1: 
				if (t->RHS[0] == T_factor)
					genFactor(*it);
				break;
			case 2:
				break;
			case 3:
				genTerm(*it);
				pushToStack();
				advance(it, 2);
				genFactor(*it);
				getFromStack();
				if (t->RHS[1] == T_STAR){
					cout << "mult $8, $3"<<endl;
					cout << "mflo $3" << endl;
					return;
				} else{
						cout << "div $8, $3"<<endl;
					if (t->RHS[1] == T_SLASH){
						cout << "mflo $3" << endl;
					} else if (t->RHS[1] == T_PCT){
						cout << "mfhi $3" << endl;
					}
				}
				break;
			case 5:
				break;
		}
	}

	void genFactor(TreeNode *t){
		list<TreeNode*>::iterator it = t->children.begin();
		int size = t->RHS.size();
		if (size == 1){ // ID or NUM
			if (t->RHS[0] == T_ID){
				cout << "lw $3, -" << getVar(*it)<<"($29)"<<endl;
			} else if (t->RHS[0] == T_NUM){
				cout << "lis $3" << endl << "	.word "<<(*it)->lexeme<<endl;
			}
		} else if (size == 3){ // LPAREN expr RPAREN
			advance(it,1);
			genExpr(*it);
		} else if (t->RHS[0] == T_NULL){ // NULL
			cout << "add $3, $0, $0"<<endl;			
		} else if (t->RHS[0] == T_AMP){ // &x
			advance(it,1);
			list<TreeNode*>::iterator itnext = (*it)->children.begin(); // id
//			cout << "lw $3, -"  << getVar(*itnext)<<"($29)"<<endl;
			cout << "lis $3"<< endl<<"	.word " << getVar(*itnext) << endl;
			cout << "sub $3, $29, $3"<<endl;
		} else if (t->RHS[0] == T_STAR){ // *x
			advance(it,1);
			list<TreeNode*>::iterator itnext = (*it)->children.begin(); // id
			cout << "lw $3, -"  << getVar(*itnext)<<"($29)"<<endl;
			cout << "lw $3, 0($3)"<<endl;
		} 
	}

	void genExpr(TreeNode *t){
		list<TreeNode*>::iterator it = t->children.begin();
		if (t->RHS.size() == 1){ // term
			genTerm(*it);
			return;
		}
			// expr PLUS or MINUS term
			genExpr(*it);
			cout << "sw $3, -4($30) ; expr"<<endl;
			cout << "sub $30, $30, $4"<<endl;
			advance(it, 2);
			genTerm(*it);
			cout << "add $30, $30, $4"<<endl;
			cout << "lw $8, -4($30)"<<endl;

		cout << (t->RHS[1] == T_PLUS ? "add" : "sub") << " $3, $8, $3"<<endl<<endl;
	}

	void genDcls(TreeNode*t){
		stringstream ss;
		int value, offset;
		if (t->RHS.size() == 0) return;
		list<TreeNode*>::iterator it = t->children.begin();
		genDcls(*it);
		advance(it,1);
		list<TreeNode*>::iterator nextit = (*it)->children.begin();
		advance(nextit,1);
		offset = getVar(*nextit);
//cout << offset<<" |||||||||||"<<endl;
		advance(it,2);
//		if((*it)->id == T_NUM){ //NUM
			ss << (*it)->lexeme;
			ss >> value;
			if (value == 0 || (*it)->id != T_NUM){
				cout << "sw $0, -" << offset << "($30)"<<endl;
			} else if (value == 1){
				cout << "sw $11, -" << offset << "($30)"<<endl;
			} else if (value == 4){
				cout << "sw $4, -" << offset << "($30)"<<endl;
			} else {
				cout << "lis $8" << endl<<".word " << (*it)->lexeme << endl;
				cout << "sw $8, -" << offset << "($30)"<<endl;
			}
	}

	void genTest(TreeNode *t){
		list<TreeNode*>::iterator it = t->children.begin();
//cout<<T_LT<< " "<<dfa.symToStr(t->RHS[1]) <<endl;
			genExpr(*it);
			pushToStack();
			advance(it,2);
			genExpr(*it);
			getFromStack();
		if (t->RHS[1] == T_LT){
			cout << "slt $3, $8, $3" << endl;
		} else if (t->RHS[1] == T_EQ){
			cout << "sub $3, $3, $8" << endl;
			cout << "sltu $3, $0, $3"<<endl;
			cout << "sub $3, $11, $3" << endl;
		} else if (t->RHS[1] == T_LE){
			cout << "slt $3, $3, $8" << endl;
			cout << "sub $3, $11, $3" << endl;
		} else if (t->RHS[1] == T_GE){
			cout << "slt $3, $8, $3" << endl;
			cout << "sub $3, $11, $3" << endl;
		} else if (t->RHS[1] == T_GT){
			cout << "slt $3, $3, $8" << endl;
		} else if ( t->RHS[1] == T_NE){
			cout << "sub $3, $3, $8" << endl;
			cout << "sltu $3, $0, $3"<<endl;
		}
	}

	void genStatements(TreeNode *t){
		list<TreeNode*>::iterator it = t->children.begin();
		int size = t->RHS.size();
		if (t->id == T_statements){
			switch(size){
				case 0: break;
				case 2: // statements statement
					genStatements(*it);
					advance(it,1);
					genStatements(*it);
					break;
			}
		} else { // statement
//cout << t<< "(((((((((((((("<<endl;
			if (t->RHS[0] == T_PRINTLN){
				advance(it,2);
				genExpr(*it);
				cout << "add $1, $3, $0"<< endl;
				cout << "sw $31, -4($30)"<< endl;
				cout << "sub $30, $30, $4"<<endl;
				cout << "lis $8"<<endl <<"	.word print"<<endl;
				cout << "jalr $8"<<endl;
				cout << "add $30, $30, $4" <<endl;
				cout << "lw $31, -4($30)"<<endl<<endl;
				includedPrintln = 1;
//				varValue.push_back(0);
			} else if (t->RHS[0] == T_lvalue){ //assignment
//				list<TreeNode*>::iterator itnext = (*it)->children.begin();
/*
				if ((*itnext)->children.size() == 2){ // *factor
					list<TreeNode*>::iterator itnextnext = (*itnext)->children.begin();
					advance(itnextnext, 1);
					cout << "sw $3, -" << getVar(*((*itnextnext)->children.begin())) << "something"<<endl;
				} else {
				}
*/
					cout << "HIHI                        2222222222222222"<<endl;
					int lvalue = getLValue(*it);
					advance(it, 2);
					genExpr(*it);
					cout << "sw $3, -" << lvalue<<"($29)"<< endl<<endl;
			} else if (t->RHS[0] == T_WHILE){
				int cur = getLabelId();
				cout << "a1while"<< cur << ':'<<endl;
				advance(it, 2);
				genTest(*it);
				cout << "beq $3, $0, a1whileEnd"<< cur<<endl;
				advance(it,3);
				genStatements(*it);
				cout << "beq $0, $0, a1while"<< cur << endl;
				cout << "a1whileEnd"<<cur<<':'<<endl;
			} else if (t->RHS[0] == T_IF){
				int cur = getLabelId();
				advance(it, 2);
				genTest(*it);
				cout << "beq $3, $0, a1ifFalse"<< cur<<endl;
				advance(it,3);
				genStatements(*it);
				cout << "beq $0, $0, a1endIf"<<cur<<endl;
				cout << "a1ifFalse"<<cur<<':' <<endl;
				advance(it,4);
				genStatements(*it);
				cout <<"a1endIf"<<cur<<':'<<endl;
			}
		}
	}

	// Generate the code for the parse tree t.
	void genCode(TreeNode *t) {
		int offset;
		list<TreeNode*>::iterator it = t->children.begin(), itnext;
		if(t->id == T_procedure){
			cout <<";;;;;;;;;;;; assign args"<<endl;
			advance(it,3);
			itnext = (*it)->children.begin();
			advance(itnext,1);
			offset = getVar(*itnext);
			cout << "sw $1, -"<<offset<<"($30)"<<endl;
			advance(it,2);
			itnext = (*it)->children.begin();
			advance(itnext,1);
			offset = getVar(*itnext);
			cout << "sw $2, -"<<offset<<"($30)"<<endl;
			cout <<";;;;;;;;;;;; end of assign args"<<endl<<endl;

			cout <<";;;;;;;;;;;; assign var"<<endl;
			advance(it, 3);
			genDcls(*it);
			cout << "lis $20" << endl << "	.word " << (symbolTable.size()<<2) <<endl;
			cout << "add $29, $30, $0"<<endl;
			cout << "sub $30, $30, $20"<<endl;
			cout <<";;;;;;;;;;;; end of assign var"<<endl<<endl;

			cout <<";;;;;;;;;;;; statements"<<endl;
			advance(it, 1);
			genStatements(*it);
			cout <<";;;;;;;;;;;; end of statements"<<endl<<endl;

			cout <<";;;;;;;;;;;; return"<<endl;
			advance(it, 2);
			genExpr(*it);
			cout <<"jr $31"<<endl;

			if(includedPrintln){
				cout <<endl<<";;;;;;;;;;;; Library functions"<<endl;
				cout << PRINTLN[0] <<endl;
				for(int i = 1;i<SIZE_PRINTLN;i++)
					cout << '	'<<PRINTLN[i]<<endl;
			}
		} else if(t->id == T_S){
			cout <<";;;;;;;;;;;; prologue"<<endl;
			cout <<"lis $4" << endl << "	.word 4"<<endl; // set 4 into $4
			cout <<"slt $11, $0, $4"<<endl; // set 1 into $11
			cout <<";;;;;;;;;;;; end of prologue"<<endl<<endl;
			advance(it, 1);
			genCode(*it);
		}
	}

	void genCode(){
		int size = symbolTable.size();
//		varValue.reserve(size);
//		for(int i = 0; i < size; i++) varValue.push_back(0);
		genCode(parseTree);
	}

	Rule *getRule(int i) {
		return &rules[i];
	}
} G;

ostream &operator<<(ostream &out, Grammar::Rule &r) {
	int i, size = r.sizeRHS();
	out << dfa.symToStr(r.sym);
	if (size == 0)
		return out;
	out << " ";
	for (i = 0;;) {
		out << dfa.symToStr(r.RHS[i]);
		i++;
		if (i >= size)
			break;
		out << " ";
	}
	return out;
}

ostream &operator<<(ostream &out, Grammar::TreeNode *t) {
	if (t == NULL)
		return out;
	int size = t->RHS.size();
	out << dfa.symToStr(t->id) << " ";
	if (size == 0) {
		out << t->lexeme;
	} else {
		for (int i = 0; i < size; i++) {
			out << dfa.symToStr(t->RHS[i]) << " ";
		}
	}
	return out;
}

int main(int argc, char** argv) {
	G.completeWLPPGrammar();
	try {
		G.readParse(cin);
	} catch (int e) {
		cerr << "ERROR at " << e << endl;
	} catch (string s) {
		cerr << "ERROR " << s << endl;
	} catch (char *s) {
		cerr << "ERROR " << s << endl;
	}
//varDfa.printAllTokens();
	G.genCode();

	if (argc >= 2) {
		G.printSymbolTable(cerr);
		G.printParseTree();
	}
	return 0;
}

