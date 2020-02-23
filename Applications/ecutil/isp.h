#ifndef _ISP_H_
#define _ISP_H_

#include "ecutil.h"

/*==============================================================*/
// CONFIG
/*==============================================================*/
#define dROM_BLOCKSIZE		0x10000
#define dROM_PAGESIZE		0x100
#define dROM_PAGECOUNT		(dROM_BLOCKSIZE / dROM_PAGESIZE)
#define dISP_WRBUF_LEN		8
#define dDLY_Chk_ROMSTA		20000			// 20msec(dTCK_Chk_ROMSTA); uint16 delay time that check io port status(IBF/OBF)
#define dTCK_Chk_ROMSTA		1				// 1us(1us) / 1us; tick time that check io port status(IBF/OBF)
#define dFILE_BUFF_SIZE		0x8000
#define dROM_ID_SIZE		0x03			// MID, DID0, DID1
/*==============================================================*/
// DEFINE
/*==============================================================*/
#define dSST_ROM_MID		0xBF
/*==============================================================*/
// ISP command
// - Before use ISP commands, must set ISP mode via I/O command(0xdc).
/*==============================================================*/
//general command
#define dISP_FOLLOW_EN		0x11			// ITE enable follow mode
#define dISP_WR_CMD			0x12
#define dISP_WR_DATA		0x13
#define dISP_RD_DATA   		0x14
#define dISP_FOLLOW_DIS		0x15
#define dISP_SET_ADDR0     	0x16
#define dISP_SET_ADDR1	 	0x17
#define dISP_SET_ADDR2	 	0x18
#define dISP_SET_ADDR3	 	0x19
#define dISP_EXIT_FLASH	 	0xFC			// exit isp mode
#define dISP_GO_EC_MAIN 	0xFD			// exit isp mode and goto ec_main
#define dISP_WDT_RESET	 	0xFE			// enable wdt reset directly

//Pass Through (SST) ROM command via FSPI
#define dISP_ROM_WRSTA		0x01
#define dISP_ROM_PAGE_WR   	0x02
#define dISP_ROM_WR_DIS  	0x04
#define dISP_ROM_RD_STA    	0x05
#define dISP_ROM_WR_EN   	0x06
#define dISP_ROM_SST_ENSTA	0x50
#define dISP_ROM_GETID      0x9f
#define dISP_ROM_SST_AAI   	0xad
#define dISP_ROM_BLK_ERS   	0xd8
	
/*==============================================================*/
// STRUCTURE
/*==============================================================*/
typedef struct _HANDLE_BIN{
	char		*name;					// file name
	FILE		*ctrl;					// file control point
	INFO_PJT	pjtinfo;
    UINT16		blocknum;
    UINT16		over_num;
} HANDLE_BIN;

/*==============================================================*/
// Extern
/*==============================================================*/
extern int isp_wr_cmd(UINT8 cmd);
extern int isp_wr_data(UINT8 *data, UINT8 lens);
extern int isp_rd_data(UINT8 cmd, UINT8 *buf, UINT8 lens);
extern int isp_chk_write_enable(void);
extern int isp_write_enable(void);
extern int isp_write_disable(void);
extern int isp_read_rom_id(UINT8 *id, UINT8 lens);
extern int isp_chk_rom_busy(void);
extern int isp_rom_erase(UINT16 block);
extern int isp_rom_programming(UINT16 block);
extern int isp_rom_verify(UINT16 block);
extern void isp_wdt_reset(void);
extern int isp_flash_ec(HANDLE_BIN *bin);

#endif	// _ISP_H_