// SonarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Sonar.h"
#include "SonarDlg.h"
#include <math.h> 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define		pi 3.14159265359
int			m_cxClient, m_cyClient; 

/////////////////////////////////////////////////////////////////////////////
// CSonarDlg dialog

CSonarDlg::CSonarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSonarDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSonarDlg)
	m_DistMax	= 50;
	m_HAngleMax = 120;
	m_HAngleMin = 100;
	m_Resol		= 50;
	m_VAngleMax = 120;
	m_VAngleMin = 100;
	m_DepthSw	= (990*m_DistMax/100)+10;
	m_ResSw		=((4*m_Resol/100)+1)*2;
	cSRF08Add	=224;				//I2C address SRF08 module
	cSVI2CAdd	=118;				//Address of the I2C Servo controller device
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//Colors definitions
	dwColor[0] = RGB(0,0,0);		//black 
	dwColor[1] = RGB(245,0,0);		//red  
	dwColor[2] = RGB(0,0,245);		//blue 
	dwColor[3] = RGB(0,245,0);		//green 
	dwColor[4] = RGB(245,245,0);	//yelow 
	dwColor[5] = RGB(245,0,245);	//magenta 
	dwColor[6] = RGB(0,245,245);	//cyan 
	dwColor[7] = RGB(127,127,127);	//gray50% 
	dwColor[8] = RGB(245,245,245);	//white 

	GraphRequested=NONE;	//Last kind of Map requested
	cGridStep=10;
}

