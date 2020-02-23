//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - DPMI (DOS Protected Mode Interface)                                *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include "typedef.h"

#ifdef __WATCOMC__
//#include <dos.h>
#include <string.h>

#include "dpmi.h"

//=============================================================================
//  __dpmi_int
//=============================================================================
int __dpmi_int(unsigned int_num, __dpmi_regs *rm_regs)
{
	/*struct SREGS sregs;
	union REGS regs;

	memset(&sregs, 0, sizeof(sregs));
	memset(&regs, 0, sizeof(regs));

	sregs.es = FP_SEG(rm_regs);
	regs.x.edi = FP_OFF(rm_regs);
	regs.x.eax = 0x0300;		// simulate real-mode interrupt
	regs.x.ebx = int_num;		// AH=flags=0
	regs.x.ecx = 0;				// number of words to copy between stacks
	int386x(0x31, &regs, &regs, &sregs);

	return regs.w.cflag ? -1 : 0;*/
	return -1;
}

//=============================================================================
//  __dpmi_allocate_dos_memory
//=============================================================================
int __dpmi_allocate_dos_memory(unsigned paragraphs, int *sel)
{
	/*union REGS regs;

	memset(&regs, 0, sizeof(regs));

	regs.w.ax = 0x0100;
	regs.w.bx = paragraphs;
	int386(0x31, &regs, &regs);
	*sel = regs.w.dx;

	return regs.w.cflag ? -1 : regs.w.ax;*/
	return -1;
}

//=============================================================================
//  __dpmi_free_dos_memory
//=============================================================================
int __dpmi_free_dos_memory(int sel)
{
	/*union REGS regs;

	memset(&regs, 0, sizeof(regs));

	regs.w.ax = 0x0101;
	regs.w.dx = sel;
	int386(0x31, &regs, &regs);

	return regs.w.cflag ? -1 : regs.w.ax;*/
	return -1;
}

//=============================================================================
//  dpmi_physical_address_mapping
//=============================================================================
int dpmi_physical_address_mapping(__dpmi_meminfo *info)
{
	/*union REGS regs;

	memset(&regs, 0, sizeof(regs));

	regs.w.ax = 0x0800;
	regs.w.bx = info->address >> 16;
	regs.w.cx = info->address;
	regs.w.si = info->size >> 16;
	regs.w.di = info->size;
	int386(0x31, &regs, &regs);

	info->address = ((unsigned long)regs.w.bx << 16) | regs.w.cx;
	return regs.w.cflag ? -1 : 0;*/
	return -1;
}

//=============================================================================
//  real_mode_int_es_di
//=============================================================================
int real_mode_int_es_di(void *buf, unsigned buf_size, __dpmi_regs *regs, unsigned int_num)
{
	int seg, sel;

	/* allocate conventional memory block (CMB) */
	seg = __dpmi_allocate_dos_memory((buf_size + 15) / 16, &sel);
	if(seg == -1)
	{
		//printf("Error: can't allocate conventional memory\n");
		return -1;
	}
	
	/* copy buffer to CMB */
	dosmemput(buf, buf_size, seg * 16);

	/* point real-mode ES and DI to CMB and do interrupt */
	regs->x.es = (uint16_t)seg;
	regs->x.di = 0;

	__dpmi_int(int_num, regs);

	/* copy CMB back to buffer */
	dosmemget(seg * 16, buf_size, buf);

	/* free CMB */
	__dpmi_free_dos_memory(sel);
	return 0;
}

//=============================================================================
//  real_mode_int_es_bp
//=============================================================================
#if 0
int real_mode_int_es_bp(void *buf, unsigned buf_size, __dpmi_regs *regs, unsigned int_num)
{
	int seg, sel;

	/* allocate conventional memory block (CMB) */
	seg = __dpmi_allocate_dos_memory((buf_size + 15) / 16, &sel);
	if(seg == -1)
	{
		//printf("Error: can't allocate conventional memory\n");
		return -1;
	}
	
	/* copy buffer to CMB */
	dosmemput(buf, buf_size, seg * 16);

	/* point real-mode ES and DI to CMB and do interrupt */
	regs->x.es = seg;
	regs->x.di = 0;
	regs->x.bp = 0;

	__dpmi_int(int_num, regs);

	/* copy CMB back to buffer */
	dosmemget(seg*16 + regs->x.bp, buf_size, buf);

	/* free CMB */
	__dpmi_free_dos_memory(sel);
	return 0;
}
#endif

#endif
