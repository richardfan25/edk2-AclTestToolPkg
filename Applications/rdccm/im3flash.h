#ifndef _IMANAGER3_FLASH_H_
#define _IMANAGER3_FLASH_H_

#define FLA_SECTOR_SIZE		(1024 * 4)
#define FLA_PAGE_SIZE		(256)

/* Functions */
uint8_t Flash_ReadStatus();
int Flash_ReadMIDPID(uint8_t* pMID, uint16_t* pDID);
uint8_t Flash_ReadMID();
uint8_t Flash_ReadDID();
uint32_t Flash_Read(uint32_t Addr, uint8_t* pData, uint32_t Len);
int Flash_SectorErase(uint32_t Addr);
int Flash_BlockErase(uint32_t Addr);
int Flash_ChipErase(uint8_t (*pfIsSuspend)(void));
int Flash_ByteProgram(uint32_t Addr, uint8_t Data);
uint32_t Flash_AAIWProgram(uint32_t Addr, uint8_t* pData, uint32_t Len);
uint32_t Flash_PageProgram(uint32_t Addr, uint8_t* pData, uint32_t Len);
void Flash_SetStatus(uint8_t Status);
void Flash_EnWriteStatus();

#endif /* _IMANAGER3_FLASH_H_ */
