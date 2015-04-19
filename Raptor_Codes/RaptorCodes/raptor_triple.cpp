/*!
*
* \file   raptor_triple.cpp
*
* \brief  Implementation of generating triple which is used in raptor encoder and decoder
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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "raptor_defines.h"
#include "raptor_triple.h"
#include "raptor_tables.h"
#include "raptor_common.h"

/* See RFC 5053:5.4.4.4 */
CTriple CTripleGenerator::Trip(U32 K, U32 X)
{
	CTriple res;
	if (m_K != K)
	{
		m_K = K;
		m_L = GetLPrim(K);
	}

	U32 A = (53591 + J[K] * 997) % m_Q;
	U32 B = 10267 * (J[K] + 1) % m_Q;
	U32 Y = (B + X * A) % m_Q;
	U32 v = Rand(Y, 0, (U32)pow(2.0f, 20.0f));
	res.d = Deg(v);
	res.a = 1 + Rand(Y, 1, m_L - 1);
	res.b = Rand(Y, 2, m_L);
	return res;
}