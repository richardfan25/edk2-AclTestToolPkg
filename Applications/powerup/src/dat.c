//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  DAT : Data File - POWERUP.DAT                                            *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************

// File Layout : POWERUP.DAT
//
// +-----------+
// |           |  magic    00 8  magic : PUHDRV30
// |           |  hsz      08 4  header size
// |           |  fsz      0C 4  file size
// |           |  hdr_off  10 4  header offset
// |           |  par_off  14 4  parameter offset
// |  Header   |  des_off  18 4  descrption offset
// |           |  raw_off  1C 4  compressed-raw offset
// |           |  xrsz     20 4  extraced raw size (decompressed)
// |           |  crsz     24 4  compressed raw size (compressed)
// |           |  raw_crc  28 4  crc32 of craw
// |           |  num_raw  2C 4  number of raw
// |           |  rsvd     30 10 reserved for future use
// +-----------+
// |           |  magic    00 8  magic : PUPARV30
// |           |  psz      08 4  parameter size
// |           |  tm_init  0C 4  initial time
// |           |  tm_zero  10 4  1st time
// |           |  rec_cnt  14 2  record cnt : 0~65535
// |           |  intv     16 2  interval
// |           |  tol      18 2  tolerance
// |           |  sort_ty  1A 1  sort by rec_no/intv
// | Parameter |  alm_sw   1B 1  alarm on/off
// |           |  alm_hi   1C 2  alarm hi freq
// |           |  alm_lo   1E 2  alarm lo freq
// |           |  video_en 20 1  video enable/disable ? INT 10h
// |           |  rst_typ  21 1  reset type : CF9 04,06,0E
// |           |  rst_tmo  22 2  reset timeout : (seconds)
// |           |  at_atx   24 1  0=atx mode, 1=at mode
// |           |  no_rtc   25 1  0=with rtc, 1=without rtc
// |           |  rsvd     26 1A reserved for future use
// +-----------+
// |           |  magic    00 8  magic : PDDESV30
// |           |  dsz      08 4  description size
// |           |  rsvd     0C 4  reserved
// |           |  uname    10 8  utility name        : POWERUP
// |           |  uver     18 8  utility version     : v3.0
// |Description|  udesc    20 30 utility description : Advantech Power Up Record Utility
// |           |  author   50 10 author name         : Richard Fan
// |           |  email    60 20 email address       : richard.fan@advantech.com.tw
// |           |  div      80 10 division            : ECG-FW
// |           |  organ    90 30 organization        : Advantech Embedded Computing Group
// |           |  copyr    C0 40 copyright           : Copyright (C) 2016-2017 Advantech Co.,Ltd. All Rights Reserved
// +-----------+
// |           |
// |           | LZW RAW Data : compressed raw data list
// |           |
// |    LZW    | raw data list : more than one raw data
// |  RAW Data |
// |           |
// |           |
// |           |

// +-----------+ 

// 
//   raw data
// +-----------+
// |   id(4)   | raw id     : indicate what type is raw data
// +-----------+
// |   len(4)  | raw length : length of raw data
// +-----------+
// |     .     | raw data   : N-bytes raw data, where the value N was stored in raw length
// |     .     |
// |raw data(N)|
// |     .     |
// |     .     |
// +-----------+	

//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <malloc.h>
#include <time.h>
#include <sys\types.h>
#include <sys\stat.h>

#include "typedef.h"
#include "t_video.h"
#include "t_msgbox.h"
#include "abios.h"
#include "sound.h"
#include "errcode.h"
#include "dat.h"
#include "crc.h"
#include "pu.h"
#include "upd.h"
#include "log.h"

#ifdef __WATCOMC__


//===========================================================================
//  variables
//===========================================================================
pu_dat_t	*dat = NULL;

