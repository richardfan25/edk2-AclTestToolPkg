#include "im3.h"

/* Configurations */
#define PMC0_IRQ_NUM			0
#define PMC1_IRQ_NUM			0

/* Variables */
im3dev_t im3;

/* Functions */
// Get resources
static void CheckHostInterface(void)
{
	uint16_t ChipID;

	outPNPIndex(0x87);
	outPNPIndex(0x87);

	outPNPIndex(0x20);
	ChipID = inPNPData() << 8;
	outPNPIndex(0x21);
	ChipID |= inPNPData();

	if (ChipID == EC_ID)
	{
		// Enable SIO devices
		outPNPIndex(0x23);
		outPNPData((inPNPData() | 0x01));

		im3.isLPC = 1;
	}

	outPNPIndex(0xAA);
}

static void GetPMCPorts(void)
{
	if (im3.isLPC)
	{
		outPNPIndex(0x87);
		outPNPIndex(0x87);

		// PMC0
		outPNPIndex(0x07);
		outPNPData(LDN_PMC0);

		outPNPIndex(0x30);  // Enable device
		outPNPData(0x01);

		outPNPIndex(0x60);
		im3.pmc[0].data = inPNPData() << 8;
		outPNPIndex(0x61);
		im3.pmc[0].data |= inPNPData();

		outPNPIndex(0x62);
		im3.pmc[0].cmd = inPNPData() << 8;
		outPNPIndex(0x63);
		im3.pmc[0].cmd |= inPNPData();

		// PMC1
		outPNPIndex(0x07);
		outPNPData(LDN_PMC1);

		outPNPIndex(0x30);  // Enable device
		outPNPData(0x01);

		outPNPIndex(0x60);
		im3.pmc[1].data = inPNPData() << 8;
		outPNPIndex(0x61);
		im3.pmc[1].data |= inPNPData();

		outPNPIndex(0x62);
		im3.pmc[1].cmd = inPNPData() << 8;
		outPNPIndex(0x63);
		im3.pmc[1].cmd |= inPNPData();

		// PMC Mailbox
		outPNPIndex(0x07);
		outPNPData(LDN_PMCMB);

		outPNPIndex(0x30);  // Enable device
		outPNPData(0x01);

		outPNPIndex(0x60);
		im3.pmcmb.index = inPNPData() << 8;
		outPNPIndex(0x61);
		im3.pmcmb.index |= inPNPData();

		outPNPIndex(0x62);
		im3.pmcmb.data = inPNPData() << 8;
		outPNPIndex(0x63);
		im3.pmcmb.data |= inPNPData();

		// PMC IO
		outPNPIndex(0x07);
		outPNPData(LDN_PMCIO);

		outPNPIndex(0x30);  // Enable device
		outPNPData(0x01);

		outPNPIndex(0x60);
		im3.pmcio.addr = inPNPData() << 8;
		outPNPIndex(0x61);
		im3.pmcio.addr |= inPNPData();
		im3.pmcio.data = im3.pmcio.addr + 4;

		outPNPIndex(0xAA);
	}
	else
	{
		uint8_t busnum;
		uint32_t tmp_u32;

		for (busnum = 0; busnum < PCI_BUS_MAX; busnum++)
		{
			tmp_u32 = PCIRead(busnum, PMC_PCI_DEV, PMC_PCI_FUN, 0);
			if (tmp_u32 != 0xFFFFFFFF)
			{
				if (((uint16_t)tmp_u32 == EC_VID) && ((uint16_t)(tmp_u32 >> 16) == DID_PMC))
				{
#if 1
					// Native Mode 
					im3.pmc[0].data = (PCIRead(busnum, PMC_PCI_DEV, PMC_PCI_FUN, 0x10) & 0xFFFE) + 0;
					im3.pmc[0].cmd = im3.pmc[0].data + 1;

					im3.pmc[1].data = (PCIRead(busnum, PMC_PCI_DEV, PMC_PCI_FUN, 0x10) & 0xFFFE) + 0x02;
					im3.pmc[1].cmd = im3.pmc[0].data + 1;

					im3.pmcmb.index = (PCIRead(busnum, PMC_PCI_DEV, PMC_PCI_FUN, 0x10) & 0xFFFE) + 0x04;
					im3.pmcmb.data = im3.pmcmb.index + 1;

					im3.pmcio.addr = (PCIRead(busnum, PMC_PCI_DEV, PMC_PCI_FUN, 0x10) & 0xFFFE) + 0x08;
					im3.pmcio.data = im3.pmcio.addr + 4;
#else
					// Legacy Mode
					im3.pmc[0].data  = (uint16_t)PCIRead(busnum, PMC_PCI_DEV, PMC_PCI_FUN, 0x44) & 0xFFFE;
					im3.pmc[0].cmd = pmcp->data + 4;

					im3.pmc[1].data  = (uint16_t)(PCIRead(busnum, PMC_PCI_DEV, PMC_PCI_FUN, 0x44) >> 16) & 0xFFFE;
					im3.pmc[1].cmd = pmcp->data + 4;

					im3.pmcmb.index = (uint16_t)PCIRead(busnum, PMC_PCI_DEV, PMC_PCI_FUN, 0x48) & 0xFFFE;
					im3.pmcmb.data = im3.pmcmb.index + 1;

					im3.pmcio.addr = (uint16_t)(PCIRead(busnum, PMC_PCI_DEV, PMC_PCI_FUN, 0x48) >> 16) & 0xFFFE;
					im3.pmcio.data = im3.pmcio.addr + 4;
#endif

					break;
				}
			}
		}
	}
}

