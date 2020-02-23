#ifndef __A9610_SMB_H
#define __A9610_SMB_H

//=============================================================================
//  5.3.7  SMBus Controller 0/1
//=============================================================================
//  Host Domain
//    . PCIe : device 6, function 2/3, configuration 10h
//    . LPC  : LDN 22h/23h, index 60h~61h
//  EC Domain
//    . SMB0 Base : 0xF880
//    . SMB1 Base : 0xF8A0
//=============================================================================
#define SMB0_BASE                        0xF880
#define SMB1_BASE                        0xF8A0

//=============================================================================
//  5.3.7.2.1  SMB_STS : SMBus 0/1 Host Status Reg.
//=============================================================================
#define SMB0_STS_REG                     (SMB0_BASE + 0x00)
#define SMB1_STS_REG                     (SMB1_BASE + 0x00)
#define SMB_STS_REG(ch)                  (SMB0_STS_REG + ch*0x20)
#define   SMB_STS_TXDONE_bm              0x80
#define   SMB_STS_TXDONE_bp              7
#define   SMB_STS_INUSE_bm               0x40
#define   SMB_STS_INUSE_bp               6
#define   SMB_STS_RXRDY_bm               0x20
#define   SMB_STS_RXRDY_bp               5
#define   SMB_STS_FAILED_bm              0x10
#define   SMB_STS_FAILED_bp              4
#define   SMB_STS_ARLERR_bm              0x8
#define   SMB_STS_ARLERR_bp              3
#define   SMB_STS_RSVD_bm                0x4
#define   SMB_STS_RSVD_bp                2
#define   SMB_STS_FINISH_bm              0x2
#define   SMB_STS_FINISH_bp              1
#define   SMB_STS_HOSTBUSY_bm            0x1
#define   SMB_STS_HOSTBUSY_bp            0

//=============================================================================
//  5.3.7.2.2  SMB_STS2 : SMBus 0/1 Host Status 2 Reg.
//=============================================================================
#define SMB0_STS2_REG                    (SMB0_BASE + 0x01)
#define SMB1_STS2_REG                    (SMB1_BASE + 0x01)
#define SMB_STS2_REG(ch)                 (SMB0_STS2_REG + ch*0x20)
#define   SMB_STS2_SLMODE_bm             0x80
#define   SMB_STS2_SLMODE_bp             7
#define   SMB_STS2_SLAVETXREQ_bm         0x40
#define   SMB_STS2_SLAVETXREQ_bp         6
#define   SMB_STS2_SSTOPSTS_bm           0x20
#define   SMB_STS2_SSTOPSTS_bp           5
#define   SMB_STS2_TOERR_bm              0x10
#define   SMB_STS2_TOERR_bp              4
#define   SMB_STS2_SMBALERTSTS_bm        0x8
#define   SMB_STS2_SMBALERTSTS_bp        3
#define   SMB_STS2_NACKERR_bm            0x4
#define   SMB_STS2_NACKERR_bp            2
#define   SMB_STS2_PECERR_bm             0x1
#define   SMB_STS2_PECERR_bp             1
#define   SMB_STS2_HOSTNOTIFYSTS_bm      0x1
#define   SMB_STS2_HOSTNOTIFYSTS_bp      0

//=============================================================================
//  5.3.7.2.3  SMB_CTL : SMBus 0/1 Host Control Reg.
//=============================================================================
#define SMB0_CTL_REG                     (SMB0_BASE + 0x02)
#define SMB1_CTL_REG                     (SMB1_BASE + 0x02)
#define SMB_CTL_REG(ch)                  (SMB0_CTL_REG + ch*0x20)
#define   SMB_CTL_PECEN_bm               0x80
#define   SMB_CTL_PECEN_bp               7
#define   SMB_CTL_START_bm               0x40
#define   SMB_CTL_START_bp               6
#define   SMB_CTL_LASTBYTE_bm            0x20
#define   SMB_CTL_LASTBYTE_bp            5
#define   SMB_CTL_SMBCMD_gm              0x1C
#define   SMB_CTL_SMBCMD_gp              2
#define   SMB_CTL_KILL_bm                0x2
#define   SMB_CTL_KILL_bp                1
#define   SMB_CTL_I2CNACKEN_bm           0x1
#define   SMB_CTL_I2CNACKEN_bp           0

