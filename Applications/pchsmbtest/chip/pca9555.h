/*==========================================================*/
/* The Device Function For TI LM95245 File					*/
/* Date: 2017.05.08											*/
/*==========================================================*/
#ifndef __PCA9555_H
#define __PCA9555_H

/*------------------------------------*/
// SMBus Address
/*------------------------------------*/
#define DIO_SMB_ADDR			0x40				// select 0x40~4E by A0-A2 pin

/*------------------------------------*/
// Register list
/*------------------------------------*/
// Status Register
#define DIO_REG_IN_PORT0		0x00				// Input port 0
#define DIO_REG_IN_PORT1		0x01				// Input port 1
#define DIO_REG_OUT_PORT0		0x02				// Output port 0
#define DIO_REG_OUT_PORT1		0x03				// Output port 1
#define DIO_REG_INV_PORT0		0x04				// Polarity Inversion port 0 - allows the user to invert the polarity of the Input port register data.
#define DIO_REG_INV_PORT1		0x05				// Polarity Inversion port 1
#define DIO_REG_CONFIG0			0x06				// Configuration port 0
#define DIO_REG_CONFIG1			0x07				// Configuration port 1

// Error Code LM95245- if error code < 0x8001 is mean low level module error. ex. smbus
#define DIO_ERR_IC_NOT_FOUND	0x8001				// Input address is not lm95245 smbus address
#define DIO_ERR_GET_RO_REG		0x8002				// Input register is read only
#define DIO_ERR_GET_WO_REG		0x8003				// Input register is write only
#define DIO_ERR_GET_UNKNOWN_REG	0x8004				// Input register is unsupported

#endif //__PCA9555_H

