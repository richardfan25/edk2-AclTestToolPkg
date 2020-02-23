//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  UPD : Update                                                             *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <malloc.h>

#include "typedef.h"
#include "cpuid.h"
#include "msr.h"
#include "abios.h"
#include "aec.h"
#include "acpi_ec.h"
#include "ite_ec.h"
#include "rdc_ec.h"
#include "e820.h"
#include "smbios.h"
#include "pci.h"
#include "smb.h"
#include "log.h"
#include "crc.h"
#include "rtc.h"
#include "imc.h"

#include "dat.h"
#include "pu.h"
#include "errcode.h"

//===========================================================================
//  extern
//===========================================================================
extern aec_t	*aec;
extern uint64_t	boot_tsc;

//===========================================================================
//  raw_find_id
//===========================================================================
pu_raw_t *raw_find_id(uint32_t id)
{
	pu_raw_t	*raw;
	uint8_t		found;
	
	raw = dat->head;
	found = 0;
	do
	{
		if (raw->id == id)
		{
			found = 1;
			break;
		}

		raw = raw->next;

	} while (raw != NULL);

	if (!found)
		raw = NULL;

	return raw;
}

//===========================================================================
//  upd_par_tm_zero
//===========================================================================
void upd_par_tm_zero(void)
{
	dat->par->tm_zero = time(NULL);
}

//===========================================================================
//  upd_rec_tm_rtc
//===========================================================================
void upd_rec_tm_rtc(void)
{
	dat->rec.tm_rtc = time(NULL);
}

//===========================================================================
//  upd_rec_cpu
//===========================================================================
uint8_t upd_rec_cpu(void)
{
	char			buf[128];

	// cpu_clk : MHz
	// it is not accuracy, remove it for to avoid confusing
	//dat->rec.cpu_clk = (float)(pu->cpu.tsc1 - pu->cpu.tsc0) / 1000000.0;
	
	// cpu_tsc : seconds
	//dat->rec.cpu_tsc = (float)boot_tsc/ (float)(pu->cpu.tsc1 - pu->cpu.tsc0);
	dat->rec.cpu_tsc = (float)boot_tsc/ (float)(1000000000.0);	// unit : Giga

	if (cpuid_get_vendor(buf))			// get cpu vendor
		return PU_ERR_GET_CPU_VENDOR;

	sprintf(pu->cpu.vendor, "%s", buf);

	if (cpuid_get_brand(buf))			// get cpu name
		return PU_ERR_GET_CPU_NAME;
	sprintf(pu->cpu.name, "%s", buf);

	pu->cpu.clock = (uint8_t)cpuid_get_clock(buf);	// get cpu clock
	
	// cpu_temp
	if (strstr(pu->cpu.vendor, "Intel"))
	{
		if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_CPU_NO_TEMP) == 0)
		{
			//pu->cpu.temp = (int)((rdmsr(0x1A2) >> 16) & 0xFF) - (int)((rdmsr(0x19C) >> 16) & 0x7F);
			pu->cpu.temp = (int)((AsmReadMsr64(0x1A2) >> 16) & 0xFF) - (int)((AsmReadMsr64(0x19C) >> 16) & 0x7F);
		}
	
		SET_BIT_MASK(dat->rec.rt_flag, PU_REC_RT_CPU_INTEL);
	}
	else
	{
		pu->cpu.temp = 0xFF;	// not Intel CPU
		CLR_BIT_MASK(dat->rec.rt_flag, PU_REC_RT_CPU_INTEL);
	}

	//-------------------------------------------------------------------------
	//  Temperature : CPU
	//-------------------------------------------------------------------------
	if ((dat->par->therm & 0xF0) == 0x00)
		dat->rec.cpu_temp = pu->cpu.temp;	// might be overwritten by aec_check
	
	return PU_OK;
}

//===========================================================================
//  upd_rec_acl_abios
//===========================================================================
void upd_rec_acl_abios(void)
{
	int			ret;
	pu_raw_t	*raw;
	char		msg[256];
		
	CLR_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_ABIOS_PRE);
	CLR_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_ABIOS_MODEL);
	CLR_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_ABIOS_VER);



	//----------------------------------------------------------
	//  abios
	//----------------------------------------------------------
	ret = abios_init();
	if (ret < 0)
	{
		// non-acl board
		sprintf(pu->abios.magic, "%s", "No ACL BIOS information found!");
	}
	else
	{
		// abios present : abios_init ok
		SET_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_ABIOS_PRE);
		sprintf(pu->abios.magic, "%s", abios.magic);

		//---------------------------------------------------------------------
		//  ABIOS compare : model
		//---------------------------------------------------------------------
		raw = raw_find_id(RAW_ID_ACL_BIOS_MODEL);
		if (raw)
		{
			if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_AB))
			{
				// ABIOS compare: model
				if (memcmp(abios.model, raw->data, raw->len) == 0)
				{
					SET_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_ABIOS_MODEL);
				}
				else
				{
					memcpy(msg, raw->data, raw->len);
					msg[raw->len] = 0;

					if (pu->mode == PU_MODE_ON_LINE)
					{
						log_printf("[%d] abios_model: err!  board=%s file=%s", pu->no, abios.model, msg);
						pu_err_raw_add(	RAW_ID_ACL_BIOS_MODEL, (uint32_t)strlen(abios.model), pu->no, (uint8_t *)abios.model);
					}
				}
			}
		}

		//---------------------------------------------------------------------
		//  ABIOS compare : ver
		//---------------------------------------------------------------------
		raw = raw_find_id(RAW_ID_ACL_BIOS_VER);
		if (raw)
		{
			// ABIOS : ver
			if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_AB))
			{
				if (memcmp(abios.ver, raw->data, raw->len) == 0)
				{
					SET_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_ABIOS_VER);
				}
				else
				{
					memcpy(msg, raw->data, raw->len);
					msg[raw->len] = 0;

					if (pu->mode == PU_MODE_ON_LINE)
					{
						log_printf("[%d] abios_ver: err!  board=%s file=%s", pu->no, abios.ver, msg);
						pu_err_raw_add(	RAW_ID_ACL_BIOS_VER, (uint32_t)strlen(abios.ver), pu->no, (uint8_t *)abios.ver);
					}
				}
			}
		}
	}
}

