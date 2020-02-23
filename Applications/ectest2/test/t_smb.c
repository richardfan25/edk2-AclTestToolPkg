#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "mbxapp.h"
#include "util.h"
#include "lm95245.h"
#include "pca9555.h"

/*===========================================================
 * Name  : scan_smb_function
 * Pupose: Scan smbus address
 * Input : did: dynamic device id
 * 		 : addr_buf: buffer for store address
 * Output: 0: scan error, others: the count of responded address
 *===========================================================*/
UINT8 scan_smb_function(UINT8 did, UINT8 *addr_buf)
{
	UINT8		address;
	UINT8		wbuf[4];
	UINT8		rbuf[4];
	UINT16		rlen;
	int 		ret;
	UINT8		cnt = 0;
	
	address = 0x0E;
	while(1)
	{
		address += 2;
		// test read word command
		ret =	mApp.smb->request(
					MBXSMB_ReadByte,		// master cmd
					did,					// device ID
					address,				// device slave address
					0x00,					// device command
					wbuf,					// write buf
					rbuf,					// read buf
					0,						// size of write buf
					&rlen					// size of read buf
				);
		
		if((ret & 0x7F) != SMB_ST_ERR_ADDR_NAK)
		{
			addr_buf[cnt] = address;
			cnt++;
		}
		if(address == 0xEE)
			break;
	}

	return cnt;
}
/*===========================================================
 * Name  : find_smbdev_addr.type
 * Pupose: find smbus dev addr on device
 * Input : None
 * Output: ECI_ACCESS_T - Hardware Access Port
 *===========================================================*/
UINT8 find_smbdev_addr(UINT8 did, UINT8 buf_len, UINT8 *buf)
{
	int  ret;
	UINT8	wbuf[4];
	UINT8	rbuf[4];
	UINT16	rlen;
	UINT8	buf_idx = 0;
	UINT8	cnt = 0;
	
	for(buf_idx = 0; buf_idx < buf_len; buf_idx++)
	{
		ret =	mApp.smb->request(
				MBXSMB_ReadByte,		// master cmd
				did,					// device ID
				buf[buf_idx],			// device slave address
				0x00,					// device command
				wbuf,					// write buf
				rbuf,					// read buf
				0,						// size of write buf
				&rlen					// size of read buf
			);
		
		if((ret & 0x7F) != SMB_ST_ERR_ADDR_NAK)
		{	
			buf[cnt] = buf[buf_idx];
			cnt++;
		}
	}
	
	return cnt;
}
/*===========================================================
 * Name  : print_smbdev_addr
 * Pupose: print smb addr
 * Input : addr_cnt- count of addr
 *		 : addr_buf- addr buf
 * Output: None
 *===========================================================*/
void print_smbdev_addr(UINT8 pin_no, UINT8 did, UINT8 addr_cnt, UINT8 *addr_buf)
{
	UINT8	len;
	UINT8	*abuf = (UINT8 *)malloc( addr_cnt );
	
	memcpy(abuf, addr_buf, addr_cnt);
	len = find_smbdev_addr(did, addr_cnt, abuf);
	if(len != 0)
	{
		fprintf(stderr, "\nSMBus %d - ", pin_no);
		while(len > 0)
		{
			len--;
			fprintf(stderr, "0x%02X, ",abuf[len]);
		}
		fprintf(stderr, "\n");
	}

	free(abuf);
	fprintf(stderr, "\n");
}

/*===========================================================
 * Name  : find_smb_bus
 * Pupose: list all valid smbus in dynamic table 
 * Input : did: smbus device id
 * Output: none
 *===========================================================*/
void find_smb_bus(UINT8 *pin_no, UINT8 *id_buf)
{
	UINT8	idx;
	UINT8	DevNo = 0;
	
	for (idx = 0; idx < mApp.dyna->size; idx++)
	{
		if((mApp.dyna->dev[idx].did != DDevNo_PECI) && (mApp.dyna->getDevType(mApp.dyna->dev[idx].did) == DDevType_SMB))
		{
			DevNo = mApp.dyna->dev[idx].pin_no & 0x07;

			if((*pin_no & (0x1 << DevNo)) == 0)
			{
				*pin_no |= 0x1 << DevNo;
				id_buf[DevNo] = mApp.dyna->dev[idx].did;
			}
		}
	}
}

