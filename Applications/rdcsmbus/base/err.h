#ifndef _ERR_H_
#define _ERR_H_

#define _DEBUG_LVL		0

#define ERROR_HEAD		"\nERROR: "


#if _DEBUG_LVL = 0
#define PNT_ERR(a)
#else
#define PNT_ERR(a)		pnt_err(a)
#endif

extern unsigned char errno;

#endif // _ERR_H_

