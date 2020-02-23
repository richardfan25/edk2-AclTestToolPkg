/*
 * Advantech Emb'Core Cross OS Library
 * Copyright (c) Advantech Co., Ltd. All Rights Reserved
 */
#ifndef _DECLARATIONS_H_
#define _DECLARATIONS_H_

#if defined(_MSC_VER)
	#if (_MSC_VER < 1300)
		typedef char				int8_t;
		typedef short				int16_t;
		typedef int					int32_t;
		typedef unsigned char		uint8_t;
		typedef unsigned short		uint16_t;
		typedef unsigned int		uint32_t;
	#else
		typedef __int8				int8_t;
		typedef __int16				int16_t;
		typedef __int32				int32_t;
		typedef unsigned __int8		uint8_t;
		typedef unsigned __int16	uint16_t;
		typedef unsigned __int32	uint32_t;
	#endif

	typedef __int64					int64_t;
	typedef unsigned __int64		uint64_t;
#else
	typedef char					int8_t;
	typedef short					int16_t;
	typedef int						int32_t;
	typedef unsigned char			uint8_t;
	typedef unsigned short			uint16_t;
	typedef unsigned int			uint32_t;
	typedef long long				int64_t;
	typedef unsigned long long		uint64_t;
#endif

#endif /* _DECLARATIONS_H_ */
