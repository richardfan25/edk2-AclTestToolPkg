#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "typedef.h"
#include "aec.h"
#include "io.h"

//=============================================================================
//  EC_STS (66h)
//=============================================================================
//
//  +-----+-----+-----+-----+-----+-----+-----+-----+-----+
//  | Bit |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
//  +-----+-----+-----+-----+-----+-----+-----+-----+-----+
//  |Name | GPF | SMI | SCI |BURST| CMD | GPF2| IBF | OBF |
//  +-----+-----+-----+-----+-----+-----+-----+-----+-----+
//         ALERT                          BUSY
//

#define ACPI_EC_CMD		0x66
#define ACPI_EC_STS		0x66
#define ACPI_EC_DAT		0x62

#define EC_CMD	0x2F6
#define EC_STS	EC_CMD
#define EC_DAT	0x2F2

//#define EC_CMD	0x66
//#define EC_STS	EC_CMD
//#define EC_DAT	0x62

#define EC_STS_OBF		0x1		// bit0
#define EC_STS_IBF		0x2		// bit1
#define EC_STS_IBF_OBF	0x3		// bit0/bit1
#define EC_STS_BUSY		0x4		// bit2
#define EC_STS_CMD		0x8		// bit3: 1=CMD,0=DAT
#define EC_STS_BURST	0x10	// bit4
#define EC_STS_SCI		0x20	// bit5
#define EC_STS_SMI		0x40	// bit6
#define EC_STS_ALERT	0x80	// bit7


#define EC_RETRIES	(1000000)

//typedef unsigned char	uint8_t;
//typedef unsigned int	uint16_t;
//typedef unsigned long	uint32_t;

unsigned char	baseaddr = 0;

//uint16_t		eccmd = 0x2f6;
//uint16_t		ecdat = 0x2f2;

char	pmc_err_str[9][16] =
{
	"OK",
	"[0]cmd error!",
	"[1]ctl error!",
	"[2]dev error!",
	"[3]len error!",
	"[4]dat error!",
	"Protocol error!",
	"Read-Only!",
	"Write-Only!"
};

//=============================================================================
//  show_prot_res
//=============================================================================
void show_prot_res(uint8_t res_prot)
{
	rdc_fw_prot_t	prot;

	uint8_t			res;
	uint8_t			sts;
	
	prot.cmd = 0x01;
	prot.dat = &sts;
	res = rdc_fw_prot_pmc(&prot);

	if (res_prot == AEC_OK && res == AEC_OK && sts == AEC_PMC_OK)
	{
		printf( "OK!\n");
	}
	else
	{
		if (res_prot != AEC_OK)
			printf( "ERR: rdc_fw_prot_rw\n");
		if (res != AEC_OK)
			printf( "ERR: rdc_fw_prot_pmc\n");
		if (sts != AEC_PMC_OK)
			printf( "ERR: code=%d, %s\n", sts, pmc_err_str[sts]);
	}
}

//=============================================================================
//  ec_wait_ibe
//=============================================================================
uint8_t ec_wait_ibe(void)
{
	uint32_t	retries = EC_RETRIES;
	uint8_t		sts;
	uint8_t		alert = 0;

	do
	{
		sts = inp(EC_STS);
		
		if ((sts & EC_STS_IBF)==0)
			break;

		if (sts & EC_STS_ALERT)
		{
			alert = 1;
			break;
		}

	} while(retries--);

	if (alert)
		return 0xFE;
	else if (retries)
		return 0;
	else
		return 0xFF;
}
//=============================================================================
//  ec_wait_obe
//=============================================================================
uint8_t ec_wait_obe(void)
{
	uint32_t	retries = EC_RETRIES;
	uint8_t		sts;
	uint8_t		alert = 0;

	do
	{
		sts = inp(EC_STS);
		
		if ((sts & EC_STS_OBF)==0)
			break;

		if (sts & EC_STS_ALERT)
		{
			alert = 1;
			break;
		}

		inp(EC_DAT);

	} while(retries--);

	if (alert)
		return 0xFE;
	else if (retries)
		return 0;
	else
		return 0xFF;
}
//=============================================================================
//  ec_wait_obf
//=============================================================================
uint8_t ec_wait_obf(void)
{
	uint32_t	retries = EC_RETRIES;
	uint8_t		sts;
	uint8_t		alert = 0;

	do
	{
		sts = inp(EC_STS);
		
		if (sts & EC_STS_OBF)
			break;

		if (sts & EC_STS_ALERT)
		{
			alert = 1;
			break;
		}

	} while(retries--);

	if (alert)
		return 0xFE;
	else if (retries)
		return 0;
	else
		return 0xFF;
}
//=============================================================================
//  acpi_wait_ibe
//=============================================================================
uint8_t acpi_wait_ibe(void)
{
	uint32_t	retries = EC_RETRIES;
	uint8_t		sts;

	do
	{
		sts = inp(ACPI_EC_STS);
		
		if ((sts & EC_STS_IBF) == 0)
			break;

		if (sts & EC_STS_ALERT)
		{
			retries = 0;
			break;
		}

	} while(retries--);

	if (retries)
		return 0;
	else
		return 0xFF;
}

//=============================================================================
//  acpi_wait_obf
//=============================================================================
uint8_t acpi_wait_obf(void)
{
	uint32_t	retries = EC_RETRIES;
	uint8_t		sts;

	do
	{
		sts = inp(ACPI_EC_STS);
		
		if (sts & EC_STS_OBF)
			break;

		if (sts & EC_STS_ALERT)
		{
			retries = 0;
			break;
		}

	} while(retries--);

	if (retries)
		return 0;
	else
		return 0xFF;
}

//=============================================================================
//  ec_write_cmd
//=============================================================================
uint8_t ec_write_cmd(uint8_t cmd)
{
	//uint32_t	retries = 50000;
	uint8_t		res;

	// make sure data port is empty
	/*
	do
	{
		if ((inportb(EC_STS) & EC_STS_OBF) == 0)
			break;

		inportb(EC_DAT);

	} while(retries--);
	*/
	/*
	while (inportb(EC_STS) & EC_STS_OBF)
	{
		inportb(EC_DAT);
		retries--;
		if (retries == 0)
			break;
	}
	*/

	res = ec_wait_obe();
	if (!res)
	{
		res = ec_wait_ibe();
		if (!res)
			outp(EC_CMD, cmd);
	}

	return res;
}

//=============================================================================
//  ec_write_dat
//=============================================================================
uint8_t ec_write_dat(uint8_t dat)
{
	uint8_t	res;

	res = ec_wait_ibe();
	if (!res)
		outp(EC_DAT, dat);

	return res;
}