CSonarDlg::~CSonarDlg()
{
	FreeMemory();	//Free memory previously used
}
void CSonarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSonarDlg)
	DDX_Slider(pDX, IDC_DistMax, m_DistMax);
	DDX_Text(pDX, IDC_HAngleMax, m_HAngleMax);
	DDV_MinMaxInt(pDX, m_HAngleMax, 0, 180);
	DDX_Text(pDX, IDC_HAngleMin, m_HAngleMin);
	DDV_MinMaxInt(pDX, m_HAngleMin, 0, 180);
	DDX_Slider(pDX, IDC_Resol, m_Resol);
	DDX_Text(pDX, IDC_VAngleMax, m_VAngleMax);
	DDV_MinMaxInt(pDX, m_VAngleMax, 0, 180);
	DDX_Text(pDX, IDC_VAngleMin, m_VAngleMin);
	DDV_MinMaxInt(pDX, m_VAngleMin, 0, 180);
	DDX_Text(pDX, IDC_ResSw, m_ResSw);
	DDX_Text(pDX, IDC_DepthSw, m_DepthSw);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSonarDlg, CDialog)
	//{{AFX_MSG_MAP(CSonarDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_DepthMap, OnDepthMap)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_Resol, OnCustomdrawResol)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_DistMax, OnCustomdrawDistMax)
	ON_BN_CLICKED(IDC_PolarMap, OnPolarMap)
	ON_BN_CLICKED(IDC_Help, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSonarDlg message handlers

BOOL CSonarDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSonarDlg::OnPaint() 
{
	int	iX,iY;			//Coordenates used for drawing


	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CClientDC	dc(this);		//Device context for painting in dialog
		if (GraphRequested==DEPTH) 
		{
			// Create a personalized drawing area 
			dc.SetMapMode (MM_ISOTROPIC);
			dc.SetWindowExt (360, 190);
			dc.SetViewportExt(m_cxClient, -m_cyClient); 
			dc.SetViewportOrg((int)m_cxClient*0.46 ,(int)m_cyClient*0.88);

			DrawDepthGrid();	//Draw Grid for Depth map
			cEcho=0;			//Echo to show
	
			for (cV=cMinV; cV<=cMaxV; cV+=cResol) 
			{ 
				for (cH=cMinH; cH<=cMaxH; cH+=cResol) 
				{ 
					pstDataPoint=pstDataRange + (((cV-cMinV)/cResol)
												*((cMaxH-cMinH)/cResol) 
												+((cH-cMinH)/cResol));			
			
					// Delete and create objects pen and brush to change color
					if (pstDataPoint->iMeasure[cEcho]>iDepth) cTone=0;
					else cTone=245-(pstDataPoint->iMeasure[cEcho]*245/iDepth);
					n_pen.CreatePen (BS_SOLID, 1, RGB(cTone,cTone,cTone)); 
					v_pen=dc.SelectObject(&n_pen) ;
					n_brush.CreateSolidBrush (RGB(cTone,cTone,cTone)); 
					v_brush=dc.SelectObject(&n_brush) ;

					dc.Rectangle(cH-(cResol/2),cV-(cResol/2),
								 cH+(cResol/2),cV+(cResol/2));
					dc.SelectObject(v_pen); 
					n_pen.DeleteObject(); 
					dc.SelectObject(v_brush); 
					n_brush.DeleteObject(); 
				}
			}		
		}
		if (GraphRequested==POLAR) 
		{


			DrawPolarGrid();	//Draw Grid for Polar map

			// Create a personalized drawing area 
			dc.SetMapMode (MM_ISOTROPIC);
			dc.SetWindowExt (360, 190);
			dc.SetViewportExt(m_cxClient, -m_cyClient); 
			dc.SetViewportOrg((int)m_cxClient*0.71 ,(int)m_cyClient*0.88);
	
			cV=cMinV;  

			for (cH=cMinH; cH<=cMaxH; cH+=cResol) 
			{ 
				pstDataPoint=pstDataRange + ((cH-cMinH)/cResol);			
				if (cH==cMinH)
					pstPrevMeasure=pstDataPoint;
				else
				{
					for (cEcho=0;cEcho<15;cEcho++)
					{
				
						if (cEcho==0)
						{
							n_pen.CreatePen (BS_SOLID, 2, dwColor[2]); 
							v_pen=dc.SelectObject(&n_pen);


							iX=(pstPrevMeasure->iMeasure[cEcho]*180/iDepth)*cos(pi*(double)((180-cH)+cResol)/180);
							iY=(pstPrevMeasure->iMeasure[cEcho]*180/iDepth)*sin(pi*(double)((180-cH)+cResol)/180);
							if (iX>=-90 && iX<=90 && iY>=0 && iY<=180)
							{
								dc.MoveTo (iX, iY); 

								iX=(pstDataPoint->iMeasure[cEcho]*180/iDepth)*cos(pi*(double)(180-cH)/180);
								iY=(pstDataPoint->iMeasure[cEcho]*180/iDepth)*sin(pi*(double)(180-cH)/180);
								if (iX>=-90 && iX<=90 && iY>=0 && iY<=180)
									dc.LineTo (iX, iY); 
							}

							dc.SelectObject(v_pen); 
							n_pen.DeleteObject(); 
						}
						else
						{
							n_pen.CreatePen (BS_SOLID, 2, RGB(cEcho*16,cEcho*16,cEcho*16)); 
							v_pen=dc.SelectObject(&n_pen) ;
							iX=(pstDataPoint->iMeasure[cEcho]*180/iDepth)*cos(pi*(double)((180-cH)+(cResol/2))/180);
							iY=(pstDataPoint->iMeasure[cEcho]*180/iDepth)*sin(pi*(double)((180-cH)+(cResol/2))/180);
							if (iX>=-90 && iX<=90 && iY>=0 && iY<=180)
							{
								dc.MoveTo (iX, iY); 

								iX=(pstDataPoint->iMeasure[cEcho]*180/iDepth)*cos(pi*(double)(180-cH)/180);
								iY=(pstDataPoint->iMeasure[cEcho]*180/iDepth)*sin(pi*(double)(180-cH)/180);
								if (iX>=-90 && iX<=90 && iY>=0 && iY<=180)
									dc.LineTo (iX, iY); 
							}

							dc.SelectObject(v_pen); 
							n_pen.DeleteObject(); 
						}
					}
					pstPrevMeasure=pstDataPoint;	
				}
			} 
		
		}
		if (GraphRequested==NONE) 
		{
			ShowHelp();
		}
 
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSonarDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}




void CSonarDlg::DrawDepthMap()
//Create a Depth map based on the data from the form,
//by acting on the Sonar unit and painting the measures taken
{

	SonarInit();		//Sonar device initialization

	CClientDC	dc(this);		//Device context for painting in dialog

	// Create a personalized drawing area 
	dc.SetMapMode (MM_ISOTROPIC);
	dc.SetWindowExt (360, 190);
	dc.SetViewportExt(m_cxClient, -m_cyClient); 
	dc.SetViewportOrg((int)m_cxClient*0.46 ,(int)m_cyClient*0.88);

	DrawDepthGrid();	//Draw Grid for Depth map

	cEcho=0;			//Echo to show
	
	for (cV=cMinV; cV<=cMaxV; cV+=cResol) { 

		for (cH=cMinH; cH<=cMaxH; cH+=cResol) { 
			SonarPos(cH, cV);
			if (cH==cMinH) Sleep(500);
			pstDataPoint=pstDataRange + (((cV-cMinV)/cResol)
										*((cMaxH-cMinH)/cResol) 
										+((cH-cMinH)/cResol));			
			DistCheck(pstDataPoint->iMeasure);

			//Paint measurement in specific tone depending on value
			if (pstDataPoint->iMeasure[cEcho]>iDepth || 
				pstDataPoint->iMeasure[cEcho]==0) cTone=0; //Echo farther from limit or no echo came (too far)
			else cTone=245-(pstDataPoint->iMeasure[cEcho]*245/iDepth);
			n_pen.CreatePen (BS_SOLID, 1, RGB(cTone,cTone,cTone)); 
			v_pen=dc.SelectObject(&n_pen) ;
			n_brush.CreateSolidBrush (RGB(cTone,cTone,cTone)); 
			v_brush=dc.SelectObject(&n_brush) ;


			dc.Rectangle(cH-(cResol/2),cV-(cResol/2),
						 cH+(cResol/2),cV+(cResol/2));
			dc.SelectObject(v_pen); 
			n_pen.DeleteObject(); 
			dc.SelectObject(v_brush); 
			n_brush.DeleteObject(); 

		}

	} 


}

void CSonarDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
		
	m_cxClient=cx; 
	m_cyClient=cy; 
	
}

