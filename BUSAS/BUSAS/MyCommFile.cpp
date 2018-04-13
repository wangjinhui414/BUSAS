#include "stdafx.h"
#include "MyCommFile.h"
#include <fstream>

MyCommFile::MyCommFile()
{
}


MyCommFile::~MyCommFile()
{
}
//void string_replace(string&s1, const string&s2, const string&s3)
//{
//	string::size_type pos = 0;
//	string::size_type a = s2.size();
//	string::size_type b = s3.size();
//	while ((pos = s1.find(s2, pos)) != string::npos)
//	{
//		s1.replace(pos, a, s3);
//		pos += b;
//	}
//}
CGroup<ReqAns> g_allreqans;
CGroup<CBinary> g_allbuscmd;
bool MyCommFile::LoadFile(string strfilename)
{
	g_allreqans.Empty();
	ifstream infile(strfilename.c_str() , ios::in);
	if (!infile)
		return false;
	char buffer[1024];

	while (infile.getline(buffer, 1024))
	{
		string strtemp = buffer;
		if (strtemp.find("//") !=-1)
			strtemp.erase(strtemp.find("//"), strtemp.size() - strtemp.find("//"));
		while (strtemp.find(" ") != -1)
		{
			strtemp.erase(strtemp.find(" "), 1);
		}

		if (strtemp.find("Req=") == 0)
		{
			strtemp.erase(0, 4);
			ReqAns reqans;
			reqans.flag = 0;
			reqans.barray = CBinary(strtemp);
			g_allreqans.push(reqans);	
		}
		if (strtemp.find("Ans=") == 0)
		{
			strtemp.erase(0, 4);
			ReqAns reqans;
			reqans.flag = 1;
			reqans.barray = CBinary(strtemp);
			g_allreqans.push(reqans);
		}
		if (strtemp.find("Bus=") == 0)
		{
			strtemp.erase(0, 4);
			g_allbuscmd.push(CBinary(strtemp));
		}
	}
	infile.close();
	return true;
}
CBinary::CBinary(const string str)
{
	string str1 = str;
	m_pData = new BYTE[str1.size()/2];
	m_nSize = 0;
	BYTE value = 0;
	char *error = NULL;
	while (!(str1.size()<2))
	{
		string strtemp = str1.substr(0, 2);
		str1.erase(0, 2);
		value = BYTE(strtol(strtemp.c_str(), &error, 16));
		ASSERT(error != NULL);
		m_pData[m_nSize++] = value;
	}
	
}
CBinary::CBinary()
{
	m_pData = new BYTE[1];
}

CBinary::~CBinary()
{
	delete[] m_pData;
	m_nSize = 0;
}
CBinary::CBinary(WORD iLength)
{
	m_pData = new BYTE[iLength > 256 ? iLength : 256];
	m_nSize = 0;
}
CBinary::CBinary(const CBinary& binData)
{
	m_pData = new BYTE[binData.m_nSize > 256 ? binData.m_nSize : 256];
	m_nSize = binData.m_nSize;
	memcpy(m_pData, binData.m_pData, binData.m_nSize);
}
CBinary::CBinary(const char *pBuffer, WORD iLength)
{
	m_pData = new BYTE[iLength > 256 ? iLength : 256];
	m_nSize = iLength;
	memcpy(m_pData, pBuffer, iLength);
}
CBinary::CBinary(BYTE *pBuffer, WORD iLength)
{
	m_pData = new BYTE[iLength > 256 ? iLength : 256];
	m_nSize = iLength;
	memcpy(m_pData, pBuffer, iLength);
}

