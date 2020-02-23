//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  ERRCODE : Error Code                                                     *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "typedef.h"
#include "errcode.h"
#include "dat.h"
#include "log.h"

//===================================================================
//  pu_err
//===================================================================
pu_err_t pu_err[53] =
{
	{ PU_OK,					"PU_OK"	},
	{ PU_STS_TMR_UPD,			"PU_STS_TMR_UPD"	},
	{ PU_STS_IMC_FOUND,			"PU_STS_IMC_FOUND" },
	{ PU_ERR_TMR_DEAD,			"PU_ERR_TMR_DEAD" },
	{ PU_ERR_MALLOC,			"PU_ERR_MALLOC" },
	{ PU_ERR_E820,				"PU_ERR_E820" },
	{ PU_ERR_GET_CPU_VENDOR,	"PU_ERR_GET_CPU_VENDOR" },
	{ PU_ERR_GET_CPU_NAME,		"PU_ERR_GET_CPU_NAME" },
	{ PU_ERR_SCAN_PCI,			"PU_ERR_SCAN_PCI" },
	{ PU_ERR_CONFIRM_PCI,		"PU_ERR_CONFIRM_PCI" },
	{ PU_ERR_SCAN_SMBUS,		"PU_ERR_SCAN_SMBUS" },
	{ PU_ERR_GET_ABIOS,			"PU_ERR_GET_ABIOS" },
	{ PU_ERR_GET_AEC,			"PU_ERR_GET_AEC" },
	{ PU_ERR_NO_SMB_CTLR,		"PU_ERR_NO_SMB_CTLR" },
	{ PU_ERR_SMB_CTLR,			"PU_ERR_SMB_CTLR" },
	{ PU_ERR_GET_SMBIOS,		"PU_ERR_GET_SMBIOS" },
	{ PU_ERR_NON_ACL_MB,		"PU_ERR_NON_ACL_MB" },
	{ PU_ERR_NO_SPD_FOUND,		"PU_ERR_NO_SPD_FOUND" },
	{ PU_ERR_CANT_SUPPORT_SPD,  "PU_ERR_CANT_SUPPORT_SPD" },
	{ PU_ERR_DAT_INIT_DAT,		"PU_ERR_DAT_INIT_DAT" },	// init
	{ PU_ERR_DAT_INIT_HDR,		"PU_ERR_DAT_INIT_HDR" },
	{ PU_ERR_DAT_INIT_PAR,		"PU_ERR_DAT_INIT_PAR" },
	{ PU_ERR_DAT_INIT_DES,		"PU_ERR_DAT_INIT_DES" },
	{ PU_ERR_DAT_RD_HDR,		"PU_ERR_DAT_RD_HDR" },	// read
	{ PU_ERR_DAT_RD_PAR,		"PU_ERR_DAT_RD_PAR" },
	{ PU_ERR_DAT_RD_DES,		"PU_ERR_DAT_RD_DES" },
	{ PU_ERR_DAT_RD_RAW,		"PU_ERR_DAT_RD_RAW" },
	{ PU_ERR_DAT_RD_REC,		"PU_ERR_DAT_RD_REC" },
	{ PU_ERR_DAT_CHK_OPEN,		"PU_ERR_DAT_CHK_OPEN" },	// check
	{ PU_ERR_DAT_CHK_FSZ,		"PU_ERR_DAT_CHK_FSZ" },
	{ PU_ERR_DAT_CHK_BRD,		"PU_ERR_DAT_CHK_BRD" },
	{ PU_ERR_DAT_CHK_HDR_MAG,	"PU_ERR_DAT_CHK_HDR_MAG" },	// magic check
	{ PU_ERR_DAT_CHK_PAR_MAG,	"PU_ERR_DAT_CHK_PAR_MAG" },
	{ PU_ERR_DAT_CHK_DES_MAG,	"PU_ERR_DAT_CHK_DES_MAG" },
	{ PU_ERR_DAT_CHK_HDR_CRC,	"PU_ERR_DAT_CHK_HDR_CRC" },	// crc check
	{ PU_ERR_DAT_CHK_PAR_CRC,	"PU_ERR_DAT_CHK_PAR_CRC" },
	{ PU_ERR_DAT_CHK_DES_CRC,	"PU_ERR_DAT_CHK_DES_CRC" },
	{ PU_ERR_DAT_CHK_RAW_CRC,	"PU_ERR_DAT_CHK_RAW_CRC" },
	{ PU_ERR_DAT_CHK_REC_CRC,	"PU_ERR_DAT_CHK_REC_CRC" },
	{ PU_ERR_DAT_RD_MALLOC,		"PU_ERR_DAT_RD_MALLOC" },	// ?
	{ PU_ERR_DAT_RD_CHK_RAW,	"PU_ERR_DAT_RD_CHK_RAW" },
	{ PU_ERR_DAT_RD_RAW_MEM,	"PU_ERR_DAT_RD_RAW_MEM" },
	{ PU_ERR_DAT_RD_REC_MEM,	"PU_ERR_DAT_RD_REC_MEM" },
	{ PU_ERR_DAT_MALLOC,		"PU_ERR_DAT_MALLOC" },
	{ PU_ERR_DAT_OPEN,			"PU_ERR_DAT_OPEN" },
	{ PU_ERR_DAT_CREATE,		"PU_ERR_DAT_CREATE" },
	{ PU_ERR_DAT_FILE_SIZE,		"PU_ERR_DAT_FILE_SIZE" },
	{ PU_ERR_DAT_RENAME,		"PU_ERR_DAT_RENAME" },
	{ PU_ERR_DAT_FILE_FORMAT,	"PU_ERR_DAT_FILE_FORMAT" },
	{ PU_ERR_DAT_GEN_REPORT,	"PU_ERR_DAT_GEN_REPORT"	},
	{ PU_ERR_REC_NOT_FOUND,		"PU_ERR_REC_NOT_FOUND" },
	{ PU_ERR_INF_MALLOC,		"PU_ERR_INF_MALLOC" },
	{ PU_ERR_ESC_ABORT,			"PU_ERR_ESC_ABORT" }

};

