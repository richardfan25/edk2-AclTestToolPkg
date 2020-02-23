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
