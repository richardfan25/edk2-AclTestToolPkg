#ifndef __CHKREC_H
#define __CHKREC_H


//=============================================================================
//  define
//=============================================================================
// value
#define CHKREC_ERR				0
#define CHKREC_OK				1
#define CHKREC_NOFEAT			2	// no feature
#define CHKREC_NOCHK			3	// user : no-check

// mask
#define CHKREC_GP_AB_MODEL		(30)
#define CHKREC_GM_AB_MODEL		(3 << CHKREC_GP_AB_MODEL)
#define CHKREC_GP_AB_VER		(28)
#define CHKREC_GM_AB_VER		(3 << CHKREC_GP_AB_VER)
#define CHKREC_GP_AEC_CHIP		(26)
#define CHKREC_GM_AEC_CHIP		(3 << CHKREC_GP_AEC_CHIP)
#define CHKREC_GP_AEC_BOARD		(24)
#define CHKREC_GM_AEC_BOARD		(3 << CHKREC_GP_AEC_BOARD)
#define CHKREC_GP_AEC_VER		(22)
#define CHKREC_GM_AEC_VER		(3 << CHKREC_GP_AEC_VER)
#define CHKREC_GP_CP_VENDOR		(20)
#define CHKREC_GM_CP_VENDOR		(3 << CHKREC_GP_CP_VENDOR)
#define CHKREC_GP_CP_NAME		(18)
#define CHKREC_GM_CP_NAME		(3 << CHKREC_GP_CP_NAME)
#define CHKREC_GP_E8_SIZE		(16)
#define CHKREC_GM_E8_SIZE		(3 << CHKREC_GP_E8_SIZE)
#define CHKREC_GP_SM_DATA		(14)
#define CHKREC_GM_SM_DATA		(3 << CHKREC_GP_SM_DATA)
#define CHKREC_GP_PC_SCAN		(12)
#define CHKREC_GM_PC_SCAN		(3 << CHKREC_GP_PC_SCAN)
#define CHKREC_GP_SB_SCAN		(10)
#define CHKREC_GM_SB_SCAN		(3 << CHKREC_GP_SB_SCAN)
#define CHKREC_GP_SP_SCAN		(8)
#define CHKREC_GM_SP_SCAN		(3 << CHKREC_GP_SP_SCAN)
#define CHKREC_GP_RTC_0A		(6)
#define CHKREC_GM_RTC_0A		(3 << CHKREC_GP_RTC_0A)
#define CHKREC_GP_RTC_0B		(4)
#define CHKREC_GM_RTC_0B		(3 << CHKREC_GP_RTC_0B)
#define CHKREC_GP_RTC_0D		(2)
#define CHKREC_GM_RTC_0D		(3 << CHKREC_GP_RTC_0D)

//=============================================================================
//  chkrec_pci_dev_t
//=============================================================================
typedef struct _chkrec_pci_dev_t
{
	uint16_t	bdf;	// [15:8]=b, [7:3]=d, [2:0]=f
	uint16_t	vid;	// vendor id
	uint16_t	did;	// device id
	uint16_t	flg;	// [1]=check, [0]=init

} chkrec_pci_dev_t;

//=============================================================================
//  functions
//=============================================================================
int chkrec_show(int idx);

#endif