//=============================================================================
//  ec_read_dat
//=============================================================================
uint8_t ec_read_dat(uint8_t *dat)
{
	uint8_t	res;

	res = ec_wait_obf();
	if (!res)
		*dat = inp(EC_DAT);

	return res;
}

//=============================================================================
//  rdc_ec_read_prot
//=============================================================================
uint8_t rdc_ec_read_prot(uint8_t cmd, uint8_t ctl, uint8_t dev, uint8_t len, uint8_t *buf)
{
	rdc_fw_prot_t	prot;

	prot.cmd = cmd;
	prot.ctl = ctl;
	prot.dev = dev;
	prot.len = len;
	prot.dat = buf;

	return rdc_fw_prot_read(&prot);
}
	
//=============================================================================
//  rdc_ec_write_prot
//=============================================================================
uint8_t rdc_ec_write_prot(uint8_t cmd, uint8_t ctl, uint8_t dev, uint8_t len, uint8_t *buf)
{
	rdc_fw_prot_t	prot;

	prot.cmd = cmd;
	prot.ctl = ctl;
	prot.dev = dev;
	prot.len = len;
	prot.dat = buf;

	return rdc_fw_prot_write(&prot);
}

//=============================================================================
//  rdc_ec_read_pmc_status
//=============================================================================
uint8_t rdc_ec_read_pmc_status(uint8_t cmd, uint8_t *buf)
{
	uint8_t		res;

	res = ec_write_cmd(cmd);
	if (res)
		return res;
	
	res = ec_read_dat(buf);
	if (res)
		return res;

	return res;
}

//=============================================================================
//  rdc_sw_ec_read_prot
//=============================================================================
uint8_t rdc_sw_ec_read_prot(uint8_t cmd, uint8_t index, uint8_t offset, uint8_t len, uint8_t *buf)
{
	rdc_sw_prot_t	prot;

	prot.cmd = cmd;
	prot.idx = index;
	prot.off = offset;
	prot.len = len;
	prot.dat = buf;

	if(index == 0xFF)
	{
		return rdc_sw_prot_noidx_read(&prot);
	}
	else
	{
		return rdc_sw_prot_idx_read(&prot);
	}
}
	
//=============================================================================
//  rdc_sw_ec_write_prot
//=============================================================================
uint8_t rdc_sw_ec_write_prot(uint8_t cmd, uint8_t index, uint8_t offset, uint8_t len, uint8_t *buf)
{
	rdc_sw_prot_t	prot;

	prot.cmd = cmd;
	prot.idx = index;
	prot.off = offset;
	prot.len = len;
	prot.dat = buf;

	if(index == 0xFF)
	{
		return rdc_sw_prot_noidx_write(&prot);
	}
	else
	{
		return rdc_sw_prot_idx_write(&prot);
	}
}

//=============================================================================
//  rdc_sw_ec_read_info_prot
//=============================================================================
uint8_t rdc_sw_ec_read_info_prot(uint8_t cmd, uint8_t index, uint8_t offset, uint8_t len, uint8_t *buf)
{
	rdc_sw_prot_t	prot;

	prot.cmd = cmd;
	prot.idx = index;
	prot.off = offset;
	prot.len = len;
	prot.dat = buf;

	return rdc_sw_prot_info_read(&prot);
}

//=============================================================================
//  show_pmc
//=============================================================================
void show_pmc(void)
{
	uint8_t		data;
	uint8_t		res;
	uint8_t		ch;

	rdc_fw_prot_t	prot;
	
	printf("[0] PMC Status : ");
	prot.cmd = 0x00;
	prot.dat = &data;
	res = rdc_fw_prot_pmc(&prot);
	if (res == AEC_OK)
		printf("0x%02X\n", data);
	else
		printf("err! 0x%02X\n", res);


	printf("[1] PMC Status : ");
	prot.cmd = 0x01;
	prot.dat = &data;
	res = rdc_fw_prot_pmc(&prot);
	if (res == AEC_OK)
		printf("0x%02X\n", data);
	else
		printf("err! 0x%02X\n", res);

	for (ch=0; ch<2; ch++)
	{
		printf("[%d] PMC Config : ", ch);
		
		// PMC SW/SCI_SW, SMI_SW
		res = rdc_ec_read_prot(0x03, 0x00, ch, 1, &data);
		if (res == AEC_OK)
			printf("0x%02X\n", data);
		else
			printf("err! 0x%02X\n", res);

		printf("[%d] PMC Pulse  : ", ch);
		// PMC SCI/SMI pulse
		res = rdc_ec_read_prot(0x03, 0x01, ch, 1, &data);
		if (res == AEC_OK)
			printf("0x%02X\n", data);
		else
			printf("err! 0x%02X\n", res);
	}
}

