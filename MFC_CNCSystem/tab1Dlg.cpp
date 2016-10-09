// tab1Dlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MFC_CNCSystem.h"
#include "tab1Dlg.h"
#include "afxdialogex.h"
#include"MFC_CNCSystemDlg.h"

// tab1Dlg 對話方塊

IMPLEMENT_DYNAMIC(tab1Dlg, CDialogEx)

tab1Dlg::tab1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_tab1, pParent)
	, m_XPos(0)
	, m_YPos(0)
{

}

tab1Dlg::~tab1Dlg()
{
}

void tab1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE_Canny, m_Img_Canny);
	DDX_Text(pDX, IDC_STATIC_XPos, m_XPos);
	DDX_Text(pDX, IDC_STATIC_YPos, m_YPos);
}


BEGIN_MESSAGE_MAP(tab1Dlg, CDialogEx)
	ON_WM_MBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_SetReferencePoint, &tab1Dlg::OnBnClickedButtonSetreferencepoint)
	ON_BN_CLICKED(IDC_BUTTON_UpDateImage, &tab1Dlg::OnBnClickedButtonUpdateimage)
END_MESSAGE_MAP()


BOOL tab1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_Img_Canny.SetWindowPos(NULL, 10, 10, 512, 424, SWP_SHOWWINDOW);
	return TRUE;
}


void tab1Dlg::ShowImage(IplImage * Image, CWnd * pWnd, int channels)
{
	CDC	*dc = pWnd->GetWindowDC();
	IplImage *Temp = NULL;

	Temp = cvCreateImage(cvSize(512, 424), IPL_DEPTH_8U, channels);
	cvResize(Image, Temp, CV_INTER_LINEAR);

	CvvImage Temp2;
	Temp2.CopyOf(Temp);
	Temp2.Show(*dc, 0, 0, Temp->width, Temp->height);
	cvReleaseImage(&Temp);
	ReleaseDC(dc);
}

void tab1Dlg::OnMButtonDown(UINT nFlags, CPoint point)
{
	if (point.x > 10  && point.x < (10 + 512) && point.y > 10 && point.y < (10 + 424))
	{
	
		

	}
	CDialogEx::OnMButtonDown(nFlags, point);
}
void tab1Dlg::Thread_Image_CannyROISetting(LPVOID lParam)
{
	CTab1threadParam * Thread_Info = (CTab1threadParam *)lParam;
	tab1Dlg * hWnd = (tab1Dlg *)CWnd::FromHandle((HWND)Thread_Info->hWnd);
	IplImage* img_Canny_Ch3;
	IplImage* img_CannyRoi_Ch1;

	CMFC_CNCSystemDlg mainDlg;

	img_Canny_Ch3 = cvCreateImage(cvSize(512, 424), IPL_DEPTH_8U, 3);
	img_CannyRoi_Ch1 = cvCreateImage(cvSize(512, 424), IPL_DEPTH_8U, 1);


	cvCvtColor(mainDlg.s_Img_Canny, img_Canny_Ch3, CV_GRAY2BGR);
	cvCopy(mainDlg.s_Img_Canny, img_CannyRoi_Ch1);


	hWnd->SetRoI(img_CannyRoi_Ch1);
	cvCopy(img_CannyRoi_Ch1, mainDlg.s_Img_CannyROI);


	cvRectangle(img_Canny_Ch3, mainDlg.RoiPoint[0], mainDlg.RoiPoint[1], CV_RGB(255, 0, 0));

	hWnd->ShowImage(img_Canny_Ch3, hWnd->GetDlgItem(IDC_IMAGE_Canny), 3);
	cvReleaseImage(&img_Canny_Ch3);
	cvReleaseImage(&img_CannyRoi_Ch1);


}
void tab1Dlg::SetRoI(IplImage* img_edge)//將工作平台以外畫成黑色Input CH1 Ouput Ch1
{
	CMFC_CNCSystemDlg mainDlg;
	IplImage* img_roi_Ch3 = cvCreateImage(cvGetSize(img_edge), IPL_DEPTH_8U, 3);
	cvCvtColor(img_edge, img_roi_Ch3, CV_GRAY2BGR);


	for (int j = 0; j< img_edge->height; j++) {
		for (size_t i = 0; i < img_edge->width; i++) {
			if (i< mainDlg.RoiPoint[0].x || i>mainDlg.RoiPoint[1].x || j<mainDlg.RoiPoint[0].y || j>mainDlg.RoiPoint[1].y) {
				cvSet2D(img_roi_Ch3, j, i, CV_RGB(0, 0, 0));
			}
		}
	}
	cvCvtColor(img_roi_Ch3, img_edge, CV_BGR2GRAY);

	cvReleaseImage(&img_roi_Ch3);

}


