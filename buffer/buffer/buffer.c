/*******************************************************************************************
File Name: buffer.c
Compiler: MS Visual Studio 2015
Author: Nana Ngassa Franck, 040 783 398
Course: CST 8152 – Compilers, Lab Section: 011
Assignment: 1
Date: 1/11/018
Purpose: Write a buffer in c

Function list:
Buffer *b_allocate(short init_capacity, char inc_factor, char o_mode);
pBuffer b_addc(pBuffer const pBD, char symbol);
void b_free(Buffer *const pBD);
int b_isfull(Buffer *const pBD);
short b_capacity(Buffer *const pBD);
short b_mark(pBuffer const pBD, short mark);
int b_mode(Buffer * const pBD);
size_t b_incfactor(Buffer * const pBD);
char b_rflag(Buffer * const pBD);
int b_print(Buffer* const pBD);
short b_retract(Buffer * const pBD);
short b_limit(Buffer *const pBD);
int b_load(FILE * const fi, Buffer * const pBD);
Buffer * b_compact(Buffer * const pBD, char symbol);
short b_reset(Buffer * const pBD);
short b_getcoffset(Buffer * const pBD);
int b_rewind(Buffer * const pBD);
char * b_location(Buffer * const pBD, short loc_offset);
Total = 18
*********************************************************************************************/
#include "buffer.h"


/*Making sure program work on different platforms*/
/*
#define BORLAND_16
#ifdef BORLAND_16
typedef int int_2b;
typedef short short_2b;
typedef long long_4b;
#endif
#ifdef MSVS_16
typedef short int_2b;
typedef short short_2b;
typedef int long_4b;
#endif

#if PLATFORM == __MSDOS__ * 10
#define HEADER "dos.h"
#elif PLATFORM == __WIN32__ * 9
#define HEADER "win.h"
#else
#define HEADER "sys.h"
#endif
#include HEADER
*/
/* Macro */
#define FULL\
if (!pBD) return RT_FAIL1 \
pBD->capacity == pBD->addc_offset ?  1 : 0  \
#include FULL

#pragma warning(disable : 4001)

/***************************************************************************************
Purpose: Creates a new Buffer in memory
Author: Nana Ngassa Franck
History/Versions: 1
Called functions: calloc(), malloc(), free() sixeof() b_free
Parameters:
short init_capacity (0 to SHRT_MAX)
char inc_factor
char o_mode ('f', 'a', 'm')
Return value:	A pointer to a Buffer, or NULL on error
Algorithm:		 Verifies valid parameters and allocates necessary initial memory
before returning a pointer to a Buffer, set the operational mode and the according increment factor.
Update the capacity variable
****************************************************************************************/

Buffer* b_allocate(short init_capacity, char inc_factor, char o_mode) {

	Buffer* buffer = NULL;
	if (init_capacity >= 0 && init_capacity <= SHRT_MAX - 1) {

		buffer = (Buffer*)calloc(1, sizeof(Buffer)); /*Allocate buffer Desciptor*/
		if (!buffer)return NULL;

		if (o_mode == 'f' && init_capacity !=0) { /*fixed mode wide inc factor 0*/

			buffer->mode = 0;
			buffer->inc_factor = 0;
		}
		else if (o_mode == 'a' && ((unsigned char)inc_factor >= 1 && (unsigned char)inc_factor <= 255)) {  /* set add mode */
			buffer->mode = 1;
			buffer->inc_factor = (unsigned char)inc_factor;
		}
		else if (o_mode == 'm' && inc_factor >= 1 && (unsigned char)inc_factor <= 100) {  /*set multi mode*/

			buffer->mode = -1;
			buffer->inc_factor = inc_factor;
		}

		else {
			free(buffer);
			return NULL;
		}

		buffer->capacity = init_capacity;  /*Set capacity to initiale capacity*/
		buffer->cb_head = (char*)malloc((init_capacity)*(sizeof(char))); /*Allocate dynamic buffer */

		if (!buffer->cb_head)
			free(buffer);
	}

	return buffer;
}


