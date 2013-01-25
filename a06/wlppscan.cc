/*  CS241 Scanner

	Starter code for the CS 241 assembler (assignments 3 and 4).
	Code contained here may be included in submissions for CS241
	assignments at the University of Waterloo.

	---------------------------------------------------------------

	To compile on a CSCF linux machine, use:

			g++ -g asm.cc -o asm

	To run:
			./asm		   < source.asm > program.mips
			valgrind ./asm  < source.asm > program.mips
 */

#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
using namespace std;

//======================================================================
//========= Declarations for the scan() function =======================
//======================================================================

// Each token has one of the following kinds.

enum Kind {
	ID,				 // Opcode or identifier (use of a label)
	NUM,				// Decimal integer
	COMMA,			  // Comma
	LPAREN,			 // (
	RPAREN,			 // )
	WHITESPACE,		 // Whitespace
	LBRACE,
	RBRACE,
	BECOMES,
	EQ,
	NE,
	LT,
	GT,
	LE,
	GE,
	PLUS,
	MINUS,
	STAR,
	SLASH,
	PCT,
	SEMI,
	LBRACK,
	RBRACK,
	AMP,
	COMMENT,
	WAIN,
	INT,
	IF,
	ELSE,
	WHILE,
	PRINTLN,
	RETURN,
	NULLSTRING,
	NEW,
	DELETE,
	NUL
};

// kindString(k) returns string a representation of kind k
// that is useful for error and debugging messages.
string kindString(Kind k);

// Each token is described by its kind and its lexeme.

struct Token {
	Kind	  kind;
	string	lexeme;
	/* toInt() returns an integer representation of the token. For tokens
	 * of kind INT (decimal integer constant) and HEXINT (hexadecimal integer
	 * constant), returns the integer constant. For tokens of kind
	 * REGISTER, returns the register number.
	 */
	int	   toInt();
};

// scan() separates an input line into a vector of Tokens.
vector<Token> scan(string input);

// =====================================================================
// The implementation of scan() and associated type definitions.
// If you just want to use the scanner, skip to the next ==== separator.

// States for the finite-state automaton that comprises the scanner.

enum State {
	ST_NUL,
	ST_START,
	ST_NE0,
	ST_ID,
	ST_NUM,
	ST_LPAREN,
	ST_RPAREN,
	ST_LBRACE,
	ST_RBRACE,
	ST_BECOMES,
	ST_EQ,
	ST_NE,
	ST_LT,
	ST_GT,
	ST_LE,
	ST_ZERO,
	ST_GE,
	ST_PLUS,
	ST_MINUS,
	ST_STAR,
	ST_SLASH,
	ST_PCT,
	ST_COMMA,
	ST_SEMI,
	ST_LBRACK,
	ST_RBRACK,
	ST_AMP,
	ST_COMMENT0,
	ST_COMMENT,
	ST_w,
	ST_wa,
	ST_wai,
	ST_wain,
	ST_i,
	ST_in,
	ST_int,
	ST_if,
	ST_e,
	ST_el,
	ST_els,
	ST_else,
	ST_wh,
	ST_whi,
	ST_whil,
	ST_while,
	ST_p,
	ST_pr,
	ST_pri,
	ST_prin,
	ST_print,
	ST_printl,
	ST_println,
	ST_r,
	ST_re,
	ST_ret,
	ST_retu,
	ST_retur,
	ST_return,
	ST_N,
	ST_NU,
	ST_NULS,
	ST_NULL,
	ST_n,
	ST_ne,
	ST_new,
	ST_d,
	ST_de,
	ST_del,
	ST_dele,
	ST_delet,
	ST_delete,
	ST_ERROR,
	ST_WHITESPACE
};

// The *kind* of token (see previous enum declaration)
// represented by each state; states that don't represent
// a token have stateKinds == NUL.

Kind stateKinds[] ={
    NUL, //ST_NUL,
    NUL, //ST_START,
    NUL, //ST_NE0,
    ID, //ST_ID,
    NUM, //ST_INT,
    LPAREN, //ST_LPAREN,
    RPAREN, //ST_RPAREN,
    LBRACE, //ST_LBRACE,
    RBRACE, //ST_RBRACE,
    BECOMES, //ST_BECOMES,
    EQ, //ST_EQ,
    NE, //ST_NE,
    LT, //ST_LT,
    GT, //ST_GT,
    LE, //ST_LE,
    NUM, //ST_ZERO,
    GE, //ST_GE,
    PLUS, //ST_PLUS,
    MINUS, //ST_MINUS,
    STAR, //ST_STAR,
    SLASH, //ST_SLASH,
    PCT, //ST_PCT,
    COMMA, //ST_COMMA,
    SEMI, //ST_SEMI,
    LBRACK, //ST_LBRACK,
    RBRACK, //ST_RBRACK,
    AMP, //ST_AMP,
    NUL, //ST_COMMENT0,
    COMMENT, //COMMENT
	ID,
	ID,
	ID,
	WAIN,
	ID,
	ID,
	INT,
	IF,
	ID,
	ID,
	ID,
	ELSE,
	ID,
	ID,
	ID,
	WHILE,
	ID,
	ID,
	ID,
	ID,
	ID,
	ID,
	PRINTLN,
	ID,
	ID,
	ID,
	ID,
	ID,
	RETURN,
	ID,
	ID,
	ID,
	NULLSTRING,
	ID,
	ID,
	NEW,
	ID,
	ID,
	ID,
	ID,
	ID,
	DELETE,
	NUL,
    WHITESPACE //ST_WHITESPACE
};

