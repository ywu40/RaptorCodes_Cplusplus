/*!
*
* \file   raptor_xorPos.h
*
* \brief  Header file of Recording the position of XOR operation
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

#ifndef __RAPTOR_XOR_H__
#define __RAPTOR_XOR_H__

#include <string.h>
#include "raptor_types.h"

class CXorPos
{
private:

  U32  m_orig;
  U32  m_dest;

public:

  CXorPos(U32 orig, U32 dest): m_orig(orig), m_dest(dest)
  {
  }

  CXorPos(const CXorPos& other)
  {
    m_orig = other.m_orig;
    m_dest = other.m_dest;
  }

  U32 getOrigPos(void)
  {
    return m_orig;
  }

  U32 getDestPos(void)
  {
    return m_dest;
  }
};

#endif /* __RAPTOR_XOR_H__ */

/* end of raptor_xorPos.h */