//=============================================================================
//  show_thermal
//=============================================================================
void show_thermal(void)
{
	uint8_t		buf[64];
	//uint16_t	*p16, data16;
	//uint32_t	*p32, data32;
	uint16_t		data16;
	int			s16;
	uint8_t		t;
	uint8_t		res;

	printf("[-] Thermal Module Status     : ");
	for (t=0; t<4; t++)
	{
		res = rdc_ec_read_prot(0x11, 0x00, t, 2, buf);
		if (res != AEC_OK)
			printf("err! ");
		else
			printf("0x%04X ", *(uint16_t*)buf);
	}
	printf("\n");


	for (t=0; t<4; t++)
	{
		res = rdc_ec_read_prot(0x11, 0x00, t, 2, buf);

		// avaliable ?
		if ((*(uint16_t*)buf & 0x1) == 0)
			continue;
		
		printf("[%d] Thermal Device Type       : ", t);
		res = rdc_ec_read_prot(0x11, 0x01, t, 1, buf);
		if (res != AEC_OK)
			printf("err!\n");
		else
			printf("0x%02X\n", buf[0]);

		
		printf("[%d] Thermal Device Error Code : ", t);
		res = rdc_ec_read_prot(0x11, 0x04, t, 1, buf);
		if (res != AEC_OK)
			printf("err!\n");
		else
			printf("0x%02X\n", buf[0]);
		

		printf("[%d] Temperature (Value, Max, Min) :\n", t);
		res = rdc_ec_read_prot(0x11, 0x10, t, 2, buf);
		if (res != AEC_OK)
		{
			printf("    err! ");
		}
		else
		{
			data16 = *(uint16_t*)buf;
			printf("    0x%04X = %d = %d.%d C, ", data16, data16, (data16-2731)/10, (data16-2731)%10);
		}
		
		res = rdc_ec_read_prot(0x11, 0x11, t, 2, buf);
		if (res != AEC_OK)
		{
			printf("err! ");
		}
		else
		{
			data16 = *(uint16_t*)buf;
			printf("0x%04X = %d = %d.%d C, ", data16, data16, (data16-2731)/10, (data16-2731)%10);
		}


		res = rdc_ec_read_prot(0x11, 0x12, t, 2, buf);
		if (res != AEC_OK)
		{
			printf("err!\n");
		}
		else
		{
			data16 = *(uint16_t*)buf;
			s16 = (int16_t)data16;
			printf("0x%04X = %d = %.1f C\n", data16, data16, (float)(s16-2731)/10.0);

			//if (data16 < 2731)
			//	data16 = 2731 - data16;
			//else
			//	data16 -= 2731;
			//printf("%d C\n", data16%10);
		}
		
		
		printf("[%d] Thermal Prot - Throttle (Sts, Hi, Lo)  :\n", t);
		res = rdc_ec_read_prot(0x11, 0x20, t, 1, buf);
		if (res != AEC_OK)
			printf("    err! ");
		else
			printf("    0x%02X, ", buf[0]);

		
		res = rdc_ec_read_prot(0x11, 0x21, t, 2, buf);
		if (res != AEC_OK)
		{
			printf("err! ");
		}
		else
		{
			data16 = *(uint16_t*)buf;
			printf("0x%04X = %d = %d.%d C, ", data16, data16, (data16-2731)/10, (data16-2731)%10);
		}
		

		res = rdc_ec_read_prot(0x11, 0x22, t, 2, buf);
		if (res != AEC_OK)
		{
			printf("err!\n");
		}
		else
		{
			data16 = *(uint16_t*)buf;
			printf("0x%04X = %d = %d.%d C\n", data16, data16, (data16-2731)/10, (data16-2731)%10);
		}
		

		printf("[%d] Thermal Prot - Poweroff (Sts, Hi, Lo) :\n", t);
		res = rdc_ec_read_prot(0x11, 0x30, t, 1, buf);
		if (res != AEC_OK)
			printf("    err! ");
		else
			printf("    0x%02X, ", buf[0]);


		res = rdc_ec_read_prot(0x11, 0x31, t, 2, buf);
		if (res != AEC_OK)
		{
			printf("err! ");
		}
		else
		{
			data16 = *(uint16_t*)buf;
			printf("0x%04X = %d = %d.%d C, ", data16, data16, (data16-2731)/10, (data16-2731)%10);
		}


		res = rdc_ec_read_prot(0x11, 0x32, t, 2, buf);
		if (res != AEC_OK)
		{
			printf("err!\n");
		}
		else
		{
			data16 = *(uint16_t*)buf;
			printf("0x%04X = %d = %d.%d C\n", data16, data16, (data16-2731)/10, (data16-2731)%10);
		}


		printf("[%d] Thermal Prot - Shutdown (Sts, Hi, Lo) :\n", t);
		res = rdc_ec_read_prot(0x11, 0x40, t, 1, buf);
		if (res != AEC_OK)
			printf("    err! ");
		else
			printf("    0x%02X ", buf[0]);

		
		res = rdc_ec_read_prot(0x11, 0x41, t, 2, buf);
		if (res != AEC_OK)
		{
			printf("err! ");
		}
		else
		{
			data16 = *(uint16_t*)buf;
			printf("0x%04X = %d = %d.%d C, ", data16, data16, (data16-2731)/10, (data16-2731)%10);
		}


		res = rdc_ec_read_prot(0x11, 0x42, t, 2, buf);
		if (res != AEC_OK)
		{
			printf("err!\n");
		}
		else
		{
			data16 = *(uint16_t*)buf;
			printf("0x%04X = %d = %d.%d C\n", data16, data16, (data16-2731)/10, (data16-2731)%10);
		}
	}//for
}
//=============================================================================
//  show_voltage
//=============================================================================
void show_voltage(void)
{
	uint8_t		buf[64];
	//uint16_t	*p16, data16;
	uint16_t		data16;
	uint8_t		v;
	uint8_t		res;

	printf("[-] Voltage Module Status : ");
	for (v=0; v<8; v++)
	{
		res = rdc_ec_read_prot(0x13, 0x00, v, 1, buf);
		if (res != AEC_OK)
			printf("err! ");
		else
			printf("0x%02X ", buf[0]);
	}
	printf("\n");
		
	for (v=0; v<8; v++)
	{
		res = rdc_ec_read_prot(0x13, 0x00, v, 1, buf);
		
		// available ?
		if ((buf[0] & 0x1) == 0)
			continue;

		printf("[%d] Voltage Type          : ", v);
		res = rdc_ec_read_prot(0x13, 0x01, v, 1, buf);
		if (res != AEC_OK)
			printf("err!\n");
		else
			printf("0x%02X\n", buf[0]);
		

		printf("[%d] Voltage Value         : ", v);
		res = rdc_ec_read_prot(0x13, 0x10, v, 2, buf);
		if (res != AEC_OK)
		{
			printf("err!\n");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			printf("0x%04X = %d = %d.%02d V\n", data16, data16, data16/100, data16%100);
		}

		
		printf("[%d] Voltage Value Max     : ", v);
		res = rdc_ec_read_prot(0x13, 0x11, v, 2, buf);
		if (res != AEC_OK)
		{
			printf("err!\n");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			printf("0x%04X = %d = %d.%02d V\n", data16, data16, data16/100, data16%100);
		}
		

		printf("[%d] Voltage Value Min     : ", v);
		res = rdc_ec_read_prot(0x13, 0x12, v, 2, buf);
		if (res != AEC_OK)
		{
			printf("err!\n");
		}
		else
		{
			data16 = *(uint16_t *)buf;
			printf("0x%04X = %d = %d.%02d V\n", data16, data16, data16/100, data16%100);
		}
		
	}//for
}
	
//=============================================================================
//  show_current
//=============================================================================
void show_current(void)
{
	uint8_t		buf[64];
	uint16_t	*p16, data16;
	uint8_t		c;
	uint8_t		res;

	printf("[-] Current Module Status : ");
	for (c=0; c<2; c++)
	{
		res = rdc_ec_read_prot(0x1B, 0x00, c, 1, buf);
		if (res != AEC_OK)
			printf("err! ");
		else
			printf("0x%02X ", buf[0]);
	}
	printf("\n");
		
	for (c=0; c<2; c++)
	{
		rdc_ec_read_prot(0x1B, 0x00, c, 1, buf);

		if ((buf[0] & 0x1) == 0)
			continue;

		rdc_ec_read_prot(0x1B, 0x01, c, 1, buf);
		printf("[%d] Current Type          : 0x%02X\n", c, buf[0]);

		rdc_ec_read_prot(0x1B, 0x10, c, 2, buf);
		p16 = (uint16_t *)&buf[0];
		data16 = *p16;
		printf("[%d] Current Value         : 0x%04X = %d mA\n", c, data16, data16);

		rdc_ec_read_prot(0x1B, 0x11, c, 2, buf);
		p16 = (uint16_t *)&buf[0];
		data16 = *p16;
		printf("[%d] Current Value Max     : 0x%04X = %d mA\n", c, data16, data16);

		rdc_ec_read_prot(0x1B, 0x12, c, 2, buf);
		p16 = (uint16_t *)&buf[0];
		data16 = *p16;
		printf("[%d] Current Value Min     : 0x%04X = %d mA\n", c, data16, data16);
	}//for
}

