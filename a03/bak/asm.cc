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
#include <cstdlib>
#include <sstream>
using namespace std;

//================================================================
// my own functions and data structure

// used in symbol table
struct Label {
	string name;
	int loc;
	Label(string name, int loc) {
		this->name = name;
		this->loc = loc;
	}
};

void error(string s, int i);
bool existInTable(vector<Label> &symbolTable, string target);

//======================================================================
//========= Declarations for the scan() function =======================
//======================================================================

// Each token has one of the following kinds.

enum Kind {
	ID, // Opcode or identifier (use of a label)
	INT, // Decimal integer
	HEXINT, // Hexadecimal integer
	REGISTER, // Register number
	COMMA, // Comma
	LPAREN, // (
	RPAREN, // )
	LABEL, // Declaration of a label (with a colon)
	DOTWORD, // .word directive
	WHITESPACE, // Whitespace
	NUL
// Bad/invalid token
};

// kindString(k) returns string a representation of kind k
// that is useful for error and debugging messages.
string kindString(Kind k);

// Each token is described by its kind and its lexeme.

struct Token {
	Kind kind;
	string lexeme;
	/* toInt() returns an integer representation of the token. For tokens
	 * of kind INT (decimal integer constant) and HEXINT (hexadecimal integer
	 * constant), returns the integer constant. For tokens of kind
	 * REGISTER, returns the register number.
	 */
	long toInt(int);
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

// The *kind* of (see previous enum declaration)
// represented by each state; states that don't represent
// a token have stateKinds == NUL.

Kind stateKinds[] = { NUL, // ST_NUL
		NUL, // ST_START
		NUL, // ST_DOLLAR
		NUL, // ST_MINUS
		REGISTER, // ST_REGISTER
		INT, // ST_INT
		ID, // ST_ID
		LABEL, // ST_LABEL
		COMMA, // ST_COMMA
		LPAREN, // ST_LPAREN
		RPAREN, // ST_RPAREN
		INT, // ST_ZERO
		NUL, // ST_ZEROX
		HEXINT, // ST_HEXINT
		WHITESPACE, // ST_COMMENT
		NUL, // ST_DOT
		NUL, // ST_DOTW
		NUL, // ST_DOTWO
		NUL, // ST_DOTWOR
		DOTWORD, // ST_DOTWORD
		WHITESPACE // ST_WHITESPACE
		};

State delta[ST_WHITESPACE + 1][256];

#define whitespace "\t\n\r "
#define letters    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define digits     "0123456789"
#define hexDigits  "0123456789ABCDEFabcdef"
#define oneToNine  "123456789"

void setT(State from, string chars, State to) {
	for (int i = 0; i < chars.length(); i++)
		delta[from][chars[i]] = to;
}

void initT() {
	int i, j;

	// The default transition is ST_NUL (i.e., no transition
	// defined for this char).
	for (i = 0; i <= ST_WHITESPACE; i++) {
		for (j = 0; j < 256; j++) {
			delta[i][j] = ST_NUL;
		}
	}
	// Non-null transitions of the finite state machine.
	// NB: in the third line below, letters digits are macros
	// that are replaced by string literals, which the compiler
	// will concatenate into a single string literal.
	setT(ST_START, whitespace, ST_WHITESPACE);
	setT(ST_WHITESPACE, whitespace, ST_WHITESPACE);
	setT(ST_START, letters, ST_ID);
	setT(ST_ID, letters digits, ST_ID);
	setT(ST_START, oneToNine, ST_INT);
	setT(ST_INT, digits, ST_INT);
	setT(ST_START, "-", ST_MINUS);
	setT(ST_MINUS, digits, ST_INT); // changed to digits to accept zeroes
	setT(ST_START, ",", ST_COMMA);
	setT(ST_START, "(", ST_LPAREN);
	setT(ST_START, ")", ST_RPAREN);
	setT(ST_START, "$", ST_DOLLAR);
	setT(ST_DOLLAR, digits, ST_REGISTER);
	setT(ST_REGISTER, digits, ST_REGISTER);
	setT(ST_START, "0", ST_ZERO);
	setT(ST_ZERO, "x", ST_ZEROX);
	setT(ST_ZERO, oneToNine, ST_INT);
	setT(ST_ZEROX, hexDigits, ST_HEXINT);
	setT(ST_HEXINT, hexDigits, ST_HEXINT);
	setT(ST_ID, ":", ST_LABEL);
	setT(ST_START, ";", ST_COMMENT);
	setT(ST_START, ".", ST_DOT);
	setT(ST_DOT, "w", ST_DOTW);
	setT(ST_DOTW, "o", ST_DOTWO);
	setT(ST_DOTWO, "r", ST_DOTWOR);
	setT(ST_DOTWOR, "d", ST_DOTWORD);

	for (j = 0; j < 256; j++)
		delta[ST_COMMENT][j] = ST_COMMENT;
}

static int initT_done = 0;

vector<Token> scan(string input) {
	// Initialize the transition table when called for the first time.
	if (!initT_done) {
		initT();
		initT_done = 1;
	}

	vector<Token> ret;

	int i = 0;
	int startIndex = 0;
	State state = ST_START;

	Kind preKind = NUL;
//	char negativeSign = '';
	bool sameKind = 0, checkOnceMore = 0;

	if (input.length() > 0) {
		while (true) {
			State nextState = ST_NUL;
			if (i < input.length())
				nextState = delta[state][(unsigned char) input[i]];
			if (nextState == ST_NUL) {
				// no more transitions possible
//cout << input.substr(startIndex, i - startIndex) << endl;
				if (stateKinds[state] == NUL) {
					throw("ERROR: in lexing after reading "
							+ input.substr(0, i));
				}
				if (stateKinds[state] != WHITESPACE) {

					////
					sameKind = preKind == stateKinds[state] && stateKinds[state] != LABEL;
					preKind = stateKinds[state];
					if (checkOnceMore && sameKind) {
						error(
								"cannot have tokens of the same kind consecutively",
								-1);
					}
					checkOnceMore = 0;

					Token token;
					token.kind = stateKinds[state];
					string raw = input.substr(startIndex, i - startIndex);

					// remove zeroes in the front ie 0000000000034 -> 34
					if (stateKinds[state] == INT)	{
						for (;raw.length() > 2;){
							if (raw[0] == '-' && raw[1] == '0'){
								raw.erase(raw.begin() + 1);
							}else if (raw[0] == '0'){
								raw.erase(raw.begin());
							} else break;
						}
					} else if (stateKinds[state] == HEXINT){
						for (;raw.length() > 2;){
							if (raw[2] == '0'){
								raw.erase(raw.begin()+2);
							} else break;
						}
					}
					token.lexeme = raw;
					ret.push_back(token);
				} else {
					//error("ERROR: cannot have tokens of the same kind consecutively", -1);
					checkOnceMore = 1;
					//cout << " whitespace"<<endl;
				}
				startIndex = i;
				state = ST_START;
				if (i >= input.length())
					break;
			} else {
				state = nextState;
				i++;
			}
		}
	}

	return ret;
}

long Token::toInt(int lineOfError = -1) {
	if (kind == INT) {
		if (lexeme.length() > 11)
			error("constant out of range: " + lexeme, lineOfError);
		long long l;
		sscanf(lexeme.c_str(), "%lld", &l);
		if (l < -2147483647 - 1 || l > 4294967295LL)
			error("constant out of range: " + lexeme, lineOfError);
		return l;
	} else if (kind == HEXINT) {
		if (lexeme.length() > 10)
			error("constant out of range: " + lexeme, lineOfError);
		long long l;
		sscanf(lexeme.c_str(), "%llx", &l);
		return l;
	} else if (kind == REGISTER) {
		if (lexeme.length() > 3)
			error("constant out of range (3 char): " + lexeme, lineOfError);
		long long l;
		sscanf(lexeme.c_str() + 1, "%lld", &l);
		if (l > 31)
			error("constant out of range (exceed 31): " + lexeme, lineOfError);
		return l;
	}
	error("attempt to convert non-integer token " + lexeme + " to Int",
			lineOfError);
}

// kindString maps each kind to a string for use in error messages.

string kS[] = { "ID", // ID
		"INT", // INT
		"HEXINT", // HEXINT
		"REGISTER", // REGISTER
		"COMMA", // COMMA
		"LPAREN", // LPAREN
		"RPAREN", // RPAREN
		"LABEL", // LABEL
		"DOTWORD", // DOTWORD
		"WHITESPACE", // WHITESPACE
		"NUL" // NUL
		};

string kindString(Kind k) {
	if (k < ID || k > NUL)
		return "INVALID";
	return kS[k];
}

// ------------------------------------ My Codes Begin

bool isBetween(long value, long low, long high) {
	if (value < low || value > high)
		return 0;
	return 1;
}

int existInTable(vector<Label*> &symbolTable, string identifier) {
	int size = symbolTable.size();
	if (size == 0)
		return -1;
	for (int k = 0; k < size; k++) {
		if (!identifier.compare(symbolTable[k]->name))
			return k;
	}
	return -1;
}

#if 1
void printBytes(unsigned long value) {
	putchar((value >> 24) & 0xff);
	putchar((value >> 16) & 0xff);
	putchar((value >> 8) & 0xff);
	putchar(value & 0xff);
}
#endif

int getFromTable(vector<Label*> &symbolTable, string identifier) {
	for (int i = 0; i < symbolTable.size(); i++) {
		if (symbolTable[i]->name == identifier) {
			return symbolTable[i]->loc;
		}
	}
}

inline void processID(vector<Token> &curline, vector<Label*> &symbolTable,
		int &PC) {
	Token token = curline[0];
	unsigned int mask = 0, size = curline.size();

	if (size == 2) { // process the type that has one opcode and one register
		Token first = curline[1];
		unsigned int s = first.kind != ID ? first.toInt() : 0; // either ID, REGISTER, INT, HEXINT
		if (token.lexeme == "jr") {
			mask = 0x8 + (s << 21);
		} else if (token.lexeme == "jalr") {
			mask = 0x9 + (s << 21);
		} else if (token.kind == DOTWORD) {
			mask = first.kind != ID ? s : getFromTable(symbolTable, first.lexeme);
		} else if (token.lexeme == "lis") {
			mask = 0x14 + (s << 11);
		} else if (token.lexeme == "mflo") {
			mask = 0x12 + (s << 11);
		} else if (token.lexeme == "mfhi") {
			mask = 0x10 + (s << 11);
		}
	} else if (size == 4) {
		unsigned int arg1 = curline[1].toInt();
		unsigned int arg2 = curline[3].toInt();
		// choose the correct opcode and add the mask value
		if (token.lexeme == "mult") {
			mask = 0x18;
		} else if (token.lexeme == "multu") {
			mask = 0x19;
		} else if (token.lexeme == "div") {
			mask = 0x1a;
		} else if (token.lexeme == "divu") {
			mask = 0x1b;
		}

		// add the registers to the mask
		mask += (arg1 << 21) + (arg2 << 16);
	} else if (size == 6) {
		unsigned int arg1 = curline[1].toInt();
		unsigned int arg2 = curline[3].toInt();
		unsigned int arg3 = curline[5].kind != ID ? curline[5].toInt()
				: getFromTable(symbolTable, curline[5].lexeme);
		if (curline[5].kind == REGISTER) {
			// choose the correct opcode and add the mask value
			if (token.lexeme == "add") {
				mask = 0x20;
			} else if (token.lexeme == "sub") {
				mask = 0x22;
			} else if (token.lexeme == "slt") {
				mask = 0x2a;
			} else if (token.lexeme == "sltu") {
				mask = 0x2b;
			}

			// add the registers to the mask
			mask += (arg2 << 21) + (arg3 << 16) + (arg1 << 11);
		} else { // the third parameter is an i
			// choose the correct opcode and add the mask value
			if (token.lexeme == "beq") {
				mask = 0x10000000;
			} else if (token.lexeme == "bne") {
				mask = 0x14000000;
			}

			// add the registers to the mask
			// (arg3 & 0xffff) takes care of negative cases
			arg3 = curline[5].kind == ID ? (arg3 - PC - 4) / 4 : arg3;
			mask += (arg1 << 21) + (arg2 << 16) + (arg3 & 0xffff);
		}
	} else if (size == 7) {
		unsigned int arg1 = curline[1].toInt();
		unsigned int arg2 = curline[3].toInt();
		unsigned int arg3 = curline[5].toInt();
		// choose the correct opcode and add the mask value
		if (token.lexeme == "sw") {
			mask = 0xac000000;
		} else if (token.lexeme == "lw") {
			mask = 0x8c000000;
		}

		// add the registers to the mask
		mask += (arg3 << 21) + (arg1 << 16) + (arg2 & 0xffff);
	}
	printBytes(mask);
	PC += 4;
}

string alphabets = letters;
string numbers = digits;
bool validIdentifier(string &s) {
	if (numbers.find(s[0]) < 0)
		return 0;
	bool result = 1;
	for (int i = 1; i < s.size(); i++) {
		result *= numbers.find(s[i]) || alphabets.find(s[i]);
	}
	return result;
}

inline void printInvalidMessage(bool valid, int i) {
	if (!valid)
		error("invalid opcode/number of arguments", i);
}

inline bool intermediateInRange(Token t) {
	int value = 0;
	if (t.kind == REGISTER) {
		return 1;
	} else if (t.kind == INT) {
		value = t.toInt();
		return value >= -32768 && value <= 32767;
	} else if (t.kind == HEXINT) {
		value = t.toInt();
		return value <= 0xffff;
	}
	return 0;
}

inline bool isIntermediate(Token t) {
	return t.kind == HEXINT || t.kind == INT;
}

int removeUselessZeroesInTheFront(vector<Token> &curline, int start,
		int labelErased, int lineOfError) {
	start += labelErased;
	int i = start;
	if (curline[i].kind == ID)
		error("debug! on calling removeUselessZeroesInTheFront", lineOfError);
	else if (curline[i].kind == REGISTER)
		return start;
	for (int value = curline[start].toInt(); !value && start + 1
			< curline.size(); i++) {
		if (curline[start + 1].kind == INT) {
			curline.erase(curline.begin() + start);
		} else if (curline[start + 1].kind == HEXINT) {
			error("addition zeroes before 0x", lineOfError);
		} else {
			break;
		}
	}

	return i - labelErased;
}

bool checkLine(int i, vector<Token> &curline_bak, vector<Label *> &symbolTable,
		vector<Label *> &cache, int &PC) {
	// copy the vector of tokens to prevent destruction
	vector<Token> curline(curline_bak);

	reiterateTesting: // simulate recursion
	int size = curline.size();
	if (!size)
		return 1;
	if (curline[0].kind == DOTWORD) {
		if (size != 2) {
			error(".word not enough/too many arguments", i);
		}
		if (curline[1].kind == INT || curline[1].kind == HEXINT) {
			curline[1].toInt(i); // toInt has its own error checking
		} else if (curline[1].kind == ID) {
			string identifier = curline[1].lexeme;

			// push the identifier in cache (since it may be declared later)
			if (!existInTable(cache, identifier) && existInTable(symbolTable,
					identifier) == -1) {
				cache.push_back(new Label(identifier, i));
			}

		} else {
			error("inappropriate arugement for .word", i);
		}
	} else if (curline[0].kind == LABEL) {
		string identifier = curline[0].lexeme.substr(0,
				curline[0].lexeme.size() - 1);
		if (!validIdentifier(identifier))
			error("invalid name for identifier", i);

		if (existInTable(symbolTable, identifier) != -1)
			error("duplication of identifier name", i);

		symbolTable.push_back(new Label(identifier, PC));

		curline.erase(curline.begin());
		goto reiterateTesting;
	} else if (curline[0].kind == ID) {
		bool valid = 0;
		if (size == 2) {
			// check the op code
			valid += (curline[0].lexeme == "jr");
			valid += (curline[0].lexeme == "jalr");

			if (!valid)
				error("invalid opcode", i);

			// check whether the remaining argument is a register
			valid = valid && (curline[1].kind == REGISTER);

			curline[1].toInt(); // check the range of the register value
		} else if (size == 6) {
			bool hasI = 0;
			if (curline[0].lexeme == "add" || curline[0].lexeme == "sub"
					|| (curline[0].lexeme == "slt") || (curline[0].lexeme
					== "sltu")) {
				valid = 1;
			} else if (curline[0].lexeme == "beq" || curline[0].lexeme == "bne") {
				hasI = 1;
				valid = 1;
			}

			if (!valid)
				error("invalid opcode", i);

			if (curline[1].kind != REGISTER)
				error("expecting a register for the first argument", i);
			if (curline[2].kind != COMMA)
				error("invalid delimiter (require a comma)", i);
			if (curline[3].kind != REGISTER)
				error("expecting a register for the second argument", i);
			if (curline[4].kind != COMMA)
				error("invalid delimiter (require a comma)", i);

			if (curline[1].kind == REGISTER && curline[2].kind == COMMA
					&& curline[3].kind == REGISTER && curline[4].kind == COMMA
					&& (curline[5].kind == REGISTER || curline[5].kind == INT
							|| curline[5].kind == HEXINT || curline[5].kind
							== ID)) {

				curline[1].toInt(); // check $d
				curline[3].toInt(); // check $s

				int value = 0xfffff; // out of bound
				if (curline[5].kind != ID) {
					value = curline[5].toInt(); // check $t
				} else {
					int index = existInTable(symbolTable, curline[5].lexeme);
					if (index != -1)
						value = symbolTable[index]->loc;
				}
				if (hasI) {
					int valueLabel = (value - PC - 4) / 4;
					if (curline[5].kind == INT && (value < -32768 || value
							> 32767) || curline[5].kind == HEXINT && (value < 0
							|| value > 0xffff) || curline[5].kind == ID
							&& (valueLabel < -32768 || valueLabel > 32767)) {
						error("branching offset out of range", i);
					} else if (curline[5].kind == REGISTER) {
						error("Expecting immediate but got REGISTER", i);
					}
				}
				valid = 1;
			}

		}

		if (!valid) {
			error("invalid (number of) arguements", i);
		}
	} else {
		error("invalid syntax", i);
	}
	PC += 4;
	return 1;
}

#if 0
bool checkLine(int i, vector<Token> &curline_bak, vector<Label *> &symbolTable,
		vector<Label *> &cache, vector<Label*> &cacheI, int &PC) {
	// copy the vector of tokens to prevent destruction
	vector<Token> curline(curline_bak);

	int size, originalSize = curline.size(), labelErased = 0;

	reiterateTesting: // simulate recursion
	size = curline.size();
	if (!size)
		return 1;
	if (curline[0].kind == DOTWORD) { ////-------- case with .word
		if (size != 2) {
			error(".word not enough/too many arguments", i);
		}
		if (curline[1].kind == INT || curline[1].kind == HEXINT) {
			curline[1].toInt(i); // toInt has its own error checking
		} else if (curline[1].kind == ID) {
			string identifier = curline[1].lexeme;

			// push the identifier in cache (since it may be declared later)
			if (existInTable(cache, identifier) == -1 && existInTable(
					symbolTable, identifier) == -1) {
				cache.push_back(new Label(identifier, i));
			}

		} else {
			error("inappropriate arugement for .word", i);
		}
	} else if (curline[0].kind == LABEL) { /////-------- case with label
		string identifier = curline[0].lexeme.substr(0,
				curline[0].lexeme.size() - 1);
		if (!validIdentifier(identifier))
			error("invalid name for identifier", i);

		if (existInTable(symbolTable, identifier) != -1)
			error("duplication of identifier name", i);

		symbolTable.push_back(new Label(identifier, PC));
		curline.erase(curline.begin());
		labelErased++;
		goto reiterateTesting;
	} else if (curline[0].kind == ID) { ////-------- case with op code
		int type = 0;
		if (size == 2) { ////-------- 2 tokens
			// check the op code
			type = curline[0].lexeme == "jr" || curline[0].lexeme == "jalr"
					|| curline[0].lexeme == "lis" || curline[0].lexeme
					== "mflo" || curline[0].lexeme == "mfhi" ? 1 : 0;

			printInvalidMessage(type, i);

			// check whether the remaining argument is a register
			if (curline[1].kind != REGISTER)
				error("argument 1 requires a REGISTER", i);

			curline[1].toInt(); // check the range of the register value

		} else if (size == 4) { ////-------- 4 tokens
			type = curline[0].lexeme == "mult" || curline[0].lexeme == "multu"
					|| curline[0].lexeme == "div" || curline[0].lexeme
					== "divu" ? 1 : 0;

			printInvalidMessage(type, i);

			if (curline[1].kind != REGISTER)
				error("expecting a register for the first argument", i);
			if (curline[2].kind != COMMA)
				error("invalid delimiter (require a comma)", i);
			if (curline[3].kind != REGISTER)
				error("expecting a register for the second argument", i);

			curline[1].toInt(); // check $d
			curline[3].toInt(); // check $s

		} else if (size == 6) { ////-------- 6 tokens
			// check the op code and its type
			type = curline[0].lexeme == "add" || curline[0].lexeme == "sub"
					|| curline[0].lexeme == "slt" || curline[0].lexeme
					== "sltu" ? 1 : 0;

			type = curline[0].lexeme == "beq" || curline[0].lexeme == "bne" ? 2
					: type;

			type = curline[0].lexeme == "sw" || curline[0].lexeme == "lw" ? 3 : type;

			printInvalidMessage(type, i);

			if (curline[1].kind != REGISTER)
				error("expecting a register for the first argument", i);
			if (curline[2].kind != COMMA)
				error("invalid delimiter (require a comma)", i);
			if (type < 3) { /// add , sub, slt, sltu, beq , bne
				if (curline[3].kind != REGISTER)
					error("expecting a register for the second argument", i);
				if (curline[4].kind != COMMA)
					error("invalid delimiter (require a comma)", i);

				if (type == 1 && curline[5].kind != REGISTER) {
					error("require a register for the third arg", i);
				} else if (type == 2 && curline[5].kind != ID
						&& curline[5].kind != INT && curline[5].kind != HEXINT) {
					error("require an ID, INT, or a HEXINT for the third arg",
							i);
				}

				if (curline[5].kind != ID) { // intermediate or register
					// remove front zeroes and get the index of the token that contains the value
					// *** from the ORIGINAL (that will be use for the final result)
					int index =
							removeUselessZeroesInTheFront(curline_bak, 5, labelErased, i);
					if (originalSize - index + 5 - labelErased!= 6) { // type 1, 2 should have only 6 args
						error("extra arguments (type 1, 2)", i);
					}
					if (!intermediateInRange(curline[index])) {
						error("branching offset out of range", i);
					}
				} else { // ID
					string identifier = curline[5].lexeme;
					if (existInTable(cache, identifier) == -1 && existInTable(
							symbolTable, identifier) == -1) {
						cache.push_back(new Label(identifier, i));
					}
					cacheI.push_back(new Label(identifier, PC));
				}

			} else { // sw, lw
				int index = removeUselessZeroesInTheFront(curline_bak, 3, labelErased, i);
				if (curline[index].kind != INT && curline[index].kind != HEXINT)
					error("expecting an intermediate for the second argument",
							i);
				if (curline[index + 1].kind != LPAREN)
					error("invalid delimiter (require a LEFT parenthesis)", i);
				if (curline[index + 2].kind != REGISTER)
					error("expecting a register for the first argument", i);
				if (curline[index + 3].kind != RPAREN)
					error("invalid delimiter (require a RIGHT parenthesis)", i);
				if (originalSize - index + 3 - labelErased!= 7) { // type 3 should have only 7 args
					error("extra arguments (type 3)", i);
				}

				if (!intermediateInRange(curline[index]))
					error("intemediate out of range", i);
			}

		} else {
			error("too much/few arguments", i);
		}
	} else {
		Invalid: error("invalid syntax", i);
	}
	PC += 4;
	return 1;
}
#endif

// a control function that breaks the check into smaller parts
inline bool check(vector<vector<Token> > &tokLines,
		vector<Label*> &symbolTable, vector<Label*> &cache,
		vector<Label*> &cacheI, int &PC) {
	int result = 1;
	for (int i = 0, size; i < tokLines.size(); i++) {
		result *= checkLine(i, tokLines[i], symbolTable, cache, cacheI, PC);
	}
	return result;
}

// print error message
inline void error(string s, int i) {
	stringstream ss;
	ss << "ERROR: " << s << " in line " << (i + 1);
	throw(ss.str());
}

// garbage collection
inline void cleanSymbolTable(vector<Label*> &v) {
	for (int i = 0; i < v.size(); i++) {
		delete v[i];
	}
}

int main() {
	// symbol table here
	vector<Label *> symbolTable, cache, cacheI;
	try {
		vector<string> srcLines;

		// Read the entire input file, storing each line as a
		// single string in the array srcLines.
		while (true) {
			string line;
			getline(cin, line);
			if (cin.fail())
				break;
			srcLines.push_back(line);
		}

		// Tokenize each line, storing the results in tokLines.
		vector<vector<Token> > tokLines;

		for (int line = 0; line < srcLines.size(); line++) {
			tokLines.push_back(scan(srcLines[line]));
		}

		// check for syntax error and generate the symbol table
		int PC = 0;
		check(tokLines, symbolTable, cache, cacheI, PC);
		for (int i = 0; i < cache.size(); i++) {
			if (existInTable(symbolTable, cache[i]->name) == -1) {
				error(cache[i]->name + " is not defined", i);
			}
		}

		for (int i = 0; i < cacheI.size(); i++) {
			int value = (getFromTable(symbolTable, cacheI[i]->name)
					- cacheI[i]->loc - 4) / 4;
			if (value < -32768 || value > 32767) {
				error("branching offset out of range", i);
			}
		}

		for (int i = 0; i < symbolTable.size(); i++)
			cerr << symbolTable[i]->name << " " << symbolTable[i]->loc << endl;

		// translate into binary
		PC = 0; // reset PC
		for (int line = 0; line < tokLines.size(); line++) {
			reiterateProcess: // simulate recursion (process the rest of the list)
			if (tokLines[line].size() <= 0)
				continue;
			Token token = tokLines[line][0];
			if (token.kind == DOTWORD) {
				processID(tokLines[line], symbolTable, PC);
			} else if (token.kind == ID) {
				processID(tokLines[line], symbolTable, PC);
			} else if (token.kind == LABEL) {
				tokLines[line].erase(tokLines[line].begin());
				goto reiterateProcess;
			}
		}

	} catch (string msg) {
		cerr << msg << endl;
	}

	//garbage collect the symbol table
	cleanSymbolTable(symbolTable);
	cleanSymbolTable(cache);
	cleanSymbolTable(cacheI);
	return 0;
}

