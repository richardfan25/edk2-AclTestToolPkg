#ifndef __A9610_CAN_H
#define __A9610_CAN_H

//=============================================================================
//  5.3.5  CAN Bus Controller 0/1
//=============================================================================
//  Features
//    . Compatible with CAN 2.0A/2.0B specification
//    . Supports speed up to 1 Mbps
//    . Programmable hardware arbitration lost retry function and
//      hardware error retry function
//  Host Domain
//    . PCIe Mem : device 5, function 0/1, configuration 10h~13h
//    . PCIe I/O : device 5, function 0/1, configuration 14h~17h
//    . LPC      : LDN 18h/19h, index 60h~61h
//  EC Domain
//    . None
//=============================================================================
#define CAN_CH_NUM      (2)

#define CAN_CH_0        0
#define CAN_CH_1        1
#define CAN_CH_MAX      (CAN_CH_NUM - 1)

//=============================================================================
//  base address
//=============================================================================
#define CAN0_BASE                        0x200  // host domain
#define CAN1_BASE                        0x300  // host domain

//=============================================================================
//  5.3.5.4.1  CAN_GLB_CTL : Global Control Reg.
//=============================================================================
#define CAN_GLB_CTL                      0x00
#define CAN0_GLB_CTL_REG                 (CAN0_BASE + CAN_GLB_CTL)
#define CAN1_GLB_CTL_REG                 (CAN1_BASE + CAN_GLB_CTL)
#define   CAN_GLB_CTL_RSVD_gm            0xFE000000
#define   CAN_GLB_CTL_RSVD_gp            25
#define   CAN_GLB_CTL_PSE_bm             0x1000000
#define   CAN_GLB_CTL_PSE_bp             24
#define   CAN_GLB_CTL_RSVD2_gm           0xFFFC00
#define   CAN_GLB_CTL_RSVD2_gp           10
#define   CAN_GLB_CTL_ERE_bm             0x200
#define   CAN_GLB_CTL_ERE_bp             9
#define   CAN_GLB_CTL_ARE_bm             0x100
#define   CAN_GLB_CTL_ARE_bp             8
#define   CAN_GLB_CTL_TBP_gm             0xC0
#define   CAN_GLB_CTL_TBP_gp             6
#define   CAN_GLB_CTL_SR_bm              0x20
#define   CAN_GLB_CTL_SR_bp              5
#define   CAN_GLB_CTL_TNAK_bm            0x10
#define   CAN_GLB_CTL_TNAK_bp            4
#define   CAN_GLB_CTL_LP_bm              0x8
#define   CAN_GLB_CTL_LP_bp              3
#define   CAN_GLB_CTL_SI_bm              0x4
#define   CAN_GLB_CTL_SI_bp              2
#define   CAN_GLB_CTL_CBA_bm             0x2
#define   CAN_GLB_CTL_CBA_bp             1
#define   CAN_GLB_CTL_RST_bm             0x1
#define   CAN_GLB_CTL_RST_bp             0

//=============================================================================
//  5.3.5.4.2  CAN_CLK_PRE : Clock Pre-Scaler Reg.
//=============================================================================
#define CAN_CLK_PRE                      0x04
#define CAN0_CLK_PRE_REG                 (CAN0_BASE + CAN_CLK_PRE)
#define CAN1_CLK_PRE_REG                 (CAN1_BASE + CAN_CLK_PRE)
#define   CAN_CLK_PRE_RSVD_gm            0xFFFFFFC0
#define   CAN_CLK_PRE_RSVD_gp            6
#define   CAN_CLK_PRE_CKDIV_gm           0x3F
#define   CAN_CLK_PRE_CKDIV_gp           0    // TQ = [2 x (CKDIV+1)] / f

