#include "im3.h"

#define SFS_BUSY	(1 << 0) //1 = Internal Write operation is in progress ; 0 = No internal Write operation is in progress
#define SFS_WEL		(1 << 1) //1 = Device is memory Write enabled ;0 = Device is not memory Write enabled
#define SFS_BP0		(1 << 2) //Indicate current level of block write protection (See Table 5) 1 R/W
#define SFS_BP1		(1 << 3) //Indicate current level of block write protection (See Table 5) 1 R/W
#define SFS_AAI		(1 << 6) //Auto Address Increment Programming status . 1 = AAI programming mode ,0 = Byte-Program mode
#define SFS_BPL		(1 << 7) //1 = BP1, BP0 are read-only bits,0 = BP1, BP0 are read/writable

//SPI Flash command
#define SFLASH_WREN			0x06	//Write-Enable
#define SFLASH_WRDI			0x04	//Write-Disable
#define SFLASH_RDID			0x9F
#define SFLASH_RID			0x90	//Read-ID
#define SFLASH_RDSR			0x05	//Read-Status-Register
#define SFLASH_WRSR			0x01	//Write-Status-Register
#define SFLASH_EWSR			0x50	//Enable-Write-Status-Register
#define SFLASH_READ			0x03	//Read
#define SFLASH_FREAD		0x0B	//High-Speed-Read
#define SFLASH_SE			0x20	//Sector-Erase
#define SFLASH_BE			0xD8	//Block-Eras
#define SFLASH_CE			0xC7	//Chip-Erase
#define SFLASH_CE_SST		0x60	//Chip-Erase
#define SFLASH_CE_ATMEL		0x62	//Chip-Erase
#define SFLASH_PP			0x02	//Byte-Program
#define SFLASH_AAIWP		0xAD	//Auto Address Increment Programming
#define SFLASH_EBSY			0x70	//Enable SO to output RY/BY# status during AAI programming

//SPI Flash Read ID address
#define SFLASH_MID_ADDR		0x00	//Manufacturer's ID address
#define SFLASH_DID_ADDR		0x01	//Device ID address

/* Marco */
#define FLASH_WAIT_BUSY()	while(Flash_ReadStatus() & SFS_BUSY)

/* Functions */
#if defined(_WIN32) || defined(_UEFI)
static void Flash_WriteEnable()
#else
inline static void Flash_WriteEnable()
#endif
{
	// Send Write Enable Command
	uint8_t SpiSend = SFLASH_WREN;
	SPI_CSEn();
	SPI_Write(&SpiSend, 1);
	SPI_CSDis();
}

#if defined(_WIN32) || defined(_UEFI)
static void Flash_WriteDisable()
#else
inline static void Flash_WriteDisable()
#endif
{
	// Send Write Enable Command
	uint8_t SpiSend = SFLASH_WRDI;
	SPI_CSEn();
	SPI_Write(&SpiSend, 1);
	SPI_CSDis();
}

uint8_t Flash_ReadStatus()
{
	/* Read Status register from flash */
	uint8_t SpiSend[1];
	uint8_t SpiReceive;

	// Send Read-Status command
	SpiSend[0] = SFLASH_RDSR;
	SPI_CSEn();
	SPI_Write(SpiSend, 1);
	SPI_Read(&SpiReceive, 1);
	SPI_CSDis();
	
	return SpiReceive;
}

int Flash_ReadMIDPID(uint8_t* pMID, uint16_t* pDID)
{
	/* JEDEC ID read */
	uint8_t SpiSend[1];

	if (pMID == NULL || pDID == NULL)
		return -1;

	// Send SFLASH_RDID command
	SpiSend[0] = SFLASH_RDID;
	SPI_CSEn();
	SPI_Write(SpiSend, 1);
	SPI_Read(pMID, 1);
	SPI_Read((uint8_t *)pDID, 2);
	SPI_CSDis();

	return 0;
}