//=============================================================================
//  5.3.7.2.4  SMB_CMD : SMBus 0/1 Host Command Reg.
//=============================================================================
#define SMB0_CMD_REG                     (SMB0_BASE + 0x03)
#define SMB1_CMD_REG                     (SMB1_BASE + 0x03)
#define SMB_CMD_REG(ch)                  (SMB0_CMD_REG + ch*0x20)
#define   SMB_CMD_HCMD_gm                0xFF
#define   SMB_CMD_HCMD_bp                0

//=============================================================================
//  5.3.7.2.5  SMB_SADD : SMBus 0/1 Transmit Slave Address Reg.
//=============================================================================
#define SMB0_SADD_REG                    (SMB0_BASE + 0x04)
#define SMB1_SADD_REG                    (SMB1_BASE + 0x04)
#define SMB_SADD_REG(ch)                 (SMB0_SADD_REG + ch*0x20)
#define   SMB_SADD_ADDRESS_gm            0xFE
#define   SMB_SADD_ADDRESS_gp            1
#define   SMB_SADD_RW_bm                 0x1
#define   SMB_SADD_RW_bp                 0

//=============================================================================
//  5.3.7.2.6  SMB_DAT0 : SMBus 0/1 Host Data0 Reg.
//=============================================================================
#define SMB0_DAT0_REG                    (SMB0_BASE + 0x05)
#define SMB1_DAT0_REG                    (SMB1_BASE + 0x05)
#define SMB_DAT0_REG(ch)                 (SMB0_DAT0_REG + ch*0x20)
#define   SMB_DAT0_DATA0_gm              0xFF
#define   SMB_DAT0_DATA0_gp              0

//=============================================================================
//  5.3.7.2.7  SMB_DAT1 : SMBus 0/1 Host Data1 Reg.
//=============================================================================
#define SMB0_DAT1_REG                    (SMB0_BASE + 0x06)
#define SMB1_DAT1_REG                    (SMB1_BASE + 0x06)
#define SMB_DAT1_REG(ch)                 (SMB0_DAT1_REG + ch*0x20)
#define   SMB_DAT1_DATA1_gm              0xFF
#define   SMB_DAT1_DATA1_gp              0

//=============================================================================
//  5.3.7.2.8  SMB_BLK : SMBus 0/1 Host Block Data Byte Reg.
//=============================================================================
#define SMB0_BLK_REG                     (SMB0_BASE + 0x07)
#define SMB1_BLK_REG                     (SMB1_BASE + 0x07)
#define SMB_BLK_REG(ch)                  (SMB0_BLK_REG + ch*0x20)
#define   SMB_BLK_BDTA_gm                0xFF
#define   SMB_BLK_BDTA_gp                0

//=============================================================================
//  5.3.7.2.9  SMB_PEC : SMBus 0/1 Packet Error Check Reg.
//=============================================================================
#define SMB0_PEC_REG                     (SMB0_BASE + 0x08)
#define SMB1_PEC_REG                     (SMB1_BASE + 0x08)
#define SMB_PEC_REG(ch)                  (SMB0_PEC_REG + ch*0x20)
#define   SMB_PEC_PECDATA_gm             0xFF
#define   SMB_PEC_PECDATA_gp             0

