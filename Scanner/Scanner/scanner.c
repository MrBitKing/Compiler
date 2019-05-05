/*Filename: scanner.c */

/*******************************************************************************************
File Name: buffer.c
Compiler: MS Visual Studio 2015
Author: Nana Ngassa Franck, 040 783 398
		Vicken Aharonian, 040 802 638
Course: CST 8152 – Compilers, Lab Section: 011
Assignment: 1
Date: 3/30/2018
Professor: Sv Ranev
Purpose: SCANNER.C: Functions implementing a Lexical Analyzer (Scanner)
as required for CST8152, Assignment #2
scanner_init() must be called before using the scanner.
The file is incomplete;
Provided by: Svillen Ranev
Version: 1
Date: 30 March 2018

Function list:

int scanner_init(Buffer * sc_buf)
static int char_class(char c);
Token malar_next_token(Buffer * sc_buf)
int get_next_state(int state, char c, int *accept)
Token aa_func02(char * lexeme)
Token aa_func03(char * lexeme)
Token aa_func05(char * lexeme)
Token aa_func08(char * lexeme)
Token aa_func11(char * lexeme)
Token aa_func12(char * lexeme)
static long atolh(char * lexeme)


Total = 11
*********************************************************************************************/

/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
* to suppress the warnings about using "unsafe" functions like fopen()
* and standard sting library functions defined in string.h.
* The define does not have any effect in Borland compiler projects.
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

/*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "table.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
It is defined in platy_st.c */
extern Buffer * str_LTBL; /*String literal table */
int line; /* current line number of the source code */
extern int scerrnum;     /* defined in platy_st.c - run-time error number */

						 /* Local(file) global objects - variables */
static Buffer *lex_buf;/*pointer to temporary lexeme buffer*/

					   /* No other global variable declarations/definitiond are allowed */

					   /* scanner.c static(local) function  prototypes */
static int char_class(char c); /* character class function */
static int get_next_state(int, char, int *); /* state machine function */
static long atolh(char * lexeme); /* converts hexadecimal string to decimal value */

								  /*Initializes scanner */
int scanner_init(Buffer * sc_buf) {
	if (b_isempty(sc_buf)) return EXIT_FAILURE;/*1*/
											   /* in case the buffer has been read previously  */
	b_rewind(sc_buf);
	b_clear(str_LTBL);
	line = 1;
	return EXIT_SUCCESS;/*0*/
						/*   scerrnum = 0;  no need - global ANSI C */
}

/*******************************************************************************
Purpose:			This buffer attempts to add the given char argument to the
character buffer
Author:				Svillen Ranev,	Franck Nana, Vicken Aharonian
History/Versions:	1.0
Called functions:	b_getc()
b_retract()
b_mark()
b_getcoffset()
b_addc()
b_reset()
Parameters:			Buffer * sc_buf
Pointer to the buffer from which to analyze
Return value:		Token
A completed token for each lexeme found in the buffer
Algorithm:			Traverse through the buffer
Determine the state corresponding to each character
*******************************************************************************/

