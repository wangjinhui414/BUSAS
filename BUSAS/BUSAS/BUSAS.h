
// BUSAS.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CBUSASApp: 
// �йش����ʵ�֣������ BUSAS.cpp
//

class CBUSASApp : public CWinApp
{
public:
	CBUSASApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CBUSASApp theApp;