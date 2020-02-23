#include "declarations.h"
#include "ioport.h"

#define PCIADDRESS(b,d,f,o)	(0x80000000 | ((unsigned long)b << 16) | ((unsigned long)d << 11) | ((unsigned long)f << 8) | (o & ~0x03))

// Private


// Public
uint32_t PCIRead(uint8_t bus, uint8_t device, uint8_t function, uint8_t reg)
{
	outpd(0xCF8, PCIADDRESS(bus, device, function, reg));
	return inpd(0xCFC);
}

void PCIWrite(uint8_t bus, uint8_t device, uint8_t function, uint8_t reg, uint32_t data)
{
	outpd(0xCF8, PCIADDRESS(bus, device, function, reg));
	outpd(0xCFC, data);
}
