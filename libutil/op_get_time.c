/**
 * @file op_get_time.c
 *
 * @remark Copyright 2002 OProfile authors
 * @remark Read the file COPYING
 * 
 * @author John Levon <moz@compsoc.man.ac.uk>
 * @author Philippe Elie <phil_el@wanadoo.fr>
 */

#include "op_get_time.h"
 
#include <time.h>
 
/**
 * op_get_time - get current date and time
 *
 * Returns a string representing the current date
 * and time, or %NULL on error.
 *
 * The string is statically allocated and should not be freed.
 */ 
char * op_get_time(void)
{
	time_t t = time(NULL);

	if (t == -1)
		return NULL;

	return ctime(&t);
}