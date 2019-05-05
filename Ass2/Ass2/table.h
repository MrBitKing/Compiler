/* Filename: table.h
* Transition Table and function declarations necessary for the scanner implementation
* as required for CST8152 - Assignment #2.
* Version: 1.18.1
* Date: 1 February 2018
* Provided by: Svillen Ranev
* The file is incomplete. You are to complete it.
***************************************************
* REPLACE THIS HEADER WITH YOUR HEADER
***************************************************
*/

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


//REPLACE *ESN* WITH YOUR ERROR STATE NUMBER
#define ES   12  /* Error state */
#define IS	 13    /* Inavalid state */

/* State transition table definition */

//REPLACE *CN* WITH YOUR COLUMN NUMBER

#define TABLE_COLUMNS 7
/*transition table - type of states defined in separate table */
//	/* State 0 */{ YOUR INITIALIZATION },
//	/* State 1 */{ YOUR INITIALIZATION },
	//.
	//.YOUR TABLE INITIALIZATION HERE
	//.
//	/* State N */  {YOUR INITIALIZATION},

int st_table[][TABLE_COLUMNS] = {

	{ 1, 6 , 4, ES, ES, ES},
    { 1, 1 , 1, ES, 3, 3},
	{ IS, IS , IS, IS, IS, IS},
	{ IS, IS , IS, IS, IS, IS},
	{ ES,  4 , 4, 7, 5, 5 },
	{ IS, IS , IS, IS, IS, IS },
	{ ES, IS , IS, 7, 5, 5},
	{ ES, 7 , 7, 8, 8, 8 },
	{ IS, IS , IS, IS, IS, IS },
	{ IS, IS , IS, IS, IS, IS },
	{ IS, IS , IS, IS, IS, IS },
	{ IS, IS , IS, IS, IS, IS },
	{ IS, IS , IS, IS, IS, IS },
	{ IS, IS , IS, IS, IS, IS },


};


	/* Accepting state table definition */
//	REPLACE *N1*, *N2*, and *N3* WITH YOUR NUMBERS
#define ASWR     1  /* accepting state with retract */
#define ASNR     2  /* accepting state with no retract */
#define NOAS     3  /* not accepting state */

int as_table[] = {  NOAS,
					NOAS,		//{ YOUR INITIALIZATION HERE - USE ASWR, ASNR, NOAS };
					ASWR,
					ASNR,
					NOAS,
					ASWR,
					NOAS,
					NOAS,
					ASWR,
					NOAS,
					ASNR,
					ASWR };







/* Accepting action function declarations */

//FOR EACH OF YOUR ACCEPTING STATES YOU MUS	T PROVIDE
//ONE FUNCTION PROTOTYPE.THEY ALL RETURN Token AND TAKE
//ONE ARGUMENT : A string REPRESENTING A TOKEN LEXEME.

Token aa_func02(char *lexeme);  /*VID AVID/ KW - ASWR*/
Token aa_func03(char *lexeme);  /*VID SVID - ASWR*/
Token aa_func05(char *lexeme);  /*DIL-ASWR*/
Token aa_func08(char *lexeme);  /*FLP-ASWR*/
Token aa_func12(char *lexeme);  /*ES - ASNR*/
Token aa_func13(char *lexeme);  /*ES - ASWR*/




//Replace XX with the number of the accepting state : 02, 03 and so on.

/* defining a new type: pointer to function (of one char * argument)
returning Token
*/

typedef Token(*PTR_AAF)(char *lexeme);


/* Accepting function (action) callback table (array) definition */
/* If you do not want to use the typedef, the equvalent declaration is:
* Token (*aa_table[])(char lexeme[]) = {
*/

PTR_AAF aa_table[] = {


	//HERE YOU MUST PROVIDE AN INITIALIZATION FOR AN ARRAY OF POINTERS
	//TO ACCEPTING FUNCTIONS.THE ARRAY HAS THE SAME SIZE AS as_table[].
	//YOU MUST INITIALIZE THE ARRAY ELEMENTS WITH THE CORRESPONDING
	//ACCEPTING FUNCTIONS(FOR THE STATES MARKED AS ACCEPTING IN as_table[]).
//	THE REST OF THE ELEMENTS MUST BE SET TO NULL.

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
	NULL,
	aa_func12,
	aa_func13,

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
