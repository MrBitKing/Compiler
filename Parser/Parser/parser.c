/*
* File Name: parser.c
* Version: 1
* Compiler: MS Visual Studio 2015
* Author: Nana Ngassa Franck, 040 783 398
*		  Vicken Aharonian, 040 802 638
* Course: CST 8152 – Compilers, Lab Section: 011
* Assignment: 3
* Date: April 21, 2017
* Professor: Svillen Ranev
* Purpose: To parse tokens in from a given program in a file
* Function list: variable_identifier(), iteration_statement(), input_statement(), output_statement(), write_outputs(), additive_arithmetic_expression(),  parser(Buffer*), match(int, int ), syn_eh(int), syn_printe(), gen_incode(char*), program(void), opt_statements(), statements(),
* statement(), statements_p(), assignment_statement(), primary_aithmetic_expression(), additive_arithmetic_expression_p(),
* multiplicative_arithmetic_expression(), multiplicative_arithmetic_expression_p(), string_expression(), primary_string_expression(), string_expression_p(),
* selection_statement(), variable_list(), variable_list_p(), assignment_expression(), conditional_expression(), logical_OR_expression(),
* logical_OR_expression_p(), logical_AND_expression(), logical_AND_expression_p(), relational_expression(), primary_a_relational_expression(),
* primary_a_relational_expression_p(), primary_s_relational_expression(), primary_s_relational_expression_p(), arithmetic_expression(),
* unary_arithmetic_expression(),
*/

#include <stdlib.h>
#include "parser.h"

/*
* Purpose: Start Parsing what's in the buffer
* Author: Nana Ngassa Franck, 040 783 398
*		  Vicken Aharonian, 040 802 638
* Versions: 1
* Called Functions: malar_next_token(), program(), match(), gen_incode()
* Parameters: Buffer * sc_buf
* Return value: N/A
* Algorithm: start the program
*/
void parser(Buffer* in_buff) {
	sc_buf = in_buff;
	lookahead = malar_next_token(sc_buf);
	program();
	match(SEOF_T, NO_ATTR);
	gen_incode("PLATY: Source file parsed");
}

/*
* Purpose: Match each token according to our grammar
* Author: Nana Ngassa Franck, 040 783 398
*		  Vicken Aharonian, 040 802 638
* Versions: 1
* Called Functions: syn_eh(), syn_printe(), malar_next_token()
* Parameters: int pr_token_code, int pr_token_attribute
* Return value: returns to the program
* Algorithm: 1. Was there a match ?
*            2. Parse tokens code whjile checking if we are at SEOF_T
*			 3. lookahead token is ERR_T ?
*/
void match(int pr_token_code, int pr_token_attribute) {

	/*if match is unsuccessful, call error handler function*/
	if (lookahead.code != pr_token_code) {
		syn_eh(pr_token_code);
		return;
	}

	/*if match is successful and the lookahead.code is SEOF_T, return*/
	if (lookahead.code == SEOF_T)
		return;

	/*parse the current token code*/
	if (pr_token_code == KW_T) {
		if (pr_token_attribute != lookahead.attribute.kwt_idx) {
			syn_eh(pr_token_code);
			return;
		}
	}
	else if (pr_token_code == LOG_OP_T || pr_token_code == ART_OP_T || pr_token_code == REL_OP_T) {
		if (pr_token_attribute != lookahead.attribute.get_int) {
			syn_eh(pr_token_code);
			return;
		}
	}

	/*if match is successful and the lookahead.code is NOT SEOF_T, advance to next token*/
	lookahead = malar_next_token(sc_buf);

	/*if the next token is error token, get next token and increment runtime error counter*/
	if (lookahead.code == ERR_T) {
		syn_printe();
		lookahead = malar_next_token(sc_buf);
		++synerrno;
		return;
	}
}