void CSonarDlg::OnDepthMap() 
{

	UpdateData(TRUE);	//take variables from Form
	if (m_HAngleMin>=0 && m_HAngleMax<=180 && 
		m_VAngleMin>=0 && m_VAngleMax<=180)

		if (m_HAngleMin<=m_HAngleMax && m_VAngleMin<=m_VAngleMax)
		{
			iDepth	=m_DepthSw	=(990*m_DistMax/100)+10;
			cMaxH	=m_HAngleMax;
			cMinH	=m_HAngleMin;
			cResol	=m_ResSw	=((4*m_Resol/100)+1)*2;
			cMaxV	=m_VAngleMax;
			cMinV	=m_VAngleMin;
			FreeMemory();			//Free memory previously used
			GraphRequested=DEPTH;
			//Asign new memory area for measurements:
			pstDataRange = new stMeasure[(((cMaxV-cMinV)/cResol)+1)
										*(((cMaxH-cMinH)/cResol)+1)];	
			DrawDepthMap();
		}
		else MessageBox("Wrong input values. HMin>HMax or VMin>VMax","Error Message",MB_ICONEXCLAMATION);
	
}


////////////////////////////////////////////////////////
// SONAR DEVICE MANAGEMENT FUNCTIONS


void CSonarDlg::SonarInit()
//Initialize Sonar values
{
	char				cHServo=0,cVServo=1;		//Servo identifications
	const unsigned char	cSVI2CAdd=118;				//Address of the I2C Servo controller device
	const unsigned char	cHOffset=59;				//Offset value for H servo
	const unsigned char	cVOffset=64;				//Offset value for V servo


	//Horizontal servo Offset 
	Board.GenerateStartCondition();
	Board.OutputByteToI2C(cSVI2CAdd);		//Slave address
	Board.GenerateAcknowledge();
	Sleep(10);
	Board.OutputByteToI2C(2);				//Registry (Offset)
	Board.GenerateAcknowledge();
	Sleep(10);
	Board.OutputByteToI2C(cHServo);			//Device (Horizontal servo) 
	Board.GenerateAcknowledge();
	Sleep(10);
	Board.OutputByteToI2C(cHOffset);		//Value 	
	Board.GenerateAcknowledge();
	Board.GenerateStopCondition();
	Sleep(10);


	//Vertical servo Offset
	Board.GenerateStartCondition();
	Board.OutputByteToI2C(cSVI2CAdd);		//Slave address
	Board.GenerateAcknowledge();
	Sleep(10);
	Board.OutputByteToI2C(2);				//Registry (Offset)
	Board.GenerateAcknowledge();
	Sleep(10);
	Board.OutputByteToI2C(cVServo);			//Device (Vertical servo) 
	Board.GenerateAcknowledge();
	Sleep(10);
	Board.OutputByteToI2C(cVOffset);		//Value 
	Board.GenerateAcknowledge();
	Board.GenerateStopCondition();
	Sleep(10);

}