//=============================================================================
//  show_pwm
//=============================================================================
void show_pwm(void)
{
	uint8_t		buf[64];
	uint32_t	*p32, data32;
	uint8_t		p;
	uint8_t		res;

	printf("[-] PWM Module Status : ");
	for (p=0; p<3; p++)
	{
		res = rdc_ec_read_prot(0x15, 0x00, p, 1, buf);
		if (res != AEC_OK)
			printf("err! ");
		else
			printf("0x%02X ", buf[0]);
	}
	printf("\n");

	for (p=0; p<3; p++)
	{
		rdc_ec_read_prot(0x15, 0x00, p, 1, buf);

		if ((buf[0] & 0x1) == 0)
			continue;

		rdc_ec_read_prot(0x15, 0x10, p, 1, buf);
		printf("[%d] PWM Duty Cycle    : 0x%02X = %d %%\n", p, buf[0], buf[0]);

		rdc_ec_read_prot(0x15, 0x11, p, 1, buf);
		printf("[%d] PWM Polarity      : 0x%02X\n", p, buf[0]);

		rdc_ec_read_prot(0x15, 0x12, p, 4, buf);
		p32 = (uint32_t *)&buf[0];
		data32 = *p32;
		printf("[%d] PWM Frequency     : 0x%08lX = %lu Hz\n", p, data32, data32);

	}//for
}
	
//=============================================================================
//  show_tachometer
//=============================================================================
void show_tachometer(void)
{
	uint8_t		buf[64];
	uint32_t	*p32, data32;
	uint8_t		t;
	uint8_t		res;

	printf("[-] Tachometer Module Status : ");
	for (t=0; t<4; t++)
	{
		res = rdc_ec_read_prot(0x17, 0x00, 0, 1, buf);
		if (res != AEC_OK)
			printf("err! ");
		else
			printf("0x%02X ", buf[0]);
	}
	printf("\n");
		
	for (t=0; t<4; t++)
	{
		rdc_ec_read_prot(0x17, 0x00, 0, 1, buf);

		if ((buf[0] & 0x1) == 0)
			continue;

		rdc_ec_read_prot(0x17, 0x01, 0, 1, buf);
		printf("[%d] Tachometer Type          : 0x%02X\n", t, buf[0]);

		rdc_ec_read_prot(0x17, 0x03, 0, 1, buf);
		printf("[%d] Tachometer Pulse         : 0x%02X\n", t, buf[0]);

		rdc_ec_read_prot(0x17, 0x10, 0, 4, buf);
		p32 = (uint32_t *)&buf[0];
		data32 = *p32;
		printf("[%d] Tachometer RPM           : 0x%08lX = %lu rpm\n", t, data32, data32);
	
	}//for
}
//=============================================================================
//  show_gpio
//=============================================================================
void show_gpio(void)
{
	uint8_t		buf[64];
	uint16_t	*p16, data16;
	uint8_t		p, dir,dat;

	rdc_ec_read_prot(0x19, 0x00, 0, 1, buf);
	printf("[--] OEM GPIO Module Status       : 0x%02X\n", buf[0]);

	rdc_ec_read_prot(0x19, 0x03, 0, 2, buf);
	p16 = (uint16_t *)&buf[0];
	data16 = *p16;
	printf("[--] OEM GPIO Availability        : 0x%04X\n", data16);

	for (p=0; p<16; p++)
	{
		rdc_ec_read_prot(0x19, 0x10, p, 1, buf);
		dir = buf[0];
		rdc_ec_read_prot(0x19, 0x11, p, 1, buf);
		dat = buf[0];
		
		printf("[%2d] OEM GPIO                     : Dir = %02X   Dat = %02X\n", p, dir, dat);
	}
	
	rdc_ec_read_prot(0x19, 0x12, 0, 2, buf);
	p16 = (uint16_t *)&buf[0];
	data16 = *p16;
	printf("[--] OEM GPIO Pin Group Direction : 0x%04X\n", data16);

	rdc_ec_read_prot(0x19, 0x13, 0, 2, buf);
	p16 = (uint16_t *)&buf[0];
	data16 = *p16;
	printf("[--] OEM GPIO Pin Group Data      : 0x%04X\n", data16);
}

//=============================================================================
//  show_panel
//=============================================================================
void show_panel(void)
{
	uint8_t		buf[64];
	uint16_t	*p16, data16;
	uint32_t	*p32, data32;
	uint8_t		p;

	// Panel 0
	for (p=0; p<2; p++)
	{
		
	rdc_ec_read_prot(0x21, 0x00, p, 1, buf);
	printf("[%d] Panel Module Status       : 0x%02X\n", p, buf[0]);

	if ((buf[0] & 0x1) == 0)
		continue;
	
	rdc_ec_read_prot(0x21, 0x10, p, 1, buf);
	printf("[%d] Backlight Enable Source   : 0x%02X\n", p, buf[0]);

	rdc_ec_read_prot(0x21, 0x11, p, 1, buf);
	printf("[%d] LVDS Reset Pin            : 0x%02X\n", p, buf[0]);

	rdc_ec_read_prot(0x21, 0x12, p, 1, buf);
	printf("[%d] Backlight Switch          : 0x%02X (1=on, 0=off)\n", p, buf[0]);

	rdc_ec_read_prot(0x21, 0x13, p, 1, buf);
	printf("[%d] Backlight Switch Polarity : 0x%02X\n", p, buf[0]);

	rdc_ec_read_prot(0x21, 0x14, p, 1, buf);
	printf("[%d] Backlight PWM Duty Cycle  : 0x%02X = %d%%\n", p, buf[0], buf[0]);

	rdc_ec_read_prot(0x21, 0x15, p, 1, buf);
	printf("[%d] Backlight PWM Polarity    : 0x%02X\n", p, buf[0]);

	rdc_ec_read_prot(0x21, 0x16, p, 4, buf);
	p32 = (uint32_t *)&buf[0];
	data32 = *p32;
	printf("[%d] Backlight PWM Frequency   : 0x%08lX = %lu Hz\n", p, data32, data32);

	rdc_ec_read_prot(0x21, 0x17, p, 2, buf);
	p16 = (uint16_t *)&buf[0];
	data16 = *p16;
	printf("[%d] Backlight Delay ON        : 0x%04X = %d ms\n", p, data16, data16);

	rdc_ec_read_prot(0x21, 0x18, p, 2, buf);
	p16 = (uint16_t *)&buf[0];
	data16 = *p16;
	printf("[%d] Backlight Delay OFF       : 0x%04X = %d ms\n", p, data16, data16);

	rdc_ec_read_prot(0x21, 0x19, p, 1, buf);
	printf("[%d] Backlight Control Mode    : 0x%02X\n", p, buf[0]);

	}//for
	
}
//=============================================================================
//  show_fan
//=============================================================================
void show_fan(void)
{
	printf("TODO...\n");
}

