#ifndef __RDC_H__
#define __RDC_H__

#include "pmc.h"
#include "rdc_i2c.h"

#define RDC_VID					0x52
#define RDC_PID_A9610			0x10
#define RDC_CHIP_ID				0x9610
#define RDC_INIT_SEC_OFFSET		0xFFF00
#define RDC_FLASH_SIZE			0x100000

/*==============================================================*/
// Register Address
/*==============================================================*/
// SPI
#define RDC_SPI_FIFO_SIZE		16
#define RDC_SPI_TX_TIMEOUT		100000
// SPI controller Register
#define RDC_REG_SPI_ADDR		0xFFC0
#define RDC_REG_SPIDO			(RDC_REG_SPI_ADDR + 0x00)		// Output Data Register
#define RDC_REG_SPIDI			(RDC_REG_SPI_ADDR + 0x04)		// Input Register
#define RDC_REG_SPIST			(RDC_REG_SPI_ADDR + 0x08)		// Status Register
#define RDC_REG_SPICS			(RDC_REG_SPI_ADDR + 0x0A)		// Chip Select Register
#define RDC_REG_SPIEST			(RDC_REG_SPI_ADDR + 0x0B)		// Error Status Register
#define RDC_REG_SPIDIV			(RDC_REG_SPI_ADDR + 0x10)		// Clock Divider Register
#define RDC_REG_SPIMCFG			(RDC_REG_SPI_ADDR + 0x12)		// Mode Configure Register
#define RDC_REG_SPICTL			(RDC_REG_SPI_ADDR + 0x13)		// Control Register
#define RDC_REG_SPIDTC			(RDC_REG_SPI_ADDR + 0x15)		// Delayed Transfer Control Register
#define RDC_REG_SPIAF			(RDC_REG_SPI_ADDR + 0x1A)		// Auto-fetch register


/*==============================================================*/
// IO space
/*==============================================================*/
#define RDC_PNP_INDEX			0x299
#define RDC_PNP_DATA			0x29A

/*==============================================================*/
// LPC LDN devices
/*==============================================================*/
#define RDC_LDN_UART0			0x02
#define RDC_LDN_UART1			0x03
#define RDC_LDN_UART2			0x04
#define RDC_LDN_UART3			0x05
#define RDC_LDN_UART4			0x06
#define RDC_LDN_UART5			0x07
#define RDC_LDN_UART6			0x08
#define RDC_LDN_UART7			0x09
#define RDC_LDN_LPT				0x0A
#define RDC_LDN_KBC				0x0B
#define RDC_LDN_PMC0			0x0C
#define RDC_LDN_PMC1			0x0D
#define RDC_LDN_MBOX			0x0E
#define RDC_LDN_ECIO			0x0F
#define RDC_LDN_UART8			0x10
#define RDC_LDN_CAN0			0x18
#define RDC_LDN_CAN1			0x19
#define RDC_LDN_I2C0			0x20
#define RDC_LDN_I2C1			0x21
#define RDC_LDN_SMBUS0			0x22
#define RDC_LDN_SMBUS1			0x23
#define RDC_LDN_GPIO0			0x24
#define RDC_LDN_GPIO1			0x25

typedef union _rdc_spi_t{
	struct{
		uint8_t	init		: 1;
		uint8_t	spi_en		: 1;
		uint8_t	rvsd		: 6;
	}bits;
	uint8_t		raw;
}rdc_spi_t;

/*==============================================================*/
uint8_t rdc_pnp_read(uint8_t idx);
void 	rdc_pnp_write(uint8_t idx, uint8_t data);
void 	rdc_pnp_unlock(void);
void 	rdc_pnp_lock(void);

int 	rdc_pmc_write_cmd(uint8_t cmd);
int 	rdc_pmc_write_data(uint8_t data);
int 	rdc_pmc_read_data(uint8_t *data);

void 	 rdc_ecio_outp(uint16_t addr, uint8_t data);
void 	 rdc_ecio_outpw(uint16_t addr, uint16_t data);
void 	 rdc_ecio_outpd(uint16_t addr, uint32_t data);
uint8_t  rdc_ecio_inp(uint16_t addr);
uint16_t rdc_ecio_inpw(uint16_t addr);
uint32_t rdc_ecio_inpd(uint16_t addr);

int 	 rdc_spi_init(void);
void 	 rdc_spi_uninit(void);
uint8_t  rdc_spi_write_data(uint8_t* data, uint32_t len);
uint32_t rdc_spi_read_data(uint8_t* data, uint32_t len);

void 	 rdc_fla_enable_write(void);
void 	 rdc_fla_disable_write(void);
uint8_t  rdc_fla_read_status(void);
int 	 rdc_fla_read_id(uint8_t *id);
int 	 rdc_fla_read_mid(uint8_t *mid);
int 	 rdc_fla_read_mid(uint8_t *did);
void 	 rdc_fla_erase_sector(uint32_t addr);
void 	 rdc_fla_erase_block(uint32_t addr);
uint32_t rdc_fla_read(uint32_t addr, uint8_t *buf, uint32_t len);
uint32_t rdc_fla_program_page(uint32_t addr, uint8_t *buf, uint32_t len);

int 	 rdc_ec_init(void);
void	 rdc_wdt_en(uint32_t ms, uint8_t event);
#endif // __ECGF_H__


