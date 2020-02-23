#ifndef _ISP_H_
#define _ISP_H_

#include <stdio.h> 

#define PJT_NAME_SIZE		8

/*==============================================================*/
typedef struct _embeded_t{
	char			project_name[32];
	char			chip[16];
	char			ver[16];
} embeded_t;

/*==============================================================*/
typedef struct _binary_t{
    FILE        	*ctrl;                      		// file control point
    char        	*name;                    			// file name
    embeded_t   	finfo;
	uint32_t		fsize;
} binary_t;

/*==============================================================*/
typedef struct _chip_t{
	uint8_t			vid;								// IC Vender
	uint8_t			pid;								// IC Product ID
} chip_t;

/*==============================================================*/
typedef struct _cb_funs_t{
	int				(*init)(void);						// codebase init function
	void			(*exit)(void);						// codebase exit function
	int				(*get_embedic_info)(embeded_t *);	// get project information
	int				(*get_binary_info)(binary_t *);		// get binary information
	int				(*update_firmware)(binary_t *);		// update ec firmware
} cb_funs_t;

/*==============================================================*/
typedef struct _codebase_t{
	uint8_t			chip_num;							// Number of supported chips
	const chip_t	*chip_list;
	uint8_t			cbid;								// id of codebase
	cb_funs_t		func;
} codebase_t;


#endif //_ISP_H_

