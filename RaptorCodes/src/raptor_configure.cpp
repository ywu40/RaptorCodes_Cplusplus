/*!
*
* \file   raptor_configure.cpp
*
* \brief  Implementation of providing and generating parameter by raptor encoder and decoder
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
#include <assert.h> 

#include "raptor_configure.h"

#define MIN(x, y) ((x < y) ? x : y)

CConfigure::CConfigure(U32 F, U32 A1, U32 W, U32 Kmax, U32 Kmin, U32 Gmax, U32 P)
                     : m_F(F), m_A1(A1), m_W(W), m_Kmax(Kmax), m_Kmin(Kmin), m_Gmax(Gmax)
{
  assert((P % m_A1) == 0);
  m_P = P;
  m_T = CalculateT();
  m_K = CalculateK();
  m_Z = CalculateZ();
  m_N = CalculateN();
  m_G = CalculateG();
  m_Kt = CalculateKt();
}

CConfigure::~CConfigure(void)
{
}

U32 CConfigure::CalculateT(void)
{
  U32 t_value = (m_P / (m_A1 * m_G)) * m_A1;
  assert((t_value % m_A1) == 0);

  return t_value;
}

U32 CConfigure::CalculateK(void)
{
  return m_K;
}

U32 CConfigure::CalculateZ(void)
{
  return (U32)ceil((DOUBLE)m_Kt / m_Kmax);
}

U32 CConfigure::CalculateN(void)
{
  return (U32)MIN(ceil((DOUBLE)ceil((DOUBLE)m_Kt / m_Z) * m_T / m_W), m_T / m_A1);
}

U32 CConfigure::CalculateG(void)
{
  return (U32)MIN(MIN(ceil((DOUBLE)m_P * m_Kmin / m_F), m_P / m_A1), m_Gmax);
}

U32 CConfigure::CalculateKt(void)
{
  return (U32)ceil((DOUBLE)m_F / m_T);
}