Token malar_next_token(Buffer * sc_buf)
{
	int i = 0;
	Token t = { 0 }; /* token to return after pattern recognition. Set all structure members to 0 */
	unsigned char c; /* input symbol */
	int state = 0; /* initial state of the FSM */
	short lexstart;  /*start offset of a lexeme in the input char buffer (array) */
	short lexend;    /*end   offset of a lexeme in the input char buffer (array)*/
	int accept = NOAS; /* type of state - initially not accepting */

					   /*	DECLARE YOUR LOCAL VARIABLES HERE IF NEEDED*/

	while (1) { /* endless loop broken by token returns it will generate a warning */

				/*GET THE NEXT SYMBOL FROM THE INPUT BUFFER*/

		c = b_getc(sc_buf);
		if (c == 0xFF || c == EOF || c == '\0') {

			t.code = SEOF_T;
			return t;
		}

		/*Special characters*/
		else if (c == ' ') continue;  /*white Space*/
		else if (c == '\t') continue; /*Horizontal tab*/
		else if (c == '\v') continue; /*Vertical tab*/
		else if (c == '\f') continue; /*Form freed*/
		else if (c == '\n' || c == '\r') {

			line++; continue; /*line feed,  and count*/
		}


		else if (c == '{') { t.code = LBR_T; /* Left Bracket  */ return t; }
		else if (c == '}') { t.code = RBR_T; /* Right Bracket */ return t; }
		else if (c == '(') { t.code = LPR_T; /* Left Bracket */  return t; }
		else if (c == ')') { t.code = RPR_T; /* Right Bracket */ return t; }
		else if (c == ',') { t.code = COM_T; /* Comma */		  return t; }
		else if (c == ';') { t.code = EOS_T; /* semi-colon */ return t; }
		else if (c == '#') { t.code = SCC_OP_T; /* Concatenation */ return t; }


		/*Arithmetic Operators*/

		else if (c == '+') { t.code = ART_OP_T; t.attribute.arr_op = PLUS; return t; }
		else if (c == '-') { t.code = ART_OP_T; t.attribute.arr_op = MINUS; return t; }
		else if (c == '*') { t.code = ART_OP_T; t.attribute.arr_op = MULT; return t; }
		else if (c == '/') { t.code = ART_OP_T; t.attribute.arr_op = DIV; return t; }

		/*Relational operators*/

		else if (c == '=') {

			c = b_getc(sc_buf);
			if (c == '=') {
				t.code = REL_OP_T;
				t.attribute.rel_op = EQ;
				return t;
			}
			else
			{
				b_retract(sc_buf);
				t.code = ASS_OP_T; /*Assigmment operator*/
			}
			return t;
		}

		else if (c == '>') { t.code = REL_OP_T; t.attribute.rel_op = GT; return t; }

		else if (c == '<') {
			if (b_getc(sc_buf) == '>') {
				t.code = REL_OP_T;
				t.attribute.rel_op = NE;
				return t;
			}

			b_retract(sc_buf);
			t.code = REL_OP_T;
			t.attribute.rel_op = LT; /*Less then */
			return t;
		}

		else if (c == '.') {

			b_mark(sc_buf, b_getcoffset(sc_buf));
			c = b_getc(sc_buf);

			if (c == 'A') {
				c = b_getc(sc_buf);

				if (c == 'N') {
					c = b_getc(sc_buf);

					if (c == 'D') {
						c = b_getc(sc_buf);

						if (c == '.') {

							t.code = LOG_OP_T;
							t.attribute.log_op = AND; /*And */

							return t;
						}
					}
				}
			}
			else if (c == 'O') {
				c = b_getc(sc_buf);

				if (c == 'R') {
					c = b_getc(sc_buf);

					if (c == '.') {

						t.code = LOG_OP_T;
						t.attribute.log_op = OR; /*Or */
						return t;
					}
				}
			}
			/* Generate an error token if above fails */
			t.code = ERR_T;
			t.attribute.err_lex[0] = '.';
			t.attribute.err_lex[1] = '\0';

			b_reset(sc_buf);

			return t;

		}
		/* Comments Operator */
		else if (c == '!') {
			c = b_getc(sc_buf);

			if (c == '!') {
				while (c != '\r' && c != '\n' && c != b_eob(sc_buf) && c != '\0' && c != 255)
					c = b_getc(sc_buf);

				line++;
				continue;
			}
			else
			{
				t.code = ERR_T;
				t.attribute.err_lex[0] = '!';
				t.attribute.err_lex[1] = c;
				t.attribute.err_lex[2] = '\0';

				while (1) {
					c = b_getc(sc_buf);

					if (c == '\n' || c == '\0' || c == 255 || c == '\r'&&  c != b_eob(sc_buf)) {
						break;
					}
				}
				line++;
			}
			return t;
		}

		/*String literals*/
		else if (c == '"') {

			b_mark(sc_buf, b_getcoffset(sc_buf)); /*Set mark*/
			lexstart = b_getcoffset(sc_buf); /*lexstart set to getoffset*/

			while (1) {

				c = b_getc(sc_buf);

				if (c == '"') {
					lexend = b_getcoffset(sc_buf); /*lexend set to getoffset*/
					b_reset(sc_buf);

					t.attribute.str_offset = b_limit(str_LTBL);

					/*Copy sc_buff in str_LTBL */
					for (i = lexstart; i < (lexend - 1); i++) {

						c = b_getc(sc_buf);

						if (c != NULL)
							b_addc(str_LTBL, c);
						return;
					}

					b_getc(sc_buf);
					b_addc(str_LTBL, '\0');
					t.code = STR_T; /*Set string state */

					return t;
				}
				else if (c == '\0' || c == 255 || c == EOF) {
					b_reset(sc_buf);
					b_retract(sc_buf);

					/*Reserve space for 3 dots*/
					for (i = 0; i < (ERR_LEN - 3); i++) {
						if (c != NULL)
							c = b_getc(sc_buf);
						return;

						if (c == '\n' || c == '\0' || c == 255) {
							break;
						}

						t.attribute.err_lex[i] = c;
					}

					t.code = ERR_T; /*Set error state*/
					t.attribute.err_lex[i++] = '.';
					t.attribute.err_lex[i++] = '.';
					t.attribute.err_lex[i++] = '.';
					t.attribute.err_lex[i++] = '\0';

					c = b_getc(sc_buf);

					while (c != '\0' && c != 255 && c != EOF) {
						c = b_getc(sc_buf);
					}

					return t;
				}
				else if (c == '\n') {
					line++;
				}
			}
		}

		else {

			if (isalpha(c) || isdigit(c)) {  /*Is digit or character*/

				lexstart = b_mark(sc_buf, b_getcoffset(sc_buf) - 1);  /*Set mark*/
				state = get_next_state(state, c, &accept);

				while (accept == NOAS) { /*while state not accepting, get next state*/
					c = b_getc(sc_buf);
					state = get_next_state(state, c, &accept);
				}
				if (accept == ASWR || accept == ER) {/*Retract if retracting state*/
					b_retract(sc_buf);
				}

				lexend = b_getcoffset(sc_buf); /*Store offset into lexend*/
				lex_buf = b_allocate((lexend - lexstart) + 1, 0, 'f'); /*Allocate lexeme buffer*/

				if (!lex_buf) {
					aa_func12("RUN TIME ERROR");
					scerrnum = 1;
					t.code = ERR_T;
					return t;
				}

				b_reset(sc_buf);
				for (i = 0; i < (lexend - lexstart); i++) { /*copy text from input to lexeme buffer*/
					c = b_getc(sc_buf);
					b_addc(lex_buf, c);
				}

				b_addc(lex_buf, '\0');
				t = aa_table[state](lex_buf->cb_head); /*Set token*/
				free(lex_buf);
				return t;
			}

			/* Anything else, Set error token */
			t.code = ERR_T;
			t.attribute.err_lex[0] = c;
			t.attribute.err_lex[1] = '\0';

			return t;
		}
	}

}
/*******************************************************************************
Purpose:			Retrieve corresponding states from symbol table
Author:				Svillen Ranev, Vicken Aharonian
History/Versions:	1.0
Called functions:	char_class()
printf()
Parameters:			int state  : Precedent State

int * accept :    Current state
Return value:		int
The next state, in the transition table
Algorithm:			Get next state from the transition table
*******************************************************************************/