/*===========================================================
 * Name  : test_smb_list_address
 * Pupose: list all valid address of smbus device in dynamic table 
 * Input : tbl: dynamic device table
 * Output: channel of smbus
 *===========================================================*/
UINT32 test_smb_list_address(void)
{
	UINT8	idx;
	UINT8	flag = 0;
	UINT8	id_idx = 0;
	UINT8	id_buf[32];
	UINT8	addr_cnt = 0;
	UINT8	addr_buf[128];
	char 	DevStr[DYNC_TYPE_STR_MAX];
	UINT8	DevNo = 0;
	
	fprintf(stderr, "List all of dynamic devices that is SMBus type as below.\n\n");
	
	fprintf(stderr, "SMBus/I2C Type Device:\n");					// get all of dynamic device for smbus and show its smbus channel
	for (idx = 0; idx < mApp.dyna->size; idx++)
	{
		if((mApp.dyna->dev[idx].did != DDevNo_PECI) && (mApp.dyna->getDevType(mApp.dyna->dev[idx].did) == DDevType_SMB))
		{
			mApp.dyna->getDevStr(mApp.dyna->dev[idx].did, DevStr);
			DevNo = mApp.dyna->dev[idx].pin_no & 0x07;
			fprintf(stderr, "  SMBus/I2C %d: %s\n", DevNo, DevStr);

			if((flag & (0x1 << DevNo)) == 0)
			{
				flag |= 0x1 << DevNo;
				id_buf[id_idx++] = mApp.dyna->dev[idx].did;
			}
		}
	}
	
	fprintf(stderr, "\nAble To Access Address:\n");					// list all of valid smbus channel and show all of valid address on channel
	if(flag == 0)
	{
		fprintf(stderr, "none\n");
		return 0;
	}
	for(id_idx = 0; id_idx < 8; id_idx++)
	{
		if((flag & (0x01 << id_idx)) != 0)
		{
			fprintf(stderr, "  SMBus/I2C %d: %x\n  ", id_idx,id_buf[id_idx]);
			fprintf(stderr, "...\r  ");
			addr_cnt = scan_smb_function(id_buf[id_idx], addr_buf);
			
			if(addr_cnt == 0)
			{
				fprintf(stderr, "none");
			}
			else{
				for(idx = 0; idx < addr_cnt; idx++)
				{
					fprintf(stderr, "0x%02X, ", addr_buf[idx]);
					if((idx & 0x7) == 0x7)
					{
						fprintf(stderr, "\n  ");
					}
				}
			}

			fprintf(stderr, "\n\n");
		}
	}
	
	return flag;
}

/*===========================================================
 * Name  : test_smb_write_eeprom
 * Pupose: use input data to fill eeprom
 * Input : cdev_id: device id 
 *		   addr: eeprom address
 *		   data: input data
 *		   option: 0 - fixed data
 *				   others - auto increase data
 * Output: none
 *===========================================================*/
