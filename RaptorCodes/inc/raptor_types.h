/*!
*
* \file   raptor_types.h
*
* \brief  Header file of the types declaration for porting
*
*
* \par    Include files
*         -     
*
* \par    Copyright (c) 2011 Yeqing Wu
*
* This library is free software; you can redistribute it and/or modify it 
* under the terms of the GNU Lesser General Public License as published by 
* the Free Software Foundation. This software is developed by refering to the software wrote by Vicente Sirvent,
* but many parts(especially some critical parts) of the raptor codes in that software have not been implemented 
* and there are lots of bugs and logical errors in that software. So I re-design software and implement it.
*
*
* This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
* See the GNU Lesser General Public License for more details.
* You should have received a copy of the GNU Lesser General Public License along with this library; 
* if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
* 
* \version 
* Revision of last commit: $Rev$ 
* Author of last commit  : $Author$ 
* Date of last commit    : $Date$ 
*
*/

#ifndef __RAPTOR_TYPES_H__
#define __RAPTOR_TYPES_H__

#include <wchar.h>

//#define TEST_PURPOSE
#define SHOW_NUMS     10

#define ROW_ONES_NUM 10000
/**********************************************************************************
** Type Definitions
***********************************************************************************/
typedef unsigned char  U8,  *PU8;
typedef unsigned short U16, *PU16;
typedef unsigned int   U32, *PU32;
typedef signed char    S8,  *PS8;
typedef signed short   S16, *PS16;
typedef signed int     S32, *PS32;

typedef signed int     BOOL;
typedef char           CHAR;
typedef int            INT;

#if defined(WIN32) || defined(_WIN32)
typedef __int64          S64;
typedef unsigned __int64 U64;
#else
typedef long long          S64;
typedef unsigned long long U64;
#endif

typedef wchar_t WCHAR;

typedef float   FLOAT;
typedef double  DOUBLE;
/**********************************************************************************
** Keywords Definitions
***********************************************************************************/
/*!
* API function calling convention
* win32: __stdcall, __cdecl or __fastcall
* others: none
*/
#ifdef WIN32
#ifndef RAPTOR_API
#define RAPTOR_API __stdcall
#endif
#else
#ifndef RAPTOR_API
#define RAPTOR_API
#endif
#endif

/*!
* NULL definition
*/
#ifndef NULL
#define NULL    (void *)0
#endif

/*!
* IN used as the input parameter descriptor
*/
#define IN

/*!
* OUT used as the output parameter descriptor
*/
#define OUT

/*!
* INOUT used as the i/o parameter descriptor
*/
#define INOUT

/*!
* true/false flag 
*/
#define TRUE  1
#define FALSE 0

/*!
*  MAX_U32 / MAX_U16 / MAX_U8
*/
#define MAX_U32    ((U32)0xFFFFFFFF)
#define MAX_U16    ((U16)0xFFFF)
#define MAX_U8     ((U8)0xFF)


#endif /* __RAPTOR_TYPES_H__ */

/* end of raptor_types.h */