#include <stdio.h>
#include <stdint.h>

#include "err.h"

void pnt_errstr(uint8_t errno)
{
	switch(errno)
	{
		case 00:
			fprintf(stderr, "I/O Access Timeout");
		break;
		default:
			fprintf(stderr, "Unknown Fail");
		break;
	}
	fprintf(stderr, ".");
}

void pnt_errmeg(uint8_t errno)
{
	fprintf(stderr, ERROR_HEAD);
	
	pnt_errstr(errno);
	
	fprintf(stderr, ".\n");
}

void pnt_errmeg_str(uint8_t errno, char * str)
{
	fprintf(stderr, ERROR_HEAD);
	
	pnt_errstr(errno);
	fprintf(stderr, "\n\t%s\n", str);
}