int get_next_state(int state, char c, int *accept)
{
	int col;
	int next;
	col = char_class(c);
	next = st_table[state][col];
#ifdef DEBUG
	printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
#endif
	/*
	The assert(int test) macro can be used to add run-time diagnostic to programs
	and to "defend" from producing unexpected results.
	assert() is a macro that expands to an if statement;
	if test evaluates to false (zero) , assert aborts the program
	(by calling abort()) and sends the following message on stderr:

	Assertion failed: test, file filename, line linenum

	The filename and linenum listed in the message are the source file name
	and line number where the assert macro appears.
	If you place the #define NDEBUG directive ("no debugging")
	in the source code before the #include <assert.h> directive,
	the effect is to comment out the assert statement.
	*/
	assert(next != IS);

	/*
	The other way to include diagnostics in a program is to use
	conditional preprocessing as shown bellow. It allows the programmer
	to send more details describing the run-time problem.
	Once the program is tested thoroughly #define DEBUG is commented out
	or #undef DEBUF is used - see the top of the file.
	*/
#ifdef DEBUG
	if (next == IS) {
		printf("Scanner Error: Illegal state:\n");
		printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
		exit(1);
}
#endif
	*accept = as_table[next];
	return next;
}

/*******************************************************************************
Purpose:	Return the column number in the transition table
Author:				Vicken Aharonian
History/Versions:	1.0
Called functions:	isalpha
Parameters:			char c
The character to assign to it's matching column
Return value:		int
The character's state
Algorithm: THIS FUNCTION RETURNS THE COLUMN NUMBER IN THE TRANSITION
TABLE st_table FOR THE INPUT CHARACTER c.
SOME COLUMNS MAY REPRESENT A CHARACTER CLASS .
FOR EXAMPLE IF COLUMN 1 REPRESENTS[A - Z]
THE FUNCTION RETURNS 1 EVERY TIME c IS ONE
OF THE LETTERS A, B, ..., Z.
*******************************************************************************/