/*********************************************************************
Purpose: Adds a character symbol to the character array
Author: Franck Nana Ngassa
History/Versions: 1
Called functions: b_isfull(), realloc()
Parameters:
Buffer * const pBD (Pointer to the Buffer)
char symbol (The character symbol to add)
Return value:	 A pointer to a Buffer, or NULL on error
Algorithm:	Verifies valid parameters, resizes character array if necessary, and puts the symbol in the array.
If the memory location of the character array has changed, set the r_flag.
***********************************************************************/
pBuffer b_addc(pBuffer const pBD, char symbol) {


	short space = 0, new_capacity = 0, new_increment = 0;
	char *point = NULL;  /* Temporary buffer*/

	if (!pBD) /*Check if error*/
		return NULL;

	if ((pBD->addc_offset) >= SHRT_MAX - 1)   /*addc_offset within limit ?*/
		return NULL;

	pBD->r_flag = 0; /*Reset flag*/

	if ((short)(pBD->addc_offset*sizeof(char)) == pBD->capacity) {  /*Array full ?*/

		if (pBD->mode == 0)  /*Return if mode 0*/
			return NULL;

		else if (pBD->mode == 1) {  /* If add mode*/
			new_capacity = pBD->capacity + (short)(sizeof(char)*(unsigned char)pBD->inc_factor);

			if (new_capacity > 0 && (new_capacity <= SHRT_MAX - 1))
				pBD->capacity = new_capacity; /*Update capacity is within limit*/

			else if (new_capacity > 0 && (new_capacity > SHRT_MAX - 1))
				pBD->capacity = SHRT_MAX - 1;  /*If out of range set to max*/
			else
				return NULL;
		}
		else if (pBD->mode == -1) {   /*If muilti mode*/
			if (pBD->capacity == SHRT_MAX - 1)
				return NULL;

			space = SHRT_MAX - 1 - pBD->capacity;
			new_increment = (short)(space * pBD->inc_factor / 100.0F);

			if (!new_increment)  /* Set to max*/
				new_capacity = (SHRT_MAX - 1);

			else
				new_capacity = new_increment + pBD->capacity;
		}
		point = realloc(pBD->cb_head, new_capacity); /*iF capacity valid , realloc */

		if (!point) {              /*Check for crash*/
			return NULL;
		}
		if (point != pBD->cb_head) { /*Check if memory emplacement was changed, set flag accordingly*/
			pBD->r_flag = SET_R_FLAG;
			pBD->cb_head = point;
		}
		pBD->capacity = new_capacity;  /*Update cpacity*/
	}
	pBD->cb_head[pBD->addc_offset++] = symbol;  /*Add symbol*/
	return pBD;
}

/****************************************************************************
Purpose: Re-initialize Buffer data members to make the Buffer appear empty
Author: Franck Nana Ngassa
History/Versions: 1
Called functions: None
Parameters: Buffer * const pBD (Pointer to the Buffer)
Return value: R_FAIL_1 on error, 0 otherwise
******************************************************************************/

int b_clear(Buffer * const pBD) {   /*Reset the buffer*/

	if (!pBD) return RT_FAIL1;
	pBD->addc_offset = 0;
	pBD->markc_offset = 0;
	pBD->r_flag = 0;
	pBD->eob = 0;
	pBD->getc_offset = 0;
	return 1;
}

/******************************************************************************
Purpose: The function de-allocates (frees) the memory occupied by the
character buffer and the Buffer structure (buffer descriptor)
Author: Franck Nana Ngassa
History/Versions: 1
Called functions: None
Parameters: Buffer * const pBD (Pointer to the Buffer)
Return value: None
******************************************************************************/

void b_free(Buffer* const pBD) {  /*Free the buffer BufferDescriptor*/

	if (!pBD) return;

	if (pBD->cb_head != NULL) {
		free(pBD->cb_head);
	}
	free(pBD);
	pBD->cb_head = NULL;
}