//=============================================================================
//  5.3.5.4.3  CAN_BUS_TIM : Bus Timing Reg.
//=============================================================================
#define CAN_BUS_TIM                      0x08
#define CAN0_BUS_TIM_REG                 (CAN0_BASE + CAN_BUS_TIM)
#define CAN1_BUS_TIM_REG                 (CAN1_BASE + CAN_BUS_TIM)
#define   CAN_BUS_TIM_RSVD_gm            0xFFFF0000
#define   CAN_BUS_TIM_RSVD_gp            16
#define   CAN_BUS_TIM_SAM_bm             0x8000
#define   CAN_BUS_TIM_SAM_bp             15
#define   CAN_BUS_TIM_RSVD_bm            0x4000
#define   CAN_BUS_TIM_RSVD_bp            14
#define   CAN_BUS_TIM_SJW_gm             0x3000
#define   CAN_BUS_TIM_SJW_gp             12
#define   CAN_BUS_TIM_RSVD2_bm           0x800
#define   CAN_BUS_TIM_RSVD2_bp           11
#define   CAN_BUS_TIM_PS2_gm             0x700
#define   CAN_BUS_TIM_PS2_gp             8
#define   CAN_BUS_TIM_RSVD3_bm           0x80
#define   CAN_BUS_TIM_RSVD3_bp           7
#define   CAN_BUS_TIM_PS1_gm             0x70
#define   CAN_BUS_TIM_PS1_gp             4
#define   CAN_BUS_TIM_RSVD4_bm           0x8
#define   CAN_BUS_TIM_RSVD4_bp           3
#define   CAN_BUS_TIM_PROG_gm            0x7
#define   CAN_BUS_TIM_PROG_gp            0

//=============================================================================
//  5.3.5.4.4  CAN_INT_ENA : Interrupt Enable Reg.
//=============================================================================
#define CAN_INT_ENA                      0x0C
#define CAN0_INT_ENA_REG                 (CAN0_BASE + CAN_INT_ENA)
#define CAN1_INT_ENA_REG                 (CAN1_BASE + CAN_INT_ENA)
#define   CAN_INT_ENA_RSVD_gm            0xFFFFF800
#define   CAN_INT_ENA_RSVD_gp            11
#define   CAN_INT_ENA_XPIE_bm            0x400
#define   CAN_INT_ENA_XPIE_bp            10
#define   CAN_INT_ENA_RBOIE_bm           0x200
#define   CAN_INT_ENA_RBOIE_bp           9
#define   CAN_INT_ENA_RBEIE_bm           0x100
#define   CAN_INT_ENA_RBEIE_bp           8
#define   CAN_INT_ENA_ALIE_bm            0x80
#define   CAN_INT_ENA_ALIE_bp            7
#define   CAN_INT_ENA_BOIE_bm            0x40
#define   CAN_INT_ENA_BOIE_bp            6
#define   CAN_INT_ENA_EPIE_bm            0x20
#define   CAN_INT_ENA_EPIE_bp            5
#define   CAN_INT_ENA_ECIE_bm            0x10
#define   CAN_INT_ENA_ECIE_bp            4
#define   CAN_INT_ENA_TX2IE_bm           0x8
#define   CAN_INT_ENA_TX2IE_bp           3
#define   CAN_INT_ENA_TX1IE_bm           0x4
#define   CAN_INT_ENA_TX1IE_bp           2
#define   CAN_INT_ENA_TX0IE_bm           0x2
#define   CAN_INT_ENA_TX0IE_bp           1
#define   CAN_INT_ENA_RXIE_bm            0x1
#define   CAN_INT_ENA_RXIE_bp            0