int char_class(char c)
{
	unsigned char symbol = c;
	int value = 0;

	/*		THIS FUNCTION RETURNS THE COLUMN NUMBER IN THE TRANSITION
	TABLE st_table FOR THE INPUT CHARACTER c.
	SOME COLUMNS MAY REPRESENT A CHARACTER CLASS .
	FOR EXAMPLE IF COLUMN 1 REPRESENTS[A - Z]
	THE FUNCTION RETURNS 1 EVERY TIME c IS ONE
	OF THE LETTERS A, B, ..., Z.*/
	if (symbol == 'A' || symbol == 'B' || symbol == 'C' || symbol == 'D' || symbol == 'E' || symbol == 'F') value = 0;
	else if (symbol == 'x') value = 1;
	else if (isalpha(symbol)) value = 2;
	else if (symbol == '0') value = 3;
	else if (symbol >= '1' && symbol <= '9') value = 4;
	else if (symbol == '.') value = 5;
	else if (symbol == '$') value = 6;
	else value = 7;

	return value;
}

/*******************************************************************************
Purpose:	ACCEPTING FUNCTION FOR THE arithmentic variable identifier AND keywords(VID - AVID / KW)
Author:				Franck Nana
History/Versions:	1.0
Called functions:	strlen()
Parameters:			char * lexeme
A lexeme predefined as an AVID or KEYWORD
Return value:		Token
A fully constructor AVID or KEYWORD token
Algorithm:			Cross reference lexeme against known keywords and generate a
WHEN CALLED THE FUNCTION MUST
1. CHECK IF THE LEXEME IS A KEYWORD.
IF YES, IT MUST RETURN A TOKEN WITH THE CORRESPONDING ATTRIBUTE
FOR THE KEYWORD.THE ATTRIBUTE CODE FOR THE KEYWORD
IS ITS INDEX IN THE KEYWORD LOOKUP TABLE(kw_table in table.h).
IF THE LEXEME IS NOT A KEYWORD, GO TO STEP 2.

2. SET a AVID TOKEN.
IF THE lexeme IS LONGER than VID_LEN(see token.h) CHARACTERS,
ONLY FIRST VID_LEN CHARACTERS ARE STORED
INTO THE VARIABLE ATTRIBUTE ARRAY vid_lex[](see token.h) .
ADD \0 AT THE END TO MAKE A C - type STRING.
return t;
*******************************************************************************/
Token aa_func02(char * lexeme) {

	Token t;
	int length;

	/*Is keyword ? */
	for (int i = 0; i < KWT_SIZE; i++) {
		if (strcmp(lexeme, kw_table[i]) == 0) {
			t.code = KW_T;
			t.attribute.kwt_idx = i;
			return t;
		}
	}

	t.code = AVID_T;  /*Set AVID_T token*/

	length = strlen(lexeme);
	int i = 0;
	while (i < length && i < VID_LEN) {
		t.attribute.vid_lex[i] = lexeme[i];
		i++;
	}
	t.attribute.vid_lex[i] = '\0';
	return t;
}

