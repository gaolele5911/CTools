
// Merlineye.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMerlineyeApp: 
// �йش����ʵ�֣������ Merlineye.cpp
//

class CMerlineyeApp : public CWinApp
{
public:
	CMerlineyeApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMerlineyeApp theApp;