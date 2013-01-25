/*  CS241 Scanner

    Starter code for the CS 241 assembler (assignments 3 and 4).
    Code contained here may be included in submissions for CS241
    assignments at the University of Waterloo.

    ---------------------------------------------------------------

    To compile on a CSCF linux machine, use:

            g++ -g asm.cc -o asm

    To run:
            ./asm           < source.asm > program.mips
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
    ID,                 // Opcode or identifier (use of a label)
    INT,                // Decimal integer
    HEXINT,             // Hexadecimal integer
    REGISTER,           // Register number
    COMMA,              // Comma
    LPAREN,             // (
    RPAREN,             // )
    LABEL,              // Declaration of a label (with a colon)
    DOTWORD,            // .word directive
    WHITESPACE,         // Whitespace
    NUL                 // Bad/invalid token
};

// kindString(k) returns string a representation of kind k
// that is useful for error and debugging messages.
string kindString(Kind k);

// Each token is described by its kind and its lexeme.

struct Token {
    Kind      kind;
    string    lexeme;
    /* toInt() returns an integer representation of the token. For tokens
     * of kind INT (decimal integer constant) and HEXINT (hexadecimal integer
     * constant), returns the integer constant. For tokens of kind
     * REGISTER, returns the register number.
     */
    int       toInt();
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
    ST_DOLLAR,
    ST_MINUS,
    ST_REGISTER,
    ST_INT,
    ST_ID,
    ST_LABEL,
    ST_COMMA,
    ST_LPAREN,
    ST_RPAREN,
    ST_ZERO,
    ST_ZEROX,
    ST_HEXINT,
    ST_COMMENT,
    ST_DOT,
    ST_DOTW,
    ST_DOTWO,
    ST_DOTWOR,
    ST_DOTWORD,
    ST_WHITESPACE
};

// The *kind* of token (see previous enum declaration)
// represented by each state; states that don't represent
// a token have stateKinds == NUL.

Kind stateKinds[] = {
    NUL,            // ST_NUL
    NUL,            // ST_START
    NUL,            // ST_DOLLAR
    NUL,            // ST_MINUS
    REGISTER,       // ST_REGISTER
    INT,            // ST_INT
    ID,             // ST_ID
    LABEL,          // ST_LABEL
    COMMA,          // ST_COMMA
    LPAREN,         // ST_LPAREN
    RPAREN,         // ST_RPAREN
    INT,            // ST_ZERO
    NUL,            // ST_ZEROX
    HEXINT,         // ST_HEXINT
    WHITESPACE,     // ST_COMMENT
    NUL,            // ST_DOT
    NUL,            // ST_DOTW
    NUL,            // ST_DOTWO
    NUL,            // ST_DOTWOR
    DOTWORD,        // ST_DOTWORD
    WHITESPACE      // ST_WHITESPACE
};

State delta[ST_WHITESPACE+1][256];

#define whitespace "\t\n\r "
#define letters    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define digits     "0123456789"
#define hexDigits  "0123456789ABCDEFabcdef"
#define oneToNine  "123456789"

void setT(State from, string chars, State to) {
    for(int i = 0; i < chars.length(); i++ ) delta[from][chars[i]] = to;
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
    setT( ST_START,      whitespace,     ST_WHITESPACE );
    setT( ST_WHITESPACE, whitespace,     ST_WHITESPACE );
    setT( ST_START,      letters,        ST_ID         );
    setT( ST_ID,         letters digits, ST_ID         );
    setT( ST_START,      oneToNine,      ST_INT        );
    setT( ST_INT,        digits,         ST_INT        );
    setT( ST_START,      "-",            ST_MINUS      );
    setT( ST_MINUS,      oneToNine,      ST_INT        );
    setT( ST_START,      ",",            ST_COMMA      );
    setT( ST_START,      "(",            ST_LPAREN     );
    setT( ST_START,      ")",            ST_RPAREN     );
    setT( ST_START,      "$",            ST_DOLLAR     );
    setT( ST_DOLLAR,     digits,         ST_REGISTER   );
    setT( ST_REGISTER,   digits,         ST_REGISTER   );
    setT( ST_START,      "0",            ST_ZERO       );
    setT( ST_ZERO,       "x",            ST_ZEROX      );
    setT( ST_ZERO,       oneToNine,      ST_INT        );
    setT( ST_ZEROX,      hexDigits,      ST_HEXINT     );
    setT( ST_HEXINT,     hexDigits,      ST_HEXINT     );
    setT( ST_ID,         ":",            ST_LABEL      );
    setT( ST_START,      ";",            ST_COMMENT    );
    setT( ST_START,      ".",            ST_DOT        );
    setT( ST_DOT,        "w",            ST_DOTW       );
    setT( ST_DOTW,       "o",            ST_DOTWO      );
    setT( ST_DOTWO,      "r",            ST_DOTWOR     );
    setT( ST_DOTWOR,     "d",            ST_DOTWORD    );

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
            if(i < input.length())
                nextState = delta[state][(unsigned char) input[i]];
            if(nextState == ST_NUL) {
                // no more transitions possible
                if(stateKinds[state] == NUL) {
                    throw("ERROR in lexing after reading " + input.substr(0, i));
                }
                if(stateKinds[state] != WHITESPACE) {
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
        if(l < -2147483647-1 || l > 4294967295LL)
            throw("ERROR: constant out of range: "+lexeme);
        return l;
    } else if(kind == HEXINT) {
        if(lexeme.length() > 10) throw("ERROR: constant out of range: "+lexeme);
        long long l;
        sscanf( lexeme.c_str(), "%llx", &l );
        return l;
    } else if(kind == REGISTER) {
        if(lexeme.length() > 3) throw("ERROR: constant out of range: "+lexeme);
        long long l;
        sscanf( lexeme.c_str()+1, "%lld", &l );
        if(l > 31)
            throw("ERROR: constant out of range: "+lexeme);
        return l;
    }
    throw("ERROR: attempt to convert non-integer token "+lexeme+" to Int");
}

// kindString maps each kind to a string for use in error messages.

string kS[] = {
    "ID",           // ID
    "INT",          // INT
    "HEXINT",       // HEXINT
    "REGISTER",     // REGISTER
    "COMMA",        // COMMA
    "LPAREN",       // LPAREN
    "RPAREN",       // RPAREN
    "LABEL",        // LABEL
    "DOTWORD",      // DOTWORD
    "WHITESPACE",   // WHITESPACE
    "NUL"           // NUL
};

string kindString( Kind k ){
    if ( k < ID || k > NUL ) return "INVALID";
    return kS[k];
}

//======================================================================
//======= A sample program demonstrating the use of the scanner. =======
//======================================================================

int main() {

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
                cerr << "  Token: " << kindString(token.kind) 
                    << " {" << token.lexeme << "}";
                if(token.kind == INT || token.kind == HEXINT ||
                        token.kind == REGISTER) {
                    cerr << token.toInt();
                }
                cerr << endl;
            }
        }
    } catch(string msg) {
        cerr << msg << endl;
    }

    return 0;
}