/*******************************************************************************
Purpose:		ACCEPTING FUNCTION FOR THE string variable identifier(VID - SVID)
Author:				Vicken Aharonian
History/Versions:	1.0
Called functions:	strlen()
Parameters:			char * lexeme
SVID lexeme
Return value:		Token
A fully constructor SVID token
Algorithm:			WHEN CALLED THE FUNCTION MUST
1. SET a SVID TOKEN.
IF THE lexeme IS LONGER than VID_LEN characters,
ONLY FIRST VID_LEN - 1 CHARACTERS ARE STORED
INTO THE VARIABLE ATTRIBUTE ARRAY vid_lex[],
AND THEN THE $ CHARACTER IS APPENDED TO THE NAME.
ADD \0 AT THE END TO MAKE A C - type STRING.
*******************************************************************************/

Token aa_func03(char * lexeme) {

	Token t;
	int i = 0;
	int length = strlen(lexeme);

	t.code = SVID_T;  /*Set SVID_T token*/

					  /*lexeme has valid length ? */
	if (length > VID_LEN) {

		while (i < length && i < VID_LEN - 1) {
			t.attribute.vid_lex[i] = lexeme[i];
			i++;
		}
		t.attribute.vid_lex[i++] = '$'; /*Add variable token*/
		t.attribute.vid_lex[i] = '\0';
	}
	else {
		/* lexeme <  VID_LEN ?*/
		while (i < length) {
			t.attribute.vid_lex[i] = lexeme[i];
			i++;
		}
		t.attribute.vid_lex[i] = '\0';
	}
	return t;
}

/*******************************************************************************
Purpose:		ACCEPTING FUNCTION FOR THE integer literal(IL)-decimal constant(DIL)
Author:				Franck Nana
History/Versions:	1.0
Called functions:	atol()
aa_func12()
Parameters:			char * lexeme
A lexeme predefined as an integer literal
Return value:		Token
A fully constructor INL_T token
Algorithm:		THE FUNCTION MUST CONVERT THE LEXEME REPRESENTING A DECIMAL CONSTANT
TO A DECIMAL INTEGER VALUE, WHICH IS THE ATTRIBUTE FOR THE TOKEN.
THE VALUE MUST BE IN THE SAME RANGE AS the value of 2 - byte integer in C.
*******************************************************************************/

Token aa_func05(char * lexeme) {

	Token t;
	long integer;
	/*Lexeme length > INL_LEN ? */
	if (strlen(lexeme) > INL_LEN) {
		return aa_func12(lexeme);
	}

	integer = atol(lexeme);/*Conversion */\
		if (integer >= 0 && integer <= SHRT_MAX) {
			t.code = INL_T;
			t.attribute.int_value = integer;
			return t;
		}

	t = aa_func12(lexeme);

	return t;

}

/*******************************************************************************
Purpose:
ACCEPTING FUNCTION FOR THE floating - point literal (FPL)
Author:				Franck Nana
History/Versions:	1.0
Called functions:	atof()
aa_func12()
Parameters:			char * lexeme
A floating-point literal lexeme
Return value:		Token
FPL_T token
Algorithm :

THE FUNCTION MUST CONVERT THE LEXEME TO A FLOATING POINT VALUE,
WHICH IS THE ATTRIBUTE FOR THE TOKEN.
THE VALUE MUST BE IN THE SAME RANGE AS the value of 4 - byte float in C.
*******************************************************************************/
Token aa_func08(char * lexeme) {

	Token t;
	double convert = 0.0;
	convert = atof(lexeme);

	if ((convert > FLT_MAX) || ((convert != 0.0) && (convert < FLT_MIN))) { /* Overflow error */
		return aa_table[ES](lexeme);
	}

	t.code = FPL_T;
	t.attribute.flt_value = (float)convert;

	return t;
}

