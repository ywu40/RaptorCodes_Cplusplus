/*!
*
* \file   raptor_encoder.h
*
* \brief  Header file of raptor codes encoder
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

#ifndef __RAPTOR_ENCODER_H__
#define __RAPTOR_ENCODER_H__

#include <queue>

#include "raptor_data.h"
#include "raptor_triple.h"
#include "raptor_xorPos.h"

class CEncoder
{
private:
	U32* m_Mseq; // m sequence defined in RFC 5053:5.4.2.3
	CTriple* m_Triples;	
	CData* m_Intermediate;
	CData* m_Source;

	U8** m_A;
	U32* m_c;
	U32* m_d;
	std::vector<CXorPos> m_xorPos;
	
	U32 m_K;
	U32 m_S;
	U32 m_H;
	U32 m_Hp;
	U32 m_L;
	U32 m_Lp;
	U32 m_N;
	U32 m_Count;
	
public:
	void CreateIS(CData* source);


private:
	void CreateMSeq(void);
	void LTEnc(U32 k, CData* inter_sym, CTriple* triples);

	void FillA_GMatrix(void);
	void ReleaseA_GMatrix(void);

	void SolveIntermediateSym(U32 K, U32 L, U8** A, U32* c, U32* d);

public:
	CEncoder(U32 K, U32 lossNum);
	~CEncoder(void);
	void AddData(CData* source);
	std::queue<CData* > getEncodedData(void);
	std::queue<CData* > encoded_sym;
};

#endif /* __RAPTOR_ENCODER_H__ */

/* end of raptor_encoder.h */