//===========================================================================
//  dat_raw_add
//===========================================================================
uint8_t dat_raw_add(uint32_t id, uint8_t *data, uint32_t len)
{
	pu_raw_t	*raw;
	uint8_t		found;

	if (!dat)
		return 0xFF;

	// check id existed ?
	found = 0;
	raw = dat->head;
	if (raw)
	{
		do
		{
			if (id == raw->id)
			{
				found = 1;
				break;
			}
			raw = raw->next;
		} while (raw);
	}
	
	if (found)
		return 0;	// don't add raw if id existed
	
	raw = (pu_raw_t *)malloc(sizeof(pu_raw_t));
	if (!raw)
		return 0xFF;

	memset(raw, 0, sizeof(pu_raw_t));

	raw->data = (uint8_t *)malloc(sizeof(uint8_t)*len);
	if (!raw->data)
	{
		free(raw);
		return 0xFF;
	}

	raw->id = id;
	raw->len = len;
	memcpy(raw->data, data, len);

	if (dat->raw_num == 0)
	{
		dat->head = raw;
		dat->head->prev = NULL;
	}
	else
	{
		dat->curr->next = raw;
		raw->prev = dat->curr;
		raw->next = NULL;
	}
	dat->curr = raw;
	dat->raw_num++;

	return 0;
}

//===========================================================================
//  dat_raw_remove
//===========================================================================
uint8_t dat_raw_remove(uint32_t id)
{
	pu_raw_t	*raw;
	pu_raw_t	*rp, *rn;

	if (!dat)
		return 0xFF;

	raw = dat->head;
	do
	{
		if (id == raw->id)
		{
			rp = raw->prev;
			rn = raw->next;

			if (rp)
				rp->next = rn;
			if (rn)
				rn->prev = rp;
			
			free(raw->data);
			free(raw);

			break;
		}
		
		raw = raw->next;

	} while (raw);

	return 0;
}

//===========================================================================
//  dat_exist
//===========================================================================
uint8_t dat_exist(void)
{
	struct stat		s;

	int		res;

	res = stat(pu->file_name, &s);
	
	if (res < 0)
		return 0;

	//pu->file_size = (uint32_t)s.st_size;

	return 1;
}