void CSonarDlg::SonarPos(int iHpos, int iVpos)
//locate the sonar sensor in the specified Horizontal and vertical
//position in degrees
{
	char				cHServo=0,cVServo=1;		//Servo identifications
	const int			iHLimit=180, iVLimit=180;	//Servo limits in degrees
	unsigned char		cHpos,cVpos;				//Position value to send to SVI2C device
	const unsigned char	cHStepsTo180=222;			//Steps in SVI2C equivalent to 180º for H servo
	const unsigned char	cVStepsTo180=235;			//Steps in SVI2C equivalent to 180º for V servo

	//Prevent going out of limits (damaging servos)
	if (iHpos>iHLimit) iHpos=iHLimit;
	if (iVpos>iVLimit) iVpos=iVLimit;
	if (iHpos<0) iHpos=0;
	if (iVpos<0) iVpos=0;


	//Convert values from degrees to steps to send to SVI2C device
	cHpos=(iHLimit-iHpos)*cHStepsTo180/iHLimit;
	cVpos=iVpos*cVStepsTo180/iVLimit;

	//Horizontal servo positioning
	Board.GenerateStartCondition();
	Board.OutputByteToI2C(cSVI2CAdd);		//Slave address
	Board.GenerateAcknowledge();
	Sleep(10);
	Board.OutputByteToI2C(3);				//Registry (Position)
	Board.GenerateAcknowledge();
	Sleep(10);
	Board.OutputByteToI2C(cHServo);			//Device (Horizontal servo) 
	Board.GenerateAcknowledge();
	Sleep(10);
	Board.OutputByteToI2C(cHpos);			//Value (in steps)	
	Board.GenerateAcknowledge();
	Board.GenerateStopCondition();
	Sleep(10);


	//Vertical servo positioning
	Board.GenerateStartCondition();
	Board.OutputByteToI2C(cSVI2CAdd);		//Slave address
	Board.GenerateAcknowledge();
	Sleep(10);
	Board.OutputByteToI2C(3);				//Registry (Position)
	Board.GenerateAcknowledge();
	Sleep(10);
	Board.OutputByteToI2C(cVServo);			//Device (Vertical servo) 
	Board.GenerateAcknowledge();
	Sleep(10);
	Board.OutputByteToI2C(cVpos);			//Value (in steps)
	Board.GenerateAcknowledge();
	Board.GenerateStopCondition();
	Sleep(100);

}




void CSonarDlg::DistCheck(int iDist[16])
//Request the SFR08 to make a distance measurement (17 echos)
//a value of 0 in one of the echo measurements means that this echo didn't come
{
	unsigned char		cDistH, cDistL;	//Distance measured (High & Low components)
	unsigned char		cEcho;			//Number of echo to read

	//Request boost and measure
	Board.GenerateStartCondition();
	Board.OutputByteToI2C(cSRF08Add);		//Slave address
	Board.GenerateAcknowledge();
	Board.OutputByteToI2C(0);				//Registry
	Board.GenerateAcknowledge();
	Board.OutputByteToI2C(81);				//Measurement in cm
	Board.GenerateAcknowledge();
	Board.GenerateStopCondition();

	Sleep(100);	//Pause for calculating distance at SRF08
	
	for (cEcho=0;cEcho<17;cEcho++)
	{
		//Request measure value
		Board.GenerateStartCondition();
		Board.OutputByteToI2C(cSRF08Add);	//Slave address
		Board.GenerateAcknowledge();
		Board.OutputByteToI2C(2+cEcho*2);	//Registry
		Board.GenerateAcknowledge();

		Board.GenerateStartCondition();
		Board.OutputByteToI2C(cSRF08Add+1);	//Slave address (read mode)
		Board.GenerateAcknowledge();
		cDistH=Board.InputByteFromI2C();	//Take High byte of measurement
		Board.GenerateAcknowledge();
		Board.GenerateStopCondition();

		Board.GenerateStartCondition();
		Board.OutputByteToI2C(cSRF08Add);	//Slave address
		Board.GenerateAcknowledge();
		Board.OutputByteToI2C(3+cEcho*2);	//Registry
		Board.GenerateAcknowledge();

		Board.GenerateStartCondition();
		Board.OutputByteToI2C(cSRF08Add+1);	//Slave address (read mode)
		Board.GenerateAcknowledge();
		cDistL=Board.InputByteFromI2C();	//Take Low byte of measurement
		Board.GenerateAcknowledge();
		Board.GenerateStopCondition();

		iDist[cEcho]=cDistH*256+cDistL;		//Calculate distance measured
	}

}



