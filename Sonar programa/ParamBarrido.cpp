// ParamBarrido.cpp : implementation file
//

#include "stdafx.h"
#include "Sonar.h"
#include "ParamBarrido.h"
//#include "SonarView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParamBarrido dialog


CParamBarrido::CParamBarrido(CWnd* pParent /*=NULL*/)
	: CDialog(CParamBarrido::IDD, pParent)
{
	//{{AFX_DATA_INIT(CParamBarrido)
	//}}AFX_DATA_INIT
}


void CParamBarrido::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParamBarrido)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParamBarrido, CDialog)
	//{{AFX_MSG_MAP(CParamBarrido)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParamBarrido message handlers

void CParamBarrido::OnOK() 
{
//	UpdateData(TRUE);	
//	if (m_XAngleMax>10) MessageBox ("Ojo. Valor > 10", "Ojo", MB_ICONEXCLAMATION);
//	UpdateWindow();
	m_pSonarDoc->DistMax=GetDlgItemInt(IDC_DistMax, NULL, 0);
	m_pSonarDoc->Resol=GetDlgItemInt(IDC_Resol, NULL, 0);
	m_pSonarDoc->HAngleMin=GetDlgItemInt(IDC_HAngleMin, NULL, 0);
	m_pSonarDoc->HAngleMax=GetDlgItemInt(IDC_HAngleMax, NULL, 0);
	m_pSonarDoc->VAngleMin=GetDlgItemInt(IDC_VAngleMin, NULL, 0);
	m_pSonarDoc->VAngleMax=GetDlgItemInt(IDC_VAngleMax, NULL, 0);
	CDialog::OnOK();
}
