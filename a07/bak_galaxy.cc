#include <stdio.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
using namespace std;

enum State{
        // complete symbols
        ST_BOF, ST_EOF, ST_id, ST_MINUS, ST_LPAREN, ST_RPAREN, ST_S, ST_expr, ST_term,
	
	// whitespace and counter of number of complete symbols
	ST_WHITESPACE,

	// intermediate states
	ST_START,
	ST_B, ST_BO,
	ST_E, ST_EO,
	ST_i,
	ST_e, ST_ex, ST_exp,
	ST_t, ST_te, ST_ter,
	
	NUM_STATE,

	ST_NUL
};

enum Symbol{
	BOF = ST_BOF, 
	EOf = ST_EOF,
	ID = ST_id,
	MINUS = ST_MINUS,
	LPAREN = ST_LPAREN,
	RPAREN = ST_RPAREN,
	S = ST_S,
	EXPR = ST_expr,
	TERM = ST_term,
	NUM_SYMBOL = ST_WHITESPACE,
};

bool hasSetup = 0;

bool isTerminal[NUM_SYMBOL] = {
	1,1,1,1,1,1,0,0,0
};

State delta[NUM_STATE][256];

inline void setState(State s0, int c, State s1){
	delta[s0][c] = s1;
}

void setupT(){
	if (hasSetup) return;
	hasSetup = 1;

	for (int i = 0; i < NUM_STATE; i++)
		for (int j = 0; j < 256; j++)
			delta[i][j] = ST_NUL;

	setState(ST_START, 'B', ST_B);
	setState(ST_B, 'O', ST_BO);
	setState(ST_BO, 'F', ST_BOF);
	setState(ST_START, 'E', ST_E);
	setState(ST_E, 'O', ST_EO);
	setState(ST_EO, 'F', ST_EOF);
	setState(ST_START, 'i', ST_i);
	setState(ST_i, 'd', ST_id);
	setState(ST_START, '-', ST_MINUS);
	setState(ST_START, '(', ST_LPAREN);
	setState(ST_START, ')', ST_RPAREN);
	setState(ST_START, 'S', ST_S);
	setState(ST_START, 'e', ST_e);
	setState(ST_e, 'x', ST_ex);
	setState(ST_ex, 'p', ST_exp);
	setState(ST_exp, 'r', ST_expr);
	setState(ST_START, 't', ST_t);
	setState(ST_t, 'e', ST_te);
	setState(ST_te, 'r', ST_ter);
	setState(ST_ter, 'm', ST_term);
	setState(ST_START, ' ', ST_WHITESPACE);	

	for (int i = 0; i < 256; i++){
		setState(ST_WHITESPACE, ' ', ST_WHITESPACE);
	}
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

class parseTreeNode{
public:
	Symbol s;
	vector <parseTreeNode> *c;
	parseTreeNode(Symbol s, vector <parseTreeNode> *c = NULL){
		this->s = s;
		this->c = c;
	}
	~parseTreeNode(){
		if (c != NULL) c->clear();
		delete c;
	}
};

void skipGrammar(FILE* in) {
  int i, numTerm, numNonTerm, numRules;

  // read the number of terminals and move to the next line
  fscanf(in, "%d", &numTerm);
  skipLine(in);
  
  // skip the lines containing the terminals
  for (i = 0; i < numTerm; i++) {
    skipLine(in);
  }
  
  // read the number of non-terminals and move to the next line
  fscanf(in, "%d", &numNonTerm);
  skipLine(in);
  
  // skip the lines containing the non-terminals
  for (i = 0; i < numNonTerm; i++) {
    skipLine(in);
  }
  
  // skip the line containing the start symbol
  skipLine(in);
  
  // read the number of rules and move to the next line
  fscanf(in, "%d", &numRules);
  skipLine(in);
  
  // skip the lines containing the production rules
  for (i = 0; i < numRules; i++) {
    skipLine(in);
  }
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
int strAt = 0;
parseTreeNode * createNode(char *str, int len, parseTreeNode *node){
	for (State cur = ST_START, next;;){
		next = delta[cur][str[strAt]];
		if (next == ST_NUL){
			if (cur < ST_WHITESPACE){
				if (node == NULL) node = new parseTreeNode((Symbol) cur);
			}
			cur = ST_START;
		} else {
			cur = next;
		}
		strAt++;
		if (strAt == len + 1) break;
	}
	return NULL;
}

/**
 * Prints the derivation with whitespace trimmed.
 *
 * @param in the file pointer for reading input
 */
parseTreeNode * createParseTree(FILE *in, parseTreeNode *t) {
  char line[256];
  int lineSize;
	setupT();

	parseTreeNode **pt = &t;

  while (1) {
    fgets(line, 256, in);
    if (feof(in)) break;
	
	char* cur = trim(line);
	int i = 0, len = strlen(cur);
	for (State stateCur = ST_START, stateNext;;){
		stateNext = delta[stateCur][cur[i]];
		if (stateNext == ST_NUL){
			if (stateCur < ST_WHITESPACE){
				if (t == NULL) t = new parseTreeNode((Symbol) stateCur);
			//	if ((*pt)->c == NULL) (*pt)->c = new vector <parseTreeNode>();
			} 
			stateCur = ST_START;
		} else {
			stateCur = stateNext;
		}
		i++;
		if (i == len + 1) {
			strAt = 0;
			break;
		}
	}
  }
	return t;

}

/**
 * Reads a .cfg file and prints the left-canonical
 * derivation without leading or trailing spaces.
 */
int main(int argc, char** argv) {
	setupT();
  skipGrammar(stdin);
  parseTreeNode *t = createParseTree(stdin, t);
	cout << (t == NULL) << endl;
	delete t;
  return 0;
}
