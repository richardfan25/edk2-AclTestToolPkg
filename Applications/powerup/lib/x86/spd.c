//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - SPD (Serial Presence Detect)                                       *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************

// off
// 00  1  num of bytes used
// 01  1  spd size
// 02  1  ram chip type   (2=edo,4=sdr,7=ddr,8=ddr2,b=ddr3,c=ddr4)
// 03  1  ram module type (0=undef,1=rdimm,2=udimm,3=sodimm)

// 04  1  bit[3:0] exp of chip size (unit=256Mb), 1=>(1<<1) * 256Mb=512Mb=64MB

// 07  1  bit[2:0] exp of num chip (unit=chip num), 2=>(1<<2)*4=16 chips
//		  bit[5:3] num of rank (unit=+1), 1=> 1+1 = 2 ranks;
// ram module size = chip_sz * num_chip * num_rank = 64MB*16*2=2048MB

// 76  1  manufacturer id

// 80  18 manufacturer part number
	
//*****************************************************************************
#include <stdio.h>
#include <string.h>

#include "spd.h"
#include "smb.h"
#include "imc.h"

//=============================================================================
//  spd_ram_type_table
//=============================================================================
spd_name_t	spd_ram_type_table[19] =
{
	{ SPD_RAM_TYPE_FP,		"FP" },
	{ SPD_RAM_TYPE_EDO,		"EDO" },
	{ SPD_RAM_TYPE_PLNB,	"PLNBL" },
	{ SPD_RAM_TYPE_SDR,		"SDR" },
	{ SPD_RAM_TYPE_ROM,		"ROM" },
	{ SPD_RAM_TYPE_SGR,		"SGRAM" },
	{ SPD_RAM_TYPE_DDR,		"DDR" },
	{ SPD_RAM_TYPE_DDR2,	"DDR2" },
	{ SPD_RAM_TYPE_DDR2FB,	"DDR2FB" },
	{ SPD_RAM_TYPE_DDR2FBP, "DDR2FBP" },
	{ SPD_RAM_TYPE_DDR3,	"DDR3" },
	{ SPD_RAM_TYPE_DDR4,	"DDR4" },
	{ SPD_RAM_TYPE_DDR4E,	"DDR4E" },
	{ SPD_RAM_TYPE_LPDDR3,	"LPDDR3" },
	{ SPD_RAM_TYPE_LPDDR4,	"LPDDR4" },
	{ SPD_RAM_TYPE_LPDDR4X,	"LPDDR4X" },
	{ SPD_RAM_TYPE_DDR5,	"DDR5" },
	{ SPD_RAM_TYPE_LPDDR5,	"LPDDR5" },
	{ SPD_RAM_TYPE_UNKNOWN,	"?-" }
};

#define NUM_SPD_RAM_TYPE	(sizeof(spd_ram_type_table)/sizeof(spd_ram_type_table[0]))
//=============================================================================
//  spd_find_ram_type
//=============================================================================
/*
char *spd_find_ram_type(uint8_t *spd)
{
	int		i;
	char	found = 0;

	for (i=0; i<NUM_SPD_RAM_TYPE; i++)
	{
		if (spd_ram_type_table[i].id == spd[0x02])
		{
			found = 1;
			break;
		}
	}

	if (found)
		return spd_ram_type_table[i].name;
	else
		return spd_ram_type_table[NUM_SPD_RAM_TYPE-1].name;	// Unknown
}
*/

char *spd_find_ram_type(uint8_t ram_type)
{
	int		i;
	char	found = 0;

	for (i=0; i<NUM_SPD_RAM_TYPE; i++)
	{
		if (spd_ram_type_table[i].id == ram_type)
		{
			found = 1;
			break;
		}
	}

	if (found)
		return spd_ram_type_table[i].name;
	else
		return spd_ram_type_table[NUM_SPD_RAM_TYPE-1].name;	// Unknown
}