uint8_t Flash_ReadMID()
{
	/* Manufacturer¡¦s ID */
	uint8_t SpiSend[4];
	uint8_t SpiReceive;

	// Send SFLASH_RID command
	SpiSend[0] = SFLASH_RID;
	SpiSend[1] = 0;
	SpiSend[2] = 0;
	SpiSend[3] = SFLASH_MID_ADDR;
	SPI_CSEn();
	SPI_Write(SpiSend, 4);
	SPI_Read(&SpiReceive, 1);
	SPI_CSDis();
	return SpiReceive;
}

uint8_t Flash_ReadDID()
{
	/* Read Device ID */
	uint8_t SpiSend[4];
	uint8_t SpiReceive;

	// Send SFLASH_RID command
	SpiSend[0] = SFLASH_RID;
	SpiSend[1] = 0;
	SpiSend[2] = 0;
	SpiSend[3] = SFLASH_DID_ADDR;
	SPI_CSEn();
	SPI_Write(SpiSend, 4);
	SPI_Read(&SpiReceive, 1);
	SPI_CSDis();
	return SpiReceive;
}

uint32_t Flash_Read(uint32_t Addr, uint8_t* pData, uint32_t Len)
{
	/* Read data from flash */
	uint32_t lRet = 0;
	uint8_t SpiSend[4];

	if (pData != NULL && Len > 0)
	{
		// Send READ Command
		SpiSend[0] = SFLASH_READ;
		SpiSend[1] = (uint8_t)((Addr & 0x00FF0000) >> 16);
		SpiSend[2] = (Addr & 0x0000FF00) >> 8;
		SpiSend[3] = (Addr & 0x000000FF);
		SPI_CSEn();
		SPI_Write(SpiSend, 4);
		lRet = SPI_Read(pData, Len);
		SPI_CSDis();
	}
	return lRet;
}

int Flash_SectorErase(uint32_t Addr)
{
	uint8_t SpiSend[4];

	Flash_WriteEnable();

	// Send Sector Erase Command
	SpiSend[0] = SFLASH_SE;
	SpiSend[1] = (uint8_t)((Addr & 0x00FF0000) >> 16);
	SpiSend[2] = (Addr & 0x0000FF00) >> 8;
	SpiSend[3] = (Addr & 0x000000FF);
	SPI_CSEn();
	SPI_Write(SpiSend, 4);
	SPI_CSDis();

	FLASH_WAIT_BUSY();
	Flash_WriteDisable();
	return 0;
}

int Flash_BlockErase(uint32_t Addr)
{
	uint8_t SpiSend[4];

	Flash_WriteEnable();

	// Send Block Erase Command
	SpiSend[0] = SFLASH_BE;
	SpiSend[1] = (uint8_t)((Addr & 0x00FF0000) >> 16);
	SpiSend[2] = (Addr & 0x0000FF00) >> 8;
	SpiSend[3] = (Addr & 0x000000FF);
	SPI_CSEn();
	SPI_Write(SpiSend, 4);
	SPI_CSDis();

	FLASH_WAIT_BUSY();
	Flash_WriteDisable();
	return 0;
}

int Flash_ChipErase(uint8_t (*pfIsSuspend)(void))
{
	uint8_t SpiSend[4];

	FLASH_WAIT_BUSY();
	Flash_WriteEnable();

	// Send Chip Erase command
	SpiSend[0] = SFLASH_CE;
	SPI_CSEn();
	SPI_Write(SpiSend, 1);
	SPI_CSDis();

	// Wait until Flash Status ready
	while (Flash_ReadStatus() & SFS_BUSY)
	{
		if (pfIsSuspend)
		{
			if (pfIsSuspend())
				return -1;
		}
	}    

	Flash_WriteDisable();
	return 0;
}

int Flash_ByteProgram(uint32_t Addr, uint8_t Data)
{
	uint8_t SpiSend[5];

	Flash_WriteEnable();

	// Send Byte Program command
	SpiSend[0] = SFLASH_PP;
	SpiSend[1] = (uint8_t)((Addr & 0x00FF0000) >> 16);
	SpiSend[2] = (Addr & 0x0000FF00) >> 8;
	SpiSend[3] = (Addr & 0x000000FF);
	SpiSend[4] = Data;
	SPI_CSEn();
	SPI_Write(SpiSend, 5);
	SPI_CSDis();

	FLASH_WAIT_BUSY();
	Flash_WriteDisable();
	return 0;
}

