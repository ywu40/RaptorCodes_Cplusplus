/*!
*
* \file   raptor_decoder.h
*
* \brief  Implementation of raptor codes decoder to recover source symbols
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

#include <string.h>
#include <vector>

#include <windows.h>

#include "raptor_decoder.h"
#include "raptor_common.h"
#include "raptor_triple.h"

using std::queue;
using std::vector;

#define DETAIL_TIME_INFO

#define EXTEND_ROWS  20

extern bool isDecodedSuccess;

CDecoder::CDecoder(U32 K, U32 N, U32 lossNum, vector<U32> ESI)
{
	m_K = K;
	m_S = GetS(m_K);
	m_H = GetH(m_K, m_S);
	m_Hp = GetHp(m_H);
	m_L = GetL(m_K);
	m_Lp = GetLPrim(m_K);
	m_N = N - lossNum;
	m_lossNum = lossNum;
#ifdef TEST_PURPOSE
	printf("K = %d, S = %d, H = %d\n", m_K, m_S, m_H);
#endif
	m_A = NULL;
	m_ESI = ESI;

	m_Degree = NULL;

	FillAMatrix();
}

CDecoder::~CDecoder(void)
{
	ReleaseAMatrix();
}

void CDecoder::ReleaseAMatrix(void)
{
	if (m_A)
	{
		for (U32 i = 0; i < m_L; ++i)
		{
			delete [] m_A[i];
		}
	}
	delete [] m_A;
	m_A = NULL;

	if (m_Degree)
	{
		delete []  m_Degree;
		m_Degree = NULL;
	}
}

void CDecoder::FillAMatrix(void)
{
	//Free A matrix
	ReleaseAMatrix();

	//Alloc A matrix
	U32 M = m_N + m_S + m_H;
	m_A = new U8*[M]; //Max number of rows that M can achieve

	for (U32 i = 0; i < M; ++i)
	{
		m_A[i] = new U8[m_L];
		memset(m_A[i], 0, m_L);
	}

	m_Degree = new U16[M];
	memset(m_Degree, 0, M * sizeof(U16));

	//Fill A Matrix (See RFC5053:5.4.2.4.2)
	for (U32 i = 0; i < m_K; ++i)
	{
		U32 a = 1 + (i / m_S % (m_S - 1));
		U32 b = i % m_S;
		m_A[b][i] = 1;
		m_Degree[b]++;
		b = (b + a) % m_S;
		m_A[b][i] = 1;
		m_Degree[b]++;
		b = (b + a) % m_S;
		m_A[b][i] = 1;
		m_Degree[b]++;
	}

	//Fill I_S Matrix
	for (U32 i = 0; i < m_S; ++i)
	{
		m_A[i][i + m_K] = 1;
		m_Degree[i]++;
	}

	//Fill I_H Matrix
	for (U32 i = m_S; i < m_S + m_H; ++i)
	{
		m_A[i][i + m_K] = 1;
		m_Degree[i]++;
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
			if (IsBitSet(Mseq[j], h))
			{
				m_A[h + m_S][j] = 1;

				m_Degree[h + m_S]++;
			}
		}
	}
	delete[] Mseq;

	//Fill GL_T matrix
	//Load triples
	CTripleGenerator triple_gen;	
	for (U32 i = 0; i < m_N; ++i)
	{
#ifdef TEST_PURPOSE
		//printf("ESI value£º%d\n", m_ESI[i]);
#endif
		CTriple triple = triple_gen.Trip(m_K, m_ESI[i]);
		U32 a = triple.a;
		U32 b = triple.b;
		U32 d = triple.d;
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
		m_Degree[i + m_S + m_H]++;

		U32 jend = MIN(d, m_L);
		for (U32 j = 1; j < jend;++j)
		{
			b = (b + a) % m_Lp;
			while (b >= m_L)
			{
				b = (b + a) % m_Lp;
			}
			m_A[i + m_S + m_H][b] = 1;
			m_Degree[i + m_S + m_H]++;
		}		
	}
}

void CDecoder::GetAMatrix(U8** A, U32 M, U32 L)
{
#ifdef TEST_PURPOSE
	printf("\nA matrix row number is£º%d£¬column number£º%d\n", M, L);
#endif

	for (U32 i = 0; i < M; ++i)
	{
		for (U32 j = 0; j < L; ++j)
		{
			A[i][j] = m_A[i][j];
#ifdef TEST_PURPOSE
			printf("%d ", A[i][j]);
		}

		printf("-----%d", m_Degree[i]);
		printf("\n");
#else
		}
#endif
	}
}

queue<CData* > CDecoder::GetIntermediateSymbols(queue<CData* > encoded)
{
	U32 N = (U32)encoded.size();
	U32 M = N + m_H + m_S - m_lossNum;
	U32 tempM = N + m_H + m_S;
	//Get temporal A MxL bit matrix to work with.
	U8** A = new U8*[M];

	for (U32 i = 0; i < M; ++i)
	{
		A[i] = new U8[m_L];
	}
	GetAMatrix(A, M, m_L);

	CData** C = new CData*[m_L]; //Intermediate symbols to be recovered
	CData** D = new CData*[M]; //Encoded symbols
	for (U32 i = 0; i < M; ++i)
	{
		if (i < m_L)
		{
			C[i] = new CData();
		}
		D[i] = new CData();
	}

	//First S + H encoded symbols init with zero.
	U32 EncodedIndex = m_S + m_H;
	U32 esiIndex = 0;
	U32 lostIndex = 0;
	for (U32 i = 0; i < N; ++i)
	{
		if (m_ESI[esiIndex] == i)
		{
			D[EncodedIndex]->SetData(encoded.front());
			encoded.pop();
			EncodedIndex++;
			esiIndex++;
		}
		else
		{
			encoded.front();
			encoded.pop();
		}
	}

#ifdef TEST_PURPOSE
	printf("\nReceived Encoded Symbols at decoder side:\n");
	U32 index = 0;
	for (U32 i = m_S + m_H; i < M; ++i)
	{
		U32 length = (*D[i]).GetLen();
		for (U32 j = 0; j < length; ++j)
		{
			index++;
			printf("%6d", (*D[i]).GetData()[j]);
			if ((index % SHOW_NUMS) == 0)
			{
				printf("\n");
			}
		}
	}
#endif

	//Get C, we have to solve A*C=D with gauss method. 
	SolveRevisedGauss(M, m_L, A, C, D);

	//free resources
#if 0
	//free A matrix
	for (U32 i = 0; i < M; ++i)
	{
		delete D[i];
		if (i < m_L)
		{
			delete[] A[i];
		}
	}
	delete[] A;
	delete[] C;
	delete[] D;
#else
	//free A matrix
	for (U32 i = 0; i < M; ++i)
	{
		if (i < m_L)
		{
			delete[] A[i]; 
			A[i] = NULL;

			delete C[i];   
			C[i] = NULL;
		}
		delete D[i];
		D[i] = NULL;

	}

	delete[] A;
	A = NULL;
	delete[] C;
	C = NULL;
	delete[] D;
	D = NULL;
#endif
	//free encoded symbols and intermediate symbols
	return encoded;
}

void CDecoder::SolveRevisedGauss(U32 M, U32 L, U8** A, CData** C, CData** D)
{
	//init sequence
	//See reference document Page 25
	/*
	- Each time row i of A is exclusive-ORed into row i¡¯ in the decoding
	schedule, then in the decoding process, symbol D[d[i]] is
	exclusive-ORed into symbol D[d[i¡¯]].
	- Each time row i is exchanged with row i¡¯ in the decoding schedule,
	then in the decoding process, the value of d[i] is exchanged with
	the value of d[i¡¯].
	- Each time column j is exchanged with column j¡¯ in the decoding
	schedule, then in the decoding process, the value of c[j] is
	exchanged with the value of c[j¡¯].
	*/
	U32* c = new U32[L];
	U32* d = new U32[M];
	for (U32 i = 0; i < M; ++i)
	{
		if (i < L)
		{
			c[i] = i;
		}
		d[i] = i;
	}

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

