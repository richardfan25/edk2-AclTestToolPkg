//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  PCINAME - PCI Name                                                       *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __X86_PCINAME_H
#define __X86_PCINAME_H

#include "typedef.h"
#include "pci.h"
//=============================================================================
//  pci_dev_name_t
//=============================================================================
typedef struct _pci_dev_name_t
{
	uint8_t		cla;
	uint8_t		subcla;
	uint8_t		intf;
	char		*name;

} pci_dev_name_t;

//=============================================================================
//  pci_ven_t
//=============================================================================
typedef struct _pci_ven_name_t
{
	uint16_t	id;
	char		*name;

} pci_ven_name_t;
	
//=============================================================================
//  extern
//=============================================================================
extern pci_dev_name_t pci_dev_name_table[141];
extern pci_ven_name_t pci_ven_name_table[129];

//=============================================================================
//  functions
//=============================================================================
char *pci_find_ven_name(pci_dev_t *pd);
char *pci_find_dev_name(pci_dev_t *pd);

#endif