//===========================================================================
//  upd_rec_acl_aec
//===========================================================================
void upd_rec_acl_aec(void)
{
	int				ret, res;
	uint8_t			buf[32];
	char			msg[256];
	uint8_t			len, tmp[8];
	uint16_t		ktemp;
	pu_raw_t		*raw;
	rdc_fw_prot_t	fprot;
	rdc_sw_prot_t	sprot;
	ite_fw_prot_t	iprot;

	CLR_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_AEC_PRE);
	CLR_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_AEC_CHIP);
	CLR_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_AEC_BOARD);
	CLR_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_AEC_VER);

	

	//----------------------------------------------------------
	//  aec
	//----------------------------------------------------------
	aec = aec_init();
	if (!aec)
	{
		// non-ec
		sprintf(pu->aec.chip, "%s", "No ACL EC found!");
		pu->aec.board[0] = 0;
		pu->aec.ver[0] = 0;
	}
	else
	{
		memset(buf, 0, sizeof(buf));
			
		switch (aec_get_type(aec))
		{
		//---------------------
		//  ITE IT8518
		//  ITE IT8528
		//---------------------
		case AEC_TYPE_ITE_8518:
		case AEC_TYPE_ITE_8528:
			sprintf(pu->aec.chip, "ITE IT%04X", aec_get_chip_id(aec));
			SET_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_AEC_PRE);

			iprot.cmd = 0xF0;
			iprot.len = 17;
			iprot.dat = buf;
			res = ite_ec_fw_prot_read(&iprot);

			// board, ver
			if (res != AEC_OK)
			{
				sprintf(pu->aec.board, "%s", "board err!");
				sprintf(pu->aec.ver, "ver err! 0x%02X", res);
			}
			else
			{
				memcpy(pu->aec.board, buf, 8);

				//memcpy(pu->aec.ver, &buf[8], 9);
				//memcpy(&aec.fw, &buf[8], 9);
				sprintf(pu->aec.ver, "%c%02X%02X_%02X%02X",
						buf[14], buf[15], buf[16], buf[9], buf[10]);
			}
			
			break;

		//---------------------
		//  ITE IT5121
		//---------------------
		case AEC_TYPE_ITE_5121:
			sprintf(pu->aec.chip, "ITE IT%04X", aec_get_chip_id(aec));
			SET_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_AEC_PRE);

			res = ite_mbox_read_fw_info(buf);
			
			// board, ver
			if (res != ITE_OK)
			{
				sprintf(pu->aec.board, "%s", "board err!");
				sprintf(pu->aec.ver, "ver err! 0x%02X", res);
			}
			else
			{
				memcpy(pu->aec.board, buf, 8);
				pu->aec.board[8] = 0;

				sprintf(pu->aec.ver, "%c%02X%02X_%02X%02X",
						buf[14], buf[15], buf[16], buf[9], buf[10]);
			}

			break;

		//---------------------
		//  RDC A9610 SW
		//---------------------
		case AEC_TYPE_RDC_SW:
			sprintf(pu->aec.chip, "RDC A%04X", aec_get_chip_id(aec));
			SET_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_AEC_PRE);

			// board name length
			sprot.cmd = RDC_SW_CMD_PLAT_INFO;
			sprot.off = 0x06;	// platform name length
			sprot.len = 1;
			sprot.dat = buf;
			res = rdc_sw_prot_noidx_read(&sprot);
			len = buf[0];

			if (res != AEC_OK)
			{
				sprintf(pu->aec.board, "%s", "board err!");
			}
			else
			{
				memset(buf, 0, sizeof(buf));

				// board name string
				sprot.cmd = RDC_SW_CMD_PLAT_INFO;
				sprot.off = 0x07;	// platform name
				sprot.len = len;
				sprot.dat = buf;
				res = rdc_sw_prot_noidx_read(&sprot);

				if (res != AEC_OK)
				{
					sprintf(pu->aec.board, "%s", "board err!");
				}
				else
				{
					strcpy(pu->aec.board, buf);
				}
			}
			
			//strcpy(pu->aec.board, "board todo!");

			// ver
			sprot.cmd = RDC_SW_CMD_RD_FWVER;
			sprot.off = 0x00;
			sprot.len = 9;
			sprot.dat = buf;
			res = rdc_sw_prot_noidx_read(&sprot);

			if (res != AEC_OK)
			{
				sprintf(pu->aec.ver, "ver err! 0x%02X", res);
			}
			else
			{
				strcpy(pu->aec.ver, buf);
			}
			
			break;

		//---------------------
		//  RDC A9610 FW
		//---------------------
		case AEC_TYPE_RDC_FW:
			sprintf(pu->aec.chip, "RDC A%04X", aec_get_chip_id(aec));
			SET_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_AEC_PRE);

			// board
			fprot.cmd = RDC_FW_CMD_RD_BOARD;
			fprot.ctl = 0x10;	// board name
			fprot.dev = 0;
			fprot.len = 16;
			fprot.dat = buf;
			res = rdc_fw_prot_read(&fprot);
			if (res != AEC_OK)
			{
				sprintf(pu->aec.board, "board err! 0x%02X", res);
			}
			else
			{
				strcpy(pu->aec.board, buf);
			}
			
			// ver
			fprot.cmd = RDC_FW_CMD_RD_BOARD;
			fprot.ctl = 0x22;
			fprot.dev = 0;
			fprot.len = 16;
			fprot.dat = buf;
			res = rdc_fw_prot_read(&fprot);

			if (res != AEC_OK)
			{
				sprintf(pu->aec.ver, "ver err! 0x%02X", res);
			}
			else
			{
				strcpy(pu->aec.ver, buf);
			}
			break;

		default:
			break;
		}

		//---------------------------------------------------------------------
		//  AEC compare
		//---------------------------------------------------------------------
		if (TST_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_AEC_PRE))
		{
			// aec_chip
			raw = raw_find_id(RAW_ID_ACL_EC_CHIP);
			if (raw)
			{
				if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_AEC))
				{
					// compare aec chip name
					if (memcmp(pu->aec.chip, raw->data, raw->len) == 0)
					{
						SET_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_AEC_CHIP);
					}
					else
					{
						memcpy(msg, raw->data, raw->len);
						msg[raw->len] = 0;

						if (pu->mode == PU_MODE_ON_LINE)
						{
							log_printf("[%d] aec_chip: err!  board=%s file=%s", pu->no, pu->aec.chip, msg);
							pu_err_raw_add(	RAW_ID_ACL_EC_CHIP, (uint32_t)strlen(pu->aec.chip), pu->no, (uint8_t *)pu->aec.chip);
						}
					}
				}
			}

			// aec_board
			raw = raw_find_id(RAW_ID_ACL_EC_BOARD);
			if (raw)
			{
				if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_AEC))
				{
					// compare aec board name
					if (memcmp(pu->aec.board, raw->data, raw->len) == 0)
					{
						SET_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_AEC_BOARD);
					}
					else
					{
						memcpy(msg, raw->data, raw->len);
						msg[raw->len] = 0;

						if (pu->mode == PU_MODE_ON_LINE)
						{
							log_printf("[%d] aec_board: err!  board=%s file=%s", pu->no, pu->aec.board, msg);
							pu_err_raw_add(	RAW_ID_ACL_EC_BOARD, (uint32_t)strlen(pu->aec.board), pu->no, (uint8_t *)pu->aec.board);
						}
					}
				}
			}
		
			// aec_ver
			raw = raw_find_id(RAW_ID_ACL_EC_VER);
			if (raw)
			{
				if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_AEC))
				{
					// compare aec version
					if (memcmp(pu->aec.ver, raw->data, raw->len) == 0)
					{
						SET_BIT_MASK(dat->rec.acl_flag, PU_REC_ACL_AEC_VER);
					}
					else
					{
						memcpy(msg, raw->data, raw->len);
						msg[raw->len] = 0;

						if (pu->mode == PU_MODE_ON_LINE)
						{
							log_printf("[%d] aec_ver: err!  board=%s file=%s", pu->no, pu->aec.ver, msg);
							pu_err_raw_add(	RAW_ID_ACL_EC_VER, (uint32_t)strlen(pu->aec.ver), pu->no, (uint8_t *)pu->aec.ver);
						}
					}
				}
			}
		}

		//-------------------------------------------------------------------------
		//  Temperature : CPU or RDC-EC
		//-------------------------------------------------------------------------
		// RDC-EC thermal
		if ((dat->par->therm & 0xF0) == 0x10)
		{
			fprot.cmd = RDC_FW_CMD_RD_THERM;
			fprot.ctl = 0x10;	// thermal temperature
			fprot.dev = (dat->par->therm & 0x3);	// bit[1:0]=therm ch
			fprot.len = 2;
			fprot.dat = tmp;
			ret = rdc_fw_prot_read(&fprot);

			if (ret == AEC_OK)
			{
				ktemp = *(uint16_t *)tmp;

				if (ktemp >= 2731)
				{
					dat->rec.cpu_temp = (uint8_t)((ktemp - 2731) / 10);
					if (((ktemp - 2731)%10) > 4)
						dat->rec.cpu_temp++;
				}
				else
				{
					dat->rec.cpu_temp = (uint8_t)(0xFF - (2731-ktemp)/10 + 1);
					if (((2731-ktemp)%10) > 4)
						dat->rec.cpu_temp--;
				}
			}
			else
			{
				dat->rec.cpu_temp = 0x80;	// error : -128C
			}
		}
	}
}