uint32_t Flash_AAIWProgram(uint32_t Addr, uint8_t* pData, uint32_t Len)
{
	/* Issue command to flash to execution Auto Address Increment (AAI) Programming */
	uint32_t i = 0, lRet = 0;
	uint8_t SpiSend[4];

	if (pData != NULL && Len >0 )
		return lRet;

	Flash_WriteEnable();

	// Send Auto Addrees Increase Word Program command
	SpiSend[0] = SFLASH_AAIWP;
	SpiSend[1] = (uint8_t)((Addr & 0x00FF0000) >> 16);
	SpiSend[2] = (Addr & 0x0000FF00) >> 8;
	SpiSend[3] = (Addr & 0x000000FF);
	SPI_CSEn();
	SPI_Write(SpiSend, 4);

	while ((Len - i) >= 2)
	{
		SPI_Write(&pData[i] , 2);
		SPI_CSDis();

		// Wait until Flash Status ready
		while(Flash_ReadStatus() & SFS_BUSY);
		i += 2;

		// Send AAI command
		SpiSend[0] = SFLASH_AAIWP;
		SPI_CSEn();
		SPI_Write(SpiSend, 1);
	}

	// Write the tail..
	if (i == Len)
	{
		SpiSend[0] = 0xff;	
		SpiSend[1] = 0xff;	
	}
	else
	{
		SpiSend[0] = pData[i];
		SpiSend[1] = 0xff;
		i += 1;
	}
	SPI_Write(SpiSend, 2);
	SPI_CSDis();	

	FLASH_WAIT_BUSY();
	Flash_WriteDisable();
	return i;
}

uint32_t Flash_PageProgram(uint32_t Addr, uint8_t* pData, uint32_t Len)
{
	/* Issue command to flash to execution page program. */
	uint8_t SpiSend[5];
	uint32_t lPageRemain, lByteToPage, i = 0;

	if (pData && Len > 0)
	{
		while (i < Len)
		{
			FLASH_WAIT_BUSY();
			Flash_WriteEnable();

			// Calculate Current Page Remain Size
			lPageRemain = 0x100 - (Addr & 0xFF);
			// Calculate Remain Data to Write
			lByteToPage = Len - i;
			// Calculate How many date write to current page
			if (lByteToPage > lPageRemain)
				lByteToPage = lPageRemain;

			// Send Page Program command
			SpiSend[0] = SFLASH_PP;
			SpiSend[1] = (uint8_t)((Addr & 0x00FF0000) >> 16);
			SpiSend[2] = (Addr & 0x0000FF00) >> 8;
			SpiSend[3] = (Addr & 0x000000FF);
			SPI_CSEn();
			SPI_Write(SpiSend, 4);
			SPI_Write(&pData[i], lByteToPage);
			SPI_CSDis();

			i += lByteToPage;
			Addr += lByteToPage;
		}

		FLASH_WAIT_BUSY();
		Flash_WriteDisable();
    }

    return i;
}

void Flash_SetStatus(uint8_t Status)
{
	/* Set status to flash status register */
    uint8_t SpiSend[2];

    Flash_WriteEnable();

    // Send Set Status command
    SpiSend[0] = SFLASH_WRSR;
    SpiSend[1] = Status;
    SPI_CSEn();
    SPI_Write(SpiSend,2);
    SPI_CSDis();

    Flash_WriteDisable();
	FLASH_WAIT_BUSY();
}

void Flash_EnWriteStatus()
{
    uint8_t SpiSend[1];

	Flash_WriteEnable();

    // Send Enable Write status command
    SpiSend[0] = SFLASH_EWSR;
    SPI_CSEn();
    SPI_Write(SpiSend, 1);
    SPI_CSDis();

    Flash_WriteDisable();
	FLASH_WAIT_BUSY();
}
