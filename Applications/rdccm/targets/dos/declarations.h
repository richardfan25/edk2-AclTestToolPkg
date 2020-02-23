#ifndef _DECLARATIONS_H_
#define _DECLARATIONS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>

typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned long		uint32_t;
typedef char				int8_t;
typedef short				int16_t;
typedef long				int32_t;
#if defined(__WATCOMC__)
typedef unsigned long long	uint64_t;
typedef long long			int64_t;
#elif defined(__TURBOC__)
//typedef unsigned double		uint64_t;
//typedef double				int64_t;
#endif

#define _DEBUG
#ifdef _DEBUG
#define DPRINTF		printf
#else
#define DPRINTF(...)
#endif

#endif /* _DECLARATIONS_H_ */
