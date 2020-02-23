#include "im3.h"

//#define _ESPI_WORKAROUND_

#ifdef _ESPI_WORKAROUND_

static uint16_t chipID = 0x9610;
static uint16_t pmc0Data = 0x62, pmc0Cmd = 0x66;
static uint16_t pmc1Data = 0x2F2, pmc1Cmd = 0x2F6;
static uint16_t pmcmbIndex = 0x29E, pmcmbData = 0x29F;
static uint16_t pmcioAddr = 0x290, pmcioData = 0x294;
static uint16_t i2c0Addr = 0xCB0, i2c1Addr = 0xCC0;
static uint16_t smbus0Addr = 0xC00, smbus1Addr = 0xC20;
static uint16_t gpio0Addr = 0xC40;

static uint8_t pnpIndex = 0, pnpData = 0;

void outPNPIndex(uint8_t index)
{
	pnpIndex = index;

	if (pnpIndex == 0x87 || pnpIndex == 0xAA)
		pnpData = 0;
}

void outPNPData(uint8_t data)
{
	if (pnpIndex == 0x07)
		pnpData = data;
}

uint8_t inPNPData(void)
{
	uint16_t data6061 = 0, data6263 = 0;

	switch (pnpData)
	{
		case LDN_PMC0:
			data6061 = pmc0Data;
			data6263 = pmc0Cmd;
			break;
		case LDN_PMC1:
			data6061 = pmc1Data;
			data6263 = pmc1Cmd;
			break;
		case LDN_PMCMB:
			data6061 = pmcmbIndex;
			data6263 = pmcmbData;
			break;
		case LDN_PMCIO:
			data6061 = pmcioAddr;
			data6263 = pmcioData;
			break;
		case LDN_I2C0:
			data6061 = i2c0Addr;
			break;
		case LDN_I2C1:
			data6061 = i2c1Addr;
			break;
		case LDN_SMBUS0:
			data6061 = smbus0Addr;
			break;
		case LDN_SMBUS1:
			data6061 = smbus1Addr;
			break;
		case LDN_GPIO0:
			data6061 = gpio0Addr;
			break;
		case 0:
			data6061 = chipID;
			break;
		default:
			return 0xFF;
	}

	switch (pnpIndex)
	{
		case 0x60:
			return ((data6061 >> 8) & 0xFF);
		case 0x61:
			return (data6061 & 0xFF);
		case 0x62:
			return ((data6263 >> 8) & 0xFF);
		case 0x63:
			return (data6263 & 0xFF);

		case 0x20:
			return ((data6061 >> 8) & 0xFF);
		case 0x21:
			return (data6061 & 0xFF);
		default:
			return 0xFF;
	}
}

#else

void outPNPIndex(uint8_t index)
{
	outp(PNP_INDEX, index);
}

void outPNPData(uint8_t data)
{
	outp(PNP_DATA, data);
}

uint8_t inPNPData(void)
{
	return inp(PNP_DATA);
}

#endif
