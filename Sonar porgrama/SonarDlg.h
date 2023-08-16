// SonarDlg.h : header file
//

#if !defined(AFX_SONARDLG_H__1BF1E669_154E_11D8_B526_00104BF1F258__INCLUDED_)
#define AFX_SONARDLG_H__1BF1E669_154E_11D8_B526_00104BF1F258__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "I2C.h"			// I2C Control
#include "Windows.h"

/////////////////////////////////////////////////////////////////////////////
// CSonarDlg dialog

class CSonarDlg : public CDialog
{
// Construction
public:
	CSonarDlg(CWnd* pParent = NULL);	// standard constructor
	~CSonarDlg();	//  destructor

private:
	void ShowHelp();
	void FreeMemory();
	void DrawDepthGrid();
	void DrawDepthMap();
	void DrawPolarGrid();
	void DrawPolarMap();
	unsigned char cGridStep;					//Grid Steps
	I2c		Board;								//I2C Board on PC
	unsigned char cSRF08Add;						//I2C address SRF08 module
	unsigned char cSVI2CAdd;					//Address of the I2C Servo controller device
	void	SonarInit();						//Sonar device initialization
	void	SonarPos(int iHpos, int iVpos);		//Sonar Position
	void	DistCheck(int Dist[16]);			//Distance Measure
	struct	stMeasure							//Measurements
	{
		int	iMeasure[16];
	};
	struct	stMeasure	*pstDataRange,			//Set of measurements
						*pstDataPoint,			//Measurements in a specific point
						*pstPrevMeasure;		//Previous Measure. For graph drawing

	unsigned char	cH, cV;			//Position in degree of focus of sonar measure
	unsigned char	cMinH,cMaxH,	//Horizontal limits for servo movement
					cMinV,cMaxV;	//Vertical limits for servo movement
	int				iDepth;			//Depth limit in cm
	unsigned char	cTone;			//Tone of gray for depth show
	unsigned char	cResol;			//Resolution factor in measurement. 1=max res
	unsigned char	cEcho;		//Echo to show
	enum	GraphKind {NONE, POLAR, DEPTH} GraphRequested;

	CPen	n_pen; 
	CPen*	v_pen; 
	CBrush	n_brush;
	CBrush*	v_brush;
	DWORD dwColor[9];

// Dialog Data
	//{{AFX_DATA(CSonarDlg)
	enum { IDD = IDD_SONAR_DIALOG };
	int		m_DistMax;
	int		m_HAngleMax;
	int		m_HAngleMin;
	int		m_Resol;
	int		m_VAngleMax;
	int		m_VAngleMin;
	int		m_ResSw;
	int		m_DepthSw;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSonarDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSonarDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDepthMap();
	afx_msg void OnCustomdrawResol(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawDistMax(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPolarMap();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SONARDLG_H__1BF1E669_154E_11D8_B526_00104BF1F258__INCLUDED_)
