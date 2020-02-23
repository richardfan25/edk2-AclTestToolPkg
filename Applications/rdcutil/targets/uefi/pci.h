#ifndef _PCI_H_
#define _PCI_H_

#define PCI_BUS_MAX		4	// 256
#define PCI_DEV_MAX		32
#define PCI_FUN_MAX		8

uint32_t PCIRead(uint8_t bus, uint8_t device, uint8_t function, uint8_t reg);
void PCIWrite(uint8_t bus, uint8_t device, uint8_t function, uint8_t reg, uint32_t data);

#endif /* _PCI_H_ */
