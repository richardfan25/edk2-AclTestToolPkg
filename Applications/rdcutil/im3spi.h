#ifndef _IMANAGER3_SPI_H_
#define _IMANAGER3_SPI_H_

/* Functions */
int SPI_Init();
void SPI_UnInit();
void SPI_CSEn();
void SPI_CSDis();
uint32_t SPI_Read(uint8_t* pData, uint32_t nLen);
uint8_t SPI_Write(uint8_t* pData, uint32_t nLen);

#endif /* _IMANAGER3_SPI_H_ */