//=============================================================================
//  5.3.5.4.5  CAN_INT_STS : Interrupt Status Reg.
//=============================================================================
#define CAN_INT_STS                      0x10
#define CAN0_INT_STS_REG                 (CAN0_BASE + CAN_INT_STS)
#define CAN1_INT_STS_REG                 (CAN1_BASE + CAN_INT_STS)
#define   CAN_INT_STS_RSVD_gm            0xFFFE0000
#define   CAN_INT_STS_RSVD_gp            17
#define   CAN_INT_STS_BED_bm             0x10000
#define   CAN_INT_STS_BED_bp             16
#define   CAN_INT_STS_RSVD2_gm           0xF800
#define   CAN_INT_STS_RSVD2_gp           11
#define   CAN_INT_STS_XPI_bm             0x400
#define   CAN_INT_STS_XPI_bp             10
#define   CAN_INT_STS_RBOI_bm            0x200
#define   CAN_INT_STS_RBOI_bp            9
#define   CAN_INT_STS_RBEI_bm            0x100
#define   CAN_INT_STS_RBEI_bp            8
#define   CAN_INT_STS_ALI_bm             0x80
#define   CAN_INT_STS_ALI_bp             7
#define   CAN_INT_STS_BOI_bm             0x40
#define   CAN_INT_STS_BOI_bp             6
#define   CAN_INT_STS_EPI_bm             0x20
#define   CAN_INT_STS_EPI_bp             5
#define   CAN_INT_STS_ECI_bm             0x10
#define   CAN_INT_STS_ECI_bp             4
#define   CAN_INT_STS_TX2I_bm            0x8
#define   CAN_INT_STS_TX2I_bp            3
#define   CAN_INT_STS_TX1I_bm            0x4
#define   CAN_INT_STS_TX1I_bp            2
#define   CAN_INT_STS_TX0I_bm            0x2
#define   CAN_INT_STS_TX0I_bp            1
#define   CAN_INT_STS_RXI_bm             0x1
#define   CAN_INT_STS_RXI_bp             0

//=============================================================================
//  5.3.5.4.6  CAN_GLB_STS : Controller Status Reg.
//=============================================================================
#define CAN_GLB_STS                      0x14
#define CAN0_GLB_STS_REG                 (CAN0_BASE + CAN_GLB_STS)
#define CAN1_GLB_STS_REG                 (CAN1_BASE + CAN_GLB_STS)
#define   CAN_GLB_STS_RSVD_gm            0xFFFFF800
#define   CAN_GLB_STS_RSVD_gp            11
#define   CAN_GLB_STS_CSM_gm             0x700
#define   CAN_GLB_STS_CSM_gp             8
#define   CAN_GLB_STS_RSVD2_gm           0xC0
#define   CAN_GLB_STS_RSVD2_gp           6
#define   CAN_GLB_STS_ECW_bm             0x20
#define   CAN_GLB_STS_ECW_bp             5
#define   CAN_GLB_STS_CEP_bm             0x10
#define   CAN_GLB_STS_CEP_bp             4
#define   CAN_GLB_STS_CPS_bm             0x8
#define   CAN_GLB_STS_CPS_bp             3
#define   CAN_GLB_STS_CBO_bm             0x4
#define   CAN_GLB_STS_CBO_bp             2
#define   CAN_GLB_STS_TIP_bm             0x2
#define   CAN_GLB_STS_TIP_bp             1
#define   CAN_GLB_STS_RIP_bm             0x1
#define   CAN_GLB_STS_RIP_bp             0

//=============================================================================
//  5.3.5.4.7  CAN_REQ_CTL : Request Reg.
//=============================================================================
#define CAN_REQ_CTL                      0x18
#define CAN0_REQ_CTL_REG                 (CAN0_BASE + CAN_REQ_CTL)
#define CAN1_REQ_CTL_REG                 (CAN1_BASE + CAN_REQ_CTL)
#define   CAN_REQ_CTL_RSVD_gm            0xFFFFFE00
#define   CAN_REQ_CTL_RSVD_gp            9
#define   CAN_REQ_CTL_RRB_bm             0x100
#define   CAN_REQ_CTL_RRB_bp             8
#define   CAN_REQ_CTL_RSVD2_gm           0xC0
#define   CAN_REQ_CTL_RSVD2_gp           6
#define   CAN_REQ_CTL_TBA2_bm            0x20
#define   CAN_REQ_CTL_TBA2_bp            5
#define   CAN_REQ_CTL_TBR2_bm            0x10
#define   CAN_REQ_CTL_TBR2_bp            4
#define   CAN_REQ_CTL_TBA1_bm            0x8
#define   CAN_REQ_CTL_TBA1_bp            3
#define   CAN_REQ_CTL_TBR1_bm            0x4
#define   CAN_REQ_CTL_TBR1_bp            2
#define   CAN_REQ_CTL_TBA0_bm            0x2
#define   CAN_REQ_CTL_TBA0_bp            1
#define   CAN_REQ_CTL_TBR0_bm            0x1
#define   CAN_REQ_CTL_TBR0_bp            0

