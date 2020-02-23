//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  COMMON - CRC (Cyclic Redundancy Check)                                   *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
//#include <stdio.h>
//#include <stdlib.h>
//#include <malloc.h>

#include "crc.h"

//=============================================================================
//  define
//=============================================================================
// crc8
#define POLY_CRC8				0x1070

// crc16
#define	POLY_CRC16_XMODEM		0x11021
#define	POLY_CRC16_XMODEM2		0x1021
#define	POLY_CRC16				0x8005
#define	POLY_CRC16_REV			0xA001
#define	POLY_CRC16_MODBUS		0xA001
#define	POLY_CRC16_KERMIT		0x8408
//#define	POLY_CRC16_KERMIT		0x1021
#define	POLY_CRC16_DNP			0x3D65
//#define	POLY_CRC16_DNP			0xA6BC
// 0011 1101 0110 0101=0x3D65
// 1010 0110 1011 1100=0xA6BC

// crc32
#define POLYNOMIAL				0x04C11DB7		// Standard CRC-32 polynomial
#define BUFFER_LEN				4096L			// Length of buffer

#define POLYNOMIAL_DOS			0x04C11DB7L		// Standard CRC-32 polynomial
#define BUFFER_LEN_DOS			4096L			// Length of buffer

#pragma pack(1)

//=============================================================================
//  crc_crc8
//=============================================================================
uint8_t crc_crc8(uint8_t *data, int len)
{
	uint8_t		crc8_table[256];
	uint8_t		crc;
	int			i, j;

	// initial crc8 table
	for (i=0; i<256; i++)
	{
		crc = (uint8_t)i;
		for (j=0; j<8; j++)
		{
			crc = (crc << 1) ^ ((crc & 0x80) ? 0x07 : 0);	// 0x07=DI
		}
		crc8_table[i] = crc & 0xFF;
	}

	crc = 0;
	for (i=0; i<len; i++)
	{
		crc = crc8_table[ crc ^ *data++ ];
		crc &= 0xFF;
	}

	return crc;
}

//=============================================================================
//  crc_crc8_dallas
//=============================================================================
uint8_t crc_crc8_dallas(uint8_t *data, int len)
{
	uint8_t		crc = 0;
	uint8_t		extract;
	uint8_t		i, sum;

	while(len--)
	{
		extract = *data++;
		for (i=8; i; i--)
		{
			sum = (crc ^ extract) & 0x01;
			crc >>= 1;
			if (sum)
			{
				crc ^= 0x8C;
			}
			extract >>= 1;
		}
	}

	return crc;
}

//=============================================================================
//  crc_crc8_test
//=============================================================================
uint8_t crc_crc8_test(uint8_t *data, int len)
{
	uint8_t		crc = 0;
	uint8_t		i;
	uint8_t		extract;
	uint8_t		sum;

	while (len-- > 0)
	{
		extract = *data++;
		for (i=8; i!=0; i--)
		{
			sum = (uint8_t)((crc & 0xFF) ^ (extract & 0xFF));
			sum = (uint8_t)((sum & 0xFF) & 0x01);
			crc = (uint8_t)((crc & 0xFF) >> 1);
			if (sum)
			{
				crc = (uint8_t)((crc & 0xFF) ^ 0x8C);
			}
			extract = (uint8_t)((extract & 0xFF) >> 1);
		}
	}

	return (int)(crc & 0xFF);
}

//=============================================================================
//  crc_crc16
//=============================================================================
uint16_t crc_crc16(uint8_t *data, int len)
{
	int		i;
	uint16_t		crc = 0;

	for (; len>0; len--)
	{
		crc = crc ^ (*data++ << 8);
		for (i=0; i<8; i++)
		{
			if (crc & 0x8000)
				crc = (crc << 1) ^ POLY_CRC16;
			else
				crc = crc << 1;
		}
	}

	return crc;
}

