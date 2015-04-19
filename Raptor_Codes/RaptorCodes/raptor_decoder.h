/*!
*
* \file   raptor_decoder.h
*
* \brief  Header file of raptor codes decoder
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

#ifndef __RAPTOR_DECODER_H__
#define __RAPTOR_DECODER_H__

#include <queue>

#include "raptor_defines.h"
#include "raptor_types.h"
#include "raptor_data.h"

class CDecoder
{
private:
	U8** m_A;

	U16* m_Degree; //record the number of 1s in each row

	U32  m_K;
	U32  m_S;
	U32  m_H;
	U32  m_Hp;
	U32  m_L;
	U32  m_Lp;
	U32  m_N;
	U32  m_lossNum;
	std::vector<U32> m_ESI;

	void FillAMatrix(void);
	void GetAMatrix(U8** A,U32 M,U32 L);
	void ReleaseAMatrix(void);

	void SolveRevisedGauss(U32 M, U32 L, U8** A, CData** C, CData** D);

	std::queue<CData* > GetIntermediateSymbols(std::queue<CData* > encoded);

public:

	CDecoder(U32 K, U32 N, U32 lossNum, std::vector<U32> ESI);
	~CDecoder(void);

	std::queue<CData* > Decode(std::queue<CData* > encoded);	
};

#endif /* __RAPTOR_DECODER_H__ */

/* end of raptor_decoder.h */