/******************************************************************************
Purpose: Determines if the character buffer is full
Author: Franck Nana Ngassa
History/Versions: 1
Called functions: None
Parameters: Buffer * const pBD (Pointer to the Buffer)
Return value:
R_FAIL_1 on error
1 if the buffer is full
0 otherwise
*****************************************************************************/


#ifdef FULL
b_isfull(pBD);
#endif
#ifndef FULL
int b_isfull(Buffer *const pBD) {  /* Check if buffer is full*/

	if (!pBD) return RT_FAIL1;

	if (pBD->capacity == pBD->addc_offset)
		return 1;
	else
		return 0;
}
#endif



/*****************************************************************************
Purpose: Returns the size of the Buffer
Author:Franck Nana Ngassa
History/Versions: 1
Called functions: None
Parameters: Buffer * const pBD (Pointer to the Buffer)
Return value: short addc_offset, R_FAIL_1 on error
******************************************************************************/

short b_limit(Buffer *const pBD) {

	if (!pBD) return RT_FAIL1;
	return pBD->addc_offset;       /* current buffer limit*/
}

/*******************************************************************************
Purpose: Returns the Buffer capacity
Author: Franck Nana Ngassa
History/Versions: 1
Called functions: None
Parameters: Buffer * const pBD (Pointer to the Buffer)
Return value: short capacity, R_FAIL_1 on error
*******************************************************************************/

short b_capacity(Buffer *const pBD) {

	if (!pBD) return RT_FAIL1;
	return pBD->capacity;  /* current bufffer capacity*/
}

/********************************************************************************
Purpose: Returns mark_offset to the calling function
Author: Franck Nana Ngassa
History/Versions: 1
Called functions: None
Parameters: Buffer * const pBD (Pointer to the Buffer)
Return value: short mark_offset, R_FAIL_1 on error
*******************************************************************************/

short b_mark(pBuffer const pBD, short mark) {

	if (!pBD) return RT_FAIL1;
	if (mark <0 || (mark > pBD->addc_offset)) return RT_FAIL1;

	/*mark within reosonable range ? return */
	return pBD->markc_offset = mark;

}

/*******************************************************************************
Purpose: Returns the Buffer mode
Author: Franck Nana Ngassa
History/Versions: 1
Called functions: None
Parameters:		Buffer * const pBD (Pointer to the Buffer)
Return value:	 int mode, R_FAIL_1 on error
********************************************************************************/

int b_mode(Buffer * const pBD) {
	if (!pBD) return RT_FAIL2;
	return pBD->mode;   /* current buffer mode*/
}

/************************************************************************************
Purpose: The function returns the non-negative value of inc_factor to the calling function
Author: Franck Nana Ngassa
History/Versions: 1
Called functions: None
Parameters:		Buffer * const pBD (Pointer to the Buffer)
Return value:	 int mode, R_FAIL_1 on error
***************************************************************************************/

size_t b_incfactor(Buffer * const pBD) {

	if (!pBD) return 0X100;
	//	if ((unsigned char)pBD->inc_factor < 0) return 0X100;
	return (size_t)(unsigned char)pBD->inc_factor;  /* current buffer inc_factor*/
}

/********************************************************************************************
Author: Franck Nana Ngassa
History / Versions : 1
Called functions : fgetc(), feof(), b_addc(), fclose()
Parameters : FILE * const fi(The file pointer),
Buffer * const pBD(Pointer to the Buffer)
Return value :
R_FAIL_2 or LOAD_FAIL on error
int count(The number of characters added to the Buffer)
Algorithm : Check for valid parameters, enter infinite loop, get the first character
from the file, check if its the end of the file, if not then continue.
Add the character to the array and increase the count.Return the number
of characters added to the array.
******************************************************************************************/

int b_load(FILE * const fi, Buffer * const pBD) {

	char character;
	short char_num = 0;

	if (!pBD || !fi) return RT_FAIL2;
	character = (char)fgetc(fi);/*Is it the end of file yet ?*/

	while (!feof(fi)) {  /*Is it the end of file yet ?*/


		if (!b_addc(pBD, character)) /*Add character to buffer, return -1 if fail*/
			return LOAD_FAIL;
		character = (char)fgetc(fi);/*Is it the end of file yet ?*/
		++char_num;  /*INCREMENT NUMBER OF CHARACTER EACH TIME */
	}

	fclose(fi);  /*CLOSE FILE*/
	return char_num;
}