//===========================================================================
//  dat_debug
//===========================================================================
#if 0
void dat_debug(void)
{
	FILE		*fp;
	pu_raw_t	*raw;
	pu_rec_t	*rec;
	int			i;

	// debug : POWERUP.DBG
	fp = fopen("POWERUP.DBG", "w");
	if (!fp)
		return;

	fprintf(fp, "Header >>>\n");
	fprintf(fp, "magic    : %8.8s\n", dat->hdr->magic);
	fprintf(fp, "hsz      : %lu = 0x%X\n",  dat->hdr->hsz, dat->hdr->hsz);
	fprintf(fp, "fsz      : %lu = 0x%X\n",  dat->hdr->fsz, dat->hdr->fsz);
	fprintf(fp, "hdr_off  : 0x%X\n", dat->hdr->hdr_off);
	fprintf(fp, "par_off  : 0x%X\n", dat->hdr->par_off);
	fprintf(fp, "des_off  : 0x%X\n", dat->hdr->des_off);
	fprintf(fp, "raw_off  : 0x%X\n", dat->hdr->raw_off);
	fprintf(fp, "xrsz     : %lu\n",  dat->hdr->xrsz);
	fprintf(fp, "crsz     : %lu\n", dat->hdr->crsz);
	fprintf(fp, "raw_crc  : 0x%08X\n", dat->hdr->raw_crc);
	fprintf(fp, "raw_num  : %lu\n", dat->hdr->raw_num);

	fprintf(fp, "Parameters >>>\n\n");
	fprintf(fp, "magic    : %8.8s\n", dat->par->magic);
	fprintf(fp, "dsz      : %lu\n", dat->par->psz);
	fprintf(fp, "tm_init  : 0x%08lX = %s\n", (uint32_t)dat->par->tm_init, ctime(&dat->par->tm_init));
	fprintf(fp, "tm_zero  : 0x%08lX = %s\n", (uint32_t)dat->par->tm_zero, ctime(&dat->par->tm_zero));
	fprintf(fp, "rec_cnt  : %lu <-----------------\n", dat->par->rec_cnt);
	fprintf(fp, "intv     : %lu\n", dat->par->intv);
	fprintf(fp, "tol      : %lu\n", dat->par->tol);
	fprintf(fp, "sort_typ : %d\n", dat->par->sort_typ);		
	//fprintf(fp, "alm_sw   : %d\n", dat->par->alm_sw);
	fprintf(fp, "alm_hi   : %d\n", dat->par->alm_hi);
	fprintf(fp, "alm_lo   : %d\n", dat->par->alm_lo);
	//fprintf(fp, "video_en : %d\n", dat->par->video_en);
	fprintf(fp, "rst_typ  : %d\n", dat->par->rst_typ);
	fprintf(fp, "rst_tmo  : %d\n", dat->par->rst_tmo);
	//fprintf(fp, "at_atx   : %d\n", dat->par->at_atx);
	//fprintf(fp, "no_rtc   : %d\n", dat->par->no_rtc);
	fprintf(fp, "feat     : 0x%04X\n", dat->par->feat);
		
	fprintf(fp, "Description >>>\n\n");
	fprintf(fp, "magic    : %8.8s\n", dat->des->magic);
	fprintf(fp, "dsz      : %lu\n",  dat->des->dsz);
	fprintf(fp, "uname    : %s\n", dat->des->uname);
	fprintf(fp, "uver     : %s\n", dat->des->uver);
	fprintf(fp, "udesc    : %s\n", dat->des->udesc);
	fprintf(fp, "author   : %s\n", dat->des->author);
	fprintf(fp, "email    : %s\n", dat->des->email);
	fprintf(fp, "division : %s\n", dat->des->div);
	fprintf(fp, "organ    : %s\n", dat->des->organ);
	fprintf(fp, "copyr    : %s\n", dat->des->copyr);
	

	fprintf(fp, "\ncompress : %.2f %%\n", (float)dat->hdr->crsz*100.0/(float)dat->hdr->xrsz);
	
	// raw list
	fprintf(fp, "\nAll of RAW ==>\n");
	raw = dat->head;
	i = 0;
	do
	{
		fprintf(fp, "RAW[%2d] : %08X  %8lu\n", i++, raw->id, raw->len);
		raw = raw->next;
	} while (raw != NULL);


	// rec list
	if (dat->par->rec_cnt > 0)
	{
		fprintf(fp, "\nAll of Records ==>\n");

		for (i=0; i<dat->par->rec_cnt; i++)
		{
			rec = (pu_rec_t *)&dat->rec_pool[i];
		
			fprintf(fp, "Rec[%d] : 0x%08X  %.2f  %.2f  %d  0x%02X  0x%04X\n",
				i, rec->tm_rtc, rec->cpu_clk, rec->cpu_tsc, rec->cpu_temp, rec->acl_flag, rec->sys_flag);
		}
	}
	
	fclose(fp);
}
#endif

