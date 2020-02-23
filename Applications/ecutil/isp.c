#include <stdio.h>

#include "env.h"
#include "ecutil.h"
#include "ec_access.h"
#include "isp.h"
#include "util.h"

#define dROMSTA_WREN_BIT	0x02				// bit1: write enable bit
#define dROMSTA_BUSY_BIT	0x01				// bit0: rom busy bit

#define MSG_ISP_INFO		1					// 0: info only for user
												// 1: isp operate step info
												// 2:
												// 3:


UINT8 wbuf[dISP_WRBUF_LEN];
UINT8 rom_type = 0x00;
UINT8 file_buf1[dFILE_BUFF_SIZE];
UINT8 file_buf2[dFILE_BUFF_SIZE];
/*==============================================================*/
// isp_wr_cmd
// Input:	none
// Output:	none
/*==============================================================*/
int isp_wr_cmd(UINT8 cmd)
{
	if(ec_wr_cmd(dISP_WR_CMD) != 0)				// send cmd
		return -1;
	if(ec_wr_cmd(cmd) != 0)
		return -1;
	
	return 0;
}

/*==============================================================*/
// isp_wr_data
// Input:	none
// Output:	none
/*==============================================================*/
int isp_wr_data(UINT8 *data, UINT8 lens)
{
	UINT8 idx;
	
	if(data == NULL){								// check data length
		printf("ERROR: Input data is NULL\n");
		return -1;
	}
	if(lens > dISP_WRBUF_LEN){
		printf("WARNING: Write data is too many on isp.\n");
		lens = dISP_WRBUF_LEN;
	}

	for(idx = 0; idx < lens; idx++){				// send data
		if(ec_wr_cmd(dISP_WR_DATA) != 0)
			return -1;
		if(ec_wr_cmd(*(data + idx)) != 0)
			return -1;	
	}
	
	return 0;
}

/*==============================================================*/
// isp_rd_data
// Input:	none
// Output:	none
/*==============================================================*/
int isp_rd_data(UINT8 cmd, UINT8 *buf, UINT8 lens)
{
	UINT8 idx;
	
	if(buf == NULL){
		printf("ERROR: CMD(0x%x) input buf is NULL\n",cmd);
		return -1;
	}
/* 	if(lens > dISP_RDBUF_LEN){
		printf("WARNING: CMD(0x%x) write data is too many on isp comm.\n",cmd)
		lens = dISP_RDBUF_LEN;
	} */
	if(isp_wr_cmd(cmd) != 0)						// send cmd
		return -1;
		
	
	for(idx = 0; idx < lens; idx++){
		if(ec_wr_cmd(dISP_RD_DATA) != 0)
			return -1;
		if(ec_rd_data(buf + idx) != 0)
			return -1;
	}
	return 0;
}

/*==============================================================*/
// isp_chk_write_enable
// Input:	none
// Output:	none
/*==============================================================*/
int isp_chk_write_enable(void)
{
	UINT16 	retry = 0;
	UINT8		data = 0;
	//printf("check write enable...\n");
	while(1){
		if(isp_rd_data(dISP_ROM_RD_STA, &data, 1) != 0){		// read rom status
			printf("Access ROM STA fail\n");
			return -1;
		}
		if((data & dROMSTA_WREN_BIT) != 0){					// check wren bit
			return 0;
		}
		if(++retry > dDLY_Chk_ROMSTA){						// check timeout
			printf("ERROR: ISP check channel timeout.\n");	
			return -1;
		}
		usleep(dTCK_Chk_ROMSTA);							// 10ms
	}
}
/*==============================================================*/
// isp_write_enable
// Input:	none
// Output:	none
/*==============================================================*/
int isp_write_enable(void)
{
	//printf("Write enable.(type 0x%02x)\n",rom_type);
	if(ec_wr_cmd(dISP_FOLLOW_EN) != 0){					// enable ITE SPI follow mode
		printf("ERROR: Enable follow mode fail\n");
		return -1;
	}
	if(rom_type == dSST_ROM_MID){
		if(isp_wr_cmd(dISP_ROM_SST_ENSTA) != 0){
			printf("ERROR: Enable SST status fail\n");
			return -1;
		}
	}
	
	if(isp_wr_cmd(dISP_ROM_WRSTA) != 0){					// set idx to WRSTA
		printf("Access to WRSTA fail\n");
		return -1;
	}
	
	wbuf[0] = 0x00;										// clear protect: write 0x00 to wrsta
	if(isp_wr_data(wbuf, 1) != 0){
		printf("Write data to WRSTA fali\n");
		return -1;
	}
	if(isp_wr_cmd(dISP_ROM_WR_EN) != 0){					// enable write to rom
		printf("Enable write to ROM fail\n");
		return -1;
	}
	
	return 0;
}
/*==============================================================*/
// isp_write_disable
// Input:	none
// Output:	none
/*==============================================================*/
int isp_write_disable(void)
{
	//printf("disable write...\n");
	if(isp_wr_cmd(dISP_ROM_WR_DIS) != 0)
		return -1;
	if(ec_wr_cmd(dISP_FOLLOW_DIS) != 0)
		return -1;
	
	return 0;
}