/*
* Purpose: Panic mode error recovery function
* Author: Nana Ngassa Franck, 040 783 398
*		  Vicken Aharonian, 040 802 638
* Versions: 1
* Called Functions: syn_printe(), malar_next_token(), exit()
* Parameters: int sync_token_code
* Return value: return to the program
* Algorithm: 1. increment synerror and print them
*            2. advance the input token
*		     3. end of file yet ?
*    		 4. find matching tokens
*/
void syn_eh(int sync_token_code) {

	/*increment runtime error counter and print error message*/
	syn_printe();
	++synerrno;

	/*error recovery*/
	do {
		/*advance the input token*/
		lookahead = malar_next_token(sc_buf);

		/*find the token code that matches sync_token_code*/
		if (lookahead.code == sync_token_code) {
			lookahead = malar_next_token(sc_buf);
			return;
		}

		/*detect source end of file*/
		else if (lookahead.code == SEOF_T) {
			exit(synerrno);
			return;
		}
		/*perform this do-while while loookahead.code does not equal sync_token_code*/
	} while (lookahead.code != sync_token_code);
}

/*
* Purpose: Print error messages
* Author: Svillen Ranev
* Versions: 1
* Called Functions: printf()
* Parameters: N/A
* Return value: N/A
* Algorithm: print error messages with details
*/
/* error printing function for Assignment 3 (Parser), W18 */
void syn_printe() {
	Token t = lookahead;

	printf("PLATY: Syntax error:  Line:%3d\n", line);
	printf("*****  Token code:%3d Attribute: ", t.code);
	switch (t.code) {
	case  ERR_T: /* ERR_T     0   Error token */
		printf("%s\n", t.attribute.err_lex);
		break;
	case  SEOF_T: /*SEOF_T    1   Source end-of-file token */
		printf("NA\n");
		break;
	case  AVID_T: /* AVID_T    2   Arithmetic Variable identifier token */
	case  SVID_T:/* SVID_T    3  String Variable identifier token */
		printf("%s\n", t.attribute.vid_lex);
		break;
	case  FPL_T: /* FPL_T     4  Floating point literal token */
		printf("%5.1f\n", t.attribute.flt_value);
		break;
	case INL_T: /* INL_T      5   Integer literal token */
		printf("%d\n", t.attribute.get_int);
		break;
	case STR_T:/* STR_T     6   String literal token */
		printf("%s\n", b_location(str_LTBL, t.attribute.str_offset));
		break;

	case SCC_OP_T: /* 7   String concatenation operator token */
		printf("NA\n");
		break;

	case  ASS_OP_T:/* ASS_OP_T  8   Assignment operator token */
		printf("NA\n");
		break;
	case  ART_OP_T:/* ART_OP_T  9   Arithmetic operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  REL_OP_T: /*REL_OP_T  10   Relational operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  LOG_OP_T:/*LOG_OP_T 11  Logical operator token */
		printf("%d\n", t.attribute.get_int);
		break;

	case  LPR_T: /*LPR_T    12  Left parenthesis token */
		printf("NA\n");
		break;
	case  RPR_T: /*RPR_T    13  Right parenthesis token */
		printf("NA\n");
		break;
	case LBR_T: /*    14   Left brace token */
		printf("NA\n");
		break;
	case RBR_T: /*    15  Right brace token */
		printf("NA\n");
		break;

	case KW_T: /*     16   Keyword token */
		printf("%s\n", kw_table[t.attribute.get_int]);
		break;

	case COM_T: /* 17   Comma token */
		printf("NA\n");
		break;
	case EOS_T: /*    18  End of statement *(semi - colon) */
		printf("NA\n");
		break;
	default:
		printf("PLATY: Scanner error: invalid token code: %d\n", t.code);
	}/*end switch*/
}/* end syn_printe()*/

 /*
 * Purpose: print the string argument
 * Author: Nana Ngassa Franck, 040 783 398
 *		  Vicken Aharonian, 040 802 638
 * Versions: 1
 * Called Functions: printf()
 * Parameters: char *string
 * Return value: N/A
 * Algorithm: Print the string passed as argument
 */