#ifdef DETAIL_TIME_INFO
	//test time
	LARGE_INTEGER m_liPerfFreq={0};
	QueryPerformanceFrequency(&m_liPerfFreq); 
	LARGE_INTEGER m_liPerfStart={0};
	QueryPerformanceCounter(&m_liPerfStart);

#endif
	/*-----------------------------------First Phase------------------------------------*/
	U32 zerosRowNum = 0;
	U32 i = 0; // i is the rank of Identity Matrix I
	U32 u = 0; // u is the column number of Matrix U
	while (i + u < L)
	{
		//Let r be the minimum integer such that at least one row of A has
		//exactly r ones in V.
		U32 ones_pos[ROW_ONES_NUM];	
		bool isRowFound = false;
		U32 nrow = 0;				

#ifdef DETAIL_TIME_INFO_1
		S32 t2 = GetTickCount();

#endif

#if 0
		U32 r = 1;
		U32 endPoint = L - i - u;
		while (r <= endPoint) //from n row
		{
#ifdef DETAIL_TIME_INFO_1
			int t3=GetTickCount();

#endif
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

#ifdef DETAIL_TIME_INFO_1
			S32 t4 = GetTickCount() - t3;
			printf("Find row with minimum 1 time: %d\n", t4);
#endif
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

#ifdef DETAIL_TIME_INFO_1
		S32 t1=GetTickCount() - t2;
		printf("First Phase-step 1: Decoding Time: %d\n", t1);

		t2 = GetTickCount();

#endif
		//TODO: Now we must work with this row
		//TODO: if nones == 2 we must choose between the maximum size component (See RFC 5053:5.5.2.2)

		if (isRowFound == false)
		{
			printf("First Phase Decoding Fail!");
			return;
		}
#else
		U32 minDegree = m_Degree[i];
		U32 posMinDeg = i;
		U32 r = minDegree;
		if (minDegree == 1)
		{
			r = 1;
		}
		else 
		{			
			if (minDegree == 0)
			{

				if (zerosRowNum > (M - L))
				{
#ifdef RECORD_DEC_TIME
					LARGE_INTEGER liPerfNow={0};
					// test time
					QueryPerformanceCounter(&liPerfNow);
					gsDecTime.m_1stPhase = (((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000)/m_liPerfFreq.QuadPart);
#endif
					//free resources
					delete[] c;
					c = NULL;
					delete[] d;
					d = NULL;
					printf("First Phase Decoding Fail! Zero row Num: %d, M=%d, L=%d\n", zerosRowNum, M, L);
					return;
				}

				U32 lastNonZeroRow = M - zerosRowNum - 1;
				for (U32 zeroIndex = lastNonZeroRow; zeroIndex > i; --zeroIndex)
				{
					if (m_Degree[zeroIndex] != 0)
					{
						lastNonZeroRow = zeroIndex;
						break;
					}
					else
					{
						zerosRowNum++;
					}
				}

				if (i != lastNonZeroRow)
				{
					U8* tmp_row = A[i];
					A[i] = A[lastNonZeroRow];
					A[lastNonZeroRow] = tmp_row;

					zerosRowNum++;

					U32 tmp_encoded = d[i];
					d[i] = d[lastNonZeroRow];
					d[lastNonZeroRow] = tmp_encoded;  

					U32 tmp_degree = m_Degree[i];
					m_Degree[i] = m_Degree[lastNonZeroRow];
					m_Degree[lastNonZeroRow] = tmp_degree;
				}

				minDegree = m_Degree[i];
				posMinDeg = i;
			}

			if (minDegree == 1)
			{
				//found row with minimum 1s
				r = 1;
			}
			else 
			{
				U32 lastNonZeroRow = M - zerosRowNum - 1;
				for (U32 s = i + 1; s <= lastNonZeroRow; ++s)
				{
					if (m_Degree[s] == 1)
					{					
						posMinDeg = s;
						r = 1;
						break;

					}
					else if ((m_Degree[s] > 0) && (m_Degree[s] < minDegree))
					{
						minDegree = m_Degree[s];
						posMinDeg = s;
						r = minDegree;
					}
				}
			}		
		}

		U32 onePosIndex = 0;
		U8* row = A[posMinDeg];
		nrow = posMinDeg;
		for (U32 j = i; j < L - u; ++j) //from take out first i column from I matrix and last u column from U matrix
		{
			if (row[j])
			{
				ones_pos[onePosIndex] = j;
				onePosIndex++;
			}
		}
#endif

		//Move row to first V row
		if (i != nrow)
		{
			//swap two rows
			U8* tmp_row = A[i];
			A[i] = A[nrow];
			A[nrow] = tmp_row;

			U32 tmp_encoded = d[i];
			d[i] = d[nrow];
			d[nrow] = tmp_encoded;

			//swap the degree of rows
			U32 tmp_degree = m_Degree[i];
			m_Degree[i] = m_Degree[nrow];
			m_Degree[nrow] = tmp_degree;
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
			c[onePos]    = c[changedPos];
			c[changedPos] = tmp_value;

			u += 1;
		}
#ifdef DETAIL_TIME_INFO_1
		t1 = GetTickCount() - t2;
		printf("First Phase-step 2: Decoding Time: %d\n", t1);

		t2 = GetTickCount();
#endif

		m_Degree[i] -= (r - 1);
		for (U32 k = (i + 1); k < M; ++k)
		{				
			if (A[k][i] == 1)
			{
				m_Degree[k] -= 1; 
			}
			
			for (U32 h = 1; h < r; ++h)
			{
				m_Degree[k] -= A[k][L - u - 1 + h];
			}

			if (A[k][i] == 1)
			{
				for (U32 l = 0; l < L; ++l)
				{
					A[k][l] ^= A[i][l];				
				}

				D[d[k]]->XorData(D[d[i]]);
			}
		}

#ifdef TEST_PURPOSE
		printf("\nA matrix is:\n");
		for (U32 step = 0; step < M; ++step)
		{
			for (U32 j = 0; j < L; ++j)
			{
				printf("%d ", A[step][j]);
			}
			printf("--------%d ", m_Degree[step]);
			printf("\n");
		}
#endif
		

#ifdef DETAIL_TIME_INFO_1
		t1 = GetTickCount() - t2;
		printf("First Phase-step 3: Decoding Time: %d\n", t1);

		t2 = GetTickCount();
#endif
		i += 1;	
	}

#ifdef TEST_PURPOSE
	printf("\ni = %d, u = %d\n", i, u);
#endif

#ifdef DETAIL_TIME_INFO

	LARGE_INTEGER liPerfNow = {0};
	QueryPerformanceCounter(&liPerfNow);
	U64 decTime = (((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000) / m_liPerfFreq.QuadPart);
	printf("First Phase Decoding Time: %d\n", decTime);
	
	QueryPerformanceCounter(&m_liPerfStart);
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
#if 0
		if ((colIndex < (L - i)) && rowIndex == M) 
		{
			printf("Second Phase Decoding Fail!");
			return;
		}
		else if (rowIndex == M)
		{
			continue;
		}
#else
		if (rowIndex == M)
		{
			printf("Second Phase Decoding Fail!");
			return;
		}
#endif

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

					D[d[k]]->XorData(D[d[colIndex]]);
				}
			}
		}
	}