void tab1Dlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (point.x > (10) && point.x < (10 + 512) && point.y > 10 && point.y < (10 + 424))
	{
		m_XPos = point.x - 10;
		m_YPos = point.y - 10;
		UpdateData(false);
		if (nFlags == MK_LBUTTON)
		{
			CMFC_CNCSystemDlg mainDlg;
			mainDlg.RoiPoint[1] = cvPoint(point.x - 10, point.y - 10);
		}
		CDialogEx::OnMouseMove(nFlags, point);
	}
}

UINT tab1Dlg::Tab1threadFun(LPVOID LParam)
{
	CTab1threadParam* para = (CTab1threadParam*)LParam;
	tab1Dlg* lpview = (tab1Dlg*)(para->m_lpPara);
	para->m_blthreading = TRUE;

	switch (para->m_case)
	{
	case 0:
		lpview->Thread_Image_CannyROISetting(LParam);

	default:
		break;
	}

	para->m_blthreading = FALSE;
	para->m_case = 0xFF;
	return 0;

}

void tab1Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (point.x > (10) && point.x < (10 + 512) && point.y > 10 && point.y < (10 + 424))
	{
		CMFC_CNCSystemDlg mainDlg;
		mainDlg.RoiPoint[0] = cvPoint(point.x - 10, point.y - 10);
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}

CvPoint GetCenterPoint(IplImage *src)
{
	int x0 = 0, y0 = 0, sum = 0;
	CvPoint center;
	CvScalar pixel;
	for (int i = 0; i < src->width; i++) {
		for (int j = 0; j < src->height; j++) {
			pixel = cvGet2D(src, j, i);
			if (pixel.val[0] > 0)
			{
				x0 = x0 + i;
				y0 = y0 + j;
				sum = sum + 1;
			}
		}
	}
	if (sum == 0) {
		center.x = 0;
		center.y = 0;
		return center;
	}
	center.x = x0 / sum;
	center.y = y0 / sum;
	return center;
}
void tab1Dlg::OnBnClickedButtonSetreferencepoint()
{
	CMFC_CNCSystemDlg mainDlg;
	IplImage* img_CannyRoi = cvCreateImage(cvGetSize(mainDlg.s_Img_CannyROI), mainDlg.s_Img_CannyROI->depth, 1);
	cvCopy(mainDlg.s_Img_CannyROI, img_CannyRoi);
	CvPoint RefPointPixel = GetCenterPoint(img_CannyRoi);//取得參考點的影像座標
	cvReleaseImage(&img_CannyRoi);
	mainDlg.kinect.Depth2CameraSpace(RefPointPixel.x, RefPointPixel.y);
	//將參考點的影像座標轉為mm
	mainDlg.KinectReferencePointXY.x = mainDlg.kinect.CameraX * 1000;
	mainDlg.KinectReferencePointXY.y = mainDlg.kinect.CameraY * 1000;

	CString str;
	str.Format(_T("( %.2f  ,  %.2f   )"), mainDlg.KinectReferencePointXY.x, mainDlg.KinectReferencePointXY.y);
	GetDlgItem(IDC_EDIT_SetReferencePoint)->SetWindowText(str);
}
void tab1Dlg::OnBnClickedButtonUpdateimage()
{
	m_threadPara.m_case = 0;
	m_threadPara.hWnd = m_hWnd;
	m_lpThread = AfxBeginThread(&tab1Dlg::Tab1threadFun, (LPVOID)&m_threadPara);
}