//===========================================================================
//  upd_rec_sys_cpu
//===========================================================================
void upd_rec_sys_cpu(void)
{
	pu_raw_t	*raw;
	char		msg[256];

	CLR_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_CPU_VENDOR);
	CLR_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_CPU_NAME);
	
	//---------------------------------------------------------------
	//  cpu vendor
	//---------------------------------------------------------------
	raw = raw_find_id(RAW_ID_CPU_VENDOR);
	if (raw)
	{
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_CP))
			{
			// compare aec board name
			if (memcmp(raw->data, pu->cpu.vendor, raw->len) == 0)
			{
				SET_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_CPU_VENDOR);
			}
			else
			{
				memcpy(msg, raw->data, raw->len);
				msg[raw->len] = 0;

				if (pu->mode == PU_MODE_ON_LINE)
				{
					log_printf("[%d] cpu_vendor: err!  board=%s file=%s", pu->no, pu->cpu.vendor, msg);
					pu_err_raw_add(	RAW_ID_CPU_VENDOR, (uint32_t)strlen(pu->cpu.vendor), pu->no, (uint8_t *)pu->cpu.vendor);
				}
			}
		}
	}

	//---------------------------------------------------------------
	//  cpu name
	//---------------------------------------------------------------
	raw = raw_find_id(RAW_ID_CPU_NAME);
	if (raw)
	{
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_CP))
		{
			// compare aec board name
			if (memcmp(raw->data, pu->cpu.name, raw->len) == 0)
			{
				SET_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_CPU_NAME);
			}
			else
			{
				memcpy(msg, raw->data, raw->len);
				msg[raw->len] = 0;

				if (pu->mode == PU_MODE_ON_LINE)
				{
					log_printf("[%d] cpu_name: err!  board=%s file=%s", pu->no, pu->cpu.name, msg);
					pu_err_raw_add(	RAW_ID_CPU_NAME, (uint32_t)strlen(pu->cpu.name), pu->no, (uint8_t *)pu->cpu.name);
				}
			}
		}
	}
}