/***********************************************************************************
Purpose: Check if the Buffer is empty
Author: Franck Nana Ngassa
History/Versions: 1
Called functions:
Parameters: Buffer * const pBD (Pointer to the Buffer)
Return value: -1 on error, int
R_FAIL_1 on error
1 if addc_offset is 0
0 otherwise
**********************************************************************************/

int b_isempty(Buffer * const pBD) {

	if (!pBD) return RT_FAIL1;

	if (pBD->addc_offset == 0)  /*Return 1 if buffer empty*/
		return 1;
	else
		return 0;
}

/***************************************************************************************
Purpose: Returns the Buffer eob
Author:Franck Nana Ngassa
History/Versions: 1
Called functions: None
Parameters:		Buffer * const pBD (Pointer to the Buffer)
Return value:	int eob, R_FAIL_1 on error
**************************************************************************************/

int b_eob(Buffer * const pBD) {

	if (!pBD) return RT_FAIL1;
	return pBD->eob;
}

/******************************************************************************************
Purpose: Gets a character at getc_offset and sets eob
Author: Franck Nana Ngassa
History/Versions: 1
Called functions: None
Parameters: Buffer * const pBD (Pointer to the Buffer)
Return value:
R_FAIL_2 if invalid Buffer
R_FAIL_1 if getc_offset and addc_offset are equal
char The character
Algorithm: Validates parameters, sets eob if the Buffer is full,
and returns a character from the array at getc_offset
*******************************************************************************************/

char b_getc(Buffer * const pBD) {

	if (!pBD) return RT_FAIL2;
	if (b_isempty(pBD)) {
		return RT_FAIL2;
	}

	if (pBD->getc_offset == pBD->addc_offset) {  /*Check if full*/
		pBD->eob = 1;
		return RT_FAIL1;
	}
	else
	{
		pBD->eob = 0;
	}

	return pBD->cb_head[pBD->getc_offset++];  /*return getc_offset and increment*/
}

/**********************************************************************************************
Purpose: Prints the content of the Buffer
Author: Franck Nana Ngassa
History/Versions: 1
Called functions:
b_isempty()
printf()
b_getc()
b_eob()
Parameters: Buffer * const pBD (Pointer to the Buffer)
Return value:	int getc_offset (the number of characters printed)
R_FAIL_1 on error
Algorithm: Verifies the buffer is not empty, prints out each character in
the buffer array and returns the number of characters printed
**********************************************************************************************/

int b_print(Buffer* const pBD) {
	int  counter = 0;
	char character;

	if (!pBD) return RT_FAIL1;
	if (b_isempty(pBD)) {				/*Signal if empty buffer*/
		printf("Empty buffer.\n");
		return RT_FAIL1;
	}
	character = b_getc(pBD);		/*get char*/

	while (!b_eob(pBD)) {				/*Infinite loop*/

		fputc(character, stdout);   /*Output character buffer*/
		character = b_getc(pBD);		/*get char*/

		++counter;		 /* Increment counter*/
	}
	printf("\n");
	return counter;
}



/**********************************************************************************************
Purpose: For all operational modes of the buffer the function
shrinks (or in some cases may expand)the buffer by 1.
Author: Franck Nana Ngassa
History/Versions: 1
Called functions: realloc()
Parameters: Buffer * const pBD (Pointer to the Buffer)
Return value: NULL on error, otherwise a Buffer pointer
Algorithm: Check for valid parameters, calculate new capacity,
realloc to get memory and save
***********************************************************************************************/