//=============================================================================
//  crc_crc16_xmodem
//=============================================================================
uint16_t crc_crc16_xmodem(uint8_t *data, int len, uint16_t crc)
{
	int		i;

	for (; len>0; len--)
	{
		crc = crc ^ (*data++ << 8);

		for (i=0; i<8; i++)
		{
			if (crc & 0x8000)
				crc = (uint16_t)((crc << 1) ^ POLY_CRC16_XMODEM);
			else
				crc = crc << 1;
		}
	}

	return crc;
}

//=============================================================================
//  crc_crc16_xmodem2
//=============================================================================
uint32_t crc_crc16_xmodem2(uint8_t *data, int len, uint32_t crc)
{
	int		i;

	for (; len>0; len--)
	{
		crc = crc ^ (*data++ << 8);
		for (i=0; i<8; i++)
		{
			crc = crc << 1;
			if (crc & 0x10000)
				crc = (crc ^ POLY_CRC16_XMODEM2) & 0xFFFF;
		}
	}

	return crc;
}

//=============================================================================
//  crc_crc16_modbus
//=============================================================================
uint16_t crc_crc16_modbus(uint8_t *data, int len)
{
	int			i;
	uint16_t	crc = 0xFFFF;

	for (; len>0; len--)
	{
		crc ^= *data++;
		for (i=0; i<8; i++)
		{
			if (crc & 0x01)
				crc = (crc >> 1) ^ POLY_CRC16_MODBUS;
			else
				crc = crc >> 1;
		}
	}

	return crc;
}

//=============================================================================
//  crc_crc16_reverse
//=============================================================================
uint16_t crc_crc16_reverse(uint8_t *data, int len)
{
	int			i;
	uint16_t	crc = 0;

	for (; len>0; len--)
	{
		crc ^= *data++;

		for (i=0; i<8; i++)
		{
			if (crc & 0x01)
				crc = (crc >> 1) ^ POLY_CRC16_REV;
			else
				crc = crc >> 1;
		}
	}

	return crc;
}

//=============================================================================
//  crc_crc16_kermit
//=============================================================================
uint16_t crc_crc16_kermit(uint8_t *data, int len)
{
	uint16_t	crc16_table[256];
	uint16_t	crc, sc;
	uint16_t	idx;
	uint8_t		c;
	int			i, j;

	for (i=0; i<256; i++)
	{
		crc = 0;
		c	= (uint8_t)i;

		for (j=0; j<8; j++)
		{
			if ((crc ^ c) & 0x0001)
				crc = (uint16_t)((crc >> 1) ^ POLY_CRC16_KERMIT);
			else
				crc = crc >> 1;
			c = c >> 1;
		}
		crc16_table[i] = crc;
	}

	crc = 0;
	for (i=0; i<len; i++)
	{
		sc = (uint16_t)*data++ & 0x00FF;
		idx = (crc ^ sc);
		crc = (crc >> 8) ^ crc16_table[idx&0xFF];
	}

	return crc;
}

//=============================================================================
//  crc_crc16_dnp
//=============================================================================
uint16_t crc_crc16_dnp(uint8_t *data, int len)
{
	int			i;
	uint16_t	crc = 0;

	for (; len>0; len--)
	{
		crc = crc ^ (*data++ << 8);

		for (i=0; i<8; i++)
		{
			if (crc & 0x8000)
				crc = (crc << 1) ^ POLY_CRC16_DNP;
			else
				crc = crc << 1;
		}
	}

	return crc;
}

//=============================================================================
//  CRC32
//
//  CRC32-IEEE 802.3 polynomial
//  x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1
//=============================================================================
static uint32_t	crc32_table[256];