void CSonarDlg::OnCustomdrawResol(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);	//take variables from Form
	m_ResSw	=((4*m_Resol/100)+1)*2;
	UpdateData(FALSE);	//Update Form
	
	*pResult = 0;
}

void CSonarDlg::OnCustomdrawDistMax(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);	//take variables from Form
	m_DepthSw	=(990*m_DistMax/100)+10;
	UpdateData(FALSE);	//Update Form

	*pResult = 0;
}

void CSonarDlg::DrawDepthGrid()
//Draw Grid for Depth map
{
	CClientDC	dc(this);		//Device context for painting in dialog
	// Create a personalized drawing area 
	dc.SetMapMode (MM_ISOTROPIC);
	dc.SetWindowExt (360, 190);
	dc.SetViewportExt(m_cxClient, -m_cyClient); 
	dc.SetViewportOrg((int)m_cxClient*0.46 ,(int)m_cyClient*0.88);

	// Clean the drawing area
	n_pen.CreatePen (BS_SOLID, 1, dwColor[1]); 
	v_pen=dc.SelectObject(&n_pen) ;
	n_brush.CreateSolidBrush (RGB(0,0,150)); 
	v_brush=dc.SelectObject(&n_brush) ;
	dc.Rectangle(-6,186,186,-6);
	dc.SelectObject(v_pen); 
	n_pen.DeleteObject(); 
	dc.SelectObject(v_brush); 
	n_brush.DeleteObject(); 


	//Draw Grid
	n_pen.CreatePen (BS_SOLID, 1, dwColor[7]); 
	v_pen=dc.SelectObject(&n_pen) ;

	dc.SetBkColor(RGB(195,195,195));
	dc.TextOut(-25,5,"    0",5);
	dc.TextOut(-25,95,"  90",4);
	dc.TextOut(-25,185,"180",3);

	for (cV=0; cV<=180; cV+=cGridStep) 
	{
		dc.MoveTo (-10, cV);
		dc.LineTo(190,cV);
		if (cV>0) {
			dc.MoveTo (-8, cV-5);
			dc.LineTo(-6,cV-5);
			dc.MoveTo (186, cV-5);
			dc.LineTo(188,cV-5);
		}

	}

	dc.SetBkColor(RGB(195,195,195));
	dc.TextOut(-5,-10," 0 ",3);
	dc.TextOut(85,-10,"90",2);
	dc.TextOut(173,-10,"180",3);

	for (cH=0; cH<=180; cH+=cGridStep) 
	{
		dc.MoveTo (cH, -10);
		dc.LineTo(cH,190);
		if (cH>0) {
			dc.MoveTo (cH-5,-8);
			dc.LineTo(cH-5,-6);
			dc.MoveTo (cH-5,186);
			dc.LineTo(cH-5,188);
		}
	}


	dc.SelectObject(v_pen); 
	n_pen.DeleteObject(); 

}

void CSonarDlg::FreeMemory()
//Free memory previously used
{
	if (GraphRequested==DEPTH) 
		//Free the memory previously used:
		delete(pstDataRange); 
	if (GraphRequested==POLAR) 
		//Free the memory previously used:
		delete(pstDataRange); 
	GraphRequested=NONE;
}

void CSonarDlg::OnPolarMap() 
{
	UpdateData(TRUE);	//take variables from Form
	if (m_HAngleMin>=0 && m_HAngleMax<=180 && 
		m_VAngleMin>=0 && m_VAngleMax<=180)

		if (m_HAngleMin<=m_HAngleMax && m_VAngleMin<=m_VAngleMax)
		{
			iDepth	=m_DepthSw	=(990*m_DistMax/100)+10;
			cMaxH	=m_HAngleMax;
			cMinH	=m_HAngleMin;
			cResol	=m_ResSw	=((4*m_Resol/100)+1)*2;
			cMaxV	=m_VAngleMax;
			cMinV	=m_VAngleMin;
			FreeMemory();			//Free memory previously used
			GraphRequested=POLAR;
			//Asign new memory area for measurements:
			pstDataRange = new stMeasure[((cMaxH-cMinH)/cResol)+2];	
			DrawPolarMap();
		}
		else MessageBox("Wrong input values. HMin>HMax or VMin>VMax","Error Message",MB_ICONEXCLAMATION);
	
}

