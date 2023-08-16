// Sonar.h : main header file for the SONAR application
//

#if !defined(AFX_SONAR_H__1BF1E667_154E_11D8_B526_00104BF1F258__INCLUDED_)
#define AFX_SONAR_H__1BF1E667_154E_11D8_B526_00104BF1F258__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSonarApp:
// See Sonar.cpp for the implementation of this class
//

class CSonarApp : public CWinApp
{
public:
	CSonarApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSonarApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSonarApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SONAR_H__1BF1E667_154E_11D8_B526_00104BF1F258__INCLUDED_)
