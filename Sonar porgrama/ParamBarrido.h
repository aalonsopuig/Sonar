#if !defined(AFX_PARAMBARRIDO_H__95229D61_1088_11D8_B526_00104BF1F258__INCLUDED_)
#define AFX_PARAMBARRIDO_H__95229D61_1088_11D8_B526_00104BF1F258__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ParamBarrido.h : header file
//
#include "SonarDoc.h"
/////////////////////////////////////////////////////////////////////////////
// CParamBarrido dialog

class CParamBarrido : public CDialog
{
// Construction
public:
	CSonarDoc* m_pSonarDoc;
	CParamBarrido(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CParamBarrido)
	enum { IDD = IDD_ParamBarrido };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParamBarrido)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation


	// Generated message map functions
	//{{AFX_MSG(CParamBarrido)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARAMBARRIDO_H__95229D61_1088_11D8_B526_00104BF1F258__INCLUDED_)