//=============================================================================
//  spd_manu_id_table
//=============================================================================
spd_name_t	spd_manu_id_table[22] =
{
	{ SPD_MANU_ID_MICRON,		"Micron"	},
	{ SPD_MANU_ID_HYNIX	,		"Hynix"		},
	{ SPD_MANU_ID_SAMSUNG,		"Samsung"	},
	{ SPD_MANU_ID_TRANSCEND,	"Transcend"	},
	{ SPD_MANU_ID_APACER,		"Apacer"	},
	{ SPD_MANU_ID_FOXCONN,		"Foxconn"	},
	{ SPD_MANU_ID_SMART_MODULAR,"SmartMod"	},
	{ SPD_MANU_ID_KINGSTON,		"Kingston"	},
	{ SPD_MANU_ID_CORSAIR,		"Corsair"	},
	{ SPD_MANU_ID_ELPIDA,		"Elpida"	},
	{ SPD_MANU_ID_NANYA,		"Nanya"		},
	{ SPD_MANU_ID_KINGMAX,		"Kingmax"	},
	{ SPD_MANU_ID_RAMAXEL,		"Ramaxel"	},
	{ SPD_MANU_ID_ADATA,		"A-DATA"	},
	{ SPD_MANU_ID_TEAM_GROUP,	"TeamGroup"},
	{ SPD_MANU_ID_PQI,			"PQI"		},
	{ SPD_MANU_ID_UNIFOSA,		"Unifosa"	},
	{ SPD_MANU_ID_DSL,			"DSL"		},
	{ SPD_MANU_ID_ADVANTECH,	"Advantech" },
	{ SPD_MANU_ID_TOSHIBA,		"Toshiba"	},
	{ SPD_MANU_ID_INNODISK,		"InnoDisk"	},
	{ SPD_MANU_ID_UNKNOWN,		"Unknown"	}
};

//#define NUM_SPD_MANU_ID	(sizeof(spd_manu_id_table)/sizeof(spd_manu_id_table[0])-1)
#define NUM_SPD_MANU_ID	(22)
//=============================================================================
//  spd_find_manu_id
//=============================================================================
/*
char *spd_find_manu_id(uint8_t *spd)
{
	int		i;
	char	found = 0;

	for (i=0; i<NUM_SPD_MANU_ID; i++)
	{
		if (spd_manu_id_table[i].id == spd[0x76])
		{
			found = 1;
			break;
		}
	}

	if (found)
		return spd_manu_id_table[i].name;
	else
		return spd_manu_id_table[NUM_SPD_MANU_ID-1].name;	// Unknown
}
*/

char *spd_find_manu_id(uint16_t mid)
{
	int			i;
	char		found = 0;
	uint16_t	rid;

	// real id : mask bit 7 (odd parity)
	rid = mid & 0xFF7F;

	for (i=0; i<NUM_SPD_MANU_ID; i++)
	{
		if (spd_manu_id_table[i].id == rid)
		{
			found = 1;
			break;
		}
	}

	if (!found)
	{
		sprintf(spd_manu_id_table[NUM_SPD_MANU_ID-1].name, "ID=%04X", mid);
	}

	if (found)
		return spd_manu_id_table[i].name;
	else
		return spd_manu_id_table[NUM_SPD_MANU_ID-1].name;	// Unknown
}

//=============================================================================
//  spd_mod_type_table
//=============================================================================
spd_name_t	spd_mod_type_table[16] =
{
	{ SPD_MOD_TYPE_UNDEF,		"Undef"			},
	{ SPD_MOD_TYPE_RDIMM,		"R-DIMM"		},
	{ SPD_MOD_TYPE_UDIMM,		"U-DIMM"		},
	{ SPD_MOD_TYPE_SODIMM,		"SO-DIMM"		},
	{ SPD_MOD_TYPE_MICRODIMM,	"Micro-DIMM"		},
	{ SPD_MOD_TYPE_MNRDIMM,		"Mini-RDIMM"	},
	{ SPD_MOD_TYPE_MNUDIMM,		"Mini-UDIMM"	},
	{ SPD_MOD_TYPE_MNCDIMM,		"Mini-CDIMM"	},
	{ SPD_MOD_TYPE_72SORDIMM,	"72-SODIMM"		},
	{ SPD_MOD_TYPE_72SOUDIMM,	"72-SOUDIMM"	},
	{ SPD_MOD_TYPE_72SOCDIMM,	"72-SOCDIMM"	},
	{ SPD_MOD_TYPE_LRDIMM,		"LR-DIMM"		},
	{ SPD_MOD_TYPE_16SODIMM,	"16-SO-DIMM"	},
	{ SPD_MOD_TYPE_32SODIMM,	"32-SO-DIMM"	},
	{ SPD_MOD_TYPE_ABSENT,		"Absent"		},
	{ SPD_MOD_TYPE_UNKNOWN,		"Unknown"		}
};

#define NUM_SPD_MOD_TYPE	(sizeof(spd_mod_type_table)/sizeof(spd_mod_type_table[0])-1)

