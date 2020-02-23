#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "util.h"

/**
 * initialize the progress bar.
 * @max = 0
 * @val = 0
 *
 * @param	style
 * @param	tip words.
 */
extern void progress_init(
    progress_t *bar, char *title, int max)
{
    bar->chr = '=';
    bar->title = title;
    bar->max = max;
    bar->offset = 100 / (float)max;
    bar->pro = (char *) malloc(max+1);
    memset(bar->pro, 32, max);
    memset(bar->pro+max, 0x00, 1);
}

extern void progress_show( progress_t *bar, float bit )
{
    int val = (int)(bit * bar->max);

    memset(bar->pro, bar->chr, val);
    printf(PROGRESS_BUS_STRING, 
        bar->title, bar->pro, (int)(bar->offset * val));
    fflush(stdout);
}

//destroy the the progress bar.
extern void progress_destroy(progress_t *bar)
{
    free(bar->pro);
}

void system_shutdown(void)
{
#ifdef _LINUX_
	//sync();
	//reboot(LINUX_REBOOT_CMD_POWER_OFF);
	if(system("shutdown --poweroff now") != 0)
		printf("ERROR: Fail to shutdown system automatically. Please shutdown PC now.");
#else // for uefi 64
	gRT->ResetSystem(EfiResetShutdown,EFI_SUCCESS,0,NULL);
#endif	
}