void test_smb_write_eeprom(eDynclDevNo cdev_id, UINT8 addr, UINT8 data, UINT8 option)
{
	UINT8		offset = 0;
	UINT8		wbuf[4];
	UINT8		rbuf[4];
	UINT16		rlen;
	UINT8		testResult = 0;
	int 		ret;
	
	while(1)
	{
		wbuf[0] = data;

		ret =	mApp.smb->request(
					MBXSMB_WriteByte,		// master cmd
					cdev_id,				// device ID
					addr,					// device slave address
					offset,					// device command
					wbuf,					// write buf
					rbuf,					// read buf
					0,						// size of write buf
					&rlen					// size of read buf
				);					
	
		if((ret & 0x7F) != 0)
		{
			fprintf(stderr, "\n");
			mApp.smb->printErrMsg(ret);
			return;
		}

		// test read word command
		ret =	mApp.smb->request(
					MBXSMB_ReadByte,		// master cmd
					cdev_id,				// device ID
					addr,					// device slave address
					offset,					// device command
					wbuf,					// write buf
					rbuf,					// read buf
					0,						// size of write buf
					&rlen					// size of read buf
				);
		
		if((ret & 0x7F) != 0)
		{
			fprintf(stderr, "\n");
			mApp.smb->printErrMsg(ret);
			return;
		}
		
		if((offset & 0xF) == 0)
		{
			fprintf(stderr, "(%02x)",offset);
		}
		fprintf(stderr, "[%02X]", rbuf[0]);
		if((offset & 0xF) == 0xF)
		{
			fprintf(stderr, "\n");
		}

		if(rbuf[0] != data)
		{
			testResult = 0;
			break;
		}
		else
			testResult = 1;
		if(option != 0)
			data = data++;
		
		if(offset == 0xFF)
			break;
		else
			offset++;
	}
		
	if(testResult == 1)
	{
		console_color(YELLOW, RED);
		fprintf(stderr, "\nPASS");
	}
	else
	{
		fprintf(stderr, "\nSMBus status: 0x%0X", ret);
		console_color(GREEN, RED);
		fprintf(stderr, "\nFAIL");
	}
	console_color(LIGHTGRAY, BLACK);
	fprintf(stderr, "\n\n");
}
/*===========================================================
 * Name  : test_smb_read_eeprom
 * Pupose: read all data in eeprom
 * Input : cdev_id: device id 
 *		   addr: eeprom address
 * Output: none
 *===========================================================*/
void test_smb_read_eeprom(eDynclDevNo cdev_id, UINT8 addr)
{
	UINT8		offset = 0;
	UINT8		wbuf[4];
	UINT8		rbuf[4];
	UINT16		rlen;
	int 		ret;

	while(1)
	{
		// test read word command
		ret =	mApp.smb->request(
					MBXSMB_ReadByte,		// master cmd
					cdev_id,				// device ID
					addr,					// device slave address
					offset,					// device command
					wbuf,					// write buf
					rbuf,					// read buf
					0,						// size of write buf
					&rlen					// size of read buf
				);
		
		if((ret & 0x7F) != 0)
		{
			fprintf(stderr, "\n");
			mApp.smb->printErrMsg(ret);
			return;
		}
		if((offset & 0xF) == 0)
		{
			fprintf(stderr, "(%02x)",offset);
		}
		fprintf(stderr, "[%02X]", rbuf[0]);
		if((offset & 0xF) == 0xF)
		{
			fprintf(stderr, "\n");
		}
		if(offset == 0xFF)
			break;
		else
			offset++;
	}
}
/*===========================================================
 * Name  : test_smb_main
 * Pupose: Test SMBus Main
 * Input : d_table: dynamic device table
 * Output: 
 *===========================================================*/
