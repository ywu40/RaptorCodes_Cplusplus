/*!
*
* \file   raptor_encoder.h
*
* \brief  Implementation of raptor codes encoder to generate encoded symbols
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

#include "raptor_encoder.h"
#include "raptor_common.h"
#include "raptor_defines.h"

using std::queue;
using std::vector;

#define EXTRA_NUMS 8

CEncoder::CEncoder(U32 K, U32 lossNum)
{
	m_K = K;
	m_S = GetS(K);
	m_H = GetH(K,m_S);
	m_Hp = GetHp(m_H);
	m_L = GetL(K);
	m_Lp = GetLPrim(K);
	m_N = m_K + lossNum + EXTRA_NUMS;
	m_Count = 0;
#ifndef TEST_PURPOSE
	printf("S value: %d, H value: %d\n", m_S, m_H);
#endif

	//Load triples
	m_Triples = new CTriple[m_N];
	CTripleGenerator triple_gen;
	for (U32 x = 0; x < m_N; ++x)
	{
		m_Triples[x] = triple_gen.Trip(m_K, x);
	}

	//alloc struct for data
	m_Intermediate = new CData[m_L];
	m_Source = new CData[m_L];
	memset(m_Source, 0, m_L * sizeof(CData));

	m_c = new U32[m_L];
	m_d = new U32[m_L];
	for (U32 i = 0; i < m_L; ++i)
	{
		m_d[i] = i;
		m_c[i] = i;
	}

	m_xorPos.reserve(m_L);

	//Fill m sequence
	CreateMSeq();

	m_A = NULL;
	FillA_GMatrix();
	SolveIntermediateSym(m_L, m_L, m_A, m_c, m_d);
}

CEncoder::~CEncoder(void)
{
	delete[] m_Mseq;
	m_Mseq = NULL;

	delete[] m_Triples;
	m_Triples = NULL;

	delete[] m_Intermediate;
	m_Intermediate = NULL;

	delete[] m_Source;
	m_Source = NULL;

	//free resources
	delete[] m_c;
	m_c = NULL;
	delete[] m_d;
	m_d = NULL;
}

void CEncoder::ReleaseA_GMatrix(void)
{
	if (m_A)
	{
		for (U32 i = 0; i < m_L; ++i)
		{
			delete [] m_A[i];
		}
	}
	delete[] m_A;
	m_A = NULL;
}

void CEncoder::FillA_GMatrix(void)
{
	//Free A matrix
	ReleaseA_GMatrix();
	//Alloc A matrix
	m_A = new U8*[m_L]; //Max number of rows that M can achieve
	for (U32 i = 0; i < m_L; ++i)
	{
		m_A[i] = new U8[m_L];
		memset(m_A[i], 0, m_L);
	}

	for (U32 i = 0; i < m_K; ++i)
	{
		U32 a = 1 + (i/m_S % (m_S - 1));
		U32 b = i % m_S;
		m_A[b][i] = 1;
		b = (b + a) % m_S;
		m_A[b][i] = 1;
		b = (b + a) % m_S;
		m_A[b][i] = 1;
	}

	//Fill I_S Matrix
	for (U32 i = 0; i < m_S; ++i)
	{
		m_A[i][i + m_K] = 1;
	}

	//Fill I_H Matrix
	for (U32 i = m_S; i < m_S + m_H; ++i)
	{
		m_A[i][i + m_K] = 1;
	}

	//Fill m sequence
	U32 mseq_len = m_K + m_S;
	U32* Mseq = new U32[mseq_len];
	for (U32 mcount = 0, i = 0; mcount < mseq_len; ++i)
	{ 
		U32 g = i ^ (i / 2);
		if (NonZeroBits(g) == m_Hp)
		{
			Mseq[mcount] = g;
			mcount++;
		}
	}	

	//Fill G_Half Matrix
	for (U32 h = 0; h < m_H; ++h)
	{
		for (U32 j = 0; j < m_K + m_S; ++j)
		{
			if (IsBitSet(Mseq[j],h))
			{
				m_A[h + m_S][j] = 1;
			}
		}
	}
	delete[] Mseq;

	//Fill GL_T matrix
	//Load triples
	for (U32 i = 0; i < m_K; ++i)
	{
#ifdef TEST_PURPOSE
		printf("ESI value£º%d\n", m_ESI[i]);
#endif
		U32 a = m_Triples[i].a;
		U32 b = m_Triples[i].b;
		U32 d = m_Triples[i].d;
		while (b >= m_L)
		{
			b = (b + a) % m_Lp;
		}
		/*
		G_LT(i,j) = 1 if and only if C[j] is included in the
		symbols that are XORed to produce LTEnc[K, (C[0], ..., C[L-1]),
		(d[i], a[i], b[i])].
		*/
		m_A[i + m_S + m_H][b] = 1;
		U32 jend = MIN(d, m_L);
		for (U32 j = 1; j < jend;++j)
		{
			b = (b + a) % m_Lp;
			while (b >= m_L)
			{
				b = (b + a) % m_Lp;
			}
			m_A[i + m_S + m_H][b] = 1;
		}		
	}