//=============================================================================
//  spd_ddr2_mod_type_table
//=============================================================================
spd_name_t	spd_ddr2_mod_type_table[8] =
{
	{ SPD_DDR2_MOD_UNDEF,		"Undef"			},
	{ SPD_DDR2_MOD_RDIMM,		"R-DIMM"		},
	{ SPD_DDR2_MOD_UDIMM,		"U-DIMM"		},
	{ SPD_DDR2_MOD_SODIM,		"SO-DIMM"		},
	{ SPD_DDR2_MOD_MDIMM,		"u-DIMM"		},
	{ SPD_DDR2_MOD_MINI_RDIMM,	"mini-RDIMM"	},
	{ SPD_DDR2_MOD_MINI_UDIMM,	"mini-UDIMM"	},
	{ SPD_DDR2_MOD_UNKNOWN,		"Unknown"		}
};
#define NUM_SPD_DDR2_MOD_TYPE	(sizeof(spd_ddr2_mod_type_table)/sizeof(spd_ddr2_mod_type_table[0])-1)

//=============================================================================
//  spd_ddr2_cycle_time_table
//=============================================================================
#define NUM_SPD_DDR2_CYCLE_TIME		(5)

spd_name_t	spd_ddr2_cycle_time_table[5] =
{
	{ 0x50,	"400"	},
	{ 0x3D,	"533"	},
	{ 0x30,	"667"	},
	{ 0x25,	"800"	},
	{ 0xFF, "??"	}
};

//=============================================================================
//  spd_ddr3_cycle_time_table : spd[0Ch]
//=============================================================================
#define NUM_SPD_DDR3_CYCLE_TIME		(7)

spd_name_t	spd_ddr3_cycle_time_table[7] =
{
	{ 0x14,	"800"  },	// 400 MHz
	{ 0x0F,	"1066" },	// 533 MHz
	{ 0x0C,	"1333" },	// 667 MHz
	{ 0x0A,	"1600" },	// 800 MHz
	{ 0x09, "1866" },	// 933 MHz
	{ 0x08, "2133" },	// 1067 MHz
	{ 0xFF, "??"   }
};

//=============================================================================
//  spd_ddr3_volt_table : spd[06h]
//=============================================================================
// 00 : 1.5V
// 02 : 1.35V
// 04 : 1.25V
#define NUM_SPD_DDR3_VOLT		(4)

spd_name_t	spd_ddr3_volt_table[4] =
{
	{ 0x00,	""  },	// DDR3  : 1.5 V
	{ 0x02,	"L" },	// DDR3L : 1.35V
	{ 0x04,	"U" },	// DDR3U : 1.25V
	{ 0xFF, ""   }
};

//=============================================================================
//  spd_ddr4_cycle_time_table : spd[12h]
//=============================================================================
#define NUM_SPD_DDR4_CYCLE_TIME		(7)

spd_name_t	spd_ddr4_cycle_time_table[7] =
{
	{ 0x0A,	"1600" },	// 800 MHz
	{ 0x09,	"1866" },	// 933 MHz
	{ 0x08,	"2133" },	// 1067 MHz
	{ 0x07,	"2400" },	// 1200 MHz
	{ 0x06,	"2666" },	// 1333 MHz
	{ 0x05,	"3200" },	// 1600 MHz
	{ 0xFF, "??"   }
};

//=============================================================================
//  spd_ddr2_ecc_table
//=============================================================================
spd_name_t	spd_ddr2_ecc_table[3] =
{
	{ 0x02,	"ECC " },
	{ 0x06,	"ECC " },
	{ 0xFF, ""   }
};

//=============================================================================
//  spd_ddr3_ecc_table
//=============================================================================
spd_name_t	spd_ddr3_ecc_table[2] =
{
	{ 0x0B,	"ECC " },
	{ 0xFF, ""   }
};

//=============================================================================
//  spd_ddr4_ecc_table
//=============================================================================
spd_name_t	spd_ddr4_ecc_table[2] =
{
	{ 0x0B,	"ECC " },
	{ 0xFF, ""   }
};

//=============================================================================
//  spd_find_module_type
//=============================================================================
/*
char *spd_find_module_type(uint8_t *spd)
{
	int		i;
	char	found = 0;

	for (i=0; i<NUM_SPD_MOD_TYPE; i++)
	{
		if (spd_mod_type_table[i].id == spd[0x03])
		{
			found = 1;
			break;
		}
	}

	if (found)
		return spd_mod_type_table[i].name;
	else
		return spd_mod_type_table[NUM_SPD_MOD_TYPE-1].name;	// Unknown
}
*/

char *spd_find_module_type(uint8_t mtype)
{
	int		i;
	char	found = 0;

	for (i=0; i<NUM_SPD_MOD_TYPE; i++)
	{
		if (spd_mod_type_table[i].id == mtype)
		{
			found = 1;
			break;
		}
	}

	if (found)
		return spd_mod_type_table[i].name;
	else
		return spd_mod_type_table[NUM_SPD_MOD_TYPE-1].name;	// Unknown
}