//=============================================================================
//  5.3.5.4.8  CAN_TXD_STS0 : Transmit Status 0 Reg.
//=============================================================================
#define CAN_TXD_STS0                     0x1C
#define CAN0_TXD_STS0_REG                (CAN0_BASE + CAN_TXD_STS0)
#define CAN1_TXD_STS0_REG                (CAN1_BASE + CAN_TXD_STS0)
#define   CAN_TXD_STS0_RSVD_gm           0xFFF80000
#define   CAN_TXD_STS0_RSVD_gp           19
#define   CAN_TXD_STS0_BEC0_gm           0x70000
#define   CAN_TXD_STS0_BEC0_gp           16
#define   CAN_TXD_STS0_RSVD2_gm          0xFF80
#define   CAN_TXD_STS0_RSVD2_gp          7
#define   CAN_TXD_STS0_TBBF0_bm          0x40
#define   CAN_TXD_STS0_TBBF0_bp          6
#define   CAN_TXD_STS0_BEO0_bm           0x20
#define   CAN_TXD_STS0_BEO0_bp           5
#define   CAN_TXD_STS0_ALO0_bm           0x10
#define   CAN_TXD_STS0_ALO0_bp           4
#define   CAN_TXD_STS0_RSVD_bm           0x8
#define   CAN_TXD_STS0_RSVD_bp           3
#define   CAN_TXD_STS0_TC0_bm            0x4
#define   CAN_TXD_STS0_TC0_bp            2
#define   CAN_TXD_STS0_TA0_bm            0x2
#define   CAN_TXD_STS0_TA0_bp            1
#define   CAN_TXD_STS0_TRC0_bm           0x1
#define   CAN_TXD_STS0_TRC0_bp           0

//=============================================================================
//  5.3.5.4.9  CAN_TXD_STS1 : Transmit Status 1 Reg.
//=============================================================================
#define CAN_TXD_STS1                     0x20
#define CAN0_TXD_STS1_REG                (CAN0_BASE + CAN_TXD_STS1)
#define CAN1_TXD_STS1_REG                (CAN1_BASE + CAN_TXD_STS1)
#define   CAN_TXD_STS1_RSVD_gm           0xFFF80000
#define   CAN_TXD_STS1_RSVD_gp           19
#define   CAN_TXD_STS1_BEC1_gm           0x70000
#define   CAN_TXD_STS1_BEC1_gp           16
#define   CAN_TXD_STS1_RSVD2_gm          0xFF80
#define   CAN_TXD_STS1_RSVD2_gp          7
#define   CAN_TXD_STS1_TBBF1_bm          0x40
#define   CAN_TXD_STS1_TBBF1_bp          6
#define   CAN_TXD_STS1_BEO1_bm           0x20
#define   CAN_TXD_STS1_BEO1_bp           5
#define   CAN_TXD_STS1_ALO1_bm           0x10
#define   CAN_TXD_STS1_ALO1_bp           4
#define   CAN_TXD_STS1_RSVD_bm           0x8
#define   CAN_TXD_STS1_RSVD_bp           3
#define   CAN_TXD_STS1_TC1_bm            0x4
#define   CAN_TXD_STS1_TC1_bp            2
#define   CAN_TXD_STS1_TA1_bm            0x2
#define   CAN_TXD_STS1_TA1_bp            1
#define   CAN_TXD_STS1_TRC1_bm           0x1
#define   CAN_TXD_STS1_TRC1_bp           0

