//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - BDA (BIOS Data Area)                                               *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
//#include <dos.h>
#include "typedef.h"

uint32_t gCount = 0;
EFI_EVENT efi_event = NULL;

void EFITimerEventClose(void)
{
	uint8_t i = 0;

	if(efi_event)
	{
		while(gBS->CloseEvent(efi_event))
		{
			delay(100);
			i++;
			if(i >= 100)
			{
				break;
			}
		}
	}
}

void EFIAPI TimerEvent(IN EFI_EVENT Event, IN VOID *Context)
{
	gCount++;
	if(gCount > 1573040)
	{
		gCount = 0;
	}
}

//===========================================================================
//  bda_get_timer
//===========================================================================
uint32_t bda_get_timer(void)
{
	//---------------------------------------------
	//  __BORLANDC__
	//---------------------------------------------
	#ifdef __BORLANDC__
	uint32_t far	*dp;

	FP_SEG(dp) = 0x40;	// @ 0040:006C
	FP_OFF(dp) = 0x6C;

	return (*dp);
	#endif

	//---------------------------------------------
	//  __WATCOMC__
	//---------------------------------------------
	#ifdef __WATCOMC__
	static uint8_t initflag = 0;
	EFI_STATUS status = 0;
	
	if(initflag == 0)
	{
		status = gBS->CreateEvent(EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_CALLBACK, TimerEvent, NULL, &efi_event);
		if(!status)
		{
			status = gBS->SetTimer(efi_event, TimerPeriodic, 55 * 1000 * 10);	//unit 100ns
			if(!status)
			{
				initflag = 1;
			}
		}
	}
	return gCount;//*((uint32_t*) (0x0000046C));
	#endif
}

//===========================================================================
//  bda_get_video_config
//===========================================================================
uint8_t bda_get_video_config(void)
{
	//---------------------------------------------
	//  __BORLANDC__
	//---------------------------------------------
	#ifdef __BORLANDC__
	uint8_t far		*dp;

	FP_SEG(dp) = 0x40;	// @ 0040:0089
	FP_OFF(dp) = 0x89;

	return (*dp);
	#endif

	//---------------------------------------------
	//  __WATCOMC__
	//---------------------------------------------
	#ifdef __WATCOMC__
	return *((uint8_t*) (0x00000489));
	#endif
}
