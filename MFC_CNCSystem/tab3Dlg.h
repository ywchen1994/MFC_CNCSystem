#pragma once
#include "afxwin.h"
#include"cv.h"
#include"highgui.h"
#include"RobotDefine.h"

struct CTab3threadParam
{
public:
	HWND hWnd;
	LPVOID m_lpPara;
	UINT   m_case;
	BOOL m_blthreading;
};

class tab3Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(tab3Dlg)

public:
	tab3Dlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~tab3Dlg();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_tab3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:

	//thread
	CTab3threadParam m_threadPara;
	CWinThread*  m_lpThread;
	static UINT Tab3threadFun(LPVOID LParam);
	CvPoint GetCenterPoint(IplImage * src);
	void Thread_Image_CannyROI(LPVOID lParam);

	void SetRoI(IplImage * img_edge);

	void CornerDetection(IplImage * edge_roi, CvPoint * CornerPoint);

	void ApproxPoly(IplImage * img_roi, IplImage *Img_ApproxPoly);

IplImage * findinside(IplImage * Img);

	void HarrisCornerToPoint(IplImage * Cornerimage, CvPoint * CornerPoint);

	void ShowImage(IplImage * Image, CWnd * pWnd, int channels);
	void Img2SixAxis(CvPoint * CornerPoint, CvPoint CenterPoint, float * pRobotX, float * pRobotY, float * pRobotZ, float * theta);

	float getDegree(CvPoint2D32f first, CvPoint2D32f second);
	
	void sequencePoint(CvPoint * corner, CvPoint center);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	CStatic m_CannyROI_Channel1;

	CStatic m_Img_Object;
};