//=============================================================================
//  5.3.5.4.10  CAN_TXD_STS2 : Transmit Status 2 Reg.
//=============================================================================
#define CAN_TXD_STS2                     0x24
#define CAN0_TXD_STS2_REG                (CAN0_BASE + CAN_TXD_STS2)
#define CAN1_TXD_STS2_REG                (CAN1_BASE + CAN_TXD_STS2)
#define   CAN_TXD_STS2_RSVD_gm           0xFFF80000
#define   CAN_TXD_STS2_RSVD_gp           19
#define   CAN_TXD_STS2_BEC2_gm           0x70000
#define   CAN_TXD_STS2_BEC2_gp           16
#define   CAN_TXD_STS2_RSVD2_gm          0xFF80
#define   CAN_TXD_STS2_RSVD2_gp          7
#define   CAN_TXD_STS2_TBBF2_bm          0x40
#define   CAN_TXD_STS2_TBBF2_bp          6
#define   CAN_TXD_STS2_BEO2_bm           0x20
#define   CAN_TXD_STS2_BEO2_bp           5
#define   CAN_TXD_STS2_ALO2_bm           0x10
#define   CAN_TXD_STS2_ALO2_bp           4
#define   CAN_TXD_STS2_RSVD_bm           0x8
#define   CAN_TXD_STS2_RSVD_bp           3
#define   CAN_TXD_STS2_TC2_bm            0x4
#define   CAN_TXD_STS2_TC2_bp            2
#define   CAN_TXD_STS2_TA2_bm            0x2
#define   CAN_TXD_STS2_TA2_bp            1
#define   CAN_TXD_STS2_TRC2_bm           0x1
#define   CAN_TXD_STS2_TRC2_bp           0

//=============================================================================
//  5.3.5.4.11  CAN_RCV_STS : Receive Status Reg.
//=============================================================================
#define CAN_RCV_STS                      0x28
#define CAN0_RCV_STS_REG                 (CAN0_BASE + CAN_RCV_STS)
#define CAN1_RCV_STS_REG                 (CAN1_BASE + CAN_RCV_STS)
#define   CAN_RCV_STS_RSVD_gm            0xFFFFFF80
#define   CAN_RCV_STS_RSVD_gp            7
#define   CAN_RCV_STS_RBEC_gm            0x70
#define   CAN_RCV_STS_RBEC_gp            4
#define   CAN_RCV_STS_RSVD_bm            0x8
#define   CAN_RCV_STS_RSVD_bp            3
#define   CAN_RCV_STS_BEOR_bm            0x4
#define   CAN_RCV_STS_BEOR_bp            2
#define   CAN_RCV_STS_RBO_bm             0x2
#define   CAN_RCV_STS_RBO_bp             1
#define   CAN_RCV_STS_RBS_bm             0x1
#define   CAN_RCV_STS_RBS_bp             0

//=============================================================================
//  5.3.5.4.12  CAN_ERR_LIM : Error Warning Limit Reg.
//=============================================================================
#define CAN_ERR_LIM                      0x2C
#define CAN0_ERR_LIM_REG                 (CAN0_BASE + CAN_ERR_LIM)
#define CAN1_ERR_LIM_REG                 (CAN1_BASE + CAN_ERR_LIM)
#define   CAN_ERR_LIM_RSVD_gm            0xFFFFFF00
#define   CAN_ERR_LIM_RSVD_gp            8
#define   CAN_ERR_LIM_EWL_gm             0xFF
#define   CAN_ERR_LIM_EWL_gp             0

//=============================================================================
//  5.3.5.4.13  CAN_TRE_CNT : TX/RX Error Counter Reg.
//=============================================================================
#define CAN_TRE_CNT                      0x30
#define CAN0_TRE_CNT_REG                 (CAN0_BASE + CAN_ERR_LIM)
#define CAN1_TRE_CNT_REG                 (CAN1_BASE + CAN_ERR_LIM)
#define   CAN_TRE_CNT_RSVD_gm            0xFF000000
#define   CAN_TRE_CNT_RSVD_gp            24
#define   CAN_TRE_CNT_REC_gm             0xFF0000
#define   CAN_TRE_CNT_REC_gp             16
#define   CAN_TRE_CNT_RSVD2_gm           0xFF00
#define   CAN_TRE_CNT_RSVD2_gp           8
#define   CAN_TRE_CNT_TEC_gm             0xFF
#define   CAN_TRE_CNT_TEC_gp             0