void CBinary::operator = (const CBinary& binData)
{
	m_pData = new BYTE[binData.m_nSize > 256 ? binData.m_nSize : 256];
	m_nSize = binData.m_nSize;
	memcpy(m_pData, binData.m_pData, binData.m_nSize);
}
CBinary& CBinary::operator +=(const CBinary& binData)
{
	if ((binData.m_nSize + m_nSize) < 256)
	{
		memcpy(&m_pData[m_nSize], binData.m_pData, binData.m_nSize);
		m_nSize += binData.m_nSize;
	}
	else
	{
		BYTE *ptemp = m_pData;
		m_pData = new BYTE[binData.m_nSize + m_nSize];
		memcpy(m_pData, ptemp, m_nSize);
		memcpy(&m_pData[m_nSize], binData.m_pData, binData.m_nSize);
		m_nSize += binData.m_nSize;
		delete[] ptemp;
	}
	return *this;
}
CBinary& CBinary::operator +=(BYTE ucNewElement)
{
	if (m_nSize < 255){
		m_pData[m_nSize] = ucNewElement;
		m_nSize++;
	}
	else
	{
		BYTE *ptemp = m_pData;
		m_pData = new BYTE[m_nSize + 1];
		memcpy(m_pData, ptemp, m_nSize);
		m_pData[m_nSize] = ucNewElement;
		m_nSize++;
	}
	return *this;
}
CBinary CBinary::operator +(const CBinary& binData)
{
	CBinary retcb;
	if (m_nSize + binData.m_nSize > 256)
	{
		retcb = CBinary(m_nSize + binData.m_nSize);
	}
	else
	{
		retcb = CBinary(256);
	}
	memcpy(retcb.m_pData, m_pData, m_nSize);
	memcpy(&retcb.m_pData[m_nSize], binData.m_pData, binData.m_nSize);
	retcb.m_nSize = m_nSize + binData.m_nSize;
	return  retcb;
}
CBinary CBinary::operator +(BYTE ucNewElement)
{
	CBinary retcb;
	if (m_nSize + 1 > 256)
	{
		retcb = CBinary(m_nSize + 1);
	}
	else
	{
		retcb = CBinary(256);
	}
	memcpy(retcb.m_pData, m_pData, m_nSize);
	retcb.m_pData[m_nSize] = ucNewElement;
	retcb.m_nSize = m_nSize + 1;
	return  retcb;
}
BYTE CBinary::operator [] (WORD nIndex) const
{
	BYTE bret = m_pData[nIndex];
	return bret;
}
BYTE& CBinary::operator [] (WORD nIndex)
{
	return m_pData[nIndex];
}
bool CBinary::operator < (const CBinary& binData)
{
	return m_nSize < binData.m_nSize ? true : false;
}
bool CBinary::operator <= (const CBinary& binData)
{
	return m_nSize <= binData.m_nSize ? true : false;
}
bool CBinary::operator >= (const CBinary& binData)
{
	return m_nSize >= binData.m_nSize ? true : false;
}
bool CBinary::operator >(const CBinary& binData)
{
	return m_nSize > binData.m_nSize ? true : false;
}
bool CBinary::operator == (const CBinary& binData)
{
	return m_nSize == binData.m_nSize ? true : false;
}

BYTE CBinary::GetAt(WORD nIndex)
{
	return m_pData[nIndex];
}
void CBinary::SetAt(WORD nIndex, BYTE ucNewElement)
{
	m_pData[nIndex] = ucNewElement;
}
void CBinary::RemoveAt(WORD nIndex, WORD nCount)
{
	memcpy(&m_pData[nIndex], &m_pData[nIndex + 1], m_nSize - nIndex - 1);
	m_nSize--;
}
void CBinary::InsertAt(WORD nIndex, BYTE ucNewElement, WORD nCount)
{
	if (m_nSize < 256)
	{
		memcpy(&m_pData[nIndex + 2], &m_pData[nIndex + 1], m_nSize - nIndex - 1);
		m_pData[nIndex + 1] = ucNewElement;
		m_nSize++;
	}
	else
	{
		BYTE *ptemp = m_pData;
		m_pData = new BYTE[m_nSize + 1];
		memcpy(&m_pData, ptemp, nIndex + 1);
		memcpy(&m_pData[nIndex + 2], &ptemp[nIndex + 1], m_nSize - nIndex - 1);
		m_pData[nIndex] = ucNewElement;
		m_pData[nIndex + 1] = ucNewElement;
		m_nSize++;
		delete[] ptemp;
	}
}

void CBinary::Empty()
{
	m_nSize = 0;
}

bool CBinary::IsEmpty()
{
	return m_nSize > 0 ? true : false;
}
WORD CBinary::GetSize()
{
	return m_nSize;
}
BYTE *CBinary::GetBuffer()
{
	return  m_pData;
}