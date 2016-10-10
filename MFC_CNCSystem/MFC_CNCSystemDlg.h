
// MFC_CNCSystemDlg.h : 標頭檔
//

#pragma once
#include "afxcmn.h"
#include"..//Kinect2Capture//Kinect2Capture.h"
#include "tab1Dlg.h"
#include "tab2Dlg.h"
#include "tab3Dlg.h"

#include "CvvImage.h"
#include"cv.h"
#include"highgui.h"
#include<afxsock.h>

#include"RobotDefine.h"


struct CMythreadParam
{
public:
	HWND hWnd;
	LPVOID m_lpPara;
	UINT   m_case;
	BOOL m_blthreading;
};

// CMFC_CNCSystemDlg 對話方塊
class CMFC_CNCSystemDlg : public CDialogEx
{
// 建構
public:
	CMFC_CNCSystemDlg(CWnd* pParent = NULL);	// 標準建構函式

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_CNCSYSTEM_DIALOG };
#endif
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援
// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	
	/****************Tab Control****************************/
	CTabCtrl m_Tab;
	tab1Dlg m_TabPage1;
	tab2Dlg m_TabPage2;
	tab3Dlg m_TabPage3;
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	/*************Thread******************/
	static UINT MythreadFun(LPVOID LParam);
	void Thread_Image_Depth(LPVOID lParam);
	void Thread_StatementUpdate(LPVOID lParam);
	void Thread_Image_RGB(LPVOID lParam);
	void Thread_Server_SixAxis(LPVOID lParam);
	void Thread_Server_Palletizing();
	void Thread_Server_Palletizing(LPVOID lParam);
	CMythreadParam m_threadPara;
	CWinThread*  m_lpThread;
	/*****************Static************************/
	static Kinect2Capture kinect;
	static IplImage * s_Img_WebCam;
	static IplImage * s_Img_RGB;
	static IplImage * s_Img_Depth;
	static IplImage * s_Img_Canny;
	static IplImage * s_Img_CannyROI;
	static CvPoint RoiPoint[2];
	static CvPoint Station1;
	static CvPoint Station2;
	static short int CarStatement;//0:車子不在辨識範圍內 1:在WebCam裡 2:在Kinect裡
	static CvPoint2D32f KinectReferencePointXY;
	static float tableReferenceDistance[512][424];
	static bool StationOneStop;
	static bool StationTwoStop;
	static bool SixAxisDone;
	static bool PalletizingDone;
	static DataPackage axis6Package;
	/****************Others*********************/
	CString m_CarStatement;
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButtonstation1();
	afx_msg void OnBnClickedButton3();
	void Thread_Server_SixAxis();
};