State delta[ST_WHITESPACE+1][256];

#define whitespace "\t\n\r "
#define letters	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define digits	 "0123456789"
#define hexDigits  "0123456789ABCDEFabcdef"
#define oneToNine  "123456789"

inline void setT(State from, string chars, State to) {
	for(int i = 0; i < chars.length(); i++ ) delta[from][chars[i]] = to;
}

inline void setID(State from){
	setT( from, letters digits, ST_ID);
}

void initT(){
	int i, j;

	// The default transition is ST_NUL (i.e., no transition
	// defined for this char).
	for ( i=0; i<=ST_WHITESPACE; i++ ) {
		for ( j=0; j<256; j++ ) {
			delta[i][j] = ST_NUL;
		}
	}

	// Non-null transitions of the finite state machine.
	// NB: in the third line below, letters digits are macros
	// that are replaced by string literals, which the compiler
	// will concatenate into a single string literal.
	setT( ST_START,	  whitespace,	 ST_WHITESPACE );
	setT( ST_WHITESPACE, whitespace,	 ST_WHITESPACE );
	setT( ST_START,	  letters,		ST_ID		 );
	setT( ST_ID,		 letters digits, ST_ID		 );
	setT( ST_START,	  oneToNine,	  ST_NUM		);
	setT( ST_NUM,		digits,		 ST_NUM		);
	setT( ST_START,	  "-",			ST_MINUS	  );
	setT( ST_MINUS,	  oneToNine,	  ST_NUM		);
	setT( ST_START,	  ",",			ST_COMMA	  );
	setT( ST_START,	  "(",			ST_LPAREN	 );
	setT( ST_START,	  ")",			ST_RPAREN	 );
	setT( ST_SLASH,   "/",			ST_COMMENT	);
	setT( ST_START,	  "=",			ST_BECOMES	);
	setT( ST_BECOMES,	"=",			ST_EQ		 );
	setT( ST_START, "0", ST_ZERO);
	setT( ST_ZERO, digits letters, ST_ERROR);
	setT( ST_START, "{", ST_LBRACE);
	setT( ST_START, "}", ST_RBRACE);
	setT( ST_START, "<", ST_LT);
	setT( ST_START, ">", ST_GT);
	setT( ST_LT, "=", ST_LE);
	setT( ST_GT, "=", ST_GE);
	setT( ST_START, "+", ST_PLUS);
	setT( ST_START, "-", ST_MINUS);
	setT( ST_START, "*", ST_STAR);
	setT( ST_START, "/", ST_SLASH);
	setT( ST_START, "%", ST_PCT);
	setT( ST_START, ";", ST_SEMI);
	setT( ST_START, "[", ST_LBRACK);
	setT( ST_START, "]", ST_RBRACK);
	setT( ST_START, "&", ST_AMP);
	setT( ST_START, "!", ST_NE0);
	setT( ST_NE0, "=", ST_NE);

#if 0
	setID( ST_w);
	setID( ST_wa);
	setID( ST_wai);
	setID( ST_wain);
	setID( ST_i);
	setID( ST_in);
	setID( ST_int);
	setID( ST_if);
	setID( ST_e);
	setID( ST_el);
	setID( ST_els);
	setID( ST_else);
	setID( ST_wh);
	setID( ST_whi);
	setID( ST_whil);
	setID( ST_while);
#endif

	for (int i = ST_w; i <= ST_delete; i++) setID((State)i);
	setT( ST_START, "w", ST_w);
	setT( ST_w, "a", ST_wa);
	setT( ST_wa, "i", ST_wai);
	setT( ST_wai, "n", ST_wain);
	setT( ST_START, "i", ST_i);
	setT( ST_i, "n", ST_in);
	setT( ST_in, "t", ST_int);
	setT( ST_i, "f", ST_if);
	setT( ST_START, "e" , ST_e);
	setT( ST_e, "l", ST_el);
	setT( ST_el, "s", ST_els);
	setT( ST_els, "e", ST_else);
	setT( ST_w, "h", ST_wh);
	setT( ST_wh, "i", ST_whi);
	setT( ST_whi, "l", ST_whil);
	setT( ST_whil, "e", ST_while);
	setT( ST_START, "p", ST_p);
	setT( ST_p, "r", ST_pr);
	setT( ST_pr, "i", ST_pri);
	setT( ST_pri, "n", ST_prin);
	setT( ST_prin, "t", ST_print);
	setT( ST_print, "l", ST_printl);
	setT( ST_printl, "n", ST_println);
	setT( ST_START, "r", ST_r);
	setT( ST_r, "e", ST_re);
	setT( ST_re, "t", ST_ret);
	setT( ST_ret, "u", ST_retu);
	setT( ST_retu, "r", ST_retur);
	setT( ST_retur, "n", ST_return);
	setT( ST_START, "N", ST_N);
	setT( ST_N, "U", ST_NU);
	setT( ST_NU, "L", ST_NULS);
	setT( ST_NULS, "L", ST_NULL);
	setT( ST_START, "n", ST_n);
	setT( ST_n, "e", ST_ne);
	setT( ST_ne, "w", ST_new);
	setT( ST_START, "d", ST_d);
	setT( ST_d, "e", ST_de);
	setT( ST_de, "l", ST_del);
	setT( ST_del, "e", ST_dele);
	setT( ST_dele, "t", ST_delet);
	setT( ST_delet, "e", ST_delete);
	for ( j=0; j<256; j++ ) delta[ST_COMMENT][j] = ST_COMMENT;
}

