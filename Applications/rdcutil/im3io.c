#include "im3.h"

#define PMC_TIMEOUT		1000000

/* PMC */
static int WAIT_IBE(pmc_port_t *pmcp)
{
	uint32_t timecount = 0;

	while ((inp(pmcp->cmd) & 0x02) != 0)
	{
		if (++timecount > PMC_TIMEOUT)
		{
			DPRINTF("WATI_IBE Timeout\r\n");
			return -1;
		}
	}
	return 0;
}

static int WAIT_OBF(pmc_port_t *pmcp)
{
	uint32_t timecount = 0;

	while ((inp(pmcp->cmd) & 0x01) == 0)
	{
		if (++timecount > PMC_TIMEOUT)
		{
			DPRINTF("WATI_OBF Timeout\r\n");
			return -1;
		}
	}
	return 0;
}

int pmc_data_in(pmc_port_t *pmcp, uint8_t *data)
{
	if (WAIT_OBF(pmcp) != 0) return -1;
	*data = inp(pmcp->data);
	return 0;
}

int pmc_data_out(pmc_port_t *pmcp, uint8_t data)
{
	if (WAIT_IBE(pmcp) != 0) return -1;
	outp(pmcp->data, data);
    return 0;
}

int pmc_cmd_out(pmc_port_t *pmcp, uint8_t data)
{
	if (WAIT_IBE(pmcp) != 0) return -1;
    outp(pmcp->cmd, data);
    return 0;
}

/* PMC Direct IO */
uint8_t ecio_inb(uint16_t addr)
{
	outpw(im3.pmcio.addr, addr & 0xFFFC);
	return inp(im3.pmcio.data + (addr & 0x0003));
}

uint16_t ecio_inw(uint16_t addr)
{
	outpw(im3.pmcio.addr, addr & 0xFFFC);
	return inpw(im3.pmcio.data + (addr & 0x0002));
}

uint32_t ecio_ind(uint16_t addr)
{
	outpw(im3.pmcio.addr, addr & 0xFFFC);
	return inpd(im3.pmcio.data);
}

void ecio_outb(uint16_t addr, uint8_t data)
{
	outpw(im3.pmcio.addr, addr & 0xFFFC);
	outp(im3.pmcio.data + (addr & 0x0003), data);
}

void ecio_outw(uint16_t addr, uint16_t data)
{
	outpw(im3.pmcio.addr, addr & 0xFFFC);
	outpw(im3.pmcio.data + (addr & 0x0002), data);
}

void ecio_outd(uint16_t addr, uint32_t data)
{
	outpw(im3.pmcio.addr, addr & 0xFFFC);
	outpd(im3.pmcio.data, data);
}