/*==============================================================*/
// isp_read_rom_id
// Input:	none
// Output:	none
/*==============================================================*/
int isp_read_rom_id(UINT8 *id, UINT8 lens)
{
    if(isp_write_enable() != 0){					// enable write to rom
		printf("enable write fail\n");
		return -1;
	}

	if(isp_rd_data(dISP_ROM_GETID, id, lens) != 0){			// Get ROM ID
		printf("get id fail\n");
		return -1;
	}
		
    if(isp_write_disable() != 0){							// disable write to rom
		printf("disable write fail\n");
		return -1;
	}
	rom_type = id[0];
	return 0;
}

/*==============================================================*/
// isp_chk_rom_busy
// Input:	none
// Output:	none
/*==============================================================*/
int isp_chk_rom_busy(void)
{
	UINT16 	retry = 0;
	UINT8		data = 0;
	//printf("check rom busy...\n");
	while(1){
		if(isp_rd_data(dISP_ROM_RD_STA, &data, 1) != 0)		// read rom status
			return -1;
			
		if((data & dROMSTA_BUSY_BIT) == 0){					// check wren bit
			return 0;
		}
		if(++retry > dDLY_Chk_ROMSTA){						// check timeout
			printf("ERROR: ISP check busy timeout.\n");	
			return -1;
		}
		usleep(dTCK_Chk_ROMSTA);							// 10ms
	}
}

/*==============================================================*/
// isp_rom_erase
// Input:	none
// Output:	none
/*==============================================================*/
int isp_rom_erase(UINT16 block)
{
	if(isp_write_enable() != 0){		// enable write to rom
		return -1;
	}
	if(isp_chk_write_enable() != 0){
		printf("Check write enable fail.\n");
		return -1;
	}
	
	if(isp_wr_cmd(dISP_ROM_BLK_ERS) != 0)
		return -1;
	
	wbuf[0] = (UINT8)block;
	wbuf[1] = 0x00;
	wbuf[2] = 0x00;
	if(isp_wr_data(wbuf, 3) != 0)
		return -1;
	
	if(isp_chk_rom_busy() != 0)
		return -1;											// busy timeout
	
	if(isp_write_disable() != 0)							// disable write to rom
		return -1;
		
	return 0;
}

/*==============================================================*/
// is_rom_program
// Input:	none
// Output:	none
/*==============================================================*/
int normal_rom_program(UINT16 block)
{
	UINT16 pagecnt;
	UINT16 idx;
	//printf("program rom block(%d)\n",block);
	
	for(pagecnt = 0; pagecnt < dROM_PAGECOUNT; pagecnt++){
		//printf("Page%d...\r",pagecnt);
		if(isp_write_enable() != 0)		// enable write to rom
			return -1;
		if(isp_chk_write_enable() != 0){
			printf("Check write enable fail.\n");
			return -1;
		}

			
		if(isp_wr_cmd(dISP_ROM_PAGE_WR) != 0)
			return -1;
		wbuf[0] = (UINT8)block;
		wbuf[1] = (UINT8)pagecnt;
		wbuf[2] = 0x00;
		if(isp_wr_data(wbuf, 3) != 0)
			return -1;
		
		if((pagecnt * dROM_PAGESIZE) < dFILE_BUFF_SIZE){
			for(idx = 0; idx < dROM_PAGESIZE; idx++){
				if(isp_wr_data(&file_buf1[pagecnt * dROM_PAGESIZE + idx], 1) != 0)
					return -1;
			}
		}
		else {
			for(idx = 0; idx < dROM_PAGESIZE; idx++){
				if(isp_wr_data(&file_buf2[pagecnt * dROM_PAGESIZE + idx - dFILE_BUFF_SIZE], 1) != 0)
					return -1;
			}
		}
		
		if(isp_chk_rom_busy() != 0)
			return -1;											// busy timeout
		if(isp_write_disable() != 0)							// disable write to rom
			return -1;
	}
	return 0;
}

