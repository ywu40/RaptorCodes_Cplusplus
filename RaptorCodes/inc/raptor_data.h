/*!
*
* \file   raptor_data.h
*
* \brief  Header file of data operation and XOR
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

#ifndef __RAPTOR_DATA_H__
#define __RAPTOR_DATA_H__

#include <string.h>
#include "raptor_types.h"

class CData
{
private:
  U8   *m_Data;
  U32  m_Len;

public:

  CData(void): m_Data(NULL), m_Len(0)
  {
  }

  CData(U8 * data, U32 len);
  CData(CData &other);

  ~CData(void)
  {
    FreeData();
  }

  const U8 * GetData(void) const { return m_Data; }
  U32 GetLen(void) const { return m_Len; }

  void FreeData(void);

  void SetData(CData * data);
  void SetData(const U8 * data, U32 len);

  void XorData(CData * data);
  void XorData(const U8 * data, U32 len);
};

#endif /* __RAPTOR_DATA_H__ */

/* end of raptor_data.h */