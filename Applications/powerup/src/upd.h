//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  UPD : Update                                                             *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __UPD_H
#define __UPD_H

#include "dat.h"

//=============================================================================
//  functions
//=============================================================================
pu_raw_t *raw_find_id(uint32_t id);

void upd_par_tm_zero(void);


void upd_rec_tm_rtc(void);
uint8_t upd_rec_cpu(void);

void upd_rec_acl_abios(void);
void upd_rec_acl_aec(void);

void upd_rec_sys_cpu(void);
void upd_rec_sys_e820(void);
void upd_rec_sys_smbios(void);
void upd_rec_sys_pci(void);
void upd_rec_sys_smb(void);
void upd_rec_sys_spd(void);
void upd_rec_sys_rtc(void);

uint8_t upd_rec_pool(void);
void upd_par_intv(void);
//uint8_t upd_inf_all(void);

uint8_t upd_sta_info(void);
void upd_rst_tmr(void);
void upd_alm_sw(void);

#endif
