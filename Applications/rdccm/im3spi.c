#include "im3.h"

#define WAIT_TX_TIMEOUT		100000
#define WAIT_RX_TIMEOUT		100000
#define SPI_FIFO_SIZE		16

// SPI status register
#define STR_BUSY		(1 << 7) //SPI Busy
#define STR_FIFU		(1 << 6) //FIFO full
#define STR_IDR			(1 << 5) //Input data ready when set.
#define STR_TDC			(1 << 4) //Output complete/FIFO empty when set.
#define STR_OFE			(1 << 3) //Output FIFO Empty.
#define STR_IFF			(1 << 2) //Input FIFO Full.

// SPI control register
#define CTR_FRE			(1 << 6) //Fast read enable( This bit can be set if it is NOT AMTEL flash, and Bit 5 must be 0 )
#define CTR_AFDIS		(1 << 5) //Auto-fetch disable when set (Default value=0)
#define CTR_FIEN		(1 << 4) //FIFO mode enable when set.

//SPI setting
#define SPI_BA			0xFFC0
#define SPI_OUTPORT		(SPI_BA + 0x00) //Boot Flash SPI 4IO Output Data Register
#define SPI_INPORT		(SPI_BA + 0x04) //Boot Flash SPI 4IO Input Register
#define SPI_STR			(SPI_BA + 0x08) //Boot Flash SPI 4IO Status Register
#define SPI_CSR			(SPI_BA + 0x0A) //Boot Flash SPI 4IO Chip Select Register
#define SPI_ESR			(SPI_BA + 0x0B) //Boot Flash SPI 4IO Error Status Register
#define SPI_CDR			(SPI_BA + 0x10) //Boot Flash SPI 4IO Clock Divider Register
#define SPI_MCR			(SPI_BA + 0x12) //Boot Flash SPI 4IO Mode Configure Register
#define SPI_CTR			(SPI_BA + 0x13) //Boot Flash SPI 4IO Control Register
#define SPI_DTR			(SPI_BA + 0x15) //Boot Flash SPI 4IO Delayed Transfer Control Register
#define SPI_AFR			(SPI_BA + 0x1A) //Boot Flash SPI 4IO Auto-fetch register

// Control Register
#define CRT_RST			(1 << 0)
#define CRT_RX_RST		(1 << 1)
#define CRT_TX_RST		(1 << 2)

/* Marco */
#define RESET_AND_WAIT(d)	{			\
	ecio_outb(SPI_CTR, d);				\
	while(ecio_inb(SPI_CTR) & (d));	}

typedef struct spi{
	uint8_t Usage;   // Unused: 0, SPI: 1
	uint16_t Div;
	uint16_t DivBk;
} spi_t;

static spi_t spi;

int SPI_Init()
{
	if (spi.Usage == 0)
	{
		spi.Usage = 1;
		spi.Div = 2;

		// Disable Auto fetch
		ecio_outb(SPI_AFR, 0x00);
		// Backup original setting
		spi.DivBk = ecio_inw(SPI_CDR);
		// Clear error bits
		ecio_outb(SPI_ESR, 0xFF);
		// CS Disable
		SPI_CSDis();
		// Configure clock divisor
		ecio_outw(SPI_CDR, spi.Div);

		//RESET_AND_WAIT(CRT_RX_RST | CRT_TX_RST);
	}
	else
	{
		DPRINTF("SPI no more resource.\r\n");
		return -1;
	}

	return 0;
}

void SPI_UnInit()
{
	ecio_outw(SPI_CDR, spi.DivBk);
	//Enable Auto fetch
	ecio_outb(SPI_AFR, 0x01);

	spi.Usage = 0;
}

void SPI_CSEn()
{
	ecio_outb(SPI_CSR, 0x00);
}

void SPI_CSDis()
{
	ecio_outb(SPI_CSR, 0x01);
}

static int SPI_WaitTRXDone()
{
    uint32_t Retry = 0;

    while(!(ecio_inb(SPI_STR) & STR_TDC))
    {
        // While Output FIFO empty
        if(++Retry > WAIT_TX_TIMEOUT)
        {
        	DPRINTF("SPI FIFO timeout! (0x%02X)\r\n", ecio_inb(SPI_STR));
            return -1;
        }
    }
    return 0;
}

uint32_t SPI_Exchange(uint8_t* pDestData, uint8_t* pSrcData , uint32_t nLen)
{
    uint32_t i, lRemain, lWrFifoSize, lPos = 0;

    if (pDestData != NULL && pSrcData != NULL)
    {
        if (SPI_WaitTRXDone() != 0)
			return lPos;

        while (lPos < nLen)
        {
			lRemain = nLen - lPos;
			lWrFifoSize = (lRemain < SPI_FIFO_SIZE) ? lRemain : SPI_FIFO_SIZE;

			// Write Data to output FIFO
			for (i = 0 ; i < lWrFifoSize ; i++)
			{
				ecio_outb(SPI_OUTPORT, pSrcData[lPos + i]);
			}

			// Wait TRX Finish
			if (SPI_WaitTRXDone() != 0)
			    break;

			// Read Data from input FIFO
			for (i = 0 ; i < lWrFifoSize ; i++)
			{
				pDestData[lPos + i] = ecio_inb(SPI_INPORT);
			}
			lPos += lWrFifoSize;
        }
    }

	return lPos;
}

uint32_t SPI_Read(uint8_t* pData, uint32_t nLen)
{
	uint32_t lRet = 0;
	uint8_t* pTXB = (uint8_t*)malloc(nLen);

	if (pTXB)
	{
		lRet = SPI_Exchange(pData, pTXB, nLen);
		free(pTXB);
	}
	return lRet;
}

uint8_t SPI_Write(uint8_t* pData, uint32_t nLen)
{
	uint32_t lRet = 0;
	uint8_t* pRXB = (uint8_t*)malloc(nLen);

	if (pRXB)
	{
		lRet = SPI_Exchange(pRXB, pData, nLen);
		free(pRXB);
	}
	return (uint8_t)lRet;
}