//=============================================================================
//  There is no register descrption in datasheet
//=============================================================================
#define RDC_CAN_IDF_INX			0x34	// reserved
#define RDC_CAN_IDF_FIT			0x38	// reserved
#define RDC_CAN_IDF_MSK			0x3C	// reserved

//=============================================================================
//  5.3.5.4.14  CAN_TXD_CTL0 : TX Frame Control 0 Reg.
//=============================================================================
#define CAN_TXD_CTL0                     0x40
#define CAN0_TXD_CTL0_REG                (CAN0_BASE + CAN_TXD_CTL0)
#define CAN1_TXD_CTL0_REG                (CAN1_BASE + CAN_TXD_CTL0)
#define   CAN_TXD_CTL0_RSVD_gm           0xFFFFFF00
#define   CAN_TXD_CTL0_RSVD_gp           8
#define   CAN_TXD_CTL0_TDL_gm            0xF0
#define   CAN_TXD_CTL0_TDL_gp            4
#define   CAN_TXD_CTL0_RSVD2_gm          0xC
#define   CAN_TXD_CTL0_RSVD2_gp          2
#define   CAN_TXD_CTL0_TRTR_bm           0x2
#define   CAN_TXD_CTL0_TRTR_bp           1
#define   CAN_TXD_CTL0_TFF_bm            0x1
#define   CAN_TXD_CTL0_TFF_bp            0

//=============================================================================
//  5.3.5.4.15  CAN_TXD_IDF0 : TX ID 0 Reg.
//=============================================================================
#define CAN_TXD_IDF0                     0x44
#define CAN0_TXD_IDF0_REG                (CAN0_BASE + CAN_TXD_IDF0)
#define CAN1_TXD_IDF0_REG                (CAN1_BASE + CAN_TXD_IDF0)
#define   CAN_TXD_IDF0_RSVD_gm           0xE0000000
#define   CAN_TXD_IDF0_RSVD_gp           29
#define   CAN_TXD_IDF0_TID_gm            0x1FFFFFFF
#define   CAN_TXD_IDF0_TID_gp            0

//=============================================================================
//  5.3.5.4.16  CAN_TXD_DAL0 : TX Data Low 0 Reg.
//=============================================================================
#define CAN_TXD_DAL0                     0x48
#define CAN0_TXD_DAL0_REG                (CAN0_BASE + CAN_TXD_DAL0)
#define CAN1_TXD_DAL0_REG                (CAN1_BASE + CAN_TXD_DAL0)
#define   CAN_TXD_DAL0_TXDL_gm           0xFFFFFFFF
#define   CAN_TXD_DAL0_TXDL_gp           0

//=============================================================================
//  5.3.5.4.17  CAN_TXD_DAH0 : TX Data High 0 Reg.
//=============================================================================
#define CAN_TXD_DAH0                     0x4C
#define CAN0_TXD_DAH0_REG                (CAN0_BASE + CAN_TXD_DAH0)
#define CAN1_TXD_DAH0_REG                (CAN1_BASE + CAN_TXD_DAH0)
#define   CAN_TXD_DAH0_TXDH_gm           0xFFFFFFFF
#define   CAN_TXD_DAH0_TXDH_gp           0

//=============================================================================
//  5.3.5.4.18  CAN_TXD_CTL1 : TX Frame Control 1 Reg.
//=============================================================================
#define CAN_TXD_CTL1                     0x50
#define CAN0_TXD_CTL1_REG                (CAN0_BASE + CAN_TXD_CTL1)
#define CAN1_TXD_CTL1_REG                (CAN1_BASE + CAN_TXD_CTL1)
#define   CAN_TXD_CTL1_RSVD_gm           0xFFFFFF00
#define   CAN_TXD_CTL1_RSVD_gp           8
#define   CAN_TXD_CTL1_TDL_gm            0xF0
#define   CAN_TXD_CTL1_TDL_gp            4
#define   CAN_TXD_CTL1_RSVD2_gm          0xC
#define   CAN_TXD_CTL1_RSVD2_gp          2
#define   CAN_TXD_CTL1_TRTR_bm           0x2
#define   CAN_TXD_CTL1_TRTR_bp           1
#define   CAN_TXD_CTL1_TFF_bm            0x1
#define   CAN_TXD_CTL1_TFF_bp            0

