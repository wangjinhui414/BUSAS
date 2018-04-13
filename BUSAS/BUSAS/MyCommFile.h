#pragma once
#ifndef _MY_COMM_FILEH_
#define _MY_COMM_FILEH_
//#include "BTtestDlg.h"
#include <string>
#include <map>
#include <vector>
using namespace std;
#define ASSERT_EX ASSERT
class MyCommFile
{
public:
	MyCommFile();
	~MyCommFile();
	bool LoadFile(string strfilename);
};

class CBinary
{
public:
	CBinary();
	CBinary(WORD iLength);
	CBinary(const CBinary& binData);
	CBinary(const char *pBuffer, WORD iLength);
	CBinary(string str);
	CBinary(BYTE *pBuffer, WORD iLength);
	~CBinary();
public:
	void operator = (const CBinary& binData);
	CBinary& operator +=(const CBinary& binData);
	CBinary& operator +=(BYTE ucNewElement);
	CBinary operator +(const CBinary& binData);
	CBinary operator +(BYTE ucNewElement);
	BYTE operator [] (WORD nIndex) const;
	BYTE& operator [] (WORD nIndex);
	bool operator < (const CBinary& binData);
	bool operator <= (const CBinary& binData);
	bool operator >= (const CBinary& binData);
	bool operator > (const CBinary& binData);
	bool operator == (const CBinary& binData);
	BYTE GetAt(WORD nIndex);
	void SetAt(WORD nIndex, BYTE ucNewElement);
	void RemoveAt(WORD nIndex, WORD nCount = 1);
	void InsertAt(WORD nIndex, BYTE ucNewElement, WORD nCount = 1);
	void Empty();

	bool IsEmpty();
	WORD GetSize();
	BYTE *GetBuffer();


	BYTE *m_pData;
	WORD m_nSize;
};

template<class TYPE>
class CGroup
{
public:
	CGroup();
	CGroup(const CGroup<TYPE>& x);
	~CGroup();
	WORD GetSize() const;
	void Empty();
	void RemoveAt(WORD nIndex, WORD nCount = 1);
	void InsertAt(WORD nIndex, TYPE newElement, WORD nCount = 1);
	TYPE GetAt(WORD nIndex) const;
	void SetAt(WORD nIndex, TYPE newElement);
	void Add(TYPE newElement);
	TYPE operator[](WORD nIndex) const;
	TYPE& operator[](WORD nIndex);
	void operator=(const CGroup<TYPE>& x);
	void SetSize(WORD nNewSize);

	void push(TYPE newElement);
	void push_back(TYPE newElement);
	void clear();
	void pop();
	TYPE top() const;
	BOOL empty();
	WORD size() const;
protected:
	TYPE* m_pData;
	WORD m_nSize;
};
//-----------------------------------------------------------
template<class TYPE>
inline void ConstructElements1(TYPE* pElements, WORD nCount)
{
	memset((void*)pElements, 0, nCount * sizeof(TYPE));
	for (; nCount--; pElements++)
		::new((void*)pElements) TYPE;
}

template<class TYPE>
inline void DestructElements1(TYPE* pElements, WORD nCount)
{
	for (; nCount--; pElements++)
		pElements->~TYPE();
}

template<class TYPE>
inline WORD CGroup<TYPE>::GetSize() const
{
	return m_nSize;
}
template<class TYPE>
inline WORD CGroup<TYPE>::size() const
{
	return m_nSize;
}
template<class TYPE>
inline BOOL CGroup<TYPE>::empty()
{
	return m_nSize == 0 ? TRUE : FALSE;
}
template<class TYPE>
inline void CGroup<TYPE>::push(TYPE newElement)
{
	WORD nIndex = m_nSize;
	SetSize(nIndex + 1);
	m_pData[nIndex] = newElement;
}
template<class TYPE>
inline void CGroup<TYPE>::push_back(TYPE newElement)
{
	WORD nIndex = m_nSize;
	SetSize(nIndex + 1);
	m_pData[nIndex] = newElement;
}
template<class TYPE>
inline void CGroup<TYPE>::clear()
{
	SetSize(0);
}
template<class TYPE>
inline void CGroup<TYPE>::pop()
{
	m_nSize -= 1;
	DestructElements1<TYPE>(&m_pData[m_nSize], 1);
}
template<class TYPE>
inline TYPE CGroup<TYPE>::top() const
{
	ASSERT_EX(m_nSize > 0);
	return m_pData[m_nSize - 1];
}
template<class TYPE>
inline void CGroup<TYPE>::Empty()
{
	SetSize(0);
}
template<class TYPE>
inline TYPE CGroup<TYPE>::GetAt(WORD nIndex) const
{
	ASSERT_EX(nIndex < m_nSize);
	return m_pData[nIndex];
}
template<class TYPE>
inline void CGroup<TYPE>::SetAt(WORD nIndex, TYPE newElement)
{
	ASSERT_EX(nIndex < m_nSize);
	m_pData[nIndex] = newElement;
}
template<class TYPE>
inline void CGroup<TYPE>::Add(TYPE newElement)
{
	WORD nIndex = m_nSize;
	SetSize(nIndex + 1);
	m_pData[nIndex] = newElement;
}
template<class TYPE>
inline TYPE CGroup<TYPE>::operator[](WORD nIndex) const
{
	ASSERT_EX(nIndex < m_nSize);
	return GetAt(nIndex);
}
template<class TYPE>
inline TYPE& CGroup<TYPE>::operator[](WORD nIndex)
{
	ASSERT_EX(nIndex < m_nSize);
	return m_pData[nIndex];
}