//===========================================================================
//  dat_create
//===========================================================================
uint8_t dat_create(void)
{
	FILE		*fp;
	uint8_t		*xraw;
	pu_raw_t	*raw;

	//-------------------------------------------------
	//  dat header
	//-------------------------------------------------
	memset(dat->hdr, 0, sizeof(pu_hdr_t));

	memcpy(dat->hdr->magic, PU_HDR_MAGIC, 8);
	dat->hdr->fsz		= 0;	// todo
	
	dat->hdr->hdr_off	= 0;
	dat->hdr->hdr_sz	= sizeof(pu_hdr_t);

	dat->hdr->par_off	= dat->hdr->hdr_off + sizeof(pu_hdr_t);
	dat->hdr->par_sz	= sizeof(pu_par_t);
	
	dat->hdr->des_off	= dat->hdr->par_off + sizeof(pu_par_t);
	dat->hdr->des_sz	= sizeof(pu_des_t);
	
	dat->hdr->raw_off	= dat->hdr->des_off + sizeof(pu_des_t);
	dat->hdr->raw_sz	= 0;	// todo

	dat->hdr->rec_off	= 0;	// todo
	dat->hdr->rec_sz	= 0;	// todo
	
	//-------------------------------------------------
	//  dat parameter
	//-------------------------------------------------
	strcpy(dat->par->magic, PU_PAR_MAGIC);

	dat->par->psz = (uint32_t)sizeof(pu_par_t);

	// others is filled in par_init()

	//-------------------------------------------------
	//  dat description
	//-------------------------------------------------
	strcpy(dat->des->magic, PU_DES_MAGIC);

	dat->des->dsz = (uint32_t)sizeof(pu_des_t);

	// utility
	strcpy(dat->des->uname, PU_DES_NAME);
	strcpy(dat->des->uver,  PU_DES_VER);
	strcpy(dat->des->udesc, PU_DES_DESC);

	// info
	strcpy(dat->des->author, PU_DES_AUTHOR);
	strcpy(dat->des->email,  PU_DES_EMAIL);
	strcpy(dat->des->div,    PU_DES_DIV);
	strcpy(dat->des->organ,  PU_DES_ORGAN);
	strcpy(dat->des->copyr,  PU_DES_COPYR);

	//-------------------------------------------------
	//  dat raw : raw_num and raw_sz
	//-------------------------------------------------
	raw = dat->head;
	dat->hdr->raw_num = 0;
	dat->hdr->raw_sz = 0;
	do
	{
		dat->hdr->raw_sz += (uint32_t)sizeof(uint32_t); // raw : id(4)
		dat->hdr->raw_sz += (uint32_t)sizeof(uint32_t); // raw : len(4)
		dat->hdr->raw_sz += raw->len;	// raw : data(N)
		raw = raw->next;

		dat->hdr->raw_num++;

	} while (raw != NULL);

	//-------------------------------------------------
	//  dat raw : pool
	//-------------------------------------------------
	dat->raw_pool = (uint8_t *)malloc(sizeof(uint8_t) * dat->hdr->raw_sz);
	if (!dat->raw_pool)
		return PU_ERR_DAT_MALLOC;

	xraw = dat->raw_pool;

	// fill raw data in raw pool, 

	// 1st raw
	raw = dat->head;
	do
	{
		// raw id
		memcpy(xraw, &raw->id, sizeof(uint32_t));
		xraw += sizeof(uint32_t);

		// raw len
		memcpy(xraw, &raw->len, sizeof(uint32_t));
		xraw += sizeof(uint32_t);

		// raw data
		memcpy(xraw, raw->data, raw->len);
		xraw += raw->len;

		// next raw
		raw = raw->next;

	} while (raw != NULL);

	//-------------------------------------------------
	//  dat rec
	//-------------------------------------------------
	dat->hdr->rec_sz	= sizeof(pu_rec_t)*(1024);
	dat->hdr->rec_off	= dat->hdr->raw_off + dat->hdr->raw_sz;
	dat->hdr->rec_num	= 0;

	dat->rec_pool = (pu_rec_t *)malloc(dat->hdr->rec_sz);
	if (!dat->rec_pool)
		return PU_ERR_DAT_MALLOC;

	memset(dat->rec_pool, 0, dat->hdr->rec_sz);
		
	//----------------------------------------------------
	//  dat
	//----------------------------------------------------
	// fsz
	dat->hdr->fsz = dat->hdr->rec_off + dat->hdr->rec_sz;



	// crc : par
	xraw = (uint8_t *)dat->par;
	dat->hdr->par_crc = crc_crc32(xraw, dat->hdr->par_sz);

	// crc : des
	xraw = (uint8_t *)dat->des;
	dat->hdr->des_crc = crc_crc32(xraw, dat->hdr->des_sz);
	
	// crc : raw
	xraw = (uint8_t *)dat->raw_pool;
	dat->hdr->raw_crc = crc_crc32(xraw, dat->hdr->raw_sz);

	// crc : rec
	xraw = (uint8_t *)dat->rec_pool;
	dat->hdr->rec_crc = crc_crc32(xraw, dat->hdr->rec_sz);

	// crc : hdr
	xraw = (uint8_t *)dat->hdr;
	dat->hdr->hdr_crc = 0;
	dat->hdr->hdr_crc = crc_crc32(xraw, dat->hdr->hdr_sz);
	
	//----------------------------------------------------
	//  Writing POWERUP.DAT
	//----------------------------------------------------
	//fp = fopen("POWERUP.DAT", "wb");
	fp = fopen(pu->file_name, "wb");
	if (!fp)
		return PU_ERR_DAT_CREATE;

	// writing header, paramter and description
	fwrite(dat->hdr, sizeof(pu_hdr_t), 1, fp);
	fwrite(dat->par, sizeof(pu_par_t), 1, fp);
	fwrite(dat->des, sizeof(pu_des_t), 1, fp);

	fwrite(dat->raw_pool, sizeof(uint8_t), dat->hdr->raw_sz, fp);
	fwrite(dat->rec_pool, sizeof(uint8_t), dat->hdr->rec_sz, fp);

	fclose(fp);

	free(dat->rec_pool);
	free(dat->raw_pool);

	return PU_OK;
}

