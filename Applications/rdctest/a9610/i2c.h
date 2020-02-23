#ifndef __A9610_I2C_H
#define __A9610_I2C_H

//=============================================================================
//  5.3.6  I2C Bus Controller 0/1
//=============================================================================
//  Host Domain
//    . PCIe : device 6, function 0/1, configuration 10h
//    . LPC  : LDN 20h/21h, index 60h~61h
//  EC Domain
//    . I2C0 Base : 0xF800
//    . I2C1 Base : 0xF810
//=============================================================================
#define I2C0_BASE                        0xF800
#define I2C1_BASE                        0xF810

//=============================================================================
//  5.3.6.3.1  I2C_CTL : I2C 0/1 Control Reg.
//=============================================================================
#define I2C0_CTL_REG                     (I2C0_BASE + 0x00)
#define I2C1_CTL_REG                     (I2C1_BASE + 0x00)
#define I2C_CTL_REG(ch)                  (I2C0_CTL_REG + ch*0x10)
#define   I2C_CTL_SWINTEN_bm             0x80
#define   I2C_CTL_SWINTEN_bp             7
#define   I2C_CTL_RXINTEN_bm             0x40
#define   I2C_CTL_RXINTEN_bp             6
#define   I2C_CTL_TXINTEN_bm             0x20
#define   I2C_CTL_TXINTEN_bp             5
#define   I2C_CTL_NAKINTEN_bm            0x10
#define   I2C_CTL_NAKINTEN_bp            4
#define   I2C_CTL_ARLINTEN_bm            0x8
#define   I2C_CTL_ARLINTEN_bp            3
#define   I2C_CTL_STPINTEN_bm            0x4
#define   I2C_CTL_STPINTEN_bp            2
#define   I2C_CTL_STOP_bm                0x2
#define   I2C_CTL_STOP_bp                1
#define   I2C_CTL_NAKEN_bm               0x1
#define   I2C_CTL_NAKEN_bp               0

//=============================================================================
//  5.3.6.3.2  I2C_STS : I2C 0/1 Status Reg.
//=============================================================================
#define I2C0_STS_REG                     (I2C0_BASE + 0x01)
#define I2C1_STS_REG                     (I2C1_BASE + 0x01)
#define I2C_STS_REG(ch)                  (I2C0_STS_REG + ch*0x10)
#define   I2C_STS_SWREQ_bm               0x80
#define   I2C_STS_SWREQ_bp               7
#define   I2C_STS_RXRDY_bm               0x40
#define   I2C_STS_RXRDY_bp               6
#define   I2C_STS_TXDONE_bm              0x20
#define   I2C_STS_TXDONE_bp              5
#define   I2C_STS_NAKERR_bm              0x10
#define   I2C_STS_NAKERR_bp              4
#define   I2C_STS_ARL_bm                 0x8
#define   I2C_STS_ARL_bp                 3
#define   I2C_STS_SLAVESTP_bm            0x4
#define   I2C_STS_SLAVESTP_bp            2
#define   I2C_STS_BBUSY_bm               0x2
#define   I2C_STS_BBUSY_bp               1
#define   I2C_STS_MS_bm                  0x1
#define   I2C_STS_MS_bp                  0

//=============================================================================
//  5.3.6.3.3  I2C_MYADD : I2C 0/1 My Address Reg.
//=============================================================================
#define I2C0_MYADD_REG                   (I2C0_BASE + 0x02)
#define I2C1_MYADD_REG                   (I2C1_BASE + 0x02)
#define I2C_MYADD_REG(ch)                (I2C0_MYADD_REG + ch*0x10)
#define   I2C_MYADD_MYADDR_gm            0xFE
#define   I2C_MYADD_MYADDR_gp            1
#define   I2C_MYADD_TCE_bm               0x1
#define   I2C_MYADD_TCE_bp               0

//=============================================================================
//  5.3.6.3.4  I2C_TXADD : I2C 0/1 Transmit Address Reg.
//=============================================================================
#define I2C0_TXADD_REG                   (I2C0_BASE + 0x03)
#define I2C1_TXADD_REG                   (I2C1_BASE + 0x03)
#define I2C_TXADD_REG(ch)                (I2C0_TXADD_REG + ch*0x10)
#define   I2C_TXADD_TXADDR_gm            0xFF
#define   I2C_TXADD_TXADDR_gp            0