#ifdef TEST_PURPOSE
	printf("\nA matrix row number is£º%d£¬column number£º%d\n", m_L, m_L);
	for (U32 i = 0; i < m_L; ++i)
	{
		for (U32 j = 0; j < m_L; ++j)
		{
			printf("%d ", m_A[i][j]);
		}
		printf("\n");
	}
#endif
}

void CEncoder::SolveIntermediateSym(U32 M, U32 L, U8** A, U32* c, U32* d)
{

	/*<----i-------->                 <----u---->       
	/*+-------------+-----------------+---------+
	|			  |					|		  |
	|		 I	  |	 All Zeros      |		  |
	|			  |				    |		  |
	+-------------+-----------------+	U     |
	|			  |				    |		  |
	|			  |				    |		  |
	| All Zeros   |		V			|		  |
	|			  |					|		  |
	|			  |					|		  |
	+-------------+-----------------+---------+
	*/

	/*-----------------------------------First Phase------------------------------------*/
	U32 i = 0; // i is the rank of Identity Matrix I
	U32 u = 0; // u is the column number of Matrix U
	while (i + u < L)
	{
		//Let r be the minimum integer such that at least one row of A has
		//exactly r ones in V.
		U32 ones_pos[ROW_ONES_NUM];	
		bool isRowFound = false;
		U32 nrow = 0;		
		U32 r = 1;
		U32 endPoint = L - i - u;

		while (r <= endPoint) //from n row
		{
			for (U32 s = i; s < M; ++s)
			{
				U32 row_ones = 0;
				U32 row_ones_pos[ROW_ONES_NUM];
				U8* row = A[s];
				U32 posIndex = 0;
				for (U32 j = i; j < L - u; ++j) //from take out first i column from I matrix and last u column from U matrix
				{
					if (row[j])
					{
						row_ones++;
						row_ones_pos[posIndex] = j;
						posIndex++;
					}
				}

				if (row_ones == r)
				{
					isRowFound = true;
					nrow  = s;
					memcpy(ones_pos, row_ones_pos, ROW_ONES_NUM);
					break;
				}
			}

			if (isRowFound == true)
			{
				break;
			}
			else
			{
				//Don't have a row with r ones, increase the 1 number of a row to search
				r += 1;
			}

		}

		//TODO: Now we must work with this row
		//TODO: if nones == 2 we must choose between the maximum size component (See RFC 5053:5.5.2.2)

		if (isRowFound == false)
		{
			printf("First Phase Decoding Fail!");
			return;
		}

		//Move row to first V row
		if (i != nrow)
		{
			U8* tmp_row = A[i];
			A[i] = A[nrow];
			A[nrow] = tmp_row;

			U32 tmp_encoded = d[i];
			d[i] = d[nrow];
			d[nrow] = tmp_encoded;
		}

		if (A[i][i] == 0) 
		{
			U32 firstOnePos = ones_pos[0];
			// Change column i and column firstOnePos
			for (U32 rowIndex = 0; rowIndex < M; ++rowIndex)
			{
				U8 tmp_value = A[rowIndex][i];
				A[rowIndex][i] = A[rowIndex][firstOnePos];
				A[rowIndex][firstOnePos] = tmp_value;
			}

			U32 tmp_value = c[i];
			c[i] = c[firstOnePos];
			c[firstOnePos] = tmp_value;
		}

		for (U32 h = 1; h < r; ++h)
		{
			U32 onePos = ones_pos[h];
			U32 changedPos = L - u - 1;
			for (U32 rowIndex = 0; rowIndex < M; ++rowIndex)
			{
				U8 tmp_value = A[rowIndex][changedPos];
				A[rowIndex][changedPos] = A[rowIndex][onePos];
				A[rowIndex][onePos] = tmp_value;
			}

			U32 tmp_value = c[onePos];
			c[onePos] = c[changedPos];
			c[changedPos] = tmp_value;

			u += 1;
		}

		for (U32 k = (i + 1); k < M; ++k)
		{
			if (A[k][i] == 1)
			{
				for (U32 l = 0; l < L; ++l)
				{
					A[k][l] ^= A[i][l];				
				}

				CXorPos xorPos(d[i], d[k]);
				m_xorPos.push_back(xorPos);
			}
		}

		i += 1;	
	}

#ifdef TEST_PURPOSE
	printf("\n");
	printf("\nA Matrix after First Phase£º\n");
	for (U32 rowIndex = 0; rowIndex < M; rowIndex++)
	{
		for (U32 columnIndex = 0; columnIndex < L; columnIndex++)
		{
			printf("%d ", A[rowIndex][columnIndex]);
		}
		printf("\n");
	}
#endif

	/*-----------------------------------Second Phase------------------------------------*/
	U32 rowIndex;
	for (U32 colIndex = i; colIndex < L; ++colIndex)
	{
		rowIndex = colIndex;
		while ((rowIndex < M) && (A[rowIndex][colIndex] == 0))
		{
			rowIndex += 1;
		}

		if (rowIndex == M)
		{
			printf("Second Phase Decoding Fail! Column Num: %d\n", colIndex);

#ifdef TEST_PURPOSE
			printf("\n");
			printf("\nA Matrix after 2nd Phase£º\n");
			for (U32 rowIndex = 0; rowIndex < M; rowIndex++)
			{
				for (U32 columnIndex = 0; columnIndex < L; columnIndex++)
				{
					printf("%d ", A[rowIndex][columnIndex]);
				}
				printf("\n");
			}
#endif
			return;
		}

		if (colIndex != rowIndex)
		{
			U8* tmp_row = A[colIndex];
			A[colIndex] = A[rowIndex];
			A[rowIndex] = tmp_row;

			U32 tmp_encoded = d[colIndex];
			d[colIndex] = d[rowIndex];
			d[rowIndex] = tmp_encoded;
		}

		for (U32 k = i; k < M; k++)
		{
			if (k != colIndex)
			{
				if (A[k][colIndex] == 1)
				{
					for (U32 l = i; l < L; l++)
					{
						A[k][l] ^= A[colIndex][l];	
					}

					CXorPos xorPos(d[colIndex], d[k]);
					m_xorPos.push_back(xorPos);
				}
			}
		}
	}

	/*-----------------------------------Third Phase------------------------------------*/
	for (U32 k = 0; k < i; ++k)
	{
		for (U32 s = i; s < L; ++s)
		{
			if (A[k][s] == 1)
			{
				A[k][s] ^= A[s][s];
				CXorPos xorPos(d[s], d[k]);
				m_xorPos.push_back(xorPos);
			}
		}
	}

#ifdef TEST_PURPOSE
	printf("\n");
	printf("\nA Matrix after Gaussian Elimination£º\n");
	for (U32 rowIndex = 0; rowIndex < L; rowIndex++)
	{
		for (U32 columnIndex = 0; columnIndex < L; columnIndex++)
		{
			printf("%d ", A[rowIndex][columnIndex]);
		}
		printf("\n");
	}
#endif

}