//=============================================================================
//  5.3.7.2.10  SMB_RSADD : SMBus 0/1 Receive Slave Address Reg.
//=============================================================================
#define SMB0_RSADD_REG                   (SMB0_BASE + 0x09)
#define SMB1_RSADD_REG                   (SMB1_BASE + 0x09)
#define SMB_RSADD_REG(ch)                (SMB0_RSADD_REG + ch*0x20)
#define   SMB_RSADD_SLAVEADDR_gm         0xFE
#define   SMB_RSADD_SLAVEADDR_gp         1
#define   SMB_RSADD_RSVD_bm              0x1
#define   SMB_RSADD_RSVD_bp              0

//=============================================================================
//  5.3.7.2.11  SMB_RSDAT0 : SMBus 0/1 Receive Slave Data 0 Reg.
//=============================================================================
#define SMB0_RSDAT0_REG                  (SMB0_BASE + 0x0A)
#define SMB1_RSDAT0_REG                  (SMB1_BASE + 0x0A)
#define SMB_RSDAT0_REG(ch)               (SMB0_RSDAT0_REG + ch*0x20)
#define   SMB_RSDAT0_RCVSLDATA0_gm       0xFF
#define   SMB_RSDAT0_RCVSLDATA0_gp       0

//=============================================================================
//  5.3.7.2.12  SMB_RSDAT1 : SMBus 0/1 Receive Slave Data 1 Reg.
//=============================================================================
#define SMB0_RSDAT1_REG                  (SMB0_BASE + 0x0B)
#define SMB1_RSDAT1_REG                  (SMB1_BASE + 0x0B)
#define SMB_RSDAT1_REG(ch)               (SMB0_RSDAT1_REG + ch*0x20)
#define   SMB_RSDAT1_RCVSLDATA1_gm       0xFF
#define   SMB_RSDAT1_RCVSLDATA1_gp       0

//=============================================================================
//  5.3.7.2.13  SMB_CTL2 : SMBus 0/1 Host Control 2 Reg.
//=============================================================================
#define SMB0_CTL2_REG                    (SMB0_BASE + 0x0C)
#define SMB1_CTL2_REG                    (SMB1_BASE + 0x0C)
#define SMB_CTL2_REG(ch)                 (SMB0_CTL2_REG + ch*0x20)
#define   SMB_CTL2_SSRESET_bm            0x80
#define   SMB_CTL2_SSRESET_bp            7
#define   SMB_CTL2_RSVD_gm               0x70
#define   SMB_CTL2_RSVD_gp               4
#define   SMB_CTL2_E32B_bm               0x8
#define   SMB_CTL2_E32B_bp               3
#define   SMB_CTL2_AAPEC_bm              0x4
#define   SMB_CTL2_AAPEC_bp              2
#define   SMB_CTL2_I2CEN_bm              0x2
#define   SMB_CTL2_I2CEN_bp              1
#define   SMB_CTL2_HOSTNOTIFYDIS_bm      0x1
#define   SMB_CTL2_HOSTNOTIFYDIS_bp      0

//=============================================================================
//  5.3.7.2.14  SMB_PINCTL : SMBus 0/1 Pin Control  Reg.
//=============================================================================
#define SMB0_PINCTL_REG                  (SMB0_BASE + 0x0D)
#define SMB1_PINCTL_REG                  (SMB1_BASE + 0x0D)
#define SMB_PINCTL_REG(ch)               (SMB0_PINCTL_REG + ch*0x20)
#define   SMB_PINCTL_RSVD_gm             0xF8
#define   SMB_PINCTL_RSVD_gp             3
#define   SMB_PINCTL_SMBCLKCTL_bm        0x4
#define   SMB_PINCTL_SMBCLKCTL_bp        2
#define   SMB_PINCTL_SMBDATACURSTS_bm    0x2
#define   SMB_PINCTL_SMBDATACURSTS_bp    1
#define   SMB_PINCTL_SMBCLKCURSTS_bm     0x1
#define   SMB_PINCTL_SMBCLKCURSTS_bp     0