//===========================================================================
//  upd_rec_sys_e820
//===========================================================================
void upd_rec_sys_e820(void)
{
	pu_raw_t	*raw;
	uint8_t		res;
	uint32_t	kbsz_min;
	uint32_t	kbsz_max;

	CLR_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_E820);

	//---------------------------------------------------------------
	//  e820 mem size
	//---------------------------------------------------------------
	res = e820_init();
	if (res == E820_OK)
	{
		raw = raw_find_id(RAW_ID_MEM_E820_KSZ);
		if (raw)
		{
			if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_E8))
			{
				// kbsz is different in legacy and uefi.
				// need to find the better comparision of kbsz
				// ex:
				//   8 GiB = 8192 MiB = 8038 MiB (E820-legacy) = 8230912 KiB
				//                    = 8045 MiB (E820-UEFI)   = 8238080 KiB
				//
				// we determine the compare range = 1 GiB (min unit of RAM)
				// so kbsz between 7GiB ~ 8GiB ==> pass
				//                        else ==> failed
				kbsz_min = *(uint32_t *)raw->data;
				kbsz_min &= 0xFFF00000;			// ex: 7 GiB
				
				kbsz_max = kbsz_min + 0x100000;	// ex: 8 GiB

				// compare kb size
				if ((e820.kbsz > kbsz_min) && (e820.kbsz <= kbsz_max))
				{
					SET_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_E820);
				}
				else
				{
					if (pu->mode == PU_MODE_ON_LINE)
					{
						log_printf("[%d] e820_size: err!  board=%d file=%d", pu->no, e820.kbsz, *(uint32_t *)raw->data);
						pu_err_raw_add(	RAW_ID_MEM_E820_KSZ, 4, pu->no,	(uint8_t *)&e820.kbsz);
					}
				}
			}
		}
	}
	e820_exit();
}

//===========================================================================
//  upd_rec_sys_smbios
//===========================================================================
void upd_rec_sys_smbios(void)
{
	pu_raw_t	*raw;
	int			ret;

	// SMBIOS not present
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SMBIOS) == 0)
		return;
		
	CLR_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_SMBIOS);
	//---------------------------------------------------------------
	//  smbios
	//---------------------------------------------------------------
	ret = smbios_init();
	if (ret == SMBIOS_OK)
	{
		raw = raw_find_id(RAW_ID_DMI_SMBIOS);
		if (raw)
		{
			if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_SM))
			{
				#if 0
				// compare aec board name
				//if (memcmp((uint8_t *)smbios.hdr->smbios_addr, raw->data, smbios.hdr->smbios_len) == 0)
				if (memcmp((uint8_t *)smbios.hdr->smbios_addr, raw->data, raw->len) == 0)
				{
					SET_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_SMBIOS);

					// debug
					//if (pu->mode == PU_MODE_ON_LINE)
					//pu_err_raw_add(	RAW_ID_DMI_SMBIOS,
					//				(uint32_t)smbios.hdr->smbios_len,
					//				pu->no,	(uint8_t *)smbios.hdr->smbios_addr);
				}
				#endif

				// change comparing rule : just compare length instead of content
				if (smbios.hdr->smbios_len == raw->len)
				{
					SET_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_SMBIOS);
				}
				else
				{
					if (pu->mode == PU_MODE_ON_LINE)
					{
						log_printf("[%d] smbios: compare err! raw_len=%d, smbios_len=%d", pu->no, raw->len, smbios.hdr->smbios_len);
						pu_err_raw_add(	RAW_ID_DMI_SMBIOS, (uint32_t)smbios.hdr->smbios_len, pu->no, (uint8_t *)smbios.hdr->smbios_addr);
					}
				}
			}
		}
		else
		{
			if (pu->mode == PU_MODE_ON_LINE)
			{
				log_printf("[%d] smbios: raw data not found.", pu->no);
				pu_err_raw_add(	RAW_ID_DMI_SMBIOS, (uint32_t)smbios.hdr->smbios_len, pu->no, (uint8_t *)smbios.hdr->smbios_addr);
			}
		}
	}
	else
	{
		log_printf("[%d] smbios: smbios_init err!", pu->no);
	}
	smbios_exit();
}