//=============================================================================
//  show_case_open
//=============================================================================
void show_case_open(int ac)
{
	uint8_t		buf[64];
	uint16_t	*p16, data16;

	rdc_ec_read_prot(0x29, 0x00, 0, 1, buf);
	printf("[-] Case Open Module Status : 0x%02X\n", buf[0]);

	rdc_ec_read_prot(0x29, 0x02, 0, 1, buf);
	printf("[-] Case Open Control       : 0x%02X\n", buf[0]);

	rdc_ec_read_prot(0x29, 0x10, 0, 2, buf);
	p16 = (uint16_t *)&buf[0];
	data16 = *p16;
	printf("[-] Case Open Spare         : 0x%04X\n", data16);

	if (ac >= 3)
	{
		buf[0] = 0x1;
		rdc_ec_write_prot(0x29, 0x02, 0, 1, buf);
	}
		
}
//=============================================================================
//  show_board_info
//=============================================================================
void show_board_info(void)
{
	uint8_t		buf[64];
	uint16_t	*p16, data16;
	uint32_t	data32;

	rdc_ec_read_prot(0x53, 0x00, 0, 1, buf);
	printf("[-] Board Info Module Status : 0x%02X\n", buf[0]);

	rdc_ec_read_prot(0x53, 0x10, 0, 16, buf);
	buf[16] = '\0';
	printf("[-] Board Name               : %s\n", buf);

	rdc_ec_read_prot(0x53, 0x11, 0, 16, buf);
	buf[16] = '\0';
	printf("[-] Manufacturer Name        : %s\n", buf);
	
	rdc_ec_read_prot(0x53, 0x12, 0, 12, buf);
	buf[12] = '\0';
	printf("[-] Chip Name                : %s\n", buf);

	rdc_ec_read_prot(0x53, 0x13, 0, 16, buf);
	buf[16] = '\0';
	printf("[-] Platform Type            : %s\n", buf);

	rdc_ec_read_prot(0x53, 0x14, 0, 4, (uint8_t *)&data32);
	printf("[-] Platform Version         : 0x%08lX\n", data32);

	rdc_ec_read_prot(0x53, 0x1E, 0, 4, (uint8_t *)&data32);
	printf("[-] Board ID                 : 0x%08lX\n", data32);

	rdc_ec_read_prot(0x53, 0x1F, 0, 16, buf);
	buf[16] = '\0';
	printf("[-] Board Serial Number      : %s\n", buf);

	// firmware version number
	rdc_ec_read_prot(0x53, 0x21, 0, 4, buf);
	p16 = (uint16_t *)&buf[2];
	data16 = *p16;
	printf("[-] Firmware Version Number  : %d.%d.%d\n", buf[0], buf[1], data16);

	rdc_ec_read_prot(0x53, 0x21, 0, 4, (uint8_t *)&data32);
	printf("[-] Firmware Version Number  : 0x%08lX\n", data32);
	
	// firmware version string
	rdc_ec_read_prot(0x53, 0x22, 0, 16, buf);
	buf[16] = '\0';
	printf("[-] Firmware Version String  : %s\n", buf);

	// firmware build time
	rdc_ec_read_prot(0x53, 0x23, 0, 26, buf);
	buf[26] = '\0';
	printf("[-] Firmware Build Time      : %s\n", buf);
	
	// firmware release date
	rdc_ec_read_prot(0x53, 0x24, 0, 16, buf);
	buf[16] = '\0';
	printf("[-] Firmware Release Date    : %s\n", buf);
}
//=============================================================================
//  show_system_info
//=============================================================================
void show_system_info(char *par)
{
	uint8_t		buf[64];
	uint32_t	data32;
	uint8_t		res;
	

	if (par)
	{
		if (strcmp(par, "lan10") == 0)
		{
			buf[0] = 0;
			rdc_ec_write_prot(0x54, 0x16, 0, 1, buf);	// LAN1 dis
		}
		else if (strcmp(par, "lan11") == 0)
		{
			buf[0] = 1;
			rdc_ec_write_prot(0x54, 0x16, 0, 1, buf);	// LAN1 en
		}
		else if (strcmp(par, "lan20") == 0)
		{
			buf[0] = 0;
			rdc_ec_write_prot(0x54, 0x16, 1, 1, buf);	// LAN2 dis
		}
		else if (strcmp(par, "lan21") == 0)
		{
			buf[0] = 1;
			rdc_ec_write_prot(0x54, 0x16, 1, 1, buf);	// LAN2 en
		}
	}
	else
	{

	rdc_ec_read_prot(0x55, 0x00, 0, 1, buf);
	printf("[-] System Info Module Status : 0x%02X\n", buf[0]);

	rdc_ec_read_prot(0x55, 0x10, 0, 4, (uint8_t *)&data32);
	printf("[-] System Boot Count         : 0x%08lX = %lu\n", data32, data32);

	rdc_ec_read_prot(0x55, 0x11, 0, 4, (uint8_t *)&data32);
	printf("[-] Power On Hours            : 0x%08lX = %lu\n", data32, data32);

	rdc_ec_read_prot(0x55, 0x13, 0, 1, buf);
	printf("[-] ACPI Mode                 : 0x%02X\n", buf[0]);

	rdc_ec_read_prot(0x55, 0x14, 0, 1, buf);
	printf("[-] Deep Sleep Control        : 0x%02X\n", buf[0]);

	rdc_ec_read_prot(0x55, 0x15, 0, 1, buf);
	printf("[-] AT/ATX Mode               : 0x%02X (AT=0, ATX=1)\n", buf[0]);

	rdc_ec_read_prot(0x55, 0x16, 0, 1, buf);
	printf("[-] LAN1 Enable Pin           : 0x%02X\n", buf[0]);
	rdc_ec_read_prot(0x55, 0x16, 1, 1, buf);
	printf("[-] LAN2 Enable Pin           : 0x%02X\n", buf[0]);

	res = rdc_ec_read_prot(0x55, 0x17, 0, 1, buf);
	printf("[-] COM1 Mode Select          : 0x%02X", buf[0]);
	if (res)
		printf(", timeout\n");
	else
		printf("\n");

	res = rdc_ec_read_prot(0x55, 0x17, 1, 1, buf);
	printf("[-] COM2 Mode Select          : 0x%02X", buf[0]);
	if (res)
		printf(", timeout\n");
	else
		printf("\n");
	
	res = rdc_ec_read_prot(0x55, 0x17, 2, 1, buf);
	printf("[-] COM3 Mode Select          : 0x%02X", buf[0]);
	if (res)
		printf(", timeout\n");
	else
		printf("\n");
	
	res = rdc_ec_read_prot(0x55, 0x17, 3, 1, buf);
	printf("[-] COM4 Mode Select          : 0x%02X", buf[0]);
	if (res)
		printf(", timeout\n");
	else
		printf("\n");


	rdc_ec_read_prot(0x55, 0x18, 0, 1, buf);
	printf("[-] EN_COM                    : 0x%02X\n", buf[0]);

	rdc_ec_read_prot(0x55, 0x19, 0, 1, buf);
	printf("[-] COMx Ring Status          : 0x%02X\n", buf[0]);

	rdc_ec_read_prot(0x55, 0x1A, 0, 1, buf);
	printf("[-] Wake On Ring              : 0x%02X (1=Enable, 0=Disable)\n", buf[0]);
	}
}
//=============================================================================
//  show_watchdog
//=============================================================================
void show_watchdog(void)
{
	uint8_t		buf[64];
	uint32_t	data32;

	rdc_ec_read_prot(0x2B, 0x00, 0, 1, buf);
	printf("[-] Watchdog Module Status : 0x%02X\n", buf[0]);

	rdc_ec_read_prot(0x2B, 0x02, 0, 1, buf);
	printf("[-] Watchdog Control       : 0x%02X\n", buf[0]);

	rdc_ec_read_prot(0x2B, 0x10, 0, 1, buf);
	printf("[-] Watchdog Event         : 0x%02X\n", buf[0]);

	rdc_ec_read_prot(0x2B, 0x11, 0, 4, (uint8_t *)&data32);
	printf("[-] Watchdog Delay Time    : 0x%08lX = %lu ms\n", data32, data32);

	rdc_ec_read_prot(0x2B, 0x12, 0, 4, (uint8_t *)&data32);
	printf("[-] Watchdog Power Cycle   : 0x%08lX = %lu ms\n", data32, data32);

	rdc_ec_read_prot(0x2B, 0x13, 0, 4, (uint8_t *)&data32);
	printf("[-] Watchdog IRQ           : 0x%08lX = %lu ms\n", data32, data32);

	rdc_ec_read_prot(0x2B, 0x14, 0, 4, (uint8_t *)&data32);
	printf("[-] Watchdog Reset         : 0x%08lX = %lu ms\n", data32, data32);

	rdc_ec_read_prot(0x2B, 0x15, 0, 4, (uint8_t *)&data32);
	printf("[-] Watchdog Pin           : 0x%08lX = %lu ms\n", data32, data32);

	rdc_ec_read_prot(0x2B, 0x16, 0, 4, (uint8_t *)&data32);
	printf("[-] Watchdog SCI           : 0x%08lX = %lu ms\n", data32, data32);

	rdc_ec_read_prot(0x2B, 0x17, 0, 1, buf);
	printf("[-] Watchdog IRQ Number    : 0x%02X\n", buf[0]);
}
//=============================================================================
//  show_storage
//=============================================================================
void show_storage(char *par)
{
	uint8_t	buf[64];
	int		i;

	if (par == NULL)
	{
	rdc_ec_read_prot(0x51, 0x00, 0, 1, buf);
	printf("[-] Storage Module Status : 0x%02X\n\t\t", buf[0]);
	if (buf[0] & 0x20)
		printf("PASSWD ");
	if (buf[0] & 0x10)
		printf("INV_KEY ");
	if (buf[0] & 0x08)
		printf("SPI_WE ");
	if (buf[0] & 0x04)
		printf("DEV_LOCK ");
	if (buf[0] & 0x02)
		printf("DEV_BUSY ");
	if (buf[0] & 0x01)
		printf("DEV_AVA ");
	printf("\n");
	}
	else if (strcmp(par, "pwd") == 0)
	{
		buf[0] = 0x01;
		buf[1] = 0x02;
		buf[2] = 0x03;
		buf[3] = 0x04;
		rdc_ec_write_prot(0x50, 0x03, 0, 4, buf);
				
		printf("[-] Storage : Private Area Access\n");
	}
	else if (strcmp(par, "pwderr") == 0)
	{
		buf[0] = 0x07;
		buf[1] = 0x07;
		buf[2] = 0x08;
		buf[3] = 0x08;
		rdc_ec_write_prot(0x50, 0x03, 0, 4, buf);
				
		printf("[-] Storage : Private Area Access\n");
	}
	else if (strcmp(par, "priw") == 0)
	{
		buf[0] = 0x55;
		buf[1] = 0xaa;
		buf[2] = 0x33;
		buf[3] = 0xcc;
		buf[4] = 0x11;
		buf[5] = 0xee;
		buf[6] = 0x00;
		buf[7] = 0xff;
		rdc_ec_write_prot(0x50, 0x04, 0, 8, buf);
		printf("[-] Storage : Private Write 55aa33cc11ee00ff\n");
	}
	else if (strcmp(par, "priW") == 0)
	{
		buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67;
		buf[4] = 0x89; buf[5] = 0xab; buf[6] = 0xcd; buf[7] = 0xef;
		rdc_ec_write_prot(0x50, 0x04, 0, 8, buf);
		printf("[-] Storage : Private Write 0123456789abcdef\n");
	}
	else if (strcmp(par, "prir") == 0)
	{
		rdc_ec_write_prot(0x51, 0x04, 0, 8, buf);
		printf("[-] Storage : Private Read => ");
		for (i=0; i<8; i++)
			printf("%02X ", buf[i]);
		printf("\n");
	}
	else if (strcmp(par, "genw") == 0)
	{
		buf[0] = 0x55;
		buf[1] = 0xaa;
		buf[2] = 0x33;
		buf[3] = 0xcc;
		buf[4] = 0x11;
		buf[5] = 0xee;
		buf[6] = 0x00;
		buf[7] = 0xff;
		rdc_ec_write_prot(0x50, 0x05, 0, 8, buf);
		printf("[-] Storage : General Write 55aa33cc11ee00ff\n");
	}
	else if (strcmp(par, "genW") == 0)
	{
		buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67;
		buf[4] = 0x89; buf[5] = 0xab; buf[6] = 0xcd; buf[7] = 0xef;
		rdc_ec_write_prot(0x50, 0x05, 0, 8, buf);
		printf("[-] Storage : General Write 0123456789abcdef\n");
	}
	else if (strcmp(par, "genr") == 0)
	{
		rdc_ec_write_prot(0x51, 0x05, 0, 8, buf);
		printf("[-] Storage : General Read => ");
		for (i=0; i<8; i++)
			printf("%02X ", buf[i]);
		printf("\n");
	}
	else
	{
		printf("pwd | pwderr | priw | priW | prir | genw | genW | genr\n");
	}
}
//=============================================================================
//  show_acpi_ec_ram
//=============================================================================
void show_acpi_ec_ram(void)
{
	uint8_t		acpi_ram[256];
	uint8_t		res;
	uint16_t	i, j;

	for (i=0; i<256; i++)
	{
		res = acpi_wait_ibe();
		if (res)
			break;
		outp(ACPI_EC_CMD, 0x80);	// EC read
	
		res = acpi_wait_ibe();
		if (res)
			break;
		outp(ACPI_EC_DAT, (uint8_t)(i & 0xFF));
		
		res = acpi_wait_obf();
		if (res)
			break;

		acpi_ram[i] = inp(ACPI_EC_DAT);
	}

	printf("ACPI RAM :\n     ");
	for (j=0; j<16; j++)
	{
		printf("%02X ", j);
		if (j==7)
			printf("  ");
	}
	printf("\n");
	for (j=0; j<55; j++)
		printf("-");
	printf("\n");

	for (j=0; j<256; j++)
	{
		if (j%16==0)
			printf("[%02X] ", j);
	
		printf("%02X ", acpi_ram[j]);
		
		if (j%16==15)
		{
			for (i=j-15; i<=j; i++)
			{
				if (acpi_ram[i]>=0x20 && acpi_ram[i]<=0x7E)
					printf("%c", acpi_ram[i]);
				else
					printf(".");
			}
			printf("\n");
		}
		else if (j%16==7)
		{
			printf("- ");
		}
	}
}