//=============================================================================
//  5.3.5.4.19  CAN_TXD_IDF1 : TX ID 1 Reg.
//=============================================================================
#define CAN_TXD_IDF1                     0x54
#define CAN0_TXD_IDF1_REG                (CAN0_BASE + CAN_TXD_IDF1)
#define CAN1_TXD_IDF1_REG                (CAN1_BASE + CAN_TXD_IDF1)
#define   CAN_TXD_IDF1_RSVD_gm           0xE0000000
#define   CAN_TXD_IDF1_RSVD_gp           29
#define   CAN_TXD_IDF1_TID_gm            0x1FFFFFFF
#define   CAN_TXD_IDF1_TID_gp            0

//=============================================================================
//  5.3.5.4.20  CAN_TXD_DAL1 : TX Data Low 1 Reg.
//=============================================================================
#define CAN_TXD_DAL1                     0x58
#define CAN0_TXD_DAL1_REG                (CAN0_BASE + CAN_TXD_DAL1)
#define CAN1_TXD_DAL1_REG                (CAN1_BASE + CAN_TXD_DAL1)
#define   CAN_TXD_DAL1_TXDL_gm           0xFFFFFFFF
#define   CAN_TXD_DAL1_TXDL_gp           0

//=============================================================================
//  5.3.5.4.21  CAN_TXD_DAH1 : TX Data High 1 Reg.
//=============================================================================
#define CAN_TXD_DAH1                     0x5C
#define CAN0_TXD_DAH1_REG                (CAN0_BASE + CAN_TXD_DAH1)
#define CAN1_TXD_DAH1_REG                (CAN1_BASE + CAN_TXD_DAH1)
#define   CAN_TXD_DAH1_TXDH_gm           0xFFFFFFFF
#define   CAN_TXD_DAH1_TXDH_gp           0

//=============================================================================
//  5.3.5.4.22  CAN_TXD_CTL2 : TX Frame Control 2 Reg.
//=============================================================================
#define CAN_TXD_CTL2                     0x60
#define CAN0_TXD_CTL2_REG                (CAN0_BASE + CAN_TXD_CTL2)
#define CAN1_TXD_CTL2_REG                (CAN1_BASE + CAN_TXD_CTL2)
#define   CAN_TXD_CTL2_RSVD_gm           0xFFFFFF00
#define   CAN_TXD_CTL2_RSVD_gp           8
#define   CAN_TXD_CTL2_TDL_gm            0xF0
#define   CAN_TXD_CTL2_TDL_gp            4
#define   CAN_TXD_CTL2_RSVD2_gm          0xC
#define   CAN_TXD_CTL2_RSVD2_gp          2
#define   CAN_TXD_CTL2_TRTR_bm           0x2
#define   CAN_TXD_CTL2_TRTR_bp           1
#define   CAN_TXD_CTL2_TFF_bm            0x1
#define   CAN_TXD_CTL2_TFF_bp            0

//=============================================================================
//  5.3.5.4.23  CAN_TXD_IDF2 : TX ID 2 Reg.
//=============================================================================
#define CAN_TXD_IDF2                     0x64
#define CAN0_TXD_IDF2_REG                (CAN0_BASE + CAN_TXD_IDF2)
#define CAN1_TXD_IDF2_REG                (CAN1_BASE + CAN_TXD_IDF2)
#define   CAN_TXD_IDF2_RSVD_gm           0xE0000000
#define   CAN_TXD_IDF2_RSVD_gp           29
#define   CAN_TXD_IDF2_TID_gm            0x1FFFFFFF
#define   CAN_TXD_IDF2_TID_gp            0

