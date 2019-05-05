/* Filename: table.h */

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

#ifndef  TABLE_H_
#define  TABLE_H_ 

#ifndef BUFFER_H_
#include "buffer.h"
#endif

#ifndef NULL
#include <_null.h> /* NULL pointer constant is defined there */
#endif

/*   Source end-of-file (SEOF) sentinel symbol
*    '\0' or one of 255,0xFF,EOF
*/

/*  Special case tokens processed separately one by one
*  in the token-driven part of the scanner
*  '=' , ' ' , '(' , ')' , '{' , '}' , == , <> , '>' , '<' , ';',
*  white space
*  !!comment , ',' , '"' , ';' , '-' , '+' , '*' , '/', # ,
*  .AND., .OR. , SEOF, 'illigal symbol',
*/


#define ER   13
#define ES   12  /* Error state */
#define IS	 -1    /* Inavalid state */

/* State transition table definition */

#define TABLE_COLUMNS 8
/*transition table - type of states defined in separate table */


int st_table[][TABLE_COLUMNS] = {


	/*					     A-F   x  az-AZ  0    1-9  .    $   other	*/
	/* Current state 0 */{ 1,   1,   1,   6,   4,   ES,  ES,  ES },		/* NOAS */
	/* Current state 1 */{ 1,   1,   1,   1,   1,   2,   3,   2 },		/* NOAS */
	/* Current state 2 */{ IS,  IS,  IS,  IS,  IS,  IS,  IS,  IS },		/* ASWR */
	/* Current state 3 */{ IS,  IS,  IS,  IS,  IS,  IS,  IS,  IS },		/* ASNR */
	/* Current state 4 */{ ES,  ES,  ES,  4,   4,   7,   5,   5 },		/* NOAS */
	/* Current state 5 */{ IS,  IS,  IS,  IS,  IS,  IS,  IS,  IS },		/* ASWR */
	/* Current state 6 */{ ES,  9,   ES,  6,   ES,  7,   ES,  5 },		/* NOAS */
	/* Current state 7 */{ ES,  8,   8,   7,   7,   8,   8,   8 },		/* NOAS */
	/* Current state 8 */{ IS,  IS,  IS,  IS,  IS,  IS,  IS,  IS },		/* ASWR */
	/* Current state 9 */{ 10,  ES,  ES,  10,  10,  ES,  ES,  11 },		/* NOAS */
	/* Current state 10 */{ 10,  ES,  ES,  10,  10,  ES,  ES,  11 },		/* NOAS */
	/* Current state 11 */{ IS,  IS,  IS,  IS,  IS,  IS,  IS,  IS },		/* ASWR */
	/* Current state 12 */{ IS,  IS,  IS,  IS,  IS,  IS,  IS,  IS },		/* ASNR */
	/* Current state 13 */{ IS,  IS,  IS,  IS,  IS,  IS,  IS,  IS }		/* NOAS */

};


/* Accepting state table definition */
#define ASWR     1  /* accepting state with retract */
#define ASNR     2 /* accepting state with no retract */
#define NOAS     3  /* not accepting state */

int as_table[] = {
	NOAS, NOAS, ASWR, ASNR, NOAS, ASWR, NOAS,
	NOAS, ASWR, NOAS, NOAS, ASWR, ASNR, NOAS
};

/* Accepting action function declarations */


Token aa_func02(char *lexeme);  /*VID AVID/ KW - ASWR*/
Token aa_func03(char *lexeme);  /*VID SVID - ASWR*/
Token aa_func05(char *lexeme);  /*DIL-ASWR*/
Token aa_func08(char *lexeme);  /*FLP-ASWR*/
Token aa_func11(char *lexeme);  /*ES - ASWR*/
Token aa_func12(char *lexeme);  /*ES - ASNR*/




								/* defining a new type: pointer to function (of one char * argument)
								returning Token
								*/

typedef Token(*PTR_AAF)(char *lexeme);


/* Accepting function (action) callback table (array) definition */
/* If you do not want to use the typedef, the equvalent declaration is:
* Token (*aa_table[])(char lexeme[]) = {
*/

PTR_AAF aa_table[] = {

	NULL,
	NULL,
	aa_func02,
	aa_func03,
	NULL,
	aa_func05,
	NULL,
	NULL,
	aa_func08,
	NULL,
	NULL,
	aa_func11,
	aa_func12,
	NULL

};

/* Keyword lookup table (.AND. and .OR. are not keywords) */

#define KWT_SIZE  10

char * kw_table[] =
{
	"ELSE",
	"FALSE",
	"IF",
	"PLATYPUS",
	"READ",
	"REPEAT",
	"THEN",
	"TRUE",
	"WHILE",
	"WRITE"
};

#endif