//=============================================================================
//  show_ec_ram
//=============================================================================
void show_ec_ram(char *par)
{
	uint8_t		ec_ram[256];	// = acpi_ram
	uint16_t	i, j;
	uint8_t		res=0;
	uint8_t		wdata[16] =
	{ 0x55,0xaa,0x33,0xcc,0x11,0xee,0x00,0xff,
	  0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef};
	uint8_t		wdata2[16] =
	{ 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0 };

	//rdc_fw_prot_t	prot;

	if (strcmp(par, "wdata") == 0)
	{
		res = rdc_ec_write_prot(0x30, 0xc0, 0, 16, wdata);
		if  (res != AEC_OK)
			printf("EC_RAM write error!\n");
	}
	else if (strcmp(par, "Wdata") == 0)
	{
		res = rdc_ec_write_prot(0x30, 0xc0, 0, 16, wdata2);
		if  (res != AEC_OK)
			printf("EC_RAM write error!\n");
	}

	for (j=0; j<256; j+=128)
	{
		res = rdc_ec_read_prot(0x31, (uint8_t)j, 0, 128, &ec_ram[j]);
		if  (res != AEC_OK)
			break;
	}

	if (res != AEC_OK)
	{
		printf( "show_ec_ram: error!\n");
	}
	else
	{
		printf("EC RAM :\n     ");
		for (j=0; j<16; j++)
		{
			printf("%02X ", j);
			if (j==7)
				printf("  ");
		}
		printf("\n");
		for (j=0; j<55; j++)
			printf("-");
		printf("\n");

		for (j=0; j<256; j++)
		{
			if (j%16==0)
				printf("[%02X] ", j);
	
			printf("%02X ", ec_ram[j]);
		
			if (j%16==15)
			{
				for (i=j-15; i<=j; i++)
				{
					if (ec_ram[i]>=0x20 && ec_ram[i]<=0x7E)
						printf("%c", ec_ram[i]);
					else
						printf(".");
				}
				printf("\n");
			}
			else if (j%16==7)
			{
				printf("- ");
			}
		}
	}
}