template<class TYPE>
CGroup<TYPE>::CGroup()
{
	m_pData = NULL;
	m_nSize = 0;
}
template<class TYPE>
CGroup<TYPE>::CGroup(const CGroup<TYPE>& x)
{
	m_pData = NULL;
	m_nSize = 0;
	for (int i = 0; i < x.GetSize(); i++)Add(x[i]);
}
template<class TYPE>
CGroup<TYPE>::~CGroup()
{
	if (m_pData != NULL){
		DestructElements1<TYPE>(m_pData, m_nSize);
		delete[](BYTE*)m_pData;
	}
}
template<class TYPE>
void CGroup<TYPE>::operator= (const CGroup<TYPE>& x)
{
	SetSize(0);
	for (int i = 0; i < x.GetSize(); i++)Add(x[i]);
}
template<class TYPE>
void CGroup<TYPE>::SetSize(WORD nNewSize)
{
	if (nNewSize == 0){
		if (m_pData != NULL){
			DestructElements1<TYPE>(m_pData, m_nSize);
			delete[](BYTE*)m_pData;
			m_pData = NULL;
		}
		m_nSize = 0;
	}
	else if (m_pData == NULL){
		m_pData = (TYPE*) new BYTE[nNewSize * sizeof(TYPE)];
		ConstructElements1<TYPE>(m_pData, nNewSize);
		m_nSize = nNewSize;
	}
	else{
		WORD nNewMax = nNewSize;
		TYPE* pNewData = (TYPE*) new BYTE[nNewMax * sizeof(TYPE)];
		memcpy(pNewData, m_pData, m_nSize * sizeof(TYPE));
		ConstructElements1<TYPE>(&pNewData[m_nSize], nNewSize - m_nSize);
		delete[](BYTE*)m_pData;
		m_pData = pNewData;
		m_nSize = nNewSize;
	}
}
template<class TYPE>
void CGroup<TYPE>::RemoveAt(WORD nIndex, WORD nCount)
{
	ASSERT_EX(nIndex + nCount <= m_nSize);

	int nMoveCount = m_nSize - (nIndex + nCount);
	DestructElements1<TYPE>(&m_pData[nIndex], nCount);
	if (nMoveCount)
		memmove(&m_pData[nIndex], &m_pData[nIndex + nCount],
		nMoveCount * sizeof(TYPE));
	m_nSize -= nCount;
}
template<class TYPE>
void CGroup<TYPE>::InsertAt(WORD nIndex, TYPE newElement, WORD nCount)
{
	if (nIndex >= m_nSize)SetSize(nIndex + nCount);
	else{
		int nOldSize = m_nSize;
		SetSize(m_nSize + nCount);
		DestructElements1<TYPE>(&m_pData[nOldSize], nCount);
		memmove(&m_pData[nIndex + nCount], &m_pData[nIndex], (nOldSize - nIndex) * sizeof(TYPE));
		ConstructElements1<TYPE>(&m_pData[nIndex], nCount);
	}
	ASSERT_EX(nIndex + nCount <= m_nSize);
	while (nCount--)m_pData[nIndex++] = newElement;
}
typedef struct ReqAns
{
	BYTE flag;
	CBinary barray;
}REQANS;
extern CGroup<ReqAns> g_allreqans;
extern CGroup<CBinary> g_allbuscmd;
#endif