//=============================================================================
//  5.3.7.2.15  SMB_CTL3 : SMBus 0/1 Host Control 3 Reg.
//=============================================================================
#define SMB0_CTL3_REG                    (SMB0_BASE + 0x0E)
#define SMB1_CTL3_REG                    (SMB1_BASE + 0x0E)
#define SMB_CTL3_REG(ch)                 (SMB0_CTL3_REG + ch*0x20)
#define   SMB_CTL3_TXINTEN_bm            0x80
#define   SMB_CTL3_TXINTEN_bp            7
#define   SMB_CTL3_RSVD_bm               0x40
#define   SMB_CTL3_RSVD_bp               6
#define   SMB_CTL3_RXINTEN_bm            0x20
#define   SMB_CTL3_RXINTEN_bp            5
#define   SMB_CTL3_FAILINTEN_bm          0x10
#define   SMB_CTL3_FAILINTEN_bp          4
#define   SMB_CTL3_ARLINTEN_bm           0x8
#define   SMB_CTL3_ARLINTEN_bp           3
#define   SMB_CTL3_RSVD2_bm              0x4
#define   SMB_CTL3_RSVD2_bp              2
#define   SMB_CTL3_FINISHINTEN_bm        0x2
#define   SMB_CTL3_FINISHINTEN_bp        1
#define   SMB_CTL3_RSVD3_bm              0x1
#define   SMB_CTL3_RSVD3_bp              0

//=============================================================================
//  5.3.7.2.16  SMB_CTL4 : SMBus 0/1 Host Control 4 Reg.
//=============================================================================
#define SMB0_CTL4_REG                    (SMB0_BASE + 0x0F)
#define SMB1_CTL4_REG                    (SMB1_BASE + 0x0F)
#define SMB_CTL4_REG(ch)                 (SMB0_CTL4_REG + ch*0x20)
#define   SMB_CTL4_HOSTNOTIFYWKEN_bm     0x80
#define   SMB_CTL4_HOSTNOTIFYWKEN_bp     7
#define   SMB_CTL4_SLAVEREQINTEN_bm      0x40
#define   SMB_CTL4_SLAVEREQINTEN_bp      6
#define   SMB_CTL4_SLSTOPINTEN_bm        0x20
#define   SMB_CTL4_SLSTOPINTEN_bp        5
#define   SMB_CTL4_TOINTEN_bm            0x10
#define   SMB_CTL4_TOINTEN_bp            4
#define   SMB_CTL4_SMBALERTINTEN_bm      0x8
#define   SMB_CTL4_SMBALERTINTEN_bp      3
#define   SMB_CTL4_NACKINTEN_bm          0x4
#define   SMB_CTL4_NACKINTEN_bp          2
#define   SMB_CTL4_PECERRINTEN_bm        0x2
#define   SMB_CTL4_PECERRINTEN_bp        1
#define   SMB_CTL4_HOSTNOTIFYINTEN_bm    0x1
#define   SMB_CTL4_HOSTNOTIFYINTEN_bp    0

//=============================================================================
//  5.3.7.2.17  SMB_NDADD : SMBus 0/1 Notify Device Address Reg.
//=============================================================================
#define SMB0_NDADD_REG                   (SMB0_BASE + 0x10)
#define SMB1_NDADD_REG                   (SMB1_BASE + 0x10)
#define SMB_NDADD_REG(ch)                (SMB0_NDADD_REG + ch*0x20)
#define   SMB_NDADD_DEVICEADDRESS_gm     0xFE
#define   SMB_NDADD_DEVICEADDRESS_gp     1
#define   SMB_NDADD_RSVD_bm              0x1
#define   SMB_NDADD_RSVD_bp              0

//=============================================================================
//  5.3.7.2.18  SMB_NDLSB : SMBus 0/1 Notify Data Low Byte Reg.
//=============================================================================
#define SMB0_NDLSB_REG                   (SMB0_BASE + 0x11)
#define SMB1_NDLSB_REG                   (SMB1_BASE + 0x11)
#define SMB_NDLSB_REG(ch)                (SMB0_NDLSB_REG + ch*0x20)
#define   SMB_NDLSB_DATALOWBYTE_gm       0xFF
#define   SMB_NDLSB_DATALOWBYTE_gp       0

