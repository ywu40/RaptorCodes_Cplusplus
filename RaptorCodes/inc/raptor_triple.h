/*!
*
* \file   raptor_triple.h
*
* \brief  Header file of generating triple
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

#ifndef __RAPTOR_TRIPLE_H__
#define __RAPTOR_TRIPLE_H__

#include "raptor_types.h"

struct CTriple
{
	U32 d;
	U32 a;
	U32 b;		
};

class CTripleGenerator
{
private:

	static const U32 m_Q = 65521;
	U32 m_K;
	U32 m_L;

public:

	/* See RFC 5053:5.4.4.4 */
	CTriple Trip(U32 K, U32 X);
};

#endif /* __RAPTOR_TRIPLE_H__ */

/* end of raptor_triple.h */