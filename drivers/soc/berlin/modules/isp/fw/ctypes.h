/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2021 - 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef CTYPES_H_
#define CTYPES_H_

// GNU C/C++
#if defined (__GNUC__)

typedef	unsigned char       UNSG8;
typedef	signed char         SIGN8;
typedef	unsigned short      UNSG16;
typedef	signed short        SIGN16;
typedef	unsigned int        UNSG32;
typedef	signed int          SIGN32;
typedef	unsigned long long  UNSG64;
typedef	signed long long    SIGN64;
typedef	float               REAL32;
typedef	double              REAL64;

#ifndef INLINE
#define INLINE	(static inline)
#endif

#endif

// NULL
#ifndef NULL
#ifdef __cplusplus
#define NULL            0
#else
#define NULL            ((void *)0)
#endif
#endif

// Multiple-word types
#ifndef	Txxb
#define	Txxb
typedef	UNSG8				T8b;
typedef	UNSG16				T16b;
typedef	UNSG32				T32b;
typedef	UNSG32				T64b[2];
typedef	UNSG32				T96b[3];
typedef	UNSG32				T128b[4];
typedef	UNSG32				T160b[5];
typedef	UNSG32				T192b[6];
typedef	UNSG32				T224b[7];
typedef	UNSG32				T256b[8];
#endif

#endif