#ifdef TEST_PURPOSE
	printf("\nA matrix before deleting the last M-L rows:\n");
	for (U32 i = 0; i < M; ++i)
	{
		for (U32 j = 0; j < L; ++j)
		{
			printf("%d ", A[i][j]);
		}
		printf("\n");
	}
#endif

	for (U32 index = L; index < M; ++index)
	{
		delete[] A[index];
	}

#ifdef DETAIL_TIME_INFO

	QueryPerformanceCounter(&liPerfNow);
	decTime = (((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000) / m_liPerfFreq.QuadPart);
	printf("Second Phase Decoding Time: %d\n", decTime);

	QueryPerformanceCounter(&m_liPerfStart);
#endif

	/*-----------------------------------Third Phase------------------------------------*/
	for (U32 k = 0; k < i; ++k)
	{
		for (U32 s = i; s < L; ++s)
		{
			if (A[k][s] == 1)
			{
				A[k][s] ^= A[s][s];

				D[d[k]]->XorData(D[d[s]]);
			}
		}
	}

#ifdef DETAIL_TIME_INFO
	QueryPerformanceCounter(&liPerfNow);
	decTime = (((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000) / m_liPerfFreq.QuadPart);
	printf("Third Phase Decoding Time: %d\n", decTime);

	QueryPerformanceCounter(&m_liPerfStart);
#endif

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

	/*-----------------------------------Fourth Phase------------------------------------*/
	/*
	It is clear at the end of successful decoding that the L symbols D[d[0]], D[d[1]],...,
	D[d[L-1]] are the values of the L symbols C[c[0]], C[c[1]],...,C[c[L-1]]
	*/
	for (U32 index = 0; index < L; ++index)
	{
		C[c[index]]->SetData(D[d[index]]);

	}

#ifdef DETAIL_TIME_INFO
	QueryPerformanceCounter(&liPerfNow);
	decTime = (((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000) / m_liPerfFreq.QuadPart);
	printf("Fourth Phase Decoding Time: %d\n", decTime);
#endif

#ifdef TEST_PURPOSE
	printf("\nIntermediate Symbols after decoding:\n");
	for (U32 i = 0; i < L; ++i)
	{
		U32 length = (*C[i]).GetLen();
		for (U32 j = 0; j < length; ++j)
		{
			U32 index = i * length + j + 1;
			printf("%6d", (*C[i]).GetData()[j]);
			if ((index % SHOW_NUMS) == 0)
			{
				printf("\n");
			}
		}
	}
#endif

#ifdef TEST_PURPOSE
	printf("\n\nThe recovered Source Symbols:\n");
#endif

#ifdef TEST_PURPOSE
	U32 ltDec1 = GetTickCount();
#endif

	CTripleGenerator triple_gen;
	for (U32 i = 0; i < m_K; ++i)
	{
		CTriple triple = triple_gen.Trip(m_K, i);
		U32 a = triple.a;
		U32 b = triple.b;
		U32 d = triple.d;

		CData* result = new CData();
		while (b >= m_L)
		{
			b = (b + a) % m_Lp;
		}
		result->SetData(C[b]);
		U32 jend = MIN(d, m_L);
		for (U32 j = 1; j < jend;++j)
		{
			b = (b + a) % m_Lp;
			while (b >= m_L)
			{
				b = (b + a) % m_Lp;
			}
			result->XorData(C[b]);
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
		delete result;
	}
#ifdef TEST_PURPOSE
	U32 ltDec2 = GetTickCount() - ltDec1;
	printf("LT Decoding Time: %d\n", ltDec2);
#endif

#ifdef TEST_PURPOSE
	printf("\n\n");
#endif

	//free resources
	delete[] c;
	delete[] d;
}

queue<CData* > CDecoder::Decode(queue<CData* > encoded)
{
	return GetIntermediateSymbols(encoded);
}