//=============================================================================
//  spd_ram_module_size
//=============================================================================
uint16_t spd_ram_module_size(uint8_t *spd)
{
	uint16_t	sz;
	uint16_t	chips;
	uint16_t	ranks;

	sz = spd[4] & 0xF;
	sz = (1 << sz);
	sz *= 256;	// 256 Mb
	sz >>= 3;	// Mb -> MB

	chips = spd[7] & 0x7;
	chips = (1 << chips);
	chips *= 4;	// 4 chips

	ranks = (spd[7] >> 3) & 0x7;
	ranks += 1;

	return (sz * chips * ranks);
}

//=============================================================================
//  spd_find_ddr2_module_type
//=============================================================================
char *spd_find_ddr2_module_type(uint8_t mtype)
{
	int		i;
	char	found = 0;

	for (i=0; i<NUM_SPD_MOD_TYPE; i++)
	{
		if (spd_ddr2_mod_type_table[i].id == mtype)
		{
			found = 1;
			break;
		}
	}

	if (found)
		return spd_ddr2_mod_type_table[i].name;
	else
		return spd_ddr2_mod_type_table[NUM_SPD_MOD_TYPE-1].name;	// Unknown
}


//=============================================================================
//  spd_find_ddr2_cycle_time
//=============================================================================
char *spd_find_ddr2_cycle_time(uint8_t cytime)
{
	int		i;
	char	found = 0;

	// NUM_SPD_DDR2_CYCLE_TIME = 5
	for (i=0; i<4; i++)
	{
		if (spd_ddr2_cycle_time_table[i].id == (uint16_t)cytime)
		{
			found = 1;
			break;
		}
	}

	if (found)
		return spd_ddr2_cycle_time_table[i].name;
	else
		return spd_ddr2_cycle_time_table[4].name;	// Unknown
}

//=============================================================================
//  spd_find_ddr3_cycle_time
//=============================================================================
char *spd_find_ddr3_cycle_time(uint8_t cytime)
{
	int		i;
	char	found = 0;

	// NUM_SPD_DDR3_CYCLE_TIME = 7
	for (i=0; i<6; i++)
	{
		if (spd_ddr3_cycle_time_table[i].id == (uint16_t)cytime)
		{
			found = 1;
			break;
		}
	}

	if (found)
		return spd_ddr3_cycle_time_table[i].name;
	else
		return spd_ddr3_cycle_time_table[6].name;	// Unknown
}

//=============================================================================
//  spd_find_ddr3_volt
//=============================================================================
char *spd_find_ddr3_volt(uint8_t volt)
{
	int		i;
	char	found = 0;

	// NUM_SPD_DDR3_CYCLE_TIME = 5
	for (i=0; i<3; i++)
	{
		if (spd_ddr3_volt_table[i].id == (uint16_t)volt)
		{
			found = 1;
			break;
		}
	}

	if (found)
		return spd_ddr3_volt_table[i].name;
	else
		return spd_ddr3_volt_table[3].name;	// Unknown
}

//=============================================================================
//  spd_find_ddr4_module_type
//=============================================================================
char *spd_find_ddr4_cycle_time(uint8_t cytime)
{
	int		i;
	char	found = 0;

	// NUM_SPD_DDR4_CYCLE_TIME = 7
	for (i=0; i<6; i++)
	{
		if (spd_ddr4_cycle_time_table[i].id == cytime)
		{
			found = 1;
			break;
		}
	}

	if (found)
		return spd_ddr4_cycle_time_table[i].name;
	else
		return spd_ddr4_cycle_time_table[6].name;	// Unknown
}

//=============================================================================
//  spd_find_ddr2_ecc
//=============================================================================
char *spd_find_ddr2_ecc(uint8_t ecc)
{
	int		i;
	char	found = 0;

	for (i=0; i<3; i++)
	{
		if (spd_ddr2_ecc_table[i].id == ecc)
		{
			found = 1;
			break;
		}
	}
	
	if (found)
		return spd_ddr2_ecc_table[i].name;	// "ECC "
	else
		return spd_ddr2_ecc_table[2].name;	// ""
}

//=============================================================================
//  spd_find_ddr3_ecc
//=============================================================================
char *spd_find_ddr3_ecc(uint8_t ecc)
{
	if (ecc == spd_ddr3_ecc_table[0].id)
		return spd_ddr3_ecc_table[0].name;	// "ECC "
	else
		return spd_ddr3_ecc_table[1].name;	// ""
}

