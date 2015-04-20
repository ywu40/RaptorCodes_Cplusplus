/*!
*
* \file   raptor_common.cpp
*
* \brief  Implementation of general functions in raptor encoder and decoder
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

#include <limits.h>
#include <math.h>

#include "raptor_defines.h"
#include "raptor_common.h"
#include "raptor_tables.h"

U32 NonZeroBits(U32 num)
{
	U32 count = 0;
	for (U32 i = 0; i < 32; ++i)
	{
		if ((num & 0x01) != 0)
		{
			count++;
		}

		num >>= 1;		
		if (num == 0) 
		{
			break;
		}
	}

	return count;
}

BOOL IsBitSet(U32 num, U32 pos)
{
	num >>= pos;
	if ((num & 0x01) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

DOUBLE Fact(U32 n)
{
	if (n == 0) 
	{
		return 1.0;
	}

	DOUBLE res = n;
	for (U32 i = n - 1; i > 0; --i)
	{
		res *= i;
	}

	return res;
}

U32 Comb(U32 n, U32 k)
{
	U32 res = (U32)(Fact(n) / (Fact(k) * Fact(n - k)));   

	return res;
}

BOOL IsPrime(U32 num)
{
	// no number can divide into 0
	if(num == 0)
	{
		return true;
	}

	U32 end_value = (U32)sqrt((FLOAT)num);
	for(U32 i = 2; i <= end_value; i++)
	{
		if((num % i) == 0)
		{
			return false;
		}
	}

	return true;   
}

/* See RFC 5053:5.4.2.3*/
U32 GetX(U32 K)
{
	U32 X;
	// Get X value
	for (X = 0; X < UINT_MAX; ++X)
	{
		if (X * (X - 1) >= 2 * K)
		{
			break;
		}
	}

	return X;
}

U32 GetS(U32 K)
{
	U32 X,S;
	X = GetX(K);
	//Get S value
	for (S = 0; S < UINT_MAX; ++S)
	{
		if ((S >= ceil(0.01 * (DOUBLE)K) + X) && IsPrime(S))
		{
			break;
		}
	}

	return S;
}

U32 GetH(U32 K, U32 S)
{
	U32 H;
	//Get H value
	for (H = 0; H < UINT_MAX; ++H)
	{
		if (Comb(H,(long)ceil((DOUBLE)H / 2.0)) >= (K + S))
		{
			break;
		}
	}

	return H;
}

U32 GetHp(U32 H)
{
	return (U32)ceil((DOUBLE)H / 2.0);
}

U32 GetL(U32 K)
{
	U32 S,H;
	S = GetS(K);	
	H = GetH(K,S);
	U32 L = K + S + H;	
	return L;
}

U32 GetLPrim(U32 K)
{
	U32 L = GetL(K);
	while(!IsPrime(L))
	{
		L++;
	}
	return L;
}

/* See RFC 5053:5.4.4.2*/
U32 Deg(U32 v)
{	
	for (U32 j = 1; j < f_len; ++j)
	{
		if ((f[j - 1] <= v) && (v < f[j]))
		{
			return d[j];
		}
	}

	ERR("CTripleGenerator::Deg: Can't find valid Degree! aborting...\n");

	return 0;
}

/* See RFC 5053:5.4.4.1*/
U32 Rand(U32 X, U32 i, U32 m)
{	
	return (V0[(X + i) % 256] ^ V1[(X / 256 + i) % 256]) % m;
}