static int initT_done = 0;

vector<Token> scan(string input){
	// Initialize the transition table when called for the first time.
	if(!initT_done) {
		initT();
		initT_done = 1;
	}

	vector<Token> ret;

	int i = 0;
	int startIndex = 0;
	State state = ST_START;

	if(input.length() > 0) {
		while(true) {
			State nextState = ST_NUL;
//cout << "input: "<<input[i]  << " "<< state <<endl;
			if(i < input.length())
				nextState = delta[state][(unsigned char) input[i]];
			if (stateKinds[state] == COMMENT){
				return ret;
			} else if (stateKinds[state] == MINUS){
				int size = ret.size()-1;
				if (size >= 0){
					Kind skind = ret[size].kind;
					if (skind == ID || skind == NUM){
						goto insertToRet;
					}
				}
			}

			if(nextState == ST_NUL) {
				// no more transitions possible
				if(stateKinds[state] == NUL) {
					throw("ERROR in lexing after reading " + input.substr(0, i));
				}

				// skip the parts after the comment //
				if(stateKinds[state] != WHITESPACE) {
insertToRet:
					Token token;
					token.kind = stateKinds[state];
					token.lexeme = input.substr(startIndex, i-startIndex);
					ret.push_back(token);
				}
				startIndex = i;
				state = ST_START;
				if(i >= input.length()) break;
			} else {
				state = nextState;
				i++;
			}
		}
	}

	return ret;
}

int Token::toInt() {
	if(kind == INT) {
		if(lexeme.length() > 11) throw("ERROR: constant out of range: "+lexeme);
		long long l;
		sscanf( lexeme.c_str(), "%lld", &l );
		if(l < -2147483647-1 || l > 2147483647)
			throw("ERROR: constant out of range: "+lexeme);
		return l;
	}
	throw("ERROR: attempt to convert non-integer token "+lexeme+" to Int");
}

// kindString maps each kind to a string for use in error messages.

string kS[] = {
    "ID",              // Opcode or identifier (use of a label)
    "NUM",                // Decimal integer
    "COMMA",            // Comma
    "LPAREN",          // (
    "RPAREN",          // )
    "WHITESPACE",      // Whitespace
    "LBRACE",
    "RBRACE",
    "BECOMES",
    "EQ",
    "NE",
    "LT",
    "GT",
    "LE",
    "GE",
    "PLUS",
    "MINUS",
    "STAR",
    "SLASH",
    "PCT",
    "SEMI",
    "LBRACK",
    "RBRACK",
    "AMP", //AMP 
	"COMMENT",
	"WAIN",
	"INT",
	"IF",
	"ELSE",
	"WHILE",
	"PRINTLN",
	"RETURN",
	"NULL",
	"NEW",
	"DELETE",
    "NUL", //NUL	
};

string kindString( Kind k ){
	if ( k < ID || k > NUL ) return "INVALID";
	return kS[k];
}

//======================================================================
//======= A sample program demonstrating the use of the scanner. =======
//======================================================================

int main() {
#if 1
	try {
		vector<string> srcLines;

		// Read the entire input file, storing each line as a
		// single string in the array srcLines.
		while(true) {
			string line;
			getline(cin, line);
			if(cin.fail()) break;
			srcLines.push_back(line);
		}

		// Tokenize each line, storing the results in tokLines.
		vector<vector<Token> > tokLines;

		for(int line = 0; line < srcLines.size(); line++) {
			tokLines.push_back(scan(srcLines[line]));
		}

		// Now we process the tokens.
		// Sample usage: print the tokens of each line.
		for(int line=0; line < tokLines.size(); line++ ) {
			for(int j=0; j < tokLines[line].size(); j++ ) {
				Token token = tokLines[line][j];
				cout << kindString(token.kind) 
					<< " "<<  token.lexeme << endl;
			}
		}
	} catch(string msg) {
		cerr << msg << endl;
	}

#endif
	return 0;

}