//===========================================================================
//  dat_read
//===========================================================================
uint8_t dat_read(void)
{
	uint8_t		*xraw;
	uint8_t		res;
	uint32_t	id, len;
	int			r;

	res = PU_OK;

	msgbox_doing(T_WHITE, T_MAGENTA, "DAT Read", "Reading POWERUP.DAT........", 4, 1);

	// raw : raw pool
	dat->raw_pool = (uint8_t *)malloc(sizeof(uint8_t)*dat->hdr->raw_sz);
	if (!dat->raw_pool)
	{
		res = PU_ERR_DAT_RD_RAW_MEM;
		goto err_dat_read;
	}

	// raw : raw pool data fill
	fseek(dat->fp, dat->hdr->raw_off, SEEK_SET);
	if (fread(dat->raw_pool, sizeof(uint8_t), dat->hdr->raw_sz, dat->fp) != (size_t)dat->hdr->raw_sz)
	{
		res = PU_ERR_DAT_RD_RAW;
		goto err_dat_read;
	}

	// raw : crc
	xraw = dat->raw_pool;
	if (dat->hdr->raw_crc != crc_crc32(xraw,dat->hdr->raw_sz))
	{
		res = PU_ERR_DAT_CHK_RAW_CRC;
		goto err_dat_read;
	}
	
	xraw = dat->raw_pool;
	
	// create raw list
	dat->raw_num = 0;
	for (r=0; (uint32_t)r<dat->hdr->raw_num; r++)
	{
		id = *(uint32_t *)xraw;
		xraw += sizeof(uint32_t);

		len = *(uint32_t *)xraw;
		xraw += sizeof(uint32_t);

		dat_raw_add(id, xraw, len);
		xraw += len;
	}

	// rec : record pool
	dat->hdr->rec_sz = (((dat->hdr->rec_num >> 10) & 0x3F) + 1) * 1024 * sizeof(pu_rec_t);

	dat->rec_pool = (pu_rec_t *)malloc(dat->hdr->rec_sz);
	if (!dat->rec_pool)
	{
		res = PU_ERR_DAT_RD_REC_MEM;
		goto err_dat_read;
	}

	// record pool filling
	fseek(dat->fp, dat->hdr->rec_off, SEEK_SET);
	fread(dat->rec_pool, sizeof(uint8_t), dat->hdr->rec_sz, dat->fp);

	// rec : crc
	xraw = (uint8_t *)dat->rec_pool;
	if (dat->hdr->rec_crc != crc_crc32(xraw, dat->hdr->rec_sz))
	{
		res = PU_ERR_DAT_CHK_REC_CRC;
		goto err_dat_read;
	}

err_dat_read:

	fclose(dat->fp);

	msgbox_doing_done();

	if (res != PU_OK)
		log_printf("[%d] %s : error = 0x%02X = %s", pu->no, __FUNCTION__, res, pu_err_str(res));

	return res;
}