//=============================================================================
//  5.3.5.4.24  CAN_TXD_DAL2 : TX Data Low 2 Reg.
//=============================================================================
#define CAN_TXD_DAL2                     0x68
#define CAN0_TXD_DAL2_REG                (CAN0_BASE + CAN_TXD_DAL2)
#define CAN1_TXD_DAL2_REG                (CAN1_BASE + CAN_TXD_DAL2)
#define   CAN_TXD_DAL2_TXDL_gm           0xFFFFFFFF
#define   CAN_TXD_DAL2_TXDL_gp           0

//=============================================================================
//  5.3.5.4.25  CAN_TXD_DAH2 : TX Data High 2 Reg.
//=============================================================================
#define CAN_TXD_DAH2                     0x6C
#define CAN0_TXD_DAH2_REG                (CAN0_BASE + CAN_TXD_DAH2)
#define CAN1_TXD_DAH2_REG                (CAN1_BASE + CAN_TXD_DAH2)
#define   CAN_TXD_DAH2_TXDH_gm           0xFFFFFFFF
#define   CAN_TXD_DAH2_TXDH_gp           0

//=============================================================================
//  5.3.5.4.26  CAN_RXD_FRM : RX Frame Type Reg.
//=============================================================================
#define CAN_RXD_FRM                      0x70
#define CAN0_RXD_FRM_REG                 (CAN0_BASE + CAN_RXD_FRM)
#define CAN1_RXD_FRM_REG                 (CAN1_BASE + CAN_RXD_FRM)
#define   CAN_RXD_FRM_RSVD_gm            0xFFFFFF00
#define   CAN_RXD_FRM_RSVD_gp            8
#define   CAN_RXD_FRM_RDL_gm             0xF0
#define   CAN_RXD_FRM_RDL_gp             4
#define   CAN_RXD_FRM_RSVD2_gm           0xC
#define   CAN_RXD_FRM_RSVD2_gp           2
#define   CAN_RXD_FRM_RRTR_bm            0x2
#define   CAN_RXD_FRM_RRTR_bp            1
#define   CAN_RXD_FRM_RFF_bm             0x1
#define   CAN_RXD_FRM_RFF_bp             0

//=============================================================================
//  5.3.5.4.27  CAN_RXD_IDF : RX ID Reg.
//=============================================================================
#define CAN_RXD_IDF                      0x74
#define CAN0_RXD_IDF_REG                 (CAN0_BASE + CAN_RXD_IDF)
#define CAN1_RXD_IDF_REG                 (CAN1_BASE + CAN_RXD_IDF)
#define   CAN_RXD_IDF_RSVD_gm            0xE0000000
#define   CAN_RXD_IDF_RSVD_gp            29
#define   CAN_RXD_IDF_RID_gm             0x1FFFFFFF
#define   CAN_RXD_IDF_RID_gp             0

//=============================================================================
//  5.3.5.4.26  CAN_RXD_DAL : RX Data Low Reg.
//=============================================================================
#define CAN_RXD_DAL                     0x78
#define CAN0_RXD_DAL_REG                (CAN0_BASE + CAN_RXD_DAL)
#define CAN1_RXD_DAL_REG                (CAN1_BASE + CAN_RXD_DAL)
#define   CAN_RXD_DAL_RXDL_gm           0xFFFFFFFF
#define   CAN_RXD_DAL_RXDL_gp           0

//=============================================================================
//  5.3.5.4.27  CAN_RXD_DAL : RX Data High Reg.
//=============================================================================
#define CAN_RXD_DAH                     0x7C
#define CAN0_RXD_DAH_REG                (CAN0_BASE + CAN_RXD_DAH)
#define CAN1_RXD_DAH_REG                (CAN1_BASE + CAN_RXD_DAH)
#define   CAN_RXD_DAH_RXDH_gm           0xFFFFFFFF
#define   CAN_RXD_DAH_RXDH_gp           0

#endif
