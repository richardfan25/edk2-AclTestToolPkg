#ifndef __INIT_SYSINT_H
#define __INIT_SYSINT_H

#include "typedef.h"
//=============================================================================
//  oem_pci_name_t
//=============================================================================
typedef struct _oem_pci_name_t
{
	uint16_t	vid;		// vendor id
	uint16_t	did;		// device id
	char		name[41];	// vendor + device name

} oem_pci_name_t;

//=============================================================================
//  functions
//=============================================================================
uint8_t	sys_init_timer_check(void);
uint8_t	sys_init_cpu_check(void);
uint8_t	sys_init_imc_spd_check(void);
uint8_t	sys_init_memory_spd_check(void);
uint8_t	sys_init_memory_map_check(void);
uint8_t	sys_init_acl_bios_check(void);
uint8_t	sys_init_acl_ec_check(void);
uint8_t	sys_init_smbios_check(void);
uint8_t	sys_init_pci_check(void);
uint8_t	sys_init_smbus_check(void);
uint8_t	sys_init_rtc_check(void);

uint8_t	sys_init(void);
void	sys_exit(void);

#endif
