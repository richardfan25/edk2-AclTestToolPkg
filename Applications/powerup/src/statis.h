//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  STATIS : Statistic
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#ifndef __STATIS_H
#define __STATIS_H


#include "typedef.h"
#include "dat.h"

#pragma pack(1)
//=============================================================================
//  pu_statis_cnt_t
//=============================================================================
typedef struct _pu_statis_cnt_t
{
	int		intv;	// interval
	int		cnt;
	uint8_t	qc;		// qc

	struct _pu_statis_cnt_t	*prev;
	struct _pu_statis_cnt_t	*next;

} pu_statis_cnt_t;

//=============================================================================
//  pu_statis_sum_t
//=============================================================================
typedef struct _pu_statis_sum_t
{
	int		total;
	int		pass;
	int		fail;

	int		total_cnt;
	int		pass_cnt;
	int		fail_cnt;

} pu_statis_sum_t;

//=============================================================================
//  pu_statis_avr_t
//=============================================================================
typedef struct _pu_statis_avr_t
{
	double	total;
	double	pass;
	double	fail;

	double	tvar;	// total variance
	double	stdv;	// standard deviation

} pu_statis_avr_t;

//=============================================================================
//  pu_statis_t
//=============================================================================
typedef struct _pu_statis_t
{
	int		num;

	pu_statis_sum_t		sum;
	pu_statis_avr_t		avr;

	pu_statis_cnt_t		*head;
	pu_statis_cnt_t		*curr;

} pu_statis_t;
#pragma pack()
//=============================================================================
//  extern
//=============================================================================


#endif
