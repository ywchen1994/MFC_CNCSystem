#pragma once
#include "afxwin.h"
#include "zbar.h"
#include"cv.h"
#include"highgui.h"
struct CTab2threadParam
{
public:
	HWND hWnd;
	LPVOID m_lpPara;
	UINT   m_case;
	BOOL m_blthreading;
};

// tab2Dlg 對話方塊

class tab2Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(tab2Dlg)

public:
	tab2Dlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~tab2Dlg();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_tab2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	//thread
	CTab2threadParam m_threadPara;
	CWinThread*  m_lpThread;
	static UINT Tab2threadFun(LPVOID LParam);
	void Thread_Image_WebCamLive(LPVOID lParam);
	void Thread_Image_KinectLive(LPVOID lParam);
	void Thread_goRobot(LPVOID lParam);

	CStatic m_Img_WebCamLive;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonconnect();
	afx_msg void OnBnClickedButtonSetpos1();
	void ShowImage(IplImage *Image, CWnd * pWnd, int channels,CvSize size);
	CStatic m_Img_KinectLive;
	static bool SetStation1;
	static bool SetStation2;
	afx_msg void OnBnClickedButtonSetpos2();
	static CvPoint QRInKinectPos;
	static CvPoint QRInWebPos;
	afx_msg void OnBnClickedButtonGo();
	afx_msg void OnBnClickedButtonForward();
	afx_msg void OnBnClickedButtonBackward();
	afx_msg void OnBnClickedButtonStop();
};
