#ifndef _RDCEC_H_
#define _RDCEC_H_

#include <stdint.h>


typedef struct
{
	uint8_t status;
	uint8_t type;
	uint16_t value;
	uint16_t max_value;
	uint16_t min_value;
}EC_VOLTAGE;

uint8_t rdc_ec_read_prot(uint8_t cmd, uint8_t ctl, uint8_t dev, uint8_t len, uint8_t *buf);
uint8_t rdc_ec_write_prot(uint8_t cmd, uint8_t ctl, uint8_t dev, uint8_t len, uint8_t *buf);
uint8_t rdc_ec_read_pmc_status(uint8_t cmd, uint8_t *buf);
uint8_t rdc_sw_ec_read_prot(uint8_t cmd, uint8_t index, uint8_t offset, uint8_t len, uint8_t *buf);
uint8_t rdc_sw_ec_write_prot(uint8_t cmd, uint8_t index, uint8_t offset, uint8_t len, uint8_t *buf);
uint8_t rdc_sw_ec_read_info_prot(uint8_t cmd, uint8_t index, uint8_t offset, uint8_t len, uint8_t *buf);

void show_pmc(void);
void show_thermal(void);
void show_voltage(void);
void show_current(void);
void show_pwm(void);
void show_tachometer(void);
void show_gpio(void);
void show_panel(void);
void show_fan(void);
void show_case_open(int ac);
void show_board_info(void);
void show_system_info(char *par);
void show_watchdog(void);
void show_storage(char *par);
void show_acpi_ec_ram(void);
void show_ec_ram(char *par);
int show_prot(uint8_t cmd, uint8_t ctl, uint8_t dev, uint8_t len, uint8_t *dat);
int ShowSWProtocol(uint8_t cmd, uint8_t index, uint8_t offset, uint8_t len, uint8_t *buf);


#endif