//===========================================================================
//  dat_update
//===========================================================================
uint8_t dat_update(void)
{
	FILE		*fp;
	uint8_t		*xraw;
	uint8_t		res;
	uint32_t	wsz;
	struct stat s;

	//-------------------------------------------------------------------------
	//  update handler
	//-------------------------------------------------------------------------
	if (dat->par->tm_zero == 0)
	{
		upd_par_tm_zero();

		upd_rst_tmr();
		
		//log_print("tm_zero == 0");
	}
	else
	{
		//log_print("tm_zero == 1");
		
		upd_rec_tm_rtc();
		
		upd_rec_cpu();

		upd_rec_acl_abios();

		upd_rec_acl_aec();

		upd_rec_sys_cpu();

		upd_rec_sys_e820();

		upd_rec_sys_smbios();

		upd_rec_sys_pci();

		upd_rec_sys_smb();

		upd_rec_sys_spd();

		upd_rec_sys_rtc();

		if (pu->mode == PU_MODE_ON_LINE)
		{
			upd_rec_pool();	// need to be end of update

			upd_par_intv();	// need to update when rec_cnt = 10
		}

		upd_sta_info();
		
		upd_rst_tmr();

		if (pu->mode == PU_MODE_ON_LINE)
			upd_alm_sw();
	}
	
	//dat_debug();

	//-------------------------------------------------
	//  packing POWERUP.DAT
	//-------------------------------------------------

	// crc : par
	xraw = (uint8_t *)dat->par;
	dat->hdr->par_crc = crc_crc32(xraw, dat->hdr->par_sz);

	// crc : rec
	xraw = (uint8_t *)dat->rec_pool;
	dat->hdr->rec_crc = crc_crc32(xraw, dat->hdr->rec_sz);

	// crc : hdr
	xraw = (uint8_t *)dat->hdr;
	dat->hdr->hdr_crc = 0;
	dat->hdr->hdr_crc = crc_crc32(xraw, dat->hdr->hdr_sz);
	
	// POWERUP.TMP
	fp = fopen("POWERUP.TMP", "wb");
	if (!fp)
	{	
		res = PU_ERR_DAT_CREATE;
		goto err_dat_update;
	}

	// writing header, paramter and description
	wsz = (uint32_t)fwrite(dat->hdr, sizeof(uint8_t), dat->hdr->hdr_sz, fp);
	if (wsz != dat->hdr->hdr_sz)
		log_printf("[%d] fwrite hdr err! should be = %d but = %d", pu->no, dat->hdr->hdr_sz, wsz);

	wsz = (uint32_t)fwrite(dat->par, sizeof(uint8_t), dat->hdr->par_sz, fp);
	if (wsz != dat->hdr->par_sz)
		log_printf("[%d] fwrite par err! should be = %d but = %d", pu->no, dat->hdr->par_sz, wsz);

	wsz = (uint32_t)fwrite(dat->des, sizeof(uint8_t), dat->hdr->des_sz, fp);
	if (wsz != dat->hdr->des_sz)
		log_printf("[%d] fwrite des err! should be = %d but = %d", pu->no, dat->hdr->des_sz, wsz);

	wsz = (uint32_t)fwrite(dat->raw_pool, sizeof(uint8_t), dat->hdr->raw_sz, fp);
	if (wsz != dat->hdr->raw_sz)
		log_printf("[%d] fwrite raw err! should be = %d but = %d", pu->no, dat->hdr->raw_sz, wsz);
	
	wsz = (uint32_t)fwrite(dat->rec_pool, sizeof(uint8_t), dat->hdr->rec_sz, fp);
	if (wsz != dat->hdr->rec_sz)
		log_printf("[%d] fwrite rec err! should be = %d but = %d", pu->no, dat->hdr->rec_sz, wsz);

	fflush(fp);
	fclose(fp);

	res = PU_OK;

err_dat_update:

	if (res == PU_OK)
	{
		// POWERUP.TMP confirmed
		if (stat("POWERUP.TMP", &s) == 0)
		{
			// remove POWERUP.DAT ok
			if (remove(pu->file_name) == 0)
			{
				if (rename("POWERUP.TMP", pu->file_name) != 0)
				{
					res = PU_ERR_DAT_RENAME;
				}
				if(stat("POWERUP.DAT", &s) != 0)
				{
					res = PU_ERR_DAT_RENAME;
				}
			}
		}
		else
		{
			log_printf("[%d] stat POWERUP.TMP err!", pu->no);
		}
	}
	else
	{
		log_printf("[%d] %s : error = 0x%02X = %s", pu->no, __FUNCTION__, res, pu_err_str(res));
	}

	return res;
}