/*******************************************************************************
Purpose:			ACCEPTING FUNCTION FOR THE integer literal(IL)-hexadecimal constant(HIL)
Author:				Vicken Aharonian
History/Versions:	1.0
Called functions:	atolh()
aa_func12()
Parameters:			char * lexeme
A lexeme predefined as an hexadecimal literal
Return value:		Token
INL_T token
Algorithm:			THE FUNCTION MUST CONVERT THE LEXEME REPRESENTING AN HEXADECIMAL CONSTANT
TO A DECIMAL INTEGER VALUE WHICH IS THE ATTRIBUTE FOR THE TOKEN.
THE VALUE MUST BE IN THE SAME RANGE AS the value of 2 - byte integer in C.

*******************************************************************************/
Token aa_func11(char * lexeme) {
	Token t;
	long hexadecimal = atolh(lexeme);

	if (strlen(lexeme) > 2 && hexadecimal >= 0 && hexadecimal <= SHRT_MAX) {

		t.code = INL_T;
		t.attribute.int_value = (int)hexadecimal;
		return t;
	}

	t = aa_func12(lexeme);

	return t;

}

/*******************************************************************************
Purpose:			ACCEPTING FUNCTION FOR THE ERROR TOKEN
Author:				Vicken Aharonian
History/Versions:	1.0
Called functions:	strlen()
Parameters:			char * lexeme
A lexeme predefined as an erroneous lexeme
Return value:		Token
A fully constructor ERR_T token
Algorithm:			THE FUNCTION SETS THE ERROR TOKEN.lexeme[] CONTAINS THE ERROR
THE ATTRIBUTE OF THE ERROR TOKEN IS THE lexeme ITSELF
AND IT MUST BE STORED in err_lex.IF THE ERROR lexeme IS LONGER
than ERR_LEN characters, ONLY THE FIRST ERR_LEN - 3 characters ARE
STORED IN err_lex.THEN THREE DOTS ... ARE ADDED TO THE END OF THE
err_lex C - type string.
BEFORE RETURNING THE FUNCTION MUST SET THE APROPRIATE TOKEN CODE
*******************************************************************************/
Token aa_func12(char * lexeme) {

	Token t;
	int i = 0;
	int length = 0;
	int lexemeLength = strlen(lexeme);

	t.code = ERR_T;  /*Set error state*/

					 /* Bigger then ERROR_LEN ? leave three spot for the dots */
	if (lexemeLength > ERR_LEN) {
		length = ERR_LEN - 3;

		while (i < length) {
			t.attribute.err_lex[i] = lexeme[i];
			i++;
		}

		for (i; i < ERR_LEN; i++) {
			t.attribute.err_lex[i] = '.'; /*Add the dots*/
		}
		t.attribute.err_lex[i] = '\0'; /*Add null terminated string*/
	}
	else {
		/*Valid length ? copy lexeme*/
		length = lexemeLength;
		while (i < length) {
			t.attribute.err_lex[i] = lexeme[i];
			i++;
		}
		t.attribute.err_lex[i] = '\0';  /*Add null terminated string*/
	}
	return t;
}

/*******************************************************************************
Purpose:			To convert an hexadecimal literal into a long integer
Author:				Franck Nana
History/Versions:	1.0
Called functions:	strtol()
Parameters:			char * lexeme
A lexeme predefined as a hexadecimal literal
Return value:		long
The hexadecimal expressed as a long integer
Algorithm:  use strtol to convert
*******************************************************************************/
long atolh(char * lexeme) {

	return strtol(lexeme, NULL, 0);   /* Convert to hexadevimal*/

}
