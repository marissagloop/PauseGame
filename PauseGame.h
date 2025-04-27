
// PauseGame.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CPauseGameApp:
// See PauseGame.cpp for the implementation of this class
//

class CPauseGameApp : public CWinApp
{
public:
	CPauseGameApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CPauseGameApp theApp;
