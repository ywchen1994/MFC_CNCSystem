#pragma once
#include "afxwin.h"
#include"cv.h"
#include"highgui.h"
struct CTab1threadParam
{
public:
	HWND hWnd;
	LPVOID m_lpPara;
	UINT   m_case;
	BOOL m_blthreading;
};



class tab1Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(tab1Dlg)

public:
	tab1Dlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~tab1Dlg();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_tab1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	BOOL OnInitDialog();


	DECLARE_MESSAGE_MAP()
public:
	CStatic m_Img_Canny;
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	void ShowImage(IplImage * Image, CWnd * pWnd, int channels);
	/******************************/
	CTab1threadParam m_threadPara;
	CWinThread*  m_lpThread;
	static UINT Tab1threadFun(LPVOID LParam);
	void Thread_Image_CannyROISetting(LPVOID lParam);
	/**********************************/
	void SetRoI(IplImage * img_edge);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	int m_XPos;
	int m_YPos;

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonSetreferencepoint();
	afx_msg void OnBnClickedButtonUpdateimage();
};