//===========================================================================
//  dat_check
//===========================================================================
uint8_t dat_check(void)
{
	uint8_t		res;
	uint8_t		*xraw;
	uint32_t	data32;
	
	res = PU_OK;

	dat->fp = fopen(pu->file_name, "rb");
	if (!dat->fp)
	{
		res = PU_ERR_DAT_CHK_OPEN;
		goto err_dat_check;
	}

	rewind(dat->fp);
	
	// hdr
	if (fread(dat->hdr, sizeof(pu_hdr_t), 1, dat->fp) != 1)
	{
		res = PU_ERR_DAT_RD_HDR;
		goto err_dat_check;
	}

	// hdr : magic
	if (memcmp(dat->hdr->magic, PU_HDR_MAGIC, 8) != 0)
	{
		res = PU_ERR_DAT_CHK_HDR_MAG;
		goto err_dat_check;
	}

	// fsz
	fseek(dat->fp, 0L, SEEK_END);
	data32 = ftell(dat->fp);

	if (data32 != dat->hdr->fsz)
	{
		res = PU_ERR_DAT_CHK_FSZ;
		goto err_dat_check;
	}

	// par
	fseek(dat->fp, dat->hdr->par_off, SEEK_SET);
	if (fread(dat->par, sizeof(uint8_t), dat->hdr->par_sz, dat->fp) != dat->hdr->par_sz)
	{
		res = PU_ERR_DAT_RD_PAR;
		goto err_dat_check;
	}

	// record count
	pu->no = dat->par->rec_cnt;

	// par : magic
	if (memcmp(dat->par->magic, PU_PAR_MAGIC, 8) != 0)
	{
		res = PU_ERR_DAT_CHK_PAR_MAG;
		goto err_dat_check;
	}

	// des
	fseek(dat->fp, dat->hdr->des_off, SEEK_SET);
	if (fread(dat->des, sizeof(uint8_t), dat->hdr->des_sz, dat->fp) != dat->hdr->des_sz)
	{
		res = PU_ERR_DAT_RD_DES;
		goto err_dat_check;
	}

	// des : magic
	if (memcmp(dat->des->magic, PU_DES_MAGIC, 8) != 0)
	{
		res = PU_ERR_DAT_CHK_DES_MAG;
		goto err_dat_check;
	}

	// hdr : crc
	data32 = dat->hdr->hdr_crc;
	dat->hdr->hdr_crc = 0;
	xraw = (uint8_t *)dat->hdr;
	if (data32 != crc_crc32(xraw, dat->hdr->hdr_sz))
	{
		res = PU_ERR_DAT_CHK_HDR_CRC;
		goto err_dat_check;
	}

	// par : crc
	xraw = (uint8_t *)dat->par;
	if (dat->hdr->par_crc != crc_crc32(xraw, dat->hdr->par_sz))
	{
		res = PU_ERR_DAT_CHK_PAR_CRC;
		goto err_dat_check;
	}

	// des : crc
	xraw = (uint8_t *)dat->des;
	if (dat->hdr->des_crc != crc_crc32(xraw, dat->hdr->des_sz))
	{
		res = PU_ERR_DAT_CHK_DES_CRC;
		goto err_dat_check;
	}

err_dat_check:

	if (res != PU_OK)
		log_printf("[%d] %s : error = 0x%02X = %s", pu->no, __FUNCTION__, res, pu_err_str(res));

	return res;
}

//===========================================================================
//  dat_check_board
//===========================================================================
uint8_t dat_check_board(void)
{
	int			ret;
	uint8_t		res;
	pu_raw_t	*raw;
	char		msg[256];

	res = PU_OK;

	ret = abios_init();
	if (ret >= 0)
	{
		raw = raw_find_id(RAW_ID_ACL_BIOS_MODEL);
		if (raw)
		{
			if (memcmp(abios.model, raw->data, raw->len) == 0)
			{
				res = PU_OK;
			}
			else
			{
				sound_error();

				sprintf(msg, "     Target Board : %s\n\nPOWERUP.DAT Board : %s\n\nThe target board is not matched with file!\n\nPlease rename POWERUP.DAT or\nuse the \"%s\" board for testing.", abios.model, raw->data, raw->data);
				msgbox_waitkey(T_WHITE, T_RED, "Error", msg, 4, 1);
				
				res = PU_ERR_DAT_CHK_BRD;
			}
		}

		abios_exit();
	}
	else
	{
		raw = raw_find_id(RAW_ID_ACL_BIOS_MODEL);
		if (raw)
		{
			if (memcmp(abios.model, raw->data, raw->len) == 0)
			{
				res = PU_OK;
			}
			else
			{
				sound_error();

				sprintf(msg, "Target Board : Non-Advantech Board\n\nPOWERUP.DAT Board : %s\n\nCan't lanuch POWERUP if board is not matched!!\nPlease rename POWERUP.DAT and test it again.", raw->data);
				msgbox_waitkey(T_WHITE, T_RED, "Error", msg, 4, 1);
				
				res = PU_ERR_DAT_CHK_BRD;
			}
		}
		else
		{
			res = PU_ERR_NON_ACL_MB;
		}
	}

	if (res != PU_OK)
		log_printf("[%d] %s : error = 0x%02X = %s", pu->no, __FUNCTION__, res, pu_err_str(res));

	return res;
}

