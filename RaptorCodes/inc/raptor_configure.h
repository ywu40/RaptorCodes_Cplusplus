/*!
*
* \file   raptor_configure.h
*
* \brief  Header file of configuring the parameters of raptor codes encoding and transmission
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

#ifndef __RAPTOR_CONFIGURE_H__
#define __RAPTOR_CONFIGURE_H__

#include "raptor_types.h"

class CConfigure
{
private:
	U32 m_F;
	U32 m_A1;		
	U32 m_W;
	U32 m_P;
	U32 m_Kmax;
	U32 m_Kmin;
	U32 m_Gmax;

	U32 m_T;
	U32 m_K;
	U32 m_Z;
	U32 m_N;
	U32 m_G;
	U32 m_Kt;

private:
	U32 CalculateT(void);
	U32 CalculateK(void);
	U32 CalculateZ(void);
	U32 CalculateN(void);
	U32 CalculateG(void);
	U32 CalculateKt(void);

public:
	CConfigure(U32 F, U32 A1, U32 W, U32 P, U32 Kmax, U32 Kmin, U32 Gmax);
	~CConfigure(void);

	//The Raptor encoder and decoder require the following information from the CDP
	U32  GetF(void)  const { return m_F; }
	U32  GetA1(void) const { return m_A1;}
	U32  GetT(void)  const { return m_T; }
	U32  GetK(void)  const { return m_K; }
	U32  GetZ(void)  const { return m_Z; }
	U32  GetN(void)  const { return m_N; }	
	U32  GetG(void)  const { return m_G; }
	U32  GetKt(void) const { return m_Kt;}
	
	/*
	 * The Raptor encoder supplies the CDP with the following information
	 * for each packet to be sent:
	 */

	// The intermediate step for calculating the encoder parameter
	U32 GetP(void)    const { return m_P; }
	U32 GetKmin(void) const { return m_Kmin; }
	U32 GetKmax(void) const { return m_Kmax; }
	U32 GetGmax(void) const { return m_Gmax; }
	U32 GetW(void)    const { return m_W; }	
};

#endif /* __RAPTOR_CONFIGURE_H__ */

/* end of raptor_configure.h */