Buffer * b_compact(Buffer * const pBD, char symbol) {

	short newCapacity = 0;
	char *temp_cb_head = NULL;

	if (!pBD) {
		return NULL;
	}

	if (pBD->addc_offset == SHRT_MAX) return NULL;

	pBD->r_flag = 0;

	newCapacity = (pBD->addc_offset + 1)*sizeof(char);  /*Increment capacity to one*/
	temp_cb_head = realloc(pBD->cb_head, newCapacity);  /*Set new capacity if valid */

	if (temp_cb_head == NULL) {
		return NULL;
	}
	else if (temp_cb_head != pBD->cb_head) { /*Set falg if memory location changed*/
		pBD->r_flag = SET_R_FLAG;
		pBD->cb_head = temp_cb_head;			/*Update memory location*/

	}

	pBD->capacity = newCapacity;
	pBD->cb_head[pBD->addc_offset++] = symbol;  /*Add symbol and increment offset*/
	return pBD;

}

/***********************************************************************************************
Purpose: Returns the Buffer r_flag
Author: Franck Nana Ngassa
History/Versions: 1
Called functions: None
Parameters:		Buffer * const pBD (Pointer to the Buffer)
Return value:	char r_flag, R_FAIL_1 on error
*/
char b_rflag(Buffer * const pBD) {

	if (!pBD)
		return RT_FAIL1;

	return pBD->r_flag;		 /*return the flag*/
}

/***************************************************************************************************
Purpose: Decrements getc_offset by 1
Author: Franck Nana Ngassa
History/Versions: 1
Called functions: None
Parameters: Buffer * const pBD (Pointer to the Buffer)
Return value:	R_FAIL_1 on error,  otherwise returns getc_offset.
***************************************************************************************************/

short b_retract(Buffer * const pBD) {

	if (!pBD) return RT_FAIL1;
	if (pBD->getc_offset == 0) return RT_FAIL1;

	pBD->getc_offset--;		 /*decrement offset*/
	return pBD->getc_offset;
}

/*************************************************************************************************
Purpose: Sets getc_offset to mark_offset
Author: Franck Nana Ngassa
History/Versions: 1
Called functions:
Parameters:		Buffer * const pBD (Pointer to the Buffer)
Return value:	R_FAIL_1 on error,  otherwise it returns getc_offset.
**************************************************************************************************/

short b_reset(Buffer * const pBD) {
	if (!pBD) return RT_FAIL1;
	pBD->getc_offset = pBD->markc_offset;  /*Set getc_offset to mark*/
	return pBD->getc_offset;
}

/**************************************************************************************************
Purpose: Returns getc_offset to the calling function
Author: Franck Nana Ngassa
History/Versions: 1
Called functions:	None
Parameters:			Buffer * const pBD (Pointer to the Buffer)
Return value:	 short getc_offset, R_FAIL_1 on error
*************************************************************************************************/

short b_getcoffset(Buffer * const pBD) {

	if (!pBD || !pBD->cb_head || !pBD->getc_offset) return RT_FAIL1;
	return pBD->getc_offset;      /*Return getc_offset*/
}

/*****************************************************************************************************
Purpose: The function set the getc_offset and markc_offset to 0,
so that the buffer can be reread again
Author: Franck Nana Ngassa
History/Versions: 1
Called functions: None
Parameters:		Buffer * const pBD (Pointer to the Buffer)
Return value:	 0, R_FAIL_1 on error
***************************************************************************************************/

int b_rewind(Buffer* const pBD) {

	if (!pBD) return RT_FAIL1;
	pBD->getc_offset = pBD->markc_offset = 0;  /*Set offset and mark to 0*/
	return 0;
}

/***************************************************************************************************
Purpose: Returns a pointer to the location of a character in the Buffer
Author: Franck Nana Ngassa
History/Versions: 1
Called functions: None
Parameters:
Buffer * const pBD (Pointer to the Buffer)
short offset (0 to loc_offset)
Return value: char *ch, NULL on error
*******************************************************************************************************/

char * b_location(Buffer* const pBD, short loc_offset) {

	if (!pBD) return NULL;
	if (loc_offset >= pBD->capacity) return NULL;  /*Return NULL if loc out of range*/

	return pBD->cb_head + loc_offset;/*Return loc_offset if in range*/

}