void CSonarDlg::DrawPolarGrid()
//Draw Grid for Polar map
{
	unsigned char cR,cX,cY;		//Used for Grid drawing
	CClientDC	dc(this);		//Device context for painting in dialog
	// Create a personalized drawing area 
	dc.SetMapMode (MM_ISOTROPIC);
	dc.SetWindowExt (360, 190);
	dc.SetViewportExt(m_cxClient, -m_cyClient); 
	dc.SetViewportOrg((int)m_cxClient*0.71 ,(int)m_cyClient*0.88);

	// Clean the drawing area
	n_pen.CreatePen (BS_SOLID, 1, RGB(195,195,195)); 
	v_pen=dc.SelectObject(&n_pen) ;
	n_brush.CreateSolidBrush (RGB(195,195,195)); 
	v_brush=dc.SelectObject(&n_brush) ;
	dc.Rectangle(-120,-30,110,190);
	dc.SelectObject(v_pen); 
	n_pen.DeleteObject(); 
	dc.SelectObject(v_brush); 
	n_brush.DeleteObject(); 
	

	//Paint background
	n_pen.CreatePen (BS_SOLID, 1, dwColor[2]); 
	v_pen=dc.SelectObject(&n_pen) ;
	n_brush.CreateSolidBrush (dwColor[8]); 
	v_brush=dc.SelectObject(&n_brush) ;
	dc.Rectangle(-96,-6,96,186);
	dc.SelectObject(v_pen); 
	n_pen.DeleteObject(); 
	dc.SelectObject(v_brush); 
	n_brush.DeleteObject(); 


	//Draw Grid
	n_pen.CreatePen (BS_SOLID, 1, RGB(195,195,195)); 
	v_pen=dc.SelectObject(&n_pen) ;
	dc.Rectangle(-90,0,90,180);
	for (cR=180; cR>0; cR-=18)
	{
		cY=(cR>90 ? cR*sin(acos(90/(double)cR)): 0);
		dc.Arc(cR,-cR,-cR,cR,90,cY,-90,cY);
	}
 	for (cH=0; cH<=90; cH+=cGridStep)
	{

		cX=(cH<60? 90 :180*cos(pi*(double)cH/180)); 
		cY=(cH<60? 90*tan(pi*(double)cH/180) : 180*sin(pi*(double)cH/180));
		dc.MoveTo (0, 0);
		dc.LineTo(cX,cY);
		dc.MoveTo (0, 0);
		dc.LineTo(-cX,cY);
	}


	dc.SelectObject(v_pen); 
	n_pen.DeleteObject(); 

}