// Export
int im3_EnterUpgradeMode(void)
{
	uint8_t data;

	// Check EC is in upgrade mode or not
	data = inp(im3.pmc[1].data);
	if (data == 0xAA)
	{
		// Check PMC direct IO can be access
		if (ecio_ind(0xFF00) == 0x30313639)	// 0x30313639 (ASCII ¡¥9610¡¦)
			return 0;
	}
	
	// Enter upgrade mode
	data = 0;
	pmc_cmd_out(&im3.pmc[1], 0x40);
	pmc_data_out(&im3.pmc[1], 0x55);
	pmc_data_out(&im3.pmc[1], 0x55);
	pmc_data_in(&im3.pmc[1], &data);
	if (data != 0xAA)
		return -1;
	
	return 0;
}

void im3_LeaveUpgradeMode(void)
{
	// Start WDT
	ecio_outb(0xF500, ecio_inb(0xF500) | 0x40);
}

void im3_ResetEC(void)
{
	// Disable Isolation cell
	//ecio_outd(0xFF40, ecio_ind(0xFF40) & ~(1L << 17)); 
	// Timeout: 1ms
	ecio_outb(0xF502, 1);		
	ecio_outb(0xF503, 0);
	ecio_outb(0xF504, 0);
	// Reset event when timeout
	ecio_outb(0xF501, 0x10);
	// Start WDT	
	ecio_outb(0xF500, ecio_inb(0xF500) | 0x40);		
}

int im3_FactoryBurn(const PEIOIS200ROInfo roInfo)
{
	int i;
	uint8_t tmp;
	uint8_t offset = (uint8_t)((uint8_t *)roInfo->ByteProtect - (uint8_t *)roInfo);
	uint8_t length = sizeof(*roInfo) - offset;

	/* Write */
	while (1)
	{
		pmc_cmd_out(&im3.pmc[1], 0x50);
		pmc_data_in(&im3.pmc[1], &tmp);

		printf("tmp = 0x%X\r\n", tmp);

		if ((tmp & 0x01) == 0)		// Check available
			return -1;
		else if ((tmp & 0x02) > 0)	// Check busy
			continue;

		break;
	}

	pmc_data_out(&im3.pmc[1], offset);	// Write Data offset
	pmc_data_out(&im3.pmc[1], length);	// Write Data length

	for (i = 0; i < length; i++)
	{
		if (pmc_data_out(&im3.pmc[1], *(((uint8_t *)roInfo) + offset + i)) != 0)
			return -1;
	}

	return 0;
}

int im3_FactoryRead(const PEIOIS200ROInfo roInfo)
{
	int i;
	uint8_t tmp;
	uint8_t offset = 0;
	uint8_t length = sizeof(*roInfo);

	/* Write */
	while (1)
	{
		pmc_cmd_out(&im3.pmc[1], 0x51);
		pmc_data_in(&im3.pmc[1], &tmp);

		if ((tmp & 0x01) == 0)		// Check available
			return -1;
		else if ((tmp & 0x02) > 0)	// Check busy
			continue;

		break;
	}

	pmc_data_out(&im3.pmc[1], offset);	// Write Data offset
	pmc_data_out(&im3.pmc[1], length);	// Write Data length

	for (i = 0; i < length; i++)
	{
		if (pmc_data_in(&im3.pmc[1], ((uint8_t *)roInfo) + offset + i) != 0)
			return -1;
	}

	return 0;
}

int im3_Initialize(void)
{
	memset(&im3, 0, sizeof(im3dev_t));

	CheckHostInterface();
	GetPMCPorts();

	if ((im3.pmc[0].cmd == 0xFFFF || im3.pmc[0].cmd== 0) ||
		(im3.pmc[0].data == 0xFFFF || im3.pmc[0].data == 0) || 
		(im3.pmc[1].cmd == 0xFFFF || im3.pmc[1].cmd== 0) ||
		(im3.pmc[1].data == 0xFFFF || im3.pmc[1].data == 0))
		return -1;

	return 0;
}