void CEncoder::CreateMSeq(void)
{
	//Fill m sequence
	U32 mseq_len = m_K + m_S;
	m_Mseq = new U32[mseq_len];
	for (U32 mcount = 0, i = 0; mcount < mseq_len; ++i)
	{
		U32 g = i ^ (i / 2);
		if (NonZeroBits(g) == m_Hp)
		{
			m_Mseq[mcount] = g;
			mcount++;
		}
	}
}

queue<CData*> CEncoder::getEncodedData(void)
{
	return encoded_sym;
}

void CEncoder::LTEnc(U32 k, CData* inter_sym, CTriple* triples)
{
#ifdef TEST_PURPOSE
	printf("\n\nOutput Encoded Symbols: \n");
#endif

	for (U32 i = 0; i < m_N; ++i)
	{
		U32 a = triples[i].a;
		U32 b = triples[i].b;
		U32 d = triples[i].d;
		CData* result = new CData();
		while (b >= m_L)
		{
			b = (b + a) % m_Lp;
		}
		result->SetData(inter_sym + b);
		U32 jend = MIN(d,m_L);
		for (U32 j = 1; j < jend; ++j)
		{
			b = (b + a) % m_Lp;
			while (b >= m_L)
			{
				b = (b + a) % m_Lp;
			}
			result->XorData(inter_sym + b);
		}

#ifdef TEST_PURPOSE
		U32 length = result->GetLen();			
		for (U32 j = 0; j < length; ++j)
		{
			U32 index = i * length + j + 1;
			printf("%6d", result->GetData()[j]);
			if ((index % SHOW_NUMS) == 0)
			{
				printf("\n");
			}
		}
#endif
		encoded_sym.push(result);
	}
}

void CEncoder::AddData(CData* source)
{
	m_Source[m_Count + m_S + m_H].XorData(source);

	m_Count++;	

	if (m_Count == m_K) //we have all source symbols!!
	{
		vector<CXorPos>::iterator iter;
		for (iter = m_xorPos.begin(); iter != m_xorPos.end(); ++iter)
		{
			m_Source[iter->getDestPos()].XorData(m_Source + iter->getOrigPos());
		}

		for (U32 index = 0; index < m_L; ++index)
		{
			m_Intermediate[m_c[index]].SetData(m_Source + m_d[index]);
		}

#ifdef TEST_PURPOSE
		printf("\nIntermediate Symbols Number: %d\n", m_L);
		U32 index = 0;
		for (U32 i = 0; i < m_L; ++i)
		{
			U32 length = m_Intermediate[i].GetLen();
			for (U32 j = 0; j < length; ++j)
			{
				index++;
				printf("%6d", m_Intermediate[i].GetData()[j]);
				if ((index % SHOW_NUMS) == 0)
				{
					printf("\n");
				}
			}			
		}
#endif

		//Get repair symbols
		//LTEnc[K, (C[0],..., C[L-1]), (d[i], a[i], b[i])]
		//the triple (d, a, b)=Trip[K,X]
		LTEnc(m_K,m_Intermediate,m_Triples);
	}
}
