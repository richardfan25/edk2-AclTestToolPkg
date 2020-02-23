#ifndef _RDCEC_H_
#define _RDCEC_H_

#include <stdint.h>


UINT8 rdc_ec_read_prot(UINT8 cmd, UINT8 ctl, UINT8 dev, UINT8 len, UINT8 *buf);
UINT8 rdc_ec_write_prot(UINT8 cmd, UINT8 ctl, UINT8 dev, UINT8 len, UINT8 *buf);
UINT8 rdc_ec_read_pmc_status(UINT8 cmd, UINT8 *buf);
UINT8 rdc_sw_ec_read_prot(UINT8 cmd, UINT8 index, UINT8 offset, UINT8 len, UINT8 *buf);
UINT8 rdc_sw_ec_write_prot(UINT8 cmd, UINT8 index, UINT8 offset, UINT8 len, UINT8 *buf);
UINT8 rdc_sw_ec_read_info_prot(UINT8 cmd, UINT8 index, UINT8 offset, UINT8 len, UINT8 *buf);

int ProtocolStatus(UINT8 cmd, UINT8 ctl, UINT8 dev, UINT8 len, UINT8 *dat);
int SWProtocolStatus(UINT8 cmd, UINT8 index, UINT8 offset, UINT8 len, UINT8 *buf);
UINT8 SWProtocolOnlyWritePMC(UINT8 cmd, UINT8 index, UINT8 offset, UINT8 len, UINT8 *buf);
int StorageSWProtocolSetLockUnlock(UINT8 cmd, UINT8 *index, UINT8 offset, UINT8 len, UINT8 *buf);
int SWRWProtocolStatusReplaceIndex(UINT8 cmd, UINT8 *index, UINT8 offset, UINT8 len, UINT8 *buf);


#endif
