#ifndef _IMANAGER3_IO_H_
#define _IMANAGER3_IO_H_

/* Functions */
// PMC
int pmc_data_in(pmc_port_t *pmcp, uint8_t *data);
int pmc_data_out(pmc_port_t *pmcp, uint8_t data);
int pmc_cmd_out(pmc_port_t *pmcp, uint8_t data);

// PMC Direct IO
uint8_t ecio_inb(uint16_t addr);
uint16_t ecio_inw(uint16_t addr);
uint32_t ecio_ind(uint16_t addr);
void ecio_outb(uint16_t addr, uint8_t data);
void ecio_outw(uint16_t addr, uint16_t data);
void ecio_outd(uint16_t addr, uint32_t data);

#endif /* _IMANAGER3_IO_H_ */
