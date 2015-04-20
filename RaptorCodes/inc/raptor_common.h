/*!
*
* \file   raptor_common.h
*
* \brief  Header file of general functions in raptor encoder and decoder
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

#ifndef __RAPTOR_COMMON_H__
#define __RAPTOR_COMMON_H__

#include "raptor_types.h"

#define MIN(x, y) ((x < y) ? x : y)

DOUBLE Fact(U32 n);
U32    Comb(U32 n, U32 k);
BOOL   IsPrime(U32 num);
U32    NonZeroBits(U32 num);
BOOL   IsBitSet(U32 num, U32 pos);

/* See RFC 5053:5.4.4.1*/
U32    Rand(U32 X, U32 i, U32 m);
/* See RFC 5053:5.4.4.2*/
U32    Deg(U32 v);
/* See RFC 5053:5.4.2.3*/
U32    GetX(U32 K);
U32    GetS(U32 K);
U32    GetH(U32 K, U32 S);
U32    GetHp(U32 H);
U32    GetL(U32 K);
U32    GetLPrim(U32 K);

#endif /* __RAPTOR_COMMON_H__ */

/* end of raptor_common.h */