//=============================================================================
//  makecrc32
//=============================================================================
static void makecrc32(void)
{
    /* Not copyrighted 1990 Mark Adler  */
    uint32_t	c;  /* crc shift register */
    uint32_t e;  /* polynomial exclusive-or pattern */
    uint16_t i;  /* counter for all possible eight bit values */
    uint16_t k;  /* byte being shifted into crc apparatus */

    /* terms of polynomial defining this crc (except x^32): */
    static const int32_t p[] = {0, 1, 2, 4, 5, 7, 8, 10, 11, 12, 16, 22, 23, 26};
    
    /* Make exclusive-or pattern from polynomial */
    e = 0;
    for (i = 0; i < sizeof(p)/sizeof(int32_t); i++)
	{
        e |= (1L << (31 - p[i]));
    }

    crc32_table[0] = 0;

    for (i = 1; i < 256; i++) {
        c = 0;
        for (k = i | 256; k != 1; k >>= 1) {
            c = c & 1 ? (c >> 1) ^ e : c >> 1;
            if (k & 1)
                c ^= e;
        }
        crc32_table[i] = c;
    }
    /* this is initialized here so this code could reside in ROM */
}

//=============================================================================
//  crc_crc32
//=============================================================================
uint32_t crc_crc32(uint8_t *data, int len)
{
	uint32_t	crc;
	uint8_t		*p;
	int			i, c;

	makecrc32();

	crc = 0xFFFFFFFF;

	p = data;

	for (i=0; i<len; i++, p++)
	{
		c = (*p & 0xFF);
        crc = ((crc >> 8) & 0x00FFFFFF) ^ crc32_table[ ((crc ^ c) & 0xFF) ];
	}

	return (crc ^ 0xFFFFFFFF);
}

//===========================================================================
//  gen_crc_table : CRC32 table initialization
//===========================================================================
static void gen_crc_table(void)
{
	uint16_t	i, j;
	uint32_t	crc_accum;

	for (i=0; i<256; i++)
	{
		crc_accum = ( (uint32_t) i << 24 );

		for (j=0;  j<8; j++)
		{
			if (crc_accum & 0x80000000L)
			{
				crc_accum = (crc_accum << 1) ^ POLYNOMIAL;
			}
			else
			{
				crc_accum = (crc_accum << 1);
			}
		}

		crc32_table[i] = crc_accum;
	}
}

//===========================================================================
//  crc_crc32_old
//===========================================================================
uint32_t crc_crc32_old(uint8_t *data, int len)
{
	uint32_t	i, j;
	uint32_t	crc_accum;

	gen_crc_table();

	crc_accum = 0xFFFFFFFF;

	for (j=0; j<(uint32_t)len; j++)
	{
		i = ((uint32_t) (crc_accum >> 24) ^ *data++) & 0xFF;
		crc_accum = (crc_accum << 8) ^ crc32_table[i];
	}

	crc_accum = ~crc_accum;

	return crc_accum;
}

//===========================================================================
//  gen_crc_dos_table : CRC32 table initialization
//===========================================================================
void gen_crc_dos_table(void)
{
	register uint16_t	i, j;
	register uint32_t	crc_accum;

	for (i=0; i<256; i++)
	{
		crc_accum = ( (uint32_t) i << 24 );

		for (j=0;  j<8; j++)
		{
			if (crc_accum & 0x80000000L)
			{
				crc_accum = (crc_accum << 1) ^ POLYNOMIAL_DOS;
			}
			else
			{
				crc_accum = (crc_accum << 1);
			}
		}

		crc32_table[i] = crc_accum;
	}
}

//===========================================================================
//  crc_crc32_dos
//===========================================================================
//  it's for powerup.exe v1.x crc32 code
//===========================================================================
uint32_t crc_crc32_dos(uint8_t *data, uint32_t data_bsz)
{
	register uint32_t	i, j;
	register uint32_t	crc_accum;

	gen_crc_dos_table();

	crc_accum = 0xFFFFFFFF;

	for (j=0; j<data_bsz; j++)
	{
		i = ((int) (crc_accum >> 24) ^ *data++) & 0xFF;
		crc_accum = (crc_accum << 8) ^ crc32_table[i];
	}

	crc_accum = ~crc_accum;

	return crc_accum;
}

#pragma pack()