//===========================================================================
//  upd_rec_sys_pci
//===========================================================================
void upd_rec_sys_pci(void)
{
	pu_raw_t	*num, *lst, *bdf;
	pci_dev_t	*pd;

	uint8_t		*lstptr, *bdfptr;
	uint8_t		bus, dev, fun;
	uint16_t	vid, did;
	uint8_t		res, err, found;
	int			i, j;
	uint8_t		*pool, *pptr;

	CLR_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_PCI_SCAN);
	//---------------------------------------------------------------
	//  pci device scan
	//---------------------------------------------------------------
	res = pci_init();
	err = 0;

	if (res != PCI_OK)
	{
		log_printf("[%d] pci_init: err! res=%d", pu->no, res);
		err |= 0x80;
	}
	else
	{
		//---------------------------------------------------------------
		//  raw : num
		//---------------------------------------------------------------
		num = raw_find_id(RAW_ID_PCI_DEV_NUM);
		if (!num)
		{
			err |= 0x1;
			if (pu->mode == PU_MODE_ON_LINE)
			{
				log_printf("[%d] pci_num: not found in file", pu->no);
				pu_err_raw_add(RAW_ID_PCI_DEV_NUM, 4, pu->no, (uint8_t *)&pci->num_dev);
			}
		}

		//---------------------------------------------------------------
		//  raw : lst
		//---------------------------------------------------------------
		lst = raw_find_id(RAW_ID_PCI_DEV_LST);
		if (!lst)
		{
			err |= 0x10;
			log_printf("[%d] pci_lst: not found in file", pu->no);
		}

		//---------------------------------------------------------------
		//  raw : bdf
		//---------------------------------------------------------------
		bdf = raw_find_id(RAW_ID_PCI_DEV_BDF);
		if (!bdf)
		{
			err |= 0x20;
			log_printf("[%d] pci_bdf: not found in file", pu->no);
		}

		//---------------------------------------------------------------
		//  raw : num
		//---------------------------------------------------------------
		if (num)
		{
			if (pci->num_dev != *(int *)num->data)
			{
				err = 1;
				log_printf("[%d] pci_num: err! board=%d  file=%d", pu->no, pci->num_dev, *(int *)num->data);
			}

			//---------------------------------------------------------------
			//  raw : lst, bdf
			//---------------------------------------------------------------
			if (lst && bdf)
			{
				// compare vid/did
				lstptr = lst->data;
				bdfptr = bdf->data;

				//---------------------------------------------------------------
				// dev list in file ----(find)---> dev list in board
				//---------------------------------------------------------------
				for (i=0; i<*(int *)num->data; i++)
				{
					vid = *(uint16_t *)lstptr;	lstptr += 2;
					did = *(uint16_t *)lstptr;	lstptr += 2;
					
					bus = *(uint8_t*)bdfptr++;
					dev = *(uint8_t*)bdfptr++;
					fun = *(uint8_t*)bdfptr++;

					pd = pci_find_bus_dev_fun_vid_did(bus, dev, fun, vid, did);
					if (pd == NULL)
					{
						err |= 0x2;
						log_printf("[%d] pci_scan: file=B:%02X D:%02X F:%02X VID:%04X DID:%04X not found in target board",
								pu->no, bus, dev, fun, vid, did);
						break;
					}
				}

				//---------------------------------------------------------------
				// dev list in board ----(find)---> dev list in file
				//---------------------------------------------------------------
				pd = pci->dev;
				for (i=0; i<pci->num_dev; i++)
				{
					lstptr = lst->data;
					bdfptr = bdf->data;

					found = 0;
					for (j=0; j<*(int *)num->data; j++)
					{
						vid = *(uint16_t *)lstptr;	lstptr += 2;
						did = *(uint16_t *)lstptr;	lstptr += 2;
				
						bus = *(uint8_t*)bdfptr++;
						dev = *(uint8_t*)bdfptr++;
						fun = *(uint8_t*)bdfptr++;
							
						if (bus == pd->bus &&
							dev == pd->dev &&
							fun == pd->fun &&
							vid == pd->cfg.ven_id &&
							did == pd->cfg.dev_id)
						{
							// found in file
							found = 1;
							break;
						}
					}

					if (found == 0)
					{
						err |= 0x4;
						log_printf("[%d] pci_scan: board=B:%02X D:%02X F:%02X VID:%04X DID:%04X not found in POWERUP.DAT file",
								pu->no, pd->bus, pd->dev, pd->fun, pd->cfg.ven_id, pd->cfg.dev_id);
						break;
					}

					pd = pd->next;
				}
			}

			// something error : logging num, bdf and lst
			if (err)
			{
				// num
				if (pu->mode == PU_MODE_ON_LINE)
					pu_err_raw_add(RAW_ID_PCI_DEV_NUM, 4, pu->no, (uint8_t *)&pci->num_dev);

				// bdf
				pool = (uint8_t *)malloc(pci->num_dev * sizeof(uint8_t) * 3);
				if (!pool)
				{
					log_printf("[%d] pci : bdf err logging, malloc err!", pu->no);
				}
				else
				{
					pptr = pool;
					pd = pci->dev;
					for (i=0; i<pci->num_dev; i++, pd = pd->next)
					{
						// bus, dev, fun, vid, did
						*pptr++ = pd->bus;
						*pptr++ = pd->dev;
						*pptr++ = pd->fun;
					}
					if (pu->mode == PU_MODE_ON_LINE)
						pu_err_raw_add(RAW_ID_PCI_DEV_BDF, (pci->num_dev * 3), pu->no, pool);
					free(pool);
				}
				
				// lst : vid, did
				pool = (uint8_t *)malloc(pci->num_dev * sizeof(uint8_t) * 4);
				if (!pool)
				{
					log_printf("[%d] pci : lst err logging, malloc err!", pu->no);
				}
				else
				{
					pptr = pool;
					pd = pci->dev;
					for (i=0; i<pci->num_dev; i++, pd = pd->next)
					{
						*(uint16_t *)pptr = (uint16_t)pd->cfg.ven_id;
						pptr += 2;
						*(uint16_t *)pptr = (uint16_t)pd->cfg.dev_id;
						pptr += 2;
					}
					if (pu->mode == PU_MODE_ON_LINE)
						pu_err_raw_add(RAW_ID_PCI_DEV_LST, (pci->num_dev * 4), pu->no, pool);
					free(pool);
				}

			}// err-logging
		}
	}

	if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_PC))
	{
		if (err == 0)
		{
			SET_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_PCI_SCAN);
		}
		else
		{
			log_printf("[%d] pci_chk: err=0x%02X", pu->no, err);
		}
	}

	// do not pci resource here due to use later
	//pci_exit();
}