int test_smb_main(void)
{
	int 	key;
	int		rtn;
	CHAR8 	keyChar;
	UINT16	u16temp;
	UINT8	u8temp=0;
	UINT8	smb_addr;
	UINT8	smb_data;
	UINT8	smb_did = 0;
	UINT8	dyna_did[8] = {0};
	UINT8	dyna_pin = 0;
	UINT8	i;
	
	while(1)
	{
		clrscr();
		kdebug(0, 0);		
		console_color(LIGHTGRAY, BLACK);
		fprintf(stderr, "[a]  List SMBus Address                .    \n");
		fprintf(stderr, "[b]  Test SMBus - Read EERPOM          .    \n");
		fprintf(stderr, "[c]  Test SMBus - Write EERPOM         .    \n");
		fprintf(stderr, "[d]  Test SMBus - Auto Write EEPROM    .    \n");
		fprintf(stderr, "[e]  Test SMBus - Read Thermal IC      .    \n");
		fprintf(stderr, "[f]  Test SMBus - Write Thermal IC     .    \n");
		fprintf(stderr, "[g]  Test SMBus - Write DIO Board      .    \n");

		gotoxy(0, ConsoleHeight - 1);
		fprintf(stderr, "Press Alt+X to return main menu.");
		//input key
		key = bioskey(0);
		keyChar = (CHAR8) (key & 0xFF);
		clrscr();
		kdebug(0, 0);
		//================================================================================================
		
		if (key == ((EFI_SCANEX_ALT << 8) | 'X') || key == ((EFI_SCANEX_ALT << 8) | 'x'))    //alt+X
		{
			break;
		}
		else if(keyChar=='A' ||  keyChar=='a' )
		{
			test_smb_list_address();
		}
		else if(keyChar=='B' ||  keyChar=='b' )
		{
			fprintf(stderr, "[SMBus TEST] Read EEPROM\n\n");

			find_smb_bus(&dyna_pin, dyna_did);
			
			fprintf(stderr, "Enter SMBus Device ID: ");
			for(i=0; i<8; i++)
			{
				if(dyna_pin & (1 << i))
				{
					fprintf(stderr, "%d-SMB%d ,", i, i);
				}
			}
			fprintf(stderr, "\n");
			
			smb_did = (UINT8) get_number_stdin("", 0, 1);
			smb_addr = (UINT8) get_number_stdin("Enter EEPROM Address(0~0xFF):", 1, 2);

			test_smb_read_eeprom(dyna_did[smb_did], smb_addr);
		}
		else if(keyChar=='C' ||  keyChar=='c' )
		{
			fprintf(stderr, "[SMBus TEST] Write EEPROM\n\n");
			
			find_smb_bus(&dyna_pin, dyna_did);
			
			fprintf(stderr, "Enter SMBus Device ID: ");
			for(i=0; i<8; i++)
			{
				if(dyna_pin & (1 << i))
				{
					fprintf(stderr, "%d-SMB%d ,", i, i);
				}
			}
			fprintf(stderr, "\n");
			
			smb_did = (UINT8) get_number_stdin("", 0, 1);
			smb_addr = (UINT8) get_number_stdin("Enter EEPROM Address(0~0xFF):", 1, 2);

			smb_data = (UINT8) get_number_stdin("Enter EEPROM value:", 1, 2);
							
			test_smb_write_eeprom(dyna_did[smb_did], smb_addr, smb_data, 0);
		}
		else if(keyChar=='D' ||  keyChar=='d' )
		{
			fprintf(stderr, "[SMBus TEST] Write EEPROM With Auto Increase Data\n\n");
			
			find_smb_bus(&dyna_pin, dyna_did);
			
			fprintf(stderr, "Enter SMBus Device ID: ");
			for(i=0; i<8; i++)
			{
				if(dyna_pin & (1 << i))
				{
					fprintf(stderr, "%d-SMB%d ,", i, i);
				}
			}
			fprintf(stderr, "\n");
			
			smb_did = (UINT8) get_number_stdin("", 0, 1);
			smb_addr = (UINT8) get_number_stdin("Enter EEPROM Address(0~0xFF):", 1, 2);

			smb_data = (UINT8) get_number_stdin("Enter EEPROM value:", 1, 2);
									
			test_smb_write_eeprom(dyna_did[smb_did], smb_addr, smb_data, 1);
		}
		else if(keyChar=='E' ||  keyChar=='e' )
		{
			fprintf(stderr, "[SMBus TEST] Read Thermal IC\n\n");
			
			find_smb_bus(&dyna_pin, dyna_did);
			
			fprintf(stderr, "Enter SMBus Device ID: ");
			for(i=0; i<8; i++)
			{
				if(dyna_pin & (1 << i))
				{
					fprintf(stderr, "%d-SMB%d ,", i, i);
				}
			}
			fprintf(stderr, "\n");
			
			smb_did = (UINT8) get_number_stdin("", 0, 1);
			
			fprintf(stderr, "Search for LM95245 please wait...\r");
			print_smbdev_addr(smb_did, dyna_did[smb_did], sizeof(lm95245_addr), lm95245_addr);
			
			smb_addr = (UINT8) get_number_stdin("Enter Thermal IC Address(0~0xFF):", 1, 2);
			
			tmic_list_reg_data(dyna_did[smb_did], smb_addr);
		}
		else if(keyChar=='F' ||  keyChar=='f' )
		{
			fprintf(stderr, "[SMBus TEST] Write Thermal IC\n\n");
			
			find_smb_bus(&dyna_pin, dyna_did);
			
			fprintf(stderr, "Enter SMBus Device ID: ");
			for(i=0; i<8; i++)
			{
				if(dyna_pin & (1 << i))
				{
					fprintf(stderr, "%d-SMB%d ,", i, i);
				}
			}
			fprintf(stderr, "\n");
			
			smb_did = (UINT8) get_number_stdin("", 0, 1);
			
			fprintf(stderr, "Search for LM95245 please wait...\r");
			print_smbdev_addr(smb_did, dyna_did[smb_did], sizeof(lm95245_addr), lm95245_addr);

			smb_addr = (UINT8) get_number_stdin("Enter Thermal IC Address(0~0xFF):", 1, 2);
			rtn = tmic_get_chip_id(dyna_did[smb_did], smb_addr, &u16temp);
			if(rtn == _MBEr_Success)
			{
				fprintf(stderr, "  Thermal IC ID: 0x%04X\n\n", u16temp);

				do{
					smb_data = (UINT8) get_number_stdin("Enter New OTP Threshold(1~127):", 0, 3);
					if(smb_data == 0xFF)
						fprintf(stderr, "Get invalid number.\n");
					else if(smb_data > 127 || smb_data == 0)
						fprintf(stderr, "The number out of range.\n");
					else
						break;
					continue;
				} while(1);

				fprintf(stderr, "Write %d to OTP Threshold ... ", smb_data);
				rtn = tmic_write_reg(dyna_did[smb_did], smb_addr, LM_REG_T_CRIT_LMT, smb_data);
				rtn = tmic_read_reg(dyna_did[smb_did], smb_addr, LM_REG_T_CRIT_LMT, &u8temp);
				if(rtn == _MBEr_Success)
				{
					if(smb_data == u8temp)
					{
						console_color(YELLOW, RED);
						fprintf(stderr, "  PASS\n");
					}
					else
					{
						console_color(GREEN, RED);
						fprintf(stderr, "  FAIL\n");
					}
				}
			}
		}
		else if(keyChar=='G' ||  keyChar=='g' )
		{
			fprintf(stderr, "[SMBus TEST] Write DIO Board\n\n");
			
			find_smb_bus(&dyna_pin, dyna_did);
			
			fprintf(stderr, "Enter SMBus Device ID: ");
			for(i=0; i<8; i++)
			{
				if(dyna_pin & (1 << i))
				{
					fprintf(stderr, "%d-SMB%d ,", i, i);
				}
			}
			fprintf(stderr, "\n");
			
			smb_did = (UINT8) get_number_stdin("", 0, 1);
			
			fprintf(stderr, "Search for PCA5555 please wait...\r");
			print_smbdev_addr(smb_did, dyna_did[smb_did], sizeof(pca9555_addr), pca9555_addr);
			
			smb_addr = (uint8_t) get_number_stdin("Enter DIO Address(0~0xFF):", 1, 2);
			
			smb_data = (uint8_t) get_number_stdin("Enter DIO New Level(0~0xFF):", 1, 2);
			
			// set all pins of port0 as output
			rtn = dio_write_reg(dyna_did[smb_did], smb_addr, DIO_REG_CONFIG0, 0);
			if(rtn == _MBEr_Success)
			{
				// set new port0 level
				rtn = dio_write_reg(dyna_did[smb_did], smb_addr, DIO_REG_OUT_PORT0, smb_data);
				if(rtn == _MBEr_Success)
				{
					// read port0 level
					rtn = dio_read_reg(dyna_did[smb_did], smb_addr, DIO_REG_IN_PORT0, &u8temp);
					if(rtn == _MBEr_Success)
					{
						if(smb_data != u8temp)
							rtn = _MBEr_Fail;
					}
				}
			}
			if(rtn != _MBEr_Success)
			{
				console_color(GREEN, RED);
				fprintf(stderr, "  FAIL\n");
			}
			else
			{
				console_color(YELLOW, RED);
				fprintf(stderr, "  PASS\n");
			}
		}
		else
			continue;

		//====
		gotoxy(0, ConsoleHeight - 1);
		console_color(LIGHTGRAY, BLACK);
		fprintf(stderr, "Press any key continue");
		bioskey(0);
	}
	menuSelect = _mid_main;
	return _MBEr_Success;
}
