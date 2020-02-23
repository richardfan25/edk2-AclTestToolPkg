//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  X86 - KEY                                                                *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
//#include <bios.h>

#include "typedef.h"
#include "key.h"

//=============================================================================
//  key_blk_read
//=============================================================================
void key_blk_read(ps2_key_t *pk)
{
	uint16_t	key;

	while (1)
	{
		key = (uint16_t)bioskey(1);
		if(key)//if (_bios_keybrd(_NKEYBRD_READY))
		{
			//key = _bios_keybrd(_NKEYBRD_READ);
			pk->sc = (key >> 8);
			pk->ac = (key & 0xFF);
			pk->kc = key;
			//pk->ss = _bios_keybrd(_NKEYBRD_SHIFTSTATUS);

			break;
		}
	} 
}

//=============================================================================
//  key_blk_read_sc
//=============================================================================
uint8_t key_blk_read_sc(void)
{
	uint16_t	key;
	uint8_t		sc;

	while (1)
	{
		key = (uint16_t)bioskey(1);
		if(key)//if (_bios_keybrd(_NKEYBRD_READY))
		{
			//key = _bios_keybrd(_NKEYBRD_READ);
			sc	= (key >> 8);

			break;
		}
	}

	return sc;
}

//=============================================================================
//  key_blk_read_ac
//=============================================================================
uint8_t key_blk_read_ac(void)
{
	uint16_t	key;
	uint8_t		ac;

	while (1)
	{
		key = (uint16_t)bioskey(1);
		if(key)//if (_bios_keybrd(_NKEYBRD_READY))
		{
			//key = _bios_keybrd(_NKEYBRD_READ);
			ac	= (key & 0xFF);

			break;
		}
	}

	return ac;
}

//=============================================================================
//  key_blk_read_ss
//=============================================================================
uint8_t key_blk_read_ss(uint16_t *shft)
{
	uint16_t	key;
	uint8_t		sc;

	while (1)
	{
		key = (uint16_t)bioskey(1);
		if(key)//if (_bios_keybrd(_NKEYBRD_READY))
		{
			//key = _bios_keybrd(_NKEYBRD_READ);

			//*shft = _bios_keybrd(_NKEYBRD_SHIFTSTATUS);
			sc	= (key >> 8);

			break;
		}
	}

	return sc;
}

//=============================================================================
//  key_non_blk_read
//=============================================================================
uint8_t key_non_blk_read(ps2_key_t *pk)
{
	uint16_t	key;

	key = (uint16_t)bioskey(1);
	if(key)//if (_bios_keybrd(_NKEYBRD_READY))
	{
		//key = _bios_keybrd(_NKEYBRD_READ);
		pk->sc = (key >> 8);
		pk->ac = (key & 0xFF);
		pk->kc = key;
		//pk->ss = _bios_keybrd(_NKEYBRD_SHIFTSTATUS);

		return pk->sc;
	}
	else
	{
		pk->sc = 0xFF;
		pk->ac = 0xFF;
		pk->kc = 0xFFFF;
		pk->ss = 0;
		return KEY_SC_NULL;
	}
}

//=============================================================================
//  key_non_blk_read_sc
//=============================================================================
uint8_t key_non_blk_read_sc(void)
{
	uint16_t	key;
	uint8_t		sc;

	key = (uint16_t)bioskey(1);
	if(key)//if (_bios_keybrd(_NKEYBRD_READY))
	{
		//key = _bios_keybrd(_NKEYBRD_READ);
		sc	= (key >> 8);
		return sc;
	}
	else
	{
		return KEY_SC_NULL;
	}
}

//=============================================================================
//  key_non_blk_read_ac
//=============================================================================
uint8_t key_non_blk_read_ac(void)
{
	uint16_t	key;
	uint8_t		ac;

	key = (uint16_t)bioskey(1);
	if(key)//if (_bios_keybrd(_NKEYBRD_READY))
	{
		//key = _bios_keybrd(_NKEYBRD_READ);
		ac	= (key & 0xFF);

		return ac;
	}
	else
	{
		return KEY_SC_NULL;
	}
}

//=============================================================================
//  key_non_blk_read_ss
//=============================================================================
uint8_t key_non_blk_read_ss(uint16_t *shft)
{
	uint16_t	key;
	uint8_t		sc;

	key = (uint16_t)bioskey(1);
	if(key)//if (_bios_keybrd(_NKEYBRD_READY))
	{
		//key = _bios_keybrd(_NKEYBRD_READ);

		//*shft = _bios_keybrd(_NKEYBRD_SHIFTSTATUS);

		sc	= (key >> 8);
		return sc;
	}
	else
	{
		return KEY_SC_NULL;
	}
}

//=============================================================================
//  key_getch
//=============================================================================
void key_getch(void)
{
	while (1)
	{
		if(bioskey(1))//if (_bios_keybrd(_NKEYBRD_READY))
			break;
	}

	// flush key buffer avoid malfunction
	while (1)
	{
		//if (_bios_keybrd(_NKEYBRD_READY))
		//	_bios_keybrd(_NKEYBRD_READ);
		bioskey(1);//else
			break;
	}
}

//=============================================================================
//  key_flush_buf
//=============================================================================
void key_flush_buf(void)
{
	while (1)
	{
		//if (_bios_keybrd(_NKEYBRD_READY))
		//	_bios_keybrd(_NKEYBRD_READ);
		bioskey(1);//else
			break;
	}
}