//===========================================================================
//  upd_rec_sys_smb
//===========================================================================
void upd_rec_sys_smb(void)
{
	pu_raw_t	*raw;
	smb_dev_t	*sd;
	uint8_t		*pool, *ptr;
	uint8_t		res, addr, err;
	int			i = 0, num;

	// SMBUS not present
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SMB) == 0)
		return;

	CLR_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_SMB_SCAN);
	
	//---------------------------------------------------------------
	//  smbus device scan
	//---------------------------------------------------------------
	err = 0;

	res = smb_init();
	if (res == SMB_OK)
	{
		// scanning smbus is too slow, just get device list and
		// check device is exist
		raw = raw_find_id(RAW_ID_SMB_DEV_NUM);
		if (raw)
		{
			num = *(uint8_t *)raw->data;
		}
		else
		{
			num = -1;
			err = 1;
			if (pu->mode == PU_MODE_ON_LINE)
			{
				log_printf("[%d] smb_dev_num: not found in file", pu->no);
				pu_err_raw_add(RAW_ID_SMB_DEV_NUM, sizeof(smb->num_dev), pu->no, (uint8_t *)&smb->num_dev);
			}
		}

		if (num > 0)
		{
			raw = raw_find_id(RAW_ID_SMB_DEV_LST);

			if (raw)
			{
				ptr = raw->data;
				for (i=0; i<num; i++)
				{
					addr = *(uint8_t *)ptr++;

					if (smb_scan_dev(addr) != SMB_OK)
					{
						err = 1;
						if (pu->mode == PU_MODE_ON_LINE)
							log_printf("[%d] smb_scan_dev: not found! addr=0x%02X defined in file but not on target board", pu->no, addr);

						pool = (uint8_t *)malloc(smb->num_dev * 1);
						if (pool)
						{
							sd = smb->dev;
							do
							{
								pool[i++] = sd->addr;
								sd = sd->next;
							} while (sd != NULL);

							if (pu->mode == PU_MODE_ON_LINE)
								pu_err_raw_add(RAW_ID_SMB_DEV_LST, smb->num_dev, pu->no, pool);
							free(pool);
						}

						break;
					}
				} //for
			}
			else
			{
				pool = (uint8_t *)malloc(smb->num_dev * 1);
				if (pool)
				{
					sd = smb->dev;
					do
					{
						pool[i++] = sd->addr;
						sd = sd->next;
					} while (sd != NULL);

					if (pu->mode == PU_MODE_ON_LINE)
						pu_err_raw_add(RAW_ID_SMB_DEV_LST, smb->num_dev, pu->no, pool);
					free(pool);
				}

				if (pu->mode == PU_MODE_ON_LINE)
					log_printf("[%d] smb_dev_lst: not found in file", pu->no);
			}
		}
	}
	else
	{
		err = 1;
		log_printf("[%d] smb_init: err!", pu->no);
	}

	if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_SB))
	{
		if (err == 0)
		{
			SET_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_SMB_SCAN);
		}
	}
}

//===========================================================================
//  upd_rec_sys_spd
//===========================================================================
void upd_rec_sys_spd(void)
{
	pu_raw_t	*raw;
	uint8_t		*ptr;
	uint8_t		addr, err;
	int			i, num;

	// SPD not present
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SPD) == 0)
		return;

	CLR_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_SMB_SPD);
	//---------------------------------------------------------------
	//  smbus device scan
	//---------------------------------------------------------------
	err = 0;

	// smb_init() : upd_rec_sys_smb()
	
	raw = raw_find_id(RAW_ID_SMB_SPD_NUM);
	if (raw)
	{
		num = *(uint8_t *)raw->data;
	}
	else
	{
		num = -1;
		err = 1;

		log_printf("[%d] spd_num: not found in file", pu->no);
	}

	if (num > 0)
	{
		raw = raw_find_id(RAW_ID_SMB_SPD_LST);

		if (raw)
		{
			ptr = raw->data;
			for (i=0; i<num; i++)
			{
				addr = *(uint8_t *)ptr++;

				// IMC SMBus
				if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_IMC_SPD))
				{
					if (imc_init() == IMC_OK)
					{
						imc_probe_spd();
						// bit5: 0=ch0, 1=ch1
						// 0xDF: mask bit5
						if (imc_scan_spd(!!(addr&0x20), (addr&0xDF)) != IMC_OK)
						{
							err = 1;
							break;
						}
					}
					else
					{
						err = 1;
						break;
					}
					
				}
				// ICH SMBus
				else
				{
					if (smb_scan_dev(addr) != SMB_OK)
					{
						err = 1;
						break;
					}
				}
			}
		}
		else
		{
			log_printf("[%d] spd_lst: not found in file", pu->no);
		}
	}
	else
	{
		err = 1;
		log_printf("[%d] spd_num: num = 0, no spd?", pu->no);
	}

	if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_SP))
	{
		if (err == 0)
		{
			SET_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_SMB_SPD);
		}
	}
}