//=============================================================================
//  spd_find_ddr4_ecc
//=============================================================================
char *spd_find_ddr4_ecc(uint8_t ecc)
{
	if (ecc == spd_ddr4_ecc_table[0].id)
		return spd_ddr4_ecc_table[0].name;	// "ECC "
	else
		return spd_ddr4_ecc_table[1].name;	// ""
}

//=============================================================================
//  spd_get_info
//=============================================================================
void spd_get_info(spd_info_t *info, uint8_t *raw)
{
	uint8_t		*ptr;
	uint8_t		ee_data, spd_rev;
	uint8_t		chip_sz, mod_org;
	uint16_t	csz = 0;
	uint16_t	cnum;
	uint16_t	ranks;
	uint16_t	mid;
	int			i;

	memset(info, 0, sizeof(spd_info_t));

	// raw[0] : spd[0] : ee_data
	// raw[1] : spd[1] : spd revision
	// raw[2] : spd[2] : ram type (ddr3, ddr4)
	// raw[3] : spd[3] : module type (SO-DIMM)
	// raw[4] : spd[4] : chip size
	// raw[5] : spd[7]/spd[12]     : module organ
	// raw[6] : spd[76h]/spd[141h] : msb of manufacturer id
	// raw[7] : spd[75h]/spd[140h] : lsb of manufacturer id
	// raw[8] : ../spd[14]         : 0x80= with thermal sensor
	// raw[9] : cycle time
	// raw[10]: ecc
	// raw[11~]: model spd[80h~91h]/spd[149h~15ch]

	ptr = raw;

	ee_data			= *ptr++;	// 0
	spd_rev			= *ptr++;	// 1
	info->ctype		= *ptr++;	// 2
	info->mtype		= *ptr++;	// 3
	chip_sz			= *ptr++;	// 4
	mod_org			= *ptr++;	// 5

	// DRAM hierachy
	// channel : memory controller : 2*DIMM or 4*DIMM
	// DIMM    : RAM module        : 2*ranks
	// rank    : RAM chip group (the same CS#) : N*chips
	// chip    : RAM bank group    : N*banks
	// bank    : RAM cell group    : N*cells
	// cell    : RAM (row, column) : N*bit     : also called "bit width"
	//                             : 1 cell = 4-bit or 8-bit or 16-bit

	if (info->ctype == SPD_RAM_TYPE_DDR4   ||
		info->ctype == SPD_RAM_TYPE_DDR4E  ||
		info->ctype == SPD_RAM_TYPE_LPDDR4 ||
		info->ctype == SPD_RAM_TYPE_LPDDR4X ||
		info->ctype == SPD_RAM_TYPE_LPDDR3 ||
		info->ctype == SPD_RAM_TYPE_DDR3)
	{
		// chip size : SPD[4].bit[3:0] : capacity per die in Mb
		
		// JEDEC Standard No.21-C page 14 : Byte 4 (0x004) SDRAM Density and Banks
		csz = chip_sz & 0xF;
		if (csz == 9)
		{
			csz = 24 * 1024;	// 24 Gb
			csz >>= 3;			// Mb -> MB
		}
		else if (csz == 8)
		{
			csz = 12 * 1024;	// 12 Gb
			csz >>= 3;			// Mb -> MB	
		}
		else
		{
			csz = (1 << csz);
			csz *= 256;	// 256 Mb
			csz >>= 3;	// Mb -> MB
		}

		// num of chips : DDR4_SPD[0C].bit[2:0], DDR3_SPD[07].bit[2:0]
		cnum = mod_org & 0x7;
		cnum = (1 << cnum);
		cnum *= 4;	// 4 chips

		// num of ranks : DDR4_SPD[0C].bit[5:3], DDR3_SPD[07].bit[5:3]
		ranks = (mod_org >> 3) & 0x7;
		ranks += 1;

		// DRAM module size = (chip size) * (num of chips) * (num of ranks)
		info->mbsz = (csz * cnum * ranks);

		// manufacturer id
		mid	= *ptr++;		// 6
		mid <<= 8;
		mid	|= *ptr++;		// 7
		info->mid = mid;
	
		info->flag	 = *ptr++;	// 8
		info->cytime = *ptr++;	// 9
		info->ecc	 = *ptr++;	// 10
	}
	else if (info->ctype == SPD_RAM_TYPE_DDR2)
	{
		switch(mod_org)
		{
			case 0x20:	csz = 128;	break;
			case 0x40:	csz = 256;	break;
			case 0x80:	csz = 512;	break;
			case 0x01:	csz = 1024;	break;
			case 0x02:	csz = 2048;	break;
			case 0x04:	csz = 4096;	break;
		}
		
		cnum = (chip_sz & 0xF) + 1;
		info->mbsz = (csz * cnum);

		// manufacturer id
		mid	= *ptr++;		// 6
		mid <<= 8;
		mid	|= *ptr++;		// 7
		info->mid = mid;
		
		info->flag = *ptr++;	// 8
		info->cytime = *ptr++;	// 9
		info->ecc	 = *ptr++;	// 10
	}


	if (info->ctype == SPD_RAM_TYPE_DDR4 ||
		info->ctype == SPD_RAM_TYPE_DDR4E ||
		info->ctype == SPD_RAM_TYPE_LPDDR4 ||
		info->ctype == SPD_RAM_TYPE_LPDDR4X)
	{
		for (i=0; i<20; i++)
		{
			info->model[i] = *ptr++;	// 11~
			//if (*ptr == 0x20)
			//	break;	// skip space
			// do not skip due to suck dram module part number of spd data

			// there is 00h in the sucking dram module of module part number.
			// replace 00h to '_' for avoid part number showing
			if (info->model[i] == 0)
				info->model[i] = '_';
		}
		info->model[i] = 0;

		info->ee_sz = ((ee_data>>4) & 0x7);
		if (info->ee_sz < 4)
			info->ee_sz = (info->ee_sz + 1) * 128;
		else
			info->ee_sz = 0;

		info->ee_used = (ee_data & 0xF);
		if (info->ee_used < 4)
			info->ee_used = (info->ee_used + 1) * 128;
		else
			info->ee_used = 0;
	}
	else if (info->ctype == SPD_RAM_TYPE_DDR3 ||
			info->ctype == SPD_RAM_TYPE_LPDDR3)
	{
		for (i=0; i<18; i++)
		{
			info->model[i] = *ptr++;	// 11~
			//if (*ptr == 0x20)	// skip space
			//	break;

			// do not skip due to suck dram module part number of spd data
			
			// there is 00h in the sucking dram module of module part number.
			// replace 00h to '_' for avoid part number showing
			if (info->model[i] == 0)
				info->model[i] = '_';
		}
		info->model[i] = 0;

		if (((ee_data>>4) & 0x7) == 1)
			info->ee_sz = 256;	// 256 bytes
		else
			info->ee_sz = 0;
		
		switch (ee_data & 0xF)
		{
			case 1:	info->ee_used = 128;	break;
			case 2:	info->ee_used = 176;	break;
			case 3:	info->ee_used = 256;	break;
			default:	info->ee_used = 0;	break;
		}
	}
	else if (info->ctype == SPD_RAM_TYPE_DDR2)
	{
		for (i=0; i<18; i++)
		{
			info->model[i] = *ptr++;	// 11~
			//if (*ptr == 0x20 || *ptr == 0x00)	// skip space
			if (*ptr == 0x00)	// not skip space
				break;	// skip space
		}
		info->model[i] = 0;

		if (spd_rev == 0x08)
			info->ee_sz = info->ee_used = 256;	// 256 bytes
		else if (spd_rev == 0x07)
			info->ee_sz = info->ee_used = 128;	// 128 bytes
		else
			info->ee_sz = info->ee_used = 0;
	}

}