//===================================================================
//  pu_err_str
//===================================================================
char *pu_err_str(uint8_t code)
{
	int		i;
	char	found = 0;

	for (i=0; i<sizeof(pu_err)/sizeof(pu_err[0]); i++)
	{
		if (code == pu_err[i].code)
		{
			found = 1;
			break;
		}
	}

	if (found)
		return pu_err[i].str;
	else
		return "unknown error!";
}

//=============================================================================
//  pu_err_raw_add
//=============================================================================
void pu_err_raw_add(uint32_t id, uint32_t len, uint16_t no, uint8_t *data)
{
	FILE			*fp;
	pu_err_raw_t	eraw;

	fp = fopen("POWERUP.ERR", "ab+");

	if (!fp)
	{
		log_printf("pu_err_raw_add : fopen err!");
		return;
	}

	memset(&eraw, 0, sizeof(pu_err_raw_t));

	eraw.id		= id;
	eraw.len	= len;
	eraw.no		= no;
	fwrite(&eraw, sizeof(pu_err_raw_t), 1, fp);

	fwrite(data, len, sizeof(uint8_t), fp);

	fclose(fp);
}

//=============================================================================
//  pu_err_raw_find
//=============================================================================
uint8_t *pu_err_raw_find(uint16_t no, uint32_t id, file_t *ef)
{
	pu_err_raw_t	*eraw;			// error record
	uint8_t			*pbuf;
	uint8_t			found = 0;
	uint32_t		cnt = 0;

	if (!ef)
		return NULL;

	pbuf = ef->buf;

	do
	{
		eraw = (pu_err_raw_t *)pbuf;

		if (eraw->no == no && eraw->id == id)
		{
			found = 1;
			break;
		}
		else
		{
			// next : not found
			pbuf += sizeof(pu_err_raw_t);
			cnt += sizeof(pu_err_raw_t);

			pbuf += eraw->len;
			cnt += eraw->len;
		}

	} while (cnt < ef->sz);

	if (found)
		return pbuf;	// pu_err_raw_t

	return NULL;
}

//=============================================================================
//  pu_err_raw_output
//=============================================================================
void pu_err_raw_output(void)
{
	FILE			*fp, *ofp;
	uint32_t		fsz;
	uint8_t			*buf;
	char			fname[32];
	uint32_t		cnt;

	pu_err_raw_t	eraw;

	fp = fopen("POWERUP.ERR", "rb");
	if (!fp)
	{
		log_printf("pu_err_raw_output : fopen err!");
		return;
	}

	fseek(fp, 0L, SEEK_END);
	fsz = ftell(fp);

	rewind(fp);

	cnt = 0;
	do
	{
		fread(&eraw, sizeof(pu_err_raw_t), 1, fp);
		fseek(fp, eraw.len, SEEK_CUR);

		cnt += (uint32_t)sizeof(pu_err_raw_t);
		cnt += (uint32_t)eraw.len;

	} while (cnt < fsz);

	if (cnt != fsz)
	{
		log_printf("pu_err_raw_output : fsz != cnt");
		return;
	}
	
	rewind(fp);
	cnt = 0;
	do
	{
		fread(&eraw, sizeof(pu_err_raw_t), 1, fp);
		cnt += (uint32_t)sizeof(pu_err_raw_t);

		buf = (uint8_t *)malloc(sizeof(uint8_t)*eraw.len);
		if (!buf)
		{
			log_printf("pu_err_raw_output : malloc err!");
			return;	
		}
		fread(buf, sizeof(uint8_t), eraw.len, fp);

		cnt += (uint32_t)eraw.len;

		sprintf(fname, "%04X%d.ERR", eraw.id & 0xFFFF, eraw.no);
		ofp = fopen(fname, "wb");
		if (ofp)
		{
			fwrite(buf, sizeof(uint8_t), eraw.len, ofp);
			fclose(ofp);
		}

		free(buf);

	} while (cnt < fsz);

	fclose(fp);

}

//=============================================================================
//  pu_raw_output
//=============================================================================
void pu_raw_output(void)
{
	FILE		*fp;
	char		fname[32];
	
	pu_raw_t	*raw;

	raw = dat->head;
	do
	{
		if (raw)
		{
			sprintf(fname, "%04X.RAW", raw->id);

			fp = fopen(fname, "wb");
			if (!fp)
			{
				log_printf("pu_raw_output : fopen err!");
				break;
			}

			fwrite(raw->data, sizeof(uint8_t), raw->len, fp);
			fclose(fp);
		}

		raw = raw->next;

	} while (raw != NULL);
}


