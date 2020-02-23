//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  COMMON - CRC (Cyclic Redundancy Check)                                   *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __COMMON_CRC_H
#define __COMMON_CRC_H

#include "typedef.h"

//=============================================================================
//  functions
//=============================================================================
uint8_t  crc_crc8(uint8_t *data, int len);
uint8_t  crc_crc8_dallas(uint8_t *data, int len);
uint8_t  crc_crc8_test(uint8_t *data, int len);

uint16_t crc_crc16(uint8_t *data, int len);
uint16_t crc_crc16_xmodem(uint8_t *data, int len, uint16_t crc);
uint32_t crc_crc16_xmodem2(uint8_t *data, int len, uint32_t crc);
uint16_t crc_crc16_modbus(uint8_t *data, int len);
uint16_t crc_crc16_reverse(uint8_t *data, int len);
uint16_t crc_crc16_kermit(uint8_t *data, int len);
uint16_t crc_crc16_dnp(uint8_t *data, int len);

uint32_t crc_crc32(uint8_t *data, int len);
uint32_t crc_crc32_old(uint8_t *data, int len);
uint32_t crc_crc32_dos(uint8_t *data, uint32_t data_bsz);

#endif