//=============================================================================
//  5.3.7.2.19  SMB_NDMSB : SMBus 0/1 Notify Data High Byte Reg.
//=============================================================================
#define SMB0_NDMSB_REG                   (SMB0_BASE + 0x12)
#define SMB1_NDMSB_REG                   (SMB1_BASE + 0x12)
#define SMB_NDMSB_REG(ch)                (SMB0_NDMSB_REG + ch*0x20)
#define   SMB_NDMSB_DATAHIGHBYTE_gm      0xFF
#define   SMB_NDMSB_DATAHIGHBYTE_gp      0

//=============================================================================
//  5.3.7.2.20  SMB_CLKCTL1 : SMBus 0/1 SMBCLK Clock Frequency Control 1 Reg.
//=============================================================================
#define SMB0_CLKCTL1_REG                 (SMB0_BASE + 0x13)
#define SMB1_CLKCTL1_REG                 (SMB1_BASE + 0x13)
#define SMB_CLKCTL1_REG(ch)              (SMB0_CLKCTL1_REG + ch*0x20)
#define   SMB_CLKCTL1_PRESCALE1_gm       0xFF
#define   SMB_CLKCTL1_PRESCALE1_gp       0

//=============================================================================
//  5.3.7.2.21  SMB_CLKCTL2 : SMBus 0/1 SMBCLK Clock Frequency Control 2 Reg.
//=============================================================================
#define SMB0_CLKCTL2_REG                 (SMB0_BASE + 0x14)
#define SMB1_CLKCTL2_REG                 (SMB1_BASE + 0x14)
#define SMB_CLKCTL2_REG(ch)              (SMB0_CLKCTL2_REG + ch*0x20)
#define   SMB_CLKCTL2_FAST_bm            0x80
#define   SMB_CLKCTL2_FAST_bp            7
#define   SMB_CLKCTL2_PRESCALE2_gm       0x7F
#define   SMB_CLKCTL2_PRESCALE2_gp       0

//=============================================================================
//  5.3.7.2.22  SMB_EXCTL : SMBus 0/1 Extra Control Reg.
//=============================================================================
#define SMB0_EXCTL_REG                   (SMB0_BASE + 0x14)
#define SMB1_EXCTL_REG                   (SMB1_BASE + 0x14)
#define SMB_EXCTL_REG(ch)                (SMB0_EXCTL_REG + ch*0x20)
#define   SMB_EXCTL_TCE_bm               0x80
#define   SMB_EXCTL_TCE_bp               7
#define   SMB_EXCTL_LATCHTIME_bm         0x40
#define   SMB_EXCTL_LATCHTIME_bp         6
#define   SMB_EXCTL_NOFILTER_bm          0x20
#define   SMB_EXCTL_NOFILTER_bp          5
#define   SMB_EXCTL_NODRIVE_bm           0x10
#define   SMB_EXCTL_NODRIVE_bp           4
#define   SMB_EXCTL_DIMC_bm              0x8
#define   SMB_EXCTL_DIMC_bp              3
#define   SMB_EXCTL_DI196_bm             0x4
#define   SMB_EXCTL_DI196_bp             2
#define   SMB_EXCTL_DIAR_bm              0x2
#define   SMB_EXCTL_DIAR_bp              1
#define   SMB_EXCTL_DIDC_bm              0x1
#define   SMB_EXCTL_DIDC_bp              0

//-----------------------------------------------------------------------------
//  SMB
//-----------------------------------------------------------------------------
#define SMB_CH_NUM              (2)
#define SMB_CH_MAX              (SMB_CH_NUM - 1)

#define SMB_SW_RST(ch)			SET_REG_BM8(SMB_CTL2_SSRESET_bm, SMB_CTL2_REG(ch))


#endif