//===========================================================================
//  upd_rec_sys_rtc
//===========================================================================
void upd_rec_sys_rtc(void)
{
	uint8_t		err;
	uint8_t		regABD[3];

	//---------------------------------------------------------------
	//  read RTC registers
	//---------------------------------------------------------------

	regABD[0] = rtc_read_reg(0x0A);
	regABD[1] = rtc_read_reg(0x0B);
	regABD[2] = rtc_read_reg(0x0D);
	
	err = 0;

	// 0Ah - status A (26h)
	// bit[7] : 1=time update progress, 0=time/data available
	// bit[6:4] : stage divisor (010=32.768kHz)
	// bit[3:0] : rate selection bit for divisor output freq. (0110=1.024 kHz)
	if ((regABD[0] & 0x7F) != 0x26)
	{
		// [6:4] = stage divisor
		// [3:0] = rate selection
		err |= 0x1;
		log_printf("[%d] rtc_0a : 0x%02X != 0x26", pu->no, regABD[0]);
	}
	
	if (regABD[0] & 0x80)
	{
		// [7] = time updating : do not check it
		log_printf("[%d] rtc_0a : 0x%02X, date/time update in progress", pu->no, regABD[0]);
	}
	
	// 0Bh - status B (02h)
	// bit[7] : 1=disable clock update, allow time to be set    0=update count normally
	// bit[6] : 1=enable periodic interrupt,     0=disable
	// bit[5] : 1=enable alarm interrupt,        0=disable
	// bit[4] : 1=enable update ended interrupt, 0=disable
	// bit[3] : 1=enable square wave frequency,  0=disable
	// bit[2] : 1=time/date in binary,           0=BCD (BCD default)
	// bit[1] : 1=24 hour mode,                  0=12 hour mode (24 default)
	// bit[0] : 1=enable daylight savings,       0=disable (default)
	if ((regABD[1] & 0x7F) != 0x02)
	{
		// & 0x7F : no more checking bit[7]

		// [7:0] = status ([1]=24/12 mode ?)
		err |= 0x2;
		log_printf("[%d] rtc_0b : 0x%02X != 0x02", pu->no, regABD[1]);
	}

	// 0Dh - status D  (80h)
	// bit[7] :1=CMOS RAM has power, 0=lost power
	// bit[6:0] : reserved (set to 0)
	if ((regABD[2] & 0x80) != 0x80)
	{
		// [7] = CMOS RAM power status
		err |= 0x4;
		log_printf("[%d] rtc_0d : 0x%02X != 0x80", pu->no, regABD[2]);
	}

	if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_RTC))
	{
		// reg A = 26h
		if (err & 0x1)
		{
			CLR_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_RTC_0A);
		}
		else
		{
			SET_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_RTC_0A);
		}

		// reg B = 02h
		if (err & 0x2)
		{
			CLR_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_RTC_0B);
		}
		else
		{
			SET_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_RTC_0B);
		}

		// reg D = 80h
		if (err & 0x4)
		{
			CLR_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_RTC_0D);
		}
		else
		{
			SET_BIT_MASK(dat->rec.sys_flag, PU_REC_SYS_RTC_0D);
		}

		// error log
		if (err)
		{
			if (pu->mode == PU_MODE_ON_LINE)
				pu_err_raw_add(RAW_ID_RTC_ABD, 3, pu->no, regABD);
		}
	}
}

//===========================================================================
//  upd_rec_pool
//===========================================================================
uint8_t upd_rec_pool(void)
{
	pu_rec_t	*rec;
	uint16_t	cnt;
	uint32_t	off;
	time_t		tm0;
	uint8_t		*xraw;
	//int			i;
	
	cnt = (uint16_t)dat->hdr->rec_num;
	off = cnt * sizeof(pu_rec_t);

	dat->par->rec_cnt = (uint16_t)dat->hdr->rec_num;

	rec = (pu_rec_t *)&dat->rec_pool[cnt];
	if (cnt == 0)
	{
		tm0 = dat->par->tm_zero;
	}
	else
	{
		rec = (pu_rec_t *)&dat->rec_pool[cnt-1];	// prev rec
		tm0 = rec->tm_rtc;
	}

	// two vars are equal
	dat->hdr->rec_num++;
	dat->par->rec_cnt++;

	// no
	dat->rec.no = dat->par->rec_cnt;	// 1~

	// intv
	dat->rec.intv = (int)dat->rec.tm_rtc - (int)tm0;

	// qc
/*
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_NO_RTC))
	{
		rec = (pu_rec_t *)&dat->rec_pool[0];

		for (i=0; i<dat->par->rec_cnt; i++)
			rec->qc = PU_REC_QC_NONE;
	}
	else
	{
		rec = (pu_rec_t *)&dat->rec_pool[0];

		if (dat->par->rec_cnt > 10)
		{
			for (i=0; i<dat->par->rec_cnt; i++)
			{
				if (dat->rec.intv == 0)
					dat->rec.qc = PU_REC_QC_NA;
				else if (dat->rec.intv < 0)
					dat->rec.qc = PU_REC_QC_ER;
				else if (dat->rec.intv > dat->par->intv)
					dat->rec.qc = PU_REC_QC_GG;
				else
					dat->rec.qc = PU_REC_QC_OK;
			}
		}
	}
*/	
	// add a new record
	rec = (pu_rec_t *)&dat->rec_pool[cnt];
	memcpy(rec, &dat->rec, sizeof(pu_rec_t));

	// increase : 1024 -> 2048 -> 3072...
	if ((dat->par->rec_cnt & 0x3FF) == 0)
	{
		if (dat->par->rec_cnt > 0)
		{
			// add 1024 empty record
			dat->hdr->rec_sz = (((dat->par->rec_cnt >> 10) & 0x3F) + 1) * 1024 * sizeof(pu_rec_t);
			dat->rec_pool = realloc(dat->rec_pool, dat->hdr->rec_sz);

			dat->hdr->fsz = dat->hdr->rec_off + dat->hdr->rec_sz;
		}
	}

	// rec : crc
	xraw = (uint8_t *)dat->rec_pool;
	dat->hdr->rec_crc = crc_crc32(xraw, dat->hdr->rec_sz);

	return PU_OK;
}