void CSonarDlg::DrawPolarMap()
//Create a Polar map based on the data from the form,
//by acting on the Sonar unit and painting the measures taken
{

	int	iX,iY;			//Coordenates used for drawing

	SonarInit();		//Sonar device initialization

	CClientDC	dc(this);		//Device context for painting in dialog

	DrawPolarGrid();	//Draw Grid for Polar map

	// Create a personalized drawing area 
	dc.SetMapMode (MM_ISOTROPIC);
	dc.SetWindowExt (360, 190);
	dc.SetViewportExt(m_cxClient, -m_cyClient); 
	dc.SetViewportOrg((int)m_cxClient*0.71 ,(int)m_cyClient*0.88);
	
	cV=cMinV;  

	for (cH=cMinH; cH<=cMaxH; cH+=cResol) 
	{ 
		SonarPos(cH, cV);
		if (cH==cMinH) Sleep(500);
		pstDataPoint=pstDataRange + ((cH-cMinH)/cResol);			
		DistCheck(pstDataPoint->iMeasure);
		if (cH==cMinH)
			pstPrevMeasure=pstDataPoint;
		else
		{
			for (cEcho=0;cEcho<17;cEcho++)
			{
				if (pstPrevMeasure->iMeasure[cEcho] ==0 || 
					pstDataPoint->iMeasure[cEcho]==0); //No echo came. Too far. Nothing is painted
				else //echo came. It is represented if fits in screen
				{
				
					if (cEcho==0)
					{
						n_pen.CreatePen (BS_SOLID, 2, dwColor[2]); 
						v_pen=dc.SelectObject(&n_pen);

						iX=(pstPrevMeasure->iMeasure[cEcho]*180/iDepth)*cos(pi*(double)((180-cH)+cResol)/180);
						iY=(pstPrevMeasure->iMeasure[cEcho]*180/iDepth)*sin(pi*(double)((180-cH)+cResol)/180);
						if (iX>=-90 && iX<=90 && iY>=0 && iY<=180) //Is the first point in the printing area? 
						{
							dc.MoveTo (iX, iY); 

							iX=(pstDataPoint->iMeasure[cEcho]*180/iDepth)*cos(pi*(double)(180-cH)/180);
							iY=(pstDataPoint->iMeasure[cEcho]*180/iDepth)*sin(pi*(double)(180-cH)/180);
							if (iX>=-90 && iX<=90 && iY>=0 && iY<=180) //Is the second point in the printing area? 
								dc.LineTo (iX, iY); 
						}

						dc.SelectObject(v_pen); 
						n_pen.DeleteObject(); 
					}
					else
					{
						n_pen.CreatePen (BS_SOLID, 2, RGB(cEcho*16,cEcho*16,cEcho*16)); 
						v_pen=dc.SelectObject(&n_pen) ;
						iX=(pstDataPoint->iMeasure[cEcho]*180/iDepth)*cos(pi*(double)((180-cH)+(cResol/2))/180);
						iY=(pstDataPoint->iMeasure[cEcho]*180/iDepth)*sin(pi*(double)((180-cH)+(cResol/2))/180);
						if (iX>=-90 && iX<=90 && iY>=0 && iY<=180) //Is the first point in the printing area?  
						{
							dc.MoveTo (iX, iY); 

							iX=(pstDataPoint->iMeasure[cEcho]*180/iDepth)*cos(pi*(double)(180-cH)/180);
							iY=(pstDataPoint->iMeasure[cEcho]*180/iDepth)*sin(pi*(double)(180-cH)/180);
							if (iX>=-90 && iX<=90 && iY>=0 && iY<=180) //Is the second point in the printing area?  
								dc.LineTo (iX, iY); 
						}

						dc.SelectObject(v_pen); 
						n_pen.DeleteObject(); 
					}
				}
			}
			pstPrevMeasure=pstDataPoint;	
		}
	} 

}

void CSonarDlg::ShowHelp()
{
	CClientDC	dc(this);		//Device context for painting in dialog

	// Create a personalized drawing area 
	dc.SetMapMode (MM_ISOTROPIC);
	dc.SetWindowExt (360, 190);
	dc.SetViewportExt(m_cxClient, -m_cyClient); 
	dc.SetViewportOrg((int)m_cxClient*0.46 ,(int)m_cyClient*0.88);

	// Clean the drawing area
	n_pen.CreatePen (BS_SOLID, 1, RGB(195,195,195)); 
	v_pen=dc.SelectObject(&n_pen) ;
	n_brush.CreateSolidBrush (RGB(195,195,195)); 
	v_brush=dc.SelectObject(&n_brush) ;
	dc.Rectangle(-30,190,190,-20);
	dc.SelectObject(v_pen); 
	n_pen.DeleteObject(); 
	dc.SelectObject(v_brush); 
	n_brush.DeleteObject(); 


	dc.SetBkColor(RGB(195,195,195));
	dc.SetTextColor(dwColor[0]);
	dc.TextOut(0,180,"Polar Map");
	dc.SetTextColor(dwColor[7]);
	dc.TextOut(10,165,"Make a polar map with 15 echos");
	dc.TextOut(10,150,"1)Chose horizontal range values");
	dc.TextOut(10,140,"2)Put in V Min the vertical possition");
	dc.TextOut(10,130,"3)Establish resolution value");
	dc.TextOut(10,120,"4)Establish Depth (Maximum distance)");
	dc.TextOut(10,110,"5)Click 'Polar Map'");

	dc.SetBkColor(RGB(195,195,195));
	dc.SetTextColor(dwColor[0]);
	dc.TextOut(0,90,"Depth Map");
	dc.SetTextColor(dwColor[7]);
	dc.TextOut(10,75,"Make a bidimensional depth map");
	dc.TextOut(10,60,"1)Chose horizontal range values");
	dc.TextOut(10,50,"2)Chose vertical range values");
	dc.TextOut(10,40,"3)Establish resolution value");
	dc.TextOut(10,30,"4)Establish Depth (Maximum distance)");
	dc.TextOut(10,20,"5)Click 'Depth Map'");


}

void CSonarDlg::OnHelp() 
{
	ShowHelp();	
	FreeMemory();
}