//===========================================================================
//  dat_verify
//===========================================================================
int dat_verify(char *fname, uint16_t *reccnt)
{
	FILE		*fp;
	uint8_t		*buf;
	uint32_t	crc;

	pu_hdr_t	hdr;
	pu_par_t	par;
	pu_des_t	des;

	*reccnt = 0;

	fp = fopen(fname, "rb");
	if (!fp)
		return -1;

	rewind(fp);

	// magic : hdr
	fread(&hdr, sizeof(pu_hdr_t), 1, fp);
	if (memcmp(hdr.magic, PU_HDR_MAGIC, sizeof(hdr.magic)) != 0)
	{
		log_printf("hdr.magic err!");
		fclose(fp);
		return -1;
	}
	// magic : par
	fread(&par, sizeof(pu_par_t), 1, fp);
	if (memcmp(par.magic, PU_PAR_MAGIC, sizeof(par.magic)) != 0)
	{
		log_printf("par.magic err!");
		fclose(fp);
		return -1;
	}

	// magic : des
	fread(&des, sizeof(pu_des_t), 1, fp);
	if (memcmp(des.magic, PU_DES_MAGIC, sizeof(des.magic)) != 0)
	{
		log_printf("des.magic err!");
		fclose(fp);
		return -1;
	}
	
	fclose(fp);

	// crc : par
	buf = (uint8_t *)&par;
	crc = crc_crc32(buf, sizeof(pu_par_t));
	if (hdr.par_crc != crc)
	{
		log_printf("par.crc err! par_crc=0x%08X  crc=0x%08X", hdr.par_crc, crc);
		return -1;
	}

	// crc : des
	buf = (uint8_t *)&des;
	crc = crc_crc32(buf, sizeof(pu_des_t));
	if (hdr.des_crc != crc)
	{
		log_printf("des.crc err! des_crc=0x%08X  crc=0x%08X", hdr.des_crc, crc);
		return -1;
	}

	// record count
	*reccnt = par.rec_cnt;

	return 0;
}

//===========================================================================
//  dat_init
//===========================================================================
uint8_t dat_init(void)
{
	if (dat)
		dat_exit();
		//free(dat);

	dat = (pu_dat_t *)malloc(sizeof(pu_dat_t));
	if (!dat)
		return PU_ERR_DAT_INIT_DAT;

	memset(dat, 0, sizeof(pu_dat_t));

	// hdr
	dat->hdr = (pu_hdr_t *)malloc(sizeof(pu_hdr_t));
	if (!dat->hdr)
		return PU_ERR_DAT_INIT_HDR;
	memset(dat->hdr, 0, sizeof(pu_hdr_t));

	// par
	dat->par = (pu_par_t *)malloc(sizeof(pu_par_t));
	if (!dat->par)
		return PU_ERR_DAT_INIT_PAR;
	memset(dat->par, 0, sizeof(pu_par_t));

	// des
	dat->des = (pu_des_t *)malloc(sizeof(pu_des_t));
	if (!dat->des)
		return PU_ERR_DAT_INIT_DES;
	memset(dat->des, 0, sizeof(pu_des_t));

	return PU_OK;
}

//===========================================================================
//  dat_exit
//===========================================================================
void dat_exit(void)
{
	pu_raw_t	*r, *raw;

	if (dat)
	{
		raw = dat->head;
		do
		{
			r = raw;
			raw = r->next;
		
			free(r->data);
			free(r);
		
		} while (raw != NULL);
	
		free(dat);
	}

	dat = NULL;
}

#endif