//===========================================================================
//  upd_par_intv
//===========================================================================
void upd_par_intv(void)
{
	pu_rec_t	*rec;
	time_t		tm0;
	int			i, j;
	int			intv_tab[10], tmp;
	uint8_t		swap = 0;
	uint32_t	sum;

	
	if (dat->par->rec_cnt != 10)
		return;

	rec = (pu_rec_t *)&dat->rec_pool[0];
	
	tm0 = dat->par->tm_zero;

	for (i=0; i<10; i++)
	{
		intv_tab[i] = (int)rec->tm_rtc - (int)tm0;

		tm0 = rec->tm_rtc;

		rec++;	// next record
	}

	// bubble sort
	for (i=0; i<(10-1); i++)
	{
		swap = 0;

		for (j=0; j<(10-i-1); j++)
		{
			if (intv_tab[j] > intv_tab[j+1])
			{
				// swap
				tmp = intv_tab[j];
				intv_tab[j] = intv_tab[j+1];
				intv_tab[j+1] = tmp;

				swap = 1;
			}
		}
		if (!swap)
			break;
	}

	// summarized 2nd~9th intv and 8 times average
	for (i=1, sum=0; i<9; i++)
	{
		sum += intv_tab[i]*10;
	}
	dat->par->intv = (uint16_t)((sum+40)/80);
	dat->par->intv += dat->par->tol;
}

//===========================================================================
//  upd_sta_info
//===========================================================================
uint8_t upd_sta_info(void)
{
	pu_rec_t	*rec;
	int			i;
	uint32_t	nx;

	if (dat->par->rec_cnt == 0)
		return PU_OK;

	rec = (pu_rec_t *)&dat->rec_pool[0];

	memset(&dat->sta, 0, sizeof(pu_sta_t));
	nx = 0;

	for (i=0; i<dat->par->rec_cnt; i++, rec++)
	{
		if (rec->intv == 0)
			dat->sta.na++;
		else if (rec->intv < 0)
			dat->sta.er++;
		else if (rec->intv > dat->par->intv)
			dat->sta.gg++;
		else
			dat->sta.ok++;
		
		//-------------------------------------------------------
		// check flag : xx
		//-------------------------------------------------------
		// AB : ABIOS : xx
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_AB) &&
			TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_ABIOS))
		{
			// model
			if ((rec->acl_flag & PU_REC_ACL_ABIOS_MODEL) == 0)
				dat->sta.nx++;
			// ver
			if ((rec->acl_flag & PU_REC_ACL_ABIOS_VER) == 0)
				dat->sta.nx++;
		}

		// AE : AEC : xxx
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_AEC) &&
			TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_AEC))
		{
			// chip
			if ((rec->acl_flag & PU_REC_ACL_AEC_CHIP) == 0)
				dat->sta.nx++;
			// board
			if ((rec->acl_flag & PU_REC_ACL_AEC_BOARD) == 0)
				dat->sta.nx++;
			// ver
			if ((rec->acl_flag & PU_REC_ACL_AEC_VER) == 0)
				dat->sta.nx++;
		}

		// CP : CPU : xx
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_CP))
		{
			// vendor
			if ((rec->sys_flag & PU_REC_SYS_CPU_VENDOR) == 0)
				dat->sta.nx++;
			// name
			if ((rec->sys_flag & PU_REC_SYS_CPU_NAME) == 0)
				dat->sta.nx++;
		}

		// E8 : E820 : x
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_E8))
		{
			if ((rec->sys_flag & PU_REC_SYS_E820) == 0)
				dat->sta.nx++;
		}

		// SM : SMBIOS : x
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_SM) &&
			TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SMBIOS))
		{
			if ((rec->sys_flag & PU_REC_SYS_SMBIOS) == 0)
				dat->sta.nx++;
		}

		// PC : PCI : x
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_PC))
		{
			if ((rec->sys_flag & PU_REC_SYS_PCI_SCAN) == 0)
				dat->sta.nx++;
		}

		// SB : SMBUS : x
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_SB) &&
			TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SMB))
		{
			if ((rec->sys_flag & PU_REC_SYS_SMB_SCAN) == 0)
				dat->sta.nx++;
		}

		// SP : SPD : x
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_SP) &&
			TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_SPD))
		{
			if ((rec->sys_flag & PU_REC_SYS_SMB_SPD) == 0)
				dat->sta.nx++;
		}

		// RTC : RTC : xxx
		if (TST_BIT_MASK(dat->par->cmp, PU_PAR_CMP_RTC))
		{
			if ((rec->sys_flag & PU_REC_SYS_RTC_0A) == 0)
				dat->sta.nx++;
			if ((rec->sys_flag & PU_REC_SYS_RTC_0B) == 0)
				dat->sta.nx++;
			if ((rec->sys_flag & PU_REC_SYS_RTC_0D) == 0)
				dat->sta.nx++;
		}
		
		if (nx != dat->sta.nx)
		{
			dat->sta.xx++;
			nx = dat->sta.nx;
		}
		else
		{
			dat->sta.oo++;
		}
	}
	dat->sta.to = dat->par->rec_cnt;

	return PU_OK;
}

//===========================================================================
//  upd_rst_tmr
//===========================================================================
void upd_rst_tmr(void)
{
	pu->tm.tmr = 0;

	// load reset time out
	pu->tm.rst_tmr = (uint8_t)dat->par->rst_tmo;
}

//===========================================================================
//  upd_alm_sw
//===========================================================================
void upd_alm_sw(void)
{
	if (TST_BIT_MASK(dat->par->feat, PU_PAR_FEAT_ALM_SW))
	{
		pu->alm.cmd = 0x80;
	}
}
