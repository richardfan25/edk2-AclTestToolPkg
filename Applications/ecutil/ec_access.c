#include <stdio.h>
#include <stdint.h>

#include "env.h"
#include "ec_access.h"

#define MSG_IO_INFO			0

#if (MSG_IO_INFO > 0)
#define mMSG_IO_GET_DATA()	printf("I/O: get data 0x%02x from 0x%04x\n",*val,dataport)
#else
#define mMSG_IO_GET_DATA()
#endif

#if (MSG_IO_INFO > 0)
#define mMSG_IO_SET_DATA()	printf("I/O: send 0x%02x data to 0x%04x\n",val,dataport)
#else
#define mMSG_IO_SET_DATA()
#endif

#if (MSG_IO_INFO > 0)
#define mMSG_IO_GET_CMD()	printf("I/O: get cmd 0x%02x from 0x%04x\n",*val,cmdport)
#else
#define mMSG_IO_GET_CMD()
#endif

#if (MSG_IO_INFO > 0)
#define mMSG_IO_SET_CMD()	printf("I/O: send 0x%02x cmd to 0x%04x\n",cmd,cmdport)
#else
#define mMSG_IO_SET_CMD()
#endif

#if (MSG_IO_INFO > 0)
#define mMSG_IO_OPEN_FAIL()	printf("I/O port(0x%x,0x%x) open fail.\n", cmd,data);
#else
#define mMSG_IO_OPEN_FAIL()	printf("ERROR: Access EC Fail.\n");
#endif

UINT16 cmdport = 0x00;
UINT16 dataport = 0x00;

/*==============================================================*/
// Get access EC port permission
// Input:	cmd	 - ioport num that is a UINT16 data.
//			data - ioport num that is a UINT16 data.
//
// Output:	-1	 - fail
//			 0	 - success
/*==============================================================*/
int ec_set_port(UINT16 cmd,UINT16 data)
{
#ifdef _LINUX_
	if(getuid() != 0){
		printf("ERROR: root permission needed.\n\n");
		return -1;
	};
	if(ioperm(cmd, 1, 1)){
		perror("ioperm");
		return -1;
	}
	if(ioperm(data, 1, 1)){
		perror("ioperm");
		return -1;
	}
#endif
	cmdport = cmd;
	dataport = data;

	if(mINB(cmd) == 0xff){
		mMSG_IO_OPEN_FAIL();
		return -1;
	}
	
	mINB(data);				// dummy read to clear OBF
	return 0;
}
/*==============================================================*/
// Close access EC port permission
// Input:	cmd	 - ioport num that is a uint16_t data.
//			data - ioport num that is a uint16_t data.
//
// Output:	-1	 - fail
//			 0	 - success
/*==============================================================*/
void ec_close_port(void)
{
#ifdef _LINUX_
	if(cmdport != 0){
		if (ioperm(cmdport, 1, 0))			//open ISP command I/O port
		{
			perror("ioperm");
		}
	}
		
	
	if(dataport != 0){
		if (ioperm(dataport, 1, 0))			//open ISP command I/O port
		{
			perror("ioperm");
		}
	}
#endif
}
/*==============================================================*/
// check status port IBF
// Input:	none
//
// Output:	-1	 - busy, timeout
//			 0	 - success, IBF clear
/*==============================================================*/
int ec_chk_IBF(UINT16 port)
{
	UINT16 retry = 0;
	
	while(mINB(port) & dIOSTA_IBF_BIT){
		if(++retry > dDLY_Chk_IOSTA){								// 2sec
			printf("ERROR: 0x%x IBF isn't clear.\n",port);
			return -1;
		}
		usleep(dTCK_Chk_IOSTA);									// loop tick 10ms
	}
	return 0;
}

/*==============================================================*/
// check status port OBF
// Input:	none
//
// Output:	-1	 - fail, no received
//			 0	 - success, OBF set
/*==============================================================*/
int ec_chk_OBF(UINT16 port)
{
	UINT16 retry = 0;
	
	while((mINB(port) & dIOSTA_OBF_BIT) == 0){
		if(++retry > dDLY_Chk_IOSTA){								// 2sec
			printf("ERROR: 0x%x OBF isn't set.\n",port);
			return -1;
		}
		usleep(dTCK_Chk_IOSTA);									// loop tick 10ms
	}
	return 0;
}

/*==============================================================*/
// write val to cmd port
// Input:	cmd	-	a byte data that want to write to cmd port
//
// Output:	-1	- busy, timeout
//			0	- success
/*==============================================================*/
int ec_wr_cmd(UINT8 cmd)
{
	if(ec_chk_IBF(cmdport) == 0){
	mMSG_IO_SET_CMD();
	mOUTB(cmdport,cmd);					// write val to cmd port
		return 0;
	}
	else 
		return -1;
}

/*==============================================================*/
// write val to data port
// Input:	val	-	a byte data that want to write to cmd port
//
// Output:	-1	- busy, timeout
//			0	- success
/*==============================================================*/
int ec_wr_data(UINT8 val)
{
	if(ec_chk_IBF(cmdport) == 0){
		mMSG_IO_SET_DATA();
		mOUTB(dataport,val);					// write val to data port
		return 0;
	}
	else 
		return -1;
}

/*==============================================================*/
// read val from cmd port
// Input:	none
// Output:	val - 	read from cmd port
/*==============================================================*/
int ec_rd_cmd(UINT8 *val)
{
	*val = mINB(cmdport);
	mMSG_IO_GET_CMD();
	return 0;
}

/*==============================================================*/
// read val from data port
// Input:	val	-	a byte data that want to write to cmd port
//
// Output:	-1	- busy, timeout
//			0	- success
/*==============================================================*/
int ec_rd_data(UINT8 *val)
{
	if(ec_chk_OBF(cmdport) == 0){
		*val = mINB(dataport);					// read val from data port
		mMSG_IO_GET_DATA();
		return 0;
	}
	else 
		return -1;
}