//=============================================================================
//  spd_get_crc
//=============================================================================
uint16_t spd_get_crc(uint8_t *spd, uint8_t *len)
{
	int			i, crc;
	uint8_t		*ptr = spd;
	int			count;
	
	if (*ptr & 0x80)
		count = 117;
	else
		count = 126;

	*len = (uint8_t)count;
	
	crc = 0;
	while (--count >= 0)
	{
		crc = crc ^ (int)*ptr++ << 8;

		for (i=0; i<8; ++i)
		{
			if (crc & 0x8000)
				crc = crc << 1 ^ 0x1021;
			else
				crc = crc << 1;
		}
	}

	return (crc & 0xFFFF);
}

//=============================================================================
//  spd_get_crc16
//=============================================================================
uint16_t spd_get_crc16(uint8_t *spd, int len)
{
	int			i, crc;
	uint8_t		*ptr = spd;
	int			count;
	
	count = len;

	crc = 0;
	while (--count >= 0)
	{
		crc = crc ^ (int)*ptr++ << 8;

		for (i=0; i<8; ++i)
		{
			if (crc & 0x8000)
				crc = crc << 1 ^ 0x1021;
			else
				crc = crc << 1;
		}
	}

	return (crc & 0xFFFF);
}

//=============================================================================
//  spd_dev_check : checking device is SPD or not
//=============================================================================
int spd_dev_check(uint8_t addr, uint8_t *data)
{
	uint8_t		spd[4];
	uint8_t		ee_used;
	uint8_t		ee_total;
	int			i;

	// spd : first 4 bytes of spd for checking spd integrity
	for (i=0; i<4; i++)
	{
		spd[i] = smb->read_byte_data(addr, (uint8_t)i);
		data[i] = spd[i];
	}
	
		
	if (spd[2] != SPD_RAM_TYPE_DDR4 && spd[2] != SPD_RAM_TYPE_DDR4E  && spd[2] != SPD_RAM_TYPE_LPDDR4 && spd[2] != SPD_RAM_TYPE_LPDDR4X &&
		spd[2] != SPD_RAM_TYPE_DDR3 && spd[2] != SPD_RAM_TYPE_LPDDR3 && spd[2] != SPD_RAM_TYPE_DDR2)
	{
		// set page 0 of ddr4 before checking reading spd
		smb->quick_command(0x6C);
		
		for (i=0; i<4; i++)
		{
			spd[i] = smb->read_byte_data(addr, (uint8_t)i);
			data[i] = spd[i];
		}
	}

	// dram type
#if 0
	if (!(	spd[2] == SPD_RAM_TYPE_EDO  ||
			spd[2] == SPD_RAM_TYPE_SDR  ||
			spd[2] == SPD_RAM_TYPE_DDR  ||
			spd[2] == SPD_RAM_TYPE_DDR2 ||
			spd[2] == SPD_RAM_TYPE_DDR3 ||
			spd[2] == SPD_RAM_TYPE_DDR4) )

		return -1;	// non-spd
#endif

	if (!(	spd[2] == SPD_RAM_TYPE_DDR2 ||
			spd[2] == SPD_RAM_TYPE_DDR3 ||
			spd[2] == SPD_RAM_TYPE_LPDDR3 ||
			spd[2] == SPD_RAM_TYPE_DDR4   ||
			spd[2] == SPD_RAM_TYPE_DDR4E  ||
			spd[2] == SPD_RAM_TYPE_LPDDR4 ||
			spd[2] == SPD_RAM_TYPE_LPDDR4X) )

		return -1;	// non-spd

	// SPD revision : DDR3, DDR4
	if (spd[2] == SPD_RAM_TYPE_DDR3 ||
		spd[2] == SPD_RAM_TYPE_LPDDR3 ||
		spd[2] == SPD_RAM_TYPE_DDR4 ||
		spd[2] == SPD_RAM_TYPE_DDR4E ||
		spd[2] == SPD_RAM_TYPE_LPDDR4 ||
		spd[2] == SPD_RAM_TYPE_LPDDR4X)
	{
		// 0xFF : undefined
		if (spd[1] == 0xFF)
			return -2;	// non-spd

		// SPD module type
		// 01h : SPD_MOD_TYPE_RDIMM
		// 0Dh : SPD_MOD_TYPE_32SODIMM
		
		// don't check module type
		/*
		if ((spd[3] < SPD_MOD_TYPE_RDIMM) ||
			(spd[3] > SPD_MOD_TYPE_32SODIMM))
			return -3;
		*/

		// ee_used
		ee_used	 = spd[0] & 0x0F;			// bit[3:0]
		ee_total = (spd[0] >> 4) & 0x7;		// bit[6:4]

		if (spd[2] == SPD_RAM_TYPE_DDR3 ||
			spd[2] == SPD_RAM_TYPE_LPDDR3)
		{
			// undef/128/176/256
			if (ee_used == 0 || ee_used > 3)
				return -4;

			// 256 bytes
			if (ee_total != 1)
				return -5;
		}
		else if (spd[2] == SPD_RAM_TYPE_DDR4 ||
				 spd[2] == SPD_RAM_TYPE_DDR4E ||
				 spd[2] == SPD_RAM_TYPE_LPDDR4 ||
				 spd[2] == SPD_RAM_TYPE_LPDDR4X)
		{
			// 0/128/256/384/512
			if (ee_used > 4)
				return -6;
	
			// 384/512
			if (ee_total < 2 || ee_total > 3)
				return -7;
		}
	}
	else if (spd[2] == SPD_RAM_TYPE_DDR2)
	{
		// 0x80:128 bytes, 0xFF:256 bytes
		if (!(spd[0] == 0x80 || spd[0] == 0xFF))
			return -8;
		
		// 0x07:24C01=128, 0x08:24C02=256
		if (!(spd[1] == 0x07 || spd[1] == 0x08))
			return -9;

		if (!(spd[3] == 0x0F || spd[3] == 0x0E || spd[3] == 0x0D || spd[3] == 0x0C))
			return -10;
			
	}
	
	return 0;
}

