#ifndef _ECUTIL_H_
#define _ECUTIL_H_

#include <stdint.h>
/*==============================================================*/
// CONFIG
/*==============================================================*/
#define	PJT_NAME			"SOM5788_"
#define PJT_INFO_POSTION	0x4000
#define PJT_NAME_SIZE		8
#define ECUTIL_VER			"V1.6"
#define ECUTIL_DATE			"2016/09/21"


#define	MAILBOX				1				// 1: access EC via mailbox
											// 0: access EC via legacy

/*==============================================================*/
// DEFINE
/*==============================================================*/
#if (MAILBOX)
	#define IOPORT_ECCMD	0x29A
	#define IOPORT_ECDAT	0x299
#else
	#define IOPORT_ECCMD	0x66
	#define IOPORT_ECDAT	0x62
#endif

/*==============================================================*/
// MACRO
/*==============================================================*/

/*==============================================================*/
// I/O command list
/*==============================================================*/
//General I/O command
#define dIO_ISP_START 			0xdc

/*==============================================================*/
// STRUCTION
/*==============================================================*/

// handle.pjt strcut
typedef struct _INFO_PJT{
	UINT8		kver[2];
	UINT8		id;
	UINT8		ver[4];
	UINT8		name[PJT_NAME_SIZE + 1];
	UINT8		chip[2];
} INFO_PJT;

#endif	// _ECUTIL_H_
