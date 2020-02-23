//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  COMMON - LOG (Logging message for debug)                                 *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#include "typedef.h"
#include "conv.h"

//=============================================================================
//  define
//=============================================================================
#define LOG_FILE_NAME		"POWERUP.LOG"	// rename log file name

//=============================================================================
//  log_print
//=============================================================================
int log_print(char *msg)
{
	struct tm	*logtm;
	FILE		*logfp;
	time_t		now;

	now = time(NULL);
	logtm = localtime(&now);

	logfp = fopen(LOG_FILE_NAME, "at+");

	if (logfp)
	{
		fprintf(logfp, "%04d/%02d/%02d %s %02d:%02d:%02d  %s\n",
				logtm->tm_year+1900,
				logtm->tm_mon+1,
				logtm->tm_mday,
				week_str[logtm->tm_wday],
			
				logtm->tm_hour,
				logtm->tm_min,
				logtm->tm_sec,

				msg);
				
		//fprintf(logfp, "%s\n", log_msg);
		fflush(logfp);
		fclose(logfp);

		return 0;
	}

	return -1;
}
//=============================================================================
//  log_printf
//=============================================================================
void log_printf(const char *fmt, ...)
{
	va_list		args;
	char		*ptr, c;
	uint8_t		align, pad0, maxlen;

	char		msg[256];
	char		buf[128];
	int			idx, i, len;
	
	idx = 0;
	va_start(args, fmt);
	ptr = (char *)fmt;

	while (*ptr)
	{
		//ch = *ptr;
		
		if (*ptr == '\\')
		{
			if (*++ptr)
				ptr++;
			continue;
		}
		else if (*ptr == '%')
		{
			// %-06X
			// left-alignment
			if (*++ptr == '-')
			{
				align = 1;	// left-alignment
			}
			else
			{
				ptr--;
				align = 0;
			}

			// padding 0
			if (*++ptr == '0')
			{
				pad0 = 1;
			}
			else
			{
				pad0 = 0;
				ptr--;
			}

			// max length
			c = *++ptr;
			if ((c>='1') && (c<='9'))
			{
				maxlen = (c & 0x0F);	// 1~9
			}
			else
			{
				ptr--;
				maxlen = 0;
			}

			switch(*++ptr)
			{

			case NULL:
				continue;

			case 'c':
				msg[idx++] = va_arg(args, char);
				break;

			case 's':
				sprintf(buf, "%s", va_arg(args, char*));
				len = (int)strlen(buf);
				for (i=0; i<len; i++)
				{
					msg[idx++] = buf[i];
				}
				break;

			case 'd':
				sprintf(buf, "%d", va_arg(args, unsigned int));
				len = (int)strlen(buf);
				for (i=0; i<len; i++)
				{
					msg[idx++] = buf[i];
				}
				break;
				
			case 'x':
				sprintf(buf, "%x", va_arg(args, unsigned int));
				len = (int)strlen(buf);
				
				if (maxlen > 0)
				{
					if (len < maxlen)
					{
						if (pad0)
						{
							for (i=0; i<(maxlen-len); i++)
								msg[idx++] = '0';
						}
						else
						{
							for (i=0; i<(maxlen-len); i++)
								msg[idx++] = ' ';
						}
					}
				}
				
				for (i=0; i<len; i++)
				{
					msg[idx++] = buf[i];
				}
				break;

			case 'X':
				sprintf(buf, "%X", va_arg(args, unsigned int));
				len = (int)strlen(buf);

				if (maxlen > 0)
				{
					if (len < maxlen)
					{
						if (pad0)
						{
							for (i=0; i<(maxlen-len); i++)
								msg[idx++] = '0';
						}
						else
						{
							for (i=0; i<(maxlen-len); i++)
								msg[idx++] = ' ';
						}
					}
				}

				for (i=0; i<len; i++)
				{
					msg[idx++] = buf[i];
				}
				break;
			}
			ptr++;
		}
		else
		{
			msg[idx++] = *ptr++;
		}
	}
	
	msg[idx] = 0;
	
	log_print(msg);
}