//=============================================================================
//  set_thermal
//=============================================================================
void set_thermal(char *low_av, char *high_av)
{
	uint16_t	lt;
	uint16_t	ht;
	uint8_t		buf[4];
	uint8_t		res;
	
	//rdc_fw_prot_t	prot;

	lt = (uint16_t)atoi(low_av);
	// c to 0.1k
	lt *= 10;
	lt += 2731;
	
	ht = (uint16_t)atoi(high_av);
	// c to 0.1k
	ht *= 10;
	ht += 2731;

	printf( "low=%d, hi=%d\n", lt, ht);
	
	buf[0] = (uint8_t)(lt & 0xFF);
	buf[1] = (uint8_t)((lt>>8) & 0xFF);

	res = rdc_ec_write_prot(0x10, 0x22, 0, 2, buf);
	show_prot_res(res);

	buf[0] = (uint8_t)(ht & 0xFF);
	buf[1] = (uint8_t)((ht>>8) & 0xFF);

	res = rdc_ec_write_prot(0x10, 0x21, 0, 2, buf);
	show_prot_res(res);
}

//=============================================================================
//  show_pmc_status
//=============================================================================
void show_pmc_status(uint8_t sts)
{
	switch(sts)
	{
		case 0:	printf("OK: completed!\n");				break;
		case 1: printf("ERR: command code error!\n");	break;
		case 2: printf("ERR: control code error!\n");	break;
		case 3: printf("ERR: device id error!\n");		break;
		case 4: printf("ERR: data length error!\n");	break;
		case 5: printf("ERR: invalid data!\n");			break;
		case 6: printf("ERR: protocol error!\n");		break;
		case 7: printf("ERR: read only!\n");			break;
		case 8: printf("ERR: write only!\n");			break;
		default:
			printf("ERR: unknown error!\n");
			break;
	}
}

//=============================================================================
//  show_prot
//=============================================================================
int show_prot(uint8_t cmd, uint8_t ctl, uint8_t dev, uint8_t len, uint8_t *dat)
{
	//uint8_t		buf[256];
	int			i;
	//uint8_t		sts[4];
	uint8_t		res;
	
	rdc_fw_prot_t	prot;
	rdc_fw_prot_t	prot_pmc;
	uint8_t			pmc, res_pmc;
	
	if (cmd < 0x2)
	{
		// PMC status
		printf( "R : cmd = %02x\n", cmd);

		prot.cmd = cmd;
		prot.dat = dat;
		res = rdc_fw_prot_pmc(&prot);
		if (res == AEC_OK)
			printf( "    dat = %02x\n", dat[0]);
		else
			printf( "    dat = err!\n");
		
	}
	else
	{
		// read command
		if (cmd & 0x1)
		{
			printf( "R : cmd = %02x %02x %02x %02x\n", cmd, ctl, dev, len);
			//memset(buf, 0, sizeof(buf));

			res = rdc_ec_read_prot(cmd, ctl, dev, len, dat);

			prot_pmc.cmd = 0x01;
			prot_pmc.dat = &pmc;
			res_pmc = rdc_fw_prot_pmc(&prot_pmc);
			
			if (res == AEC_OK && res_pmc == AEC_OK && pmc == AEC_PMC_OK)
			{
				printf( "    dat = ");
				for (i=0; i<len; i++)
					printf( "%02x ", dat[i]);
				printf( "\n    dat = ");
			
				for (i=0; i<len; i++)
				{
					if (dat[i]>0x7e || dat[i]<0x20)
						printf( ".");
					else
						printf( "%c", dat[i]);
				}
				
				if (len == 4)
				{
					printf( "\n    dat = %u", *(uint32_t *)dat);
				}
				else if (len == 2)
				{
					printf( "\n    dat = %u", *(uint16_t *)dat);
				}
				else if (len == 1)
				{
					printf( "\n    dat = %u", *(uint8_t *)dat);
				}
				printf( "\n");
				
			}
			else
			{
				show_prot_res(res);	
			}
			printf( "\n");
		}
		// write command
		else
		{
			printf( "W : cmd = %02x %02x %02x %02x\n", cmd, ctl, dev, len);
			printf( "    dat = ");
			for (i=0; i<len; i++)
				printf( "%02x ", dat[i]);
			printf( "\n");

			res = rdc_ec_write_prot(cmd, ctl, dev, len, dat);
			show_prot_res(res);
		}
	}

	return 0;
}

//=============================================================================
//  ShowSWProtocol
//=============================================================================
int ShowSWProtocol(uint8_t cmd, uint8_t index, uint8_t offset, uint8_t len, uint8_t *buf)
{
	rdc_sw_prot_t	prot;
	uint8_t res;

	prot.cmd = cmd;
	prot.idx = index;
	prot.off = offset;
	prot.len = len;
	prot.dat = buf;

	printf( "R : cmd = %02x\n", cmd);
	
	if(index == 0xFF)
	{
		res = rdc_sw_prot_noidx_read(&prot);
	}
	else
	{
		res = rdc_sw_prot_idx_read(&prot);
	}

	if (res == AEC_OK)
		printf( "    dat = %02x\n", buf[0]);
	else
		printf( "    dat = err!\n");

	return 0;
}

//=============================================================================
//  main
//=============================================================================
#if 0
int main(int ac, char **av)
{
	uint16_t	cmd=0, ctl=0, dev=0, len=0;
	uint8_t		dat[256];
	int			i;
	uint8_t		res;

	if (ac < 2)
	{
		printf("rdctest - %s %s\n", __DATE__, __TIME__);
		printf("rdctest <arg>\n");
		printf("\t<arg> = pmc   : pmc status/config\n");
		printf("\t<arg> = therm : thermal\n");
		printf("\t<arg> = volt  : voltage\n");
		printf("\t<arg> = curr  : current\n");
		printf("\t<arg> = pwm   : pwm\n");
		printf("\t<arg> = tach  : tachometer\n");
		printf("\t<arg> = gpio  : gpio\n");
		printf("\t<arg> = panel : panel\n");
		printf("\t<arg> = fan   : fan\n");
		printf("\t<arg> = case  : case open\n");
		printf("\t<arg> = brd   : board info\n");

		printf("\t<arg> = sys   : system info\n");
		printf("\t\tsys [lan10|lan11|lan20|lan21]\n");

		printf("\t<arg> = wdt   : watchdog\n");
		printf("\t<arg> = sto   : storage\n");
		printf("\t\tsto [pwd|pwderr|priw|priW|prir|genw|genW|genr]\n");
		printf("\t<arg> = acpi  : ACPI EC RAM\n");

		printf("\t<arg> = ecram : EC RAM = ACPI EC RAM\n");
		printf("\t\tecram [wdata|Wdata]\n");
	
		printf("\t<arg> = prot  : protocol\n");
		return -1;
	}

	res = aec_init();
	if (res != AEC_OK)
	{
		printf( "aec_init err!\n");
		return -1;
	}

	if (strcmp(av[1], "pmc") == 0)			show_pmc();
	else if (strcmp(av[1], "therm") == 0)
	{
		if (ac == 2)
			show_thermal();
		else if (ac == 4)
			set_thermal(av[2], av[3]);
	}
	else if (strcmp(av[1], "volt") == 0)	show_voltage();
	else if (strcmp(av[1], "curr") == 0)	show_current();
	else if (strcmp(av[1], "pwm") == 0)		show_pwm();
	else if (strcmp(av[1], "tach") == 0)	show_tachometer();
	else if (strcmp(av[1], "gpio") == 0)	show_gpio();
	else if (strcmp(av[1], "panel") == 0)	show_panel();
	else if (strcmp(av[1], "fan") == 0)		show_fan();
	else if (strcmp(av[1], "case") == 0)
	{
		show_case_open(ac);
	}
	else if (strcmp(av[1], "brd") == 0)		show_board_info();
	else if (strcmp(av[1], "sys") == 0)
	{
		if (ac == 2)
			show_system_info(NULL);
		else if (ac == 3)
			show_system_info(av[2]);
	}
	else if (strcmp(av[1], "wdt") == 0)		show_watchdog();
	else if (strcmp(av[1], "sto") == 0)
	{
		if (ac == 2)
			show_storage(NULL);
		else if (ac == 3)
			show_storage(av[2]);
	}
	else if (strcmp(av[1], "acpi") == 0)	show_acpi_ec_ram();
	else if (strcmp(av[1], "ecram") == 0)
	{
		if (ac == 2)
			show_ec_ram(NULL);
		else if (ac == 3)
			show_ec_ram(av[2]);
	}
	else if (strcmp(av[1], "prot") == 0)
	{
		if (ac == 3)
		{
			sscanf(av[2], "%X", &cmd);
			show_prot((uint8_t)cmd, (uint8_t)ctl, (uint8_t)dev, (uint8_t)len, dat);
		}
		else if (ac >= 6)
		{
			sscanf(av[2], "%X", &cmd);
			sscanf(av[3], "%X", &ctl);
			sscanf(av[4], "%X", &dev);
			sscanf(av[5], "%X", &len);
			for (i=0; i<(ac-5); i++)
				sscanf(av[i+6], "%X", &dat[i]);
		
			show_prot((uint8_t)cmd, (uint8_t)ctl, (uint8_t)dev, (uint8_t)len, dat);
		}
	}

	aec_exit();

	return 0;
}
#endif