/*==============================================================*/
// sst_rom_program
// Input:	none
// Output:	none
/*==============================================================*/
int sst_rom_program(UINT16 block)
{
	UINT32 idx;
	//printf("program sst rom block(%d)\n",block);
    if(isp_write_enable() != 0)		// enable write to rom
		return -1;
	if(isp_chk_write_enable() != 0){
		printf("Check write enable fail.\n");
		return -1;
	}
	
	for(idx = 0; idx < dROM_BLOCKSIZE; idx+=2){
		//printf("block idx%d...\n",idx);
		isp_wr_cmd(dISP_ROM_SST_AAI);
		if(idx == 0){										// send block address
			wbuf[0] = (UINT8)block;									// adr2
			wbuf[1] = 0x00;									// adr1
			wbuf[2] = 0x00;									// adr0
			if(isp_wr_data(wbuf, 3) != 0)
				return -1;
		}
		if(idx < dFILE_BUFF_SIZE){
			if(isp_wr_data(&file_buf1[idx], 2) != 0)
				return -1;
		}
		else{
			if(isp_wr_data(&file_buf2[idx - dFILE_BUFF_SIZE], 2) != 0)
				return -1;
		}
		if(isp_chk_rom_busy() != 0)
			return -1;										// busy timeout
	}
	
	if(isp_write_disable() != 0)							// disable write to rom
		return -1;
	
	return 0;
}

/*==============================================================*/
// is_rom_program
// Input:	none
// Output:	none
/*==============================================================*/
int isp_rom_programming(UINT16 block)
{
	if(rom_type == dSST_ROM_MID)
		return sst_rom_program(block);
	else
		return normal_rom_program(block);
}

/*==============================================================*/
// isp_rom_verify
// Input:	none
// Output:	none
/*==============================================================*/
int isp_rom_verify(UINT16 block)
{
	UINT32	addr0;
	UINT32	addr1;
	UINT8		chk_buf1[dFILE_BUFF_SIZE];
	UINT8		chk_buf2[dFILE_BUFF_SIZE];
	
	for(addr0 = 0; addr0 < dFILE_BUFF_SIZE; addr0++){				// init check buf
		chk_buf1[addr0] = 0xff;
		chk_buf2[addr0] = 0xff;
	}
	
	if(ec_wr_cmd(dISP_SET_ADDR3) != 0)
		return -1;
	if(ec_wr_cmd(0x00) != 0)
		return -1;
	if(ec_wr_cmd(dISP_SET_ADDR2) != 0)
		return -1;
	if(ec_wr_cmd((UINT8)block) != 0)
		return -1;

	for(addr1 = 0; addr1 < dROM_PAGESIZE; addr1++){					// read
		if(ec_wr_cmd(dISP_SET_ADDR1) != 0)
			return -1;
		if(ec_wr_cmd((UINT8)addr1) != 0)
			return -1;

		if(addr1 * dROM_PAGESIZE < dFILE_BUFF_SIZE){
			for(addr0 = 0; addr0 < dROM_PAGESIZE; addr0++){
				if(ec_wr_cmd(dISP_SET_ADDR0) != 0)
					goto eReadECRom;
				if(ec_wr_cmd((UINT8)addr0) != 0)
					goto eReadECRom;
				if(ec_wr_cmd(dISP_RD_DATA) != 0)
					goto eReadECRom;
				if(ec_rd_data(&chk_buf1[addr1 * dROM_PAGESIZE + addr0]) != 0)
					goto eReadECRom;
			}
		}
		else{
			for(addr0 = 0; addr0 < dROM_PAGESIZE; addr0++){
				if(ec_wr_cmd(dISP_SET_ADDR0) != 0)
					goto eReadECRom;
				if(ec_wr_cmd((UINT8)addr0) != 0)
					goto eReadECRom;
				if(ec_wr_cmd(dISP_RD_DATA) != 0)
					goto eReadECRom;
				if(ec_rd_data(&chk_buf2[addr1 * dROM_PAGESIZE + addr0 - dFILE_BUFF_SIZE]) != 0)
					goto eReadECRom;
			}
		}
	}
	
	for(addr0 = 0; addr0 < dROM_BLOCKSIZE; addr0++){
		if(addr0 < dFILE_BUFF_SIZE){
			if(file_buf1[addr0] != chk_buf1[addr0]){
				//printf("ERROR: Verify fail.(%x-%x)\n",block ,addr0);
				goto eVerifyRom;
			}
		}
		else{
			if(file_buf2[addr0 - dFILE_BUFF_SIZE] != chk_buf2[addr0 - dFILE_BUFF_SIZE]){
				//printf("ERROR: Verify fail.(%x-%x)\n",block ,addr0);
				goto eVerifyRom;
			}
		}
	}
	return 0;
eVerifyRom:
	if(addr0 < dFILE_BUFF_SIZE)
		printf("checkbuf=0x%02x\tfilebuf=0x%02x\n",chk_buf1[addr0],file_buf1[addr0]);
	else
		printf("checkbuf=0x%02x\tfilebuf=0x%02x\n",chk_buf2[addr0 - dFILE_BUFF_SIZE],file_buf2[addr0 - dFILE_BUFF_SIZE]);
eReadECRom:
	printf("\nVerify Fail\n");
	printf("addr1: 0x%02x,\taddr0: 0x%02x\n",addr1 ,addr0);
	return -1;
}