//=============================================================================
// 5.3.6.3.5  I2C_DAT : I2C 0/1 Transmit/Receive Data Reg.
//=============================================================================
#define I2C0_DAT_REG                     (I2C0_BASE + 0x04)
#define I2C1_DAT_REG                     (I2C1_BASE + 0x04)
#define I2C_DAT_REG(ch)                  (I2C0_DAT_REG + ch*0x10)
#define   I2C_DAT_DATA_gm                0xFF
#define   I2C_DAT_DATA_gp                0

//=============================================================================
//  5.3.6.3.6  I2C_CLK1 : I2C 0/1 I2C Clock Frequency Control 1 Reg.
//=============================================================================
#define I2C0_CLK1_REG                    (I2C0_BASE + 0x05)
#define I2C1_CLK1_REG                    (I2C1_BASE + 0x05)
#define I2C_CLK1_REG(ch)                 (I2C0_CLK1_REG + ch*0x10)
#define   I2C_CLK1_PRESCALE1_gm          0xFF
#define   I2C_CLK1_PRESCALE1_gp          0

//=============================================================================
//  5.3.6.3.7  I2C_CLK2 : I2C 0/1 I2C Clock Frequency Control 2 Reg.
//=============================================================================
#define I2C0_CLK2_REG                    (I2C0_BASE + 0x06)
#define I2C1_CLK2_REG                    (I2C1_BASE + 0x06)
#define I2C_CLK2_REG(ch)                 (I2C0_CLK2_REG + ch*0x10)
#define   I2C_CLK2_FAST_bm               0x80
#define   I2C_CLK2_FAST_bp               7
#define   I2C_CLK2_PRESCALE2_gm          0x7F
#define   I2C_CLK2_PRESCALE2_gp          0

//=============================================================================
//  5.3.6.3.8  I2C_EXCTL : I2C 0/1 Extra Control Reg.
//=============================================================================
#define I2C0_EXCTL_REG                   (I2C0_BASE + 0x07)
#define I2C1_EXCTL_REG                   (I2C1_BASE + 0x07)
#define I2C_EXCTL_REG(ch)				 (I2C0_EXCTL_REG + ch*0x10)
#define   I2C_EXCTL_I2CRST_bm            0x80
#define   I2C_EXCTL_I2CRST_bp            7
#define   I2C_EXCTL_LATCHTIME_bm         0x40
#define   I2C_EXCTL_LATCHTIME_bp         6
#define   I2C_EXCTL_NOFILTER_bm          0x20
#define   I2C_EXCTL_NOFILTER_bp          5
#define   I2C_EXCTL_NODRIVE_bm           0x10
#define   I2C_EXCTL_NODRIVE_bp           4
#define   I2C_EXCTL_DIMC_bm              0x8
#define   I2C_EXCTL_DIMC_bp              3
#define   I2C_EXCTL_DI196_bm             0x4
#define   I2C_EXCTL_DI196_bp             2
#define   I2C_EXCTL_DIAR_bm              0x2
#define   I2C_EXCTL_DIAR_bp              1
#define   I2C_EXCTL_DIDC_bm              0x1
#define   I2C_EXCTL_DIDC_bp              0

//=============================================================================
//  5.3.6.3.9  I2C_SEM : I2C 0/1 Semaphore Reg.
//=============================================================================
#define I2C0_SEM_REG                     (I2C0_BASE + 0x08)
#define I2C1_SEM_REG                     (I2C1_BASE + 0x08)
#define I2C_SEM_REG(ch)                  (I2C0_SEM_REG + ch*0x10)
#define   I2C_SEM_RSVD_gm                0xFE
#define   I2C_SEM_RSVD_gp                1
#define   I2C_SEM_INUSE_bm               0x1
#define   I2C_SEM_INUSE_bp               0

//-----------------------------------------------------------------------------
//  I2C
//-----------------------------------------------------------------------------
#define I2C_CH_NUM		(2)

#define I2C_CH_0        0
#define I2C_CH_1        1
#define I2C_CH_MAX      (I2C_CH_NUM - 1)       

#endif