//=============================================================================
//  spd_imc_dev_check : checking device is SPD or not
//=============================================================================
int spd_imc_dev_check(uint8_t addr, uint8_t *data)
{
	uint8_t		spd[4];
	uint8_t		ee_used;
	uint8_t		ee_total;
	int			i;

	// spd : first 4 bytes of spd for checking spd integrity
	for (i=0; i<4; i++)
	{
		spd[i] = imc_read_byte(addr, (uint8_t)i);
		data[i] = spd[i];
	}
	
		
	if ((spd[2] != SPD_RAM_TYPE_DDR4) &&
		(spd[2] != SPD_RAM_TYPE_DDR4E) &&
		(spd[2] != SPD_RAM_TYPE_LPDDR4) &&
		(spd[2] != SPD_RAM_TYPE_LPDDR4X) &&
		(spd[2] != SPD_RAM_TYPE_DDR3) &&
		(spd[2] != SPD_RAM_TYPE_LPDDR3) &&
		(spd[2] != SPD_RAM_TYPE_DDR2))
	{
		// set page 0 of ddr4 before checking reading spd
		imc_set_spd_page(0);

		for (i=0; i<4; i++)
		{
			spd[i] = imc_read_byte(addr, (uint8_t)i);
			data[i] = spd[i];
		}
	}
	
	if (!(	spd[2] == SPD_RAM_TYPE_DDR2 ||
			spd[2] == SPD_RAM_TYPE_DDR3 ||
			spd[2] == SPD_RAM_TYPE_LPDDR3 ||
			spd[2] == SPD_RAM_TYPE_DDR4 ||
			spd[2] == SPD_RAM_TYPE_DDR4E ||
			spd[2] == SPD_RAM_TYPE_LPDDR4 ||
			spd[2] == SPD_RAM_TYPE_LPDDR4X) )

		return -1;	// non-spd

	// SPD revision : DDR3, DDR4
	if (spd[2] == SPD_RAM_TYPE_DDR3 ||
		spd[2] == SPD_RAM_TYPE_LPDDR3 ||
		spd[2] == SPD_RAM_TYPE_DDR4 ||
		spd[2] == SPD_RAM_TYPE_DDR4E ||
		spd[2] == SPD_RAM_TYPE_LPDDR4 ||
		spd[2] == SPD_RAM_TYPE_LPDDR4X)
	{
		// 0xFF : undefined
		if (spd[1] == 0xFF)
			return -2;	// non-spd

		// SPD module type
		// 01h : SPD_MOD_TYPE_RDIMM
		// 0Dh : SPD_MOD_TYPE_32SODIMM
		if ((spd[3] < SPD_MOD_TYPE_RDIMM) ||
			(spd[3] > SPD_MOD_TYPE_32SODIMM))
			return -3;

		// ee_used
		ee_used	 = spd[0] & 0x0F;			// bit[3:0]
		ee_total = (spd[0] >> 4) & 0x7;		// bit[6:4]

		if (spd[2] == SPD_RAM_TYPE_DDR3 ||
			spd[2] == SPD_RAM_TYPE_LPDDR3)
		{
			// undef/128/176/256
			if (ee_used == 0 || ee_used > 3)
				return -4;

			// 256 bytes
			if (ee_total != 1)
				return -5;
		}
		else if (spd[2] == SPD_RAM_TYPE_DDR4 ||
				 spd[2] == SPD_RAM_TYPE_DDR4E ||
				 spd[2] == SPD_RAM_TYPE_LPDDR4 ||
				 spd[2] == SPD_RAM_TYPE_LPDDR4X)
				 
		{
			// 0/128/256/384/512
			if (ee_used > 4)
				return -6;
	
			// 384/512
			if (ee_total < 2 || ee_total > 3)
				return -7;
		}
	}
	else if (spd[2] == SPD_RAM_TYPE_DDR2)
	{
		// 0x80:128 bytes, 0xFF:256 bytes
		if (!(spd[0] == 0x80 || spd[0] == 0xFF))
			return -8;
		
		// 0x07:24C01=128, 0x08:24C02=256
		if (!(spd[1] == 0x07 || spd[1] == 0x08))
			return -9;

		if (!(spd[3] == 0x0F || spd[3] == 0x0E || spd[3] == 0x0D || spd[3] == 0x0C))
			return -10;
			
	}
	
	return 0;
}