/*==============================================================*/
// isp_wdt_reset
// Input:	none
// Output:	none
/*==============================================================*/
void isp_wdt_reset(void)
{
	ec_wr_cmd(dISP_WDT_RESET);
}

/*==============================================================*/
// isp_flash_ec
// Input:	none
// Output:	none
/*==============================================================*/
int isp_flash_ec(HANDLE_BIN *bin)
{
	UINT16	blk;
	UINT32	cnt;
	UINT8		rom_id[dROM_ID_SIZE];
	progress_t	bar;
	int	progress_max = bin->blocknum * 2;
	UINT16	progress_cnt = 0;
	
    progress_init(&bar, "", progress_max);
	
	if(ec_wr_cmd(dIO_ISP_START) != 0){							// enter ISP mode
		printf("ERROR: Enter ISP mode fail.\n");
		goto eFlashErr;
	}
	
	if(isp_read_rom_id(rom_id, dROM_ID_SIZE) != 0){				// get new rom_id
		printf("ERROR: ISP get ROM ID fail\n");
		goto eFlashErr;
	}
	
/* 	printf("ROM ID: ");
	for(idx = 0; idx < lens; idx++)
		printf("%03x ",id[idx]);
	printf("\n"); */

	progress_show(&bar, (float)progress_cnt/progress_max);
	for(blk = 0; blk < bin->blocknum; blk++){
		
		for(cnt = 0; cnt < dFILE_BUFF_SIZE; cnt++){				// init file buf
			file_buf1[cnt] = 0xff;
			file_buf2[cnt] = 0xff;
		}
		
		if(fread(file_buf1,1,dFILE_BUFF_SIZE,bin->ctrl) == 0){	// read file
			printf("ERROR: Read bin file fail.\n(1-%d)", blk);
			goto eFlashErr;
		}
		if(fread(file_buf2,1,dFILE_BUFF_SIZE,bin->ctrl) == 0){
			printf("ERROR: Read bin file fail.\n(2-%d)", blk);
			goto eFlashErr;
		}
		
		//printf("\nErase rom block%d...",blk);
		if(isp_rom_erase(blk) != 0)								// erase rom
			goto eFlashErr;
		progress_cnt++;
		progress_show(&bar, (float)progress_cnt/progress_max);

		for(cnt = 0; cnt < dFILE_BUFF_SIZE; cnt++){				// check file data 
			if((file_buf1[cnt] != 0xff) || (file_buf2[cnt] != 0xff))
				break;
		}
		if(cnt != dFILE_BUFF_SIZE){								// skip if all byte is 0xff in block
			//printf("\nProgramming...");
			if(isp_rom_programming(blk) != 0)					// programming
				goto eFlashErr;
					
			//printf("\nVerify...");
			if(isp_rom_verify(blk) != 0)						// verify
				goto eFlashErr;
		}
		progress_cnt++;
		progress_show(&bar, (float)progress_cnt/progress_max);
	}
	progress_show(&bar, (float)progress_max/progress_max);
	
	if(ec_wr_cmd(dISP_GO_EC_MAIN) != 0){						// exit ISP mode and goto ec_main
		printf("ERROR: Exit ISP mode fail.\n");
		goto eFlashErr;
	}
	
	return 0;
	
eFlashErr:														// flash error
	progress_destroy(&bar);
	return -1;
}