void gen_incode(char* string) {
	/*prints the string in parameter*/
	printf("%s\n", string);
}

/*
* <program> -> PLATYPUS {<opt_statements>} SEOF
*
* FIRST(<program>) = { KW_T(PLATYPUS) }
*/
void program(void) {
	match(KW_T, PLATYPUS);
	match(LBR_T, NO_ATTR);

	opt_statements();

	match(RBR_T, NO_ATTR);
	gen_incode("PLATY: Program parsed");
}

/*
* <opt_statements> -> <statements> | ϵ
*
* FIRST(<opt_statements>) = { FIRST (<statements>) }
*					      = { SVID_T, AVID_T, KW_T(READ), KW_T(WRITE), KW_T(WHILE), KW_T(IF), ϵ }
*/

void opt_statements() {

	if (lookahead.code == SVID_T || lookahead.code == AVID_T) {
		statements();
		return;
	}

	else if (lookahead.code == KW_T) {

		if (lookahead.attribute.get_int != PLATYPUS
			&& lookahead.attribute.get_int != ELSE
			&& lookahead.attribute.get_int != THEN
			&& lookahead.attribute.get_int != REPEAT) {
			statements();
			return;
		}
	}

	gen_incode("PLATY: Opt_statements parsed");

}

/*
* <statements’> -> <statement> < statements’> | ϵ
*
* FIRST (<statements’>) = { SVID_T, AVID_T, KW_T(READ), KW_T(WRITE), KW_T(WHILE), KW_T(IF), ϵ }
*/
void statements(void) {
	statement();
	statements_p();
}

/*
* <statement> ->
* <assignment statement>
* | <selection statement>
* | <iteration statement>
* | <input statement>
* | <output statement>
*
* FIRST (<statement>) = { FIRST(<assignment statement>), FIRST(<selection statements>), FIRST(<iteration statement>), FIRST(<input statement>), FIRST(<output statement>) }
*  					  = { SVID_T, AVID_T, KW_T(READ), KW_T(WRITE), KW_T(WHILE), KW_T(IF) }
*/

void statement(void) {

	if (lookahead.code == SVID_T || lookahead.code == AVID_T) {
		assignment_statement();
		return;
	}

	else if (lookahead.code == KW_T) {

		if (lookahead.attribute.get_int == IF) selection_statement();
		else if (lookahead.attribute.get_int == WHILE) iteration_statement();
		else if (lookahead.attribute.get_int == READ) input_statement();
		else if (lookahead.attribute.get_int == WRITE) output_statement();
		return;
	}

	else syn_printe();

}

/*
* <assignment statement> -> <assignment expression>;
*
* FIRST (<assignment statement>) = {FIRST(<assignment expression>) }
* 							     = { AVID_T , SVID_T }
*/
void assignment_statement(void) {

	assignment_expression();
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Assignment statement parsed");
}

/*
* < assignment expression> ->
* AVID = <arithmetic expression>
* | SVID = <string expression>
*
* FIRST (<assignment expression>) = {FIRST(<assignment statement>) }
*								  = { AVID_T , SVID_T }
*/
void assignment_expression(void) {

	if (lookahead.code == AVID_T) {

		match(AVID_T, NO_ATTR);
		match(ASS_OP_T, EQ);
		arithmetic_expression();
		gen_incode("PLATY: Assignment expression (arithmetic) parsed");
		return;
	}

	else if (lookahead.code == SVID_T) {

		match(SVID_T, NO_ATTR);
		match(ASS_OP_T, EQ);
		string_expression();
		gen_incode("PLATY: Assignment expression (string) parsed");
		return;
	}

	else syn_printe();
}

/*
* <selection statement> ->
* IF <pre-condition> (<conditional expression>) THEN { <opt_statements> }
* ELSE { <opt_statements> } ;
*
* FIRST(<selection statement>) = { KW_T(IF) }
*/

void selection_statement(void) {

	match(KW_T, IF);
	precondition();
	match(LPR_T, NO_ATTR);
	conditional_expression();
	match(RPR_T, NO_ATTR);

	match(KW_T, THEN);
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);

	match(KW_T, ELSE);
	match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);

	gen_incode("PLATY: Selection statement parsed");

}

/*
*<pre - condition> -> TRUE | FALSE
*
*/

void precondition() {

	/*check if lookahead is a keyword*/
	if (lookahead.code == KW_T) {
		if (lookahead.attribute.kwt_idx == TRUE)
			match(KW_T, TRUE);
		else if (lookahead.attribute.kwt_idx == FALSE)
			match(KW_T, FALSE);
	}

	/*else print error message*/
	else syn_printe();
}

/*
* <iteration statement> ->
* WHILE <pre-condition> (<conditional expression>)
* REPEAT { <statements>};
*
* FIRST(<selection statement>) = { KW_T(WHILE) }
* <pre-condition> ->
* TRUE | FALSE
*/

void iteration_statement(void) {

	match(KW_T, WHILE);

	precondition();

	match(LPR_T, NO_ATTR);
	conditional_expression();
	match(RPR_T, NO_ATTR);
	match(KW_T, REPEAT);
	match(LBR_T, NO_ATTR);

	statements();

	match(RBR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Iteration statement parsed");
}

/*
* <statements’> -> <statement> < statements’> | ϵ
*
* FIRST (<statements’>) = { SVID_T, AVID_T, KW_T(READ), KW_T(WRITE), KW_T(WHILE), KW_T(IF), ϵ }
*
*/

void statements_p(void) {

	if (lookahead.code == SVID_T || lookahead.code == AVID_T) {
		statements();
		statements_p();
		return;
	}

	else if (lookahead.code == KW_T) {

		if (lookahead.attribute.get_int != PLATYPUS
			&& lookahead.attribute.get_int != ELSE
			&& lookahead.attribute.get_int != THEN
			&& lookahead.attribute.get_int != REPEAT) {
			statements();
			return;
		}
	}

	return;
}

/*
* <arithmetic expression> -> <unary arithmetic expression> | <additive arithmetic expression>
*
* FIRST(<arithmetic expression>) = {-,+,AVID_T,FPL_T,INL_T, ( }
*/

void arithmetic_expression(void) {

	if (lookahead.code == ART_OP_T) {
		if (lookahead.attribute.arr_op == PLUS || lookahead.attribute.arr_op == MINUS) {
			unary_arithmetic_expression();
		}
		else {
			syn_printe();
			return;
		}
		gen_incode("PLATY: Arithmetic expression parsed");
		return;

	}

	else if (lookahead.code == AVID_T || lookahead.code == FPL_T ||
		lookahead.code == INL_T || lookahead.code == LPR_T) {
		additive_arithmetic_expression();
		gen_incode("PLATY: Arithmetic expression parsed");
		return;
	}

	syn_printe();
	return;
}

/*
* <unary arithmetic expression> -> - <primary arithmetic expression> | + <primary arithmetic expression>
*
* FIRST(<unary arithmetic expression>) = { -, + }
*/
void unary_arithmetic_expression(void) {

	if (lookahead.code == ART_OP_T) {
		if (lookahead.attribute.arr_op == PLUS) {
			match(ART_OP_T, PLUS);
			primary_arithmetic_expression();
		}
		else if (lookahead.attribute.arr_op == MINUS) {
			match(ART_OP_T, MINUS);
			primary_arithmetic_expression();
		}
		else {
			syn_printe();
			return;
		}
		gen_incode("PLATY: Unary arithmetic expression parsed");
		return;
	}
}

/*
* <additive arithmetic expression> -> <multiplicative arithmetic expression> <additive arithmetic expression’>
*
* FIRST(<additive arithmetic expression>) = { AVID_T, FPL_T, INL_T, ( }
*/

void additive_arithmetic_expression(void) {
	multiplicative_arithmetic_expression();
	additive_arithmetic_expression_p();
}

/*
* <additive arithmetic expression’> ->
* + <multiplicative arithmetic expression> <additive arithmetic expression’>
* | - <multiplicative arithmetic expression> <additive arithmetic expression’>
* | ϵ
*
* FIRST(<additive arithmetic expression’>) = { +, -, e }
*/

void additive_arithmetic_expression_p(void) {

	/* look for the tokens */
	if (lookahead.code == ART_OP_T) {
		if (lookahead.attribute.arr_op == PLUS) {
			match(ART_OP_T, PLUS);
			multiplicative_arithmetic_expression();
			additive_arithmetic_expression_p();
			gen_incode("PLATY: Additive arithmetic expression parsed");
			return;
		}
		else if (lookahead.attribute.arr_op == MINUS) {
			match(ART_OP_T, MINUS);
			multiplicative_arithmetic_expression();
			additive_arithmetic_expression_p();
			gen_incode("PLATY: Additive arithmetic expression parsed");
			return;
		}
		syn_printe();
		return;
	}
}

/*
* <multiplicative arithmetic expression> -> <primary arithmetic expression> <multiplicative arithmetic expression’>
*
* FIRST(<multiplicative arithmetic expression>) = { AVID_T, FPL_T, INL_T, ( }
*/

void multiplicative_arithmetic_expression(void) {
	primary_arithmetic_expression();
	multiplicative_arithmetic_expression_p();
}

/*
* <multiplicative arithmetic expression’>
* * <primary arithmetic expression><multiplicative arithmetic expression’>
* | / <primary arithmetic expression><multiplicative arithmetic expression’>
* | ϵ
*
* FIRST(<multiplicative arithmetic expression’>) = {*,/,ϵ}
*/

void multiplicative_arithmetic_expression_p(void) {

	if (lookahead.code == ART_OP_T) {
		if (lookahead.attribute.arr_op == MULT) {
			match(ART_OP_T, MULT);
			primary_arithmetic_expression();
			multiplicative_arithmetic_expression_p();
			gen_incode("PLATY: Multiplicative arithmetic expression parsed");
			return;
		}
		else if (lookahead.attribute.arr_op == DIV) {
			match(ART_OP_T, DIV);
			primary_arithmetic_expression();
			multiplicative_arithmetic_expression_p();
			gen_incode("PLATY: Multiplicative arithmetic expression parsed");
			return;
		}
	}
}

/*
* <primary arithmetic expression> ->
* AVID_T
* | FPL_T
* | INL_T
* | (<arithmetic expression>)
*
* FIRST(<primary arithmetic expression>) = { AVID_T, FPL_T, INL_T, ( }
*/

void primary_arithmetic_expression(void) {

	if (lookahead.code == AVID_T || lookahead.code == FPL_T || lookahead.code == INL_T) {
		match(lookahead.code, NO_ATTR);
	}

	else if (lookahead.code == LPR_T) {
		match(LPR_T, NO_ATTR);
		arithmetic_expression();
		match(RPR_T, NO_ATTR);
	}

	gen_incode("PLATY: Primary arithmetic expression parsed");
}

/*
* <string expression> -> <primary string expression> <string expression’>
*
* FIRST(<string expression>) = { SVID_T, STR_T }
*/

void string_expression(void) {
	primary_string_expression();
	string_expression_p();
	gen_incode("PLATY: String expression parsed");
}

/*
* <string expression’> -> # <primary string expression> <string expression’> | ϵ
*
* FIRST(<string expression’>) = { #, ϵ }
*/

void string_expression_p(void) {

	if (lookahead.code == SCC_OP_T) {
		match(SCC_OP_T, NO_ATTR);
		primary_string_expression();
		string_expression_p();
		return;
	}
}

/*
* <primary string expression> -> SVID_T | STR_T
*
* FIRST(<primary string expression>) = {SVID_T, STR_T}
*/

void primary_string_expression(void) {

	if (lookahead.code == SVID_T) {
		match(SVID_T, NO_ATTR);
	}
	else if (lookahead.code == STR_T) {
		match(STR_T, NO_ATTR);
	}
	gen_incode("PLATY: Primary string expression parsed");
}


/*
* <conditional expression> -> <logical OR expression>
*
* FIRST(<conditional expression>) = {FIRST(<logical OR expression>)}
*								  = { AVID_T, FPL_T, STR_T, SVID_T, INL_T }
*/

void conditional_expression(void) {
	logical_or_expression();
	gen_incode("PLATY: Conditional expression parsed");
}

/*
* <logical AND expression> -> <relational expression> <logical AND expression’>
*
* FIRST (<logical AND expression>) = {FIRST(<relational expression>)}
*								   = { AVID_T, FPL_T, STR_T, SVID_T, INL_T }
*/

void logical_and_expression(void) {
	relational_expression();
	logical_and_expression_p();
}

/*
* <logical AND expression’> ->	.AND. <relational expression> <logical AND expression’>
*
* FIRST(<logical AND expression’>) = {.AND. , ϵ  }
*/

void logical_and_expression_p(void) {

	if (lookahead.code == LOG_OP_T) {

		if (lookahead.attribute.log_op == AND) {
			match(LOG_OP_T, AND);
			relational_expression();
			logical_and_expression_p();
			gen_incode("PLATY: Logical AND expression parsed");
			return;
		}
	}

}

/*
* <logical OR expression> -> <logical AND expression> <logical OR expression’>
*
* FIRST(<logical OR expression>) = { AVID_T, FPL_T, INL_T, SVID_T, STR_T }
*/

void logical_or_expression(void) {
	logical_and_expression();
	logical_or_expression_p();
}

/*
* <logical OR expression’> -> .OR. <logical AND expression> <logical OR expression’>
*
* FIRST(<logical OR expression’>) = { .OR. , ϵ }
*/

void logical_or_expression_p(void) {

	if (lookahead.code == LOG_OP_T) {
		if (lookahead.attribute.log_op == OR) {
			match(LOG_OP_T, OR);
			logical_and_expression();
			logical_or_expression_p();
			gen_incode("PLATY: Logical OR expression parsed");
			return;
		}
	}
	return;
}


/*
* <relational expression> -> <relational expression> ->
* <primary a_relational expression> <primary a_relational expression’> |
* <primary s_relational expression> <primary s_relational expression’
*
* FIRST(<relational expression>) = { AVID_T, FPL_T, INL_T, SVID_T, STR_T }
*/
void relational_expression(void) {

	if (lookahead.code == AVID_T || lookahead.code == FPL_T || lookahead.code == INL_T) {
		primary_a_relational_expression();
		primary_a_relational_expression_p();
	}
	else if (lookahead.code == SVID_T || lookahead.code == STR_T) {
		primary_s_relational_expression();
		primary_s_relational_expression_p();
	}

	else {
		syn_printe();
	}
	gen_incode("PLATY: Relational expression parsed");
}

/*
* <primary a_relational expression> -> AVID_T | FPL_T | INL_T
*
* FIRST(<primary a_relational expression>) = { AVID_T, FPL_T, INL_T }
*/

void primary_a_relational_expression(void) {

	if (lookahead.code == AVID_T || lookahead.code == FPL_T || lookahead.code == INL_T) {
		match(lookahead.code, NO_ATTR);
		gen_incode("PLATY: Primary a_relational expression parsed");
	}
	else if (lookahead.code == SVID_T || lookahead.code == STR_T) {
		syn_printe();
		gen_incode("PLATY: Primary a_relational expression parsed");
	}

	else {
		syn_printe();
	}
}


/*
* <primary a_relational expression’> -> == <primary a_relational expression> | <> <primary a_relational expression>
* | > <primary a_relational expression> | < <primary a_relational expression>
*
* FIRST (<primary a_relational expression’>) = { ==, <>, >, < }
*/

void primary_a_relational_expression_p(void) {


	if (lookahead.code == REL_OP_T) {

		if (lookahead.attribute.rel_op == EQ || lookahead.attribute.rel_op == NE ||
			lookahead.attribute.rel_op == GT || lookahead.attribute.rel_op == LT)
			match(REL_OP_T, lookahead.attribute.rel_op);
		primary_a_relational_expression();
		return;
	}
	else
	{
		syn_printe();
		return;
	}
	return;
}


/*
* <primary s_relational expression> -> <primary string expression>
*
* FIRST(<primary s_relational_expression>) = { SVID_T, STR_T }
*/

void primary_s_relational_expression(void) {

	if (lookahead.code == SVID_T || lookahead.code == STR_T) {
		primary_string_expression();
		gen_incode("PLATY: Primary s_relational expression parsed");
		return;
	}

	else if (lookahead.code == AVID_T || lookahead.code == INL_T ||
		lookahead.code == FPL_T) {

		syn_printe();
		gen_incode("PLATY: Primary s_relational expression parsed");
		return;
	}
	syn_printe();
	return;
}

/*
* <primary s_relational expression’> ->
* == <primary s_relational expression> |
* <> <primary s_relational expression> |
* > <primary s_relational expression> |
* < <primary s_relational expression> |
*
* FIRST (<primary s_relational expression’>) = { ==, <>, >, < }
*/

void primary_s_relational_expression_p(void) {

	if (lookahead.code == REL_OP_T) {
		if (lookahead.attribute.rel_op == EQ || lookahead.attribute.rel_op == GT ||
			lookahead.attribute.rel_op == NE || lookahead.attribute.rel_op == LT) {
			match(REL_OP_T, lookahead.attribute.rel_op);
			primary_s_relational_expression();
			return;
		}
		syn_printe();
		return;
	}
	else {
		return;
	}
}


/*
* <input statement> -> READ (<variable list>);
*
* FIRST(<input statement>) = { KW_T(READ) }
*
*/
void input_statement(void) {

	match(KW_T, READ);
	match(LPR_T, NO_ATTR);

	variable_list();

	match(RPR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Input statement parsed");
}

/*
* <variable list> -> <variable identifier> <variable list’>
*
* FIRST(<variable list>) = { FIRST(<variable identifier’>) }
* 						 = { AVID_T, SVID_T }
*/

void variable_list(void) {
	variable_identifier();
	variable_list_p();
	gen_incode("PLATY: Variable list parsed");
}

/*
* <variable identifier> -> AVID_T | SVID_T
*
* FIRST(<variable identifier>) = { AVID_T, SVID_T }
*/

void variable_identifier(void) {

	/* look for the tokens */
	if (lookahead.code == AVID_T || lookahead.code == SVID_T) {

		match(lookahead.code, NO_ATTR);
		return;
	}
	syn_printe();
	return;
}

/*
* <variable list’> -> , <variable identifier> <variable list’> | ϵ
*
* FIRST(<variable list’>) = { COM_T , ϵ }
*/

void variable_list_p(void) {

	/* look for the comma token */
	if (lookahead.code == COM_T) {
		match(COM_T, NO_ATTR);
		variable_identifier();
		variable_list_p();
	}
}

/*
* <output statement> -> WRITE (<write outputs>);
*
* FIRST(<output statement>) = { KW_T(WRITE) }
*
*/

void output_statement(void) {
	match(KW_T, WRITE);
	match(LPR_T, NO_ATTR);

	write_outputs();

	match(RPR_T, NO_ATTR);
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Output statement parsed");
}

/*
* <write outputs> -> <variable list> | STR_T | ϵ
*
* FIRST(<write outputs>) = { FIRST<variable list> }
* 						 = { SVID_T, AVID_T, STR_T, ϵ }
*/

void write_outputs(void) {

	if (lookahead.code == AVID_T || lookahead.code == SVID_T) {
		variable_list();
		return;
	}

	else if (lookahead.code == STR_T) {
		match(STR_T, NO_ATTR);
		gen_incode("PLATY: Output list (string literal) parsed");
		return;
	}
	gen_incode("PLATY: Output list (empty) parsed");
	return;
}
