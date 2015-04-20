/*!
*
* \file   raptor_data.cpp
*
* \brief  Implementation of raptor codes data operation APIs
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

#include "raptor_data.h"

CData::CData(U8 * data, U32 len): m_Data(NULL), m_Len(0)
{
	SetData(data, len);
}

CData::CData(CData &other): m_Data(NULL), m_Len(0)
{
	SetData(other.m_Data, other.m_Len);
}

void CData::FreeData(void)
{
	if (m_Data)
	{
		delete[] m_Data;
		m_Data = NULL;
	}	
}

void CData::SetData(CData* data)
{
	return SetData(data->GetData(), data->GetLen());
}

void CData::SetData(const U8* data, U32 len)
{
	FreeData();
	if (len > 0 && data)
	{
		m_Data = new U8[len];
		memcpy(m_Data, data, len);
		m_Len = len;
	}
}

void CData::XorData(CData* data)
{
	return XorData(data->GetData(), data->GetLen());
}

void CData::XorData(const U8* data, U32 len)
{
	if (len > m_Len)
	{
		U8* tmp = new U8[len];
		memset(tmp, 0, len);
		if (m_Len > 0)
		{
			memcpy(tmp, m_Data, m_Len);
		}
		FreeData();
		m_Data = tmp;
		m_Len = len;
	}
	for (U32 i = 0; i < m_Len; ++i)
	{
		m_Data[i] ^= data[i];
	}
}

