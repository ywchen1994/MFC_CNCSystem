// tab3Dlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MFC_CNCSystem.h"
#include "tab3Dlg.h"
#include "afxdialogex.h"
#include "MFC_CNCSystemDlg.h"

// tab3Dlg 對話方塊

IMPLEMENT_DYNAMIC(tab3Dlg, CDialogEx)

tab3Dlg::tab3Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_tab3, pParent)
{

}

tab3Dlg::~tab3Dlg()
{
}

void tab3Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_IMAGE_CannyROI_Channel1, m_CannyROI_Channel1);
	DDX_Control(pDX, IDC_IMAGE_Object, m_Img_Object);
}


BEGIN_MESSAGE_MAP(tab3Dlg, CDialogEx)
	ON_WM_LBUTTONDOWN()
	
END_MESSAGE_MAP()

BOOL tab3Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_CannyROI_Channel1.SetWindowPos(NULL, 10, 10, 320, 265, SWP_SHOWWINDOW);
	m_Img_Object.SetWindowPos(NULL, 10, 10+ 265, 320, 265, SWP_SHOWWINDOW);
	return TRUE;
}


void tab3Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_threadPara.m_case = 0;
	m_threadPara.hWnd = m_hWnd;
	m_lpThread = AfxBeginThread(&tab3Dlg::Tab3threadFun, (LPVOID)&m_threadPara);

	CDialogEx::OnLButtonDown(nFlags, point);
}
UINT tab3Dlg::Tab3threadFun(LPVOID LParam)
{
	CTab3threadParam* para = (CTab3threadParam*)LParam;
	tab3Dlg* lpview = (tab3Dlg*)(para->m_lpPara);
	para->m_blthreading = TRUE;

	switch (para->m_case)
	{
	case 0:
		lpview->Thread_Image_CannyROI(LParam);
	case 1:
	
	default:
		break;
	}

	para->m_blthreading = FALSE;
	para->m_case = 0xFF;
	return 0;
}
CvPoint tab3Dlg::GetCenterPoint(IplImage *src)
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
void tab3Dlg::Thread_Image_CannyROI(LPVOID lParam)
{
	CTab3threadParam * Thread_Info = (CTab3threadParam *)lParam;
	tab3Dlg * hWnd = (tab3Dlg *)CWnd::FromHandle((HWND)Thread_Info->hWnd);
	CMFC_CNCSystemDlg mainDlg;
	IplImage *Img_CannyROI_CH1 =cvCreateImage(cvSize(512,424),IPL_DEPTH_8U,1);
	IplImage *Img_ApproxPoly_Ch1 = cvCreateImage(cvSize(512, 424), IPL_DEPTH_8U, 1);
	IplImage *Img_ApproxPoly_Ch3 = cvCreateImage(cvSize(512, 424), IPL_DEPTH_8U, 3);
	CvPoint CenterPoint;
	CvPoint CornerPoint[4];
	
	while (1)
	{	
			cvCopy(mainDlg.s_Img_Canny,Img_CannyROI_CH1);
			if (mainDlg.StationTwoStop  && CMFC_CNCSystemDlg::axis6Package.beProcessed)
			{
				//更新canny ROI
				hWnd->SetRoI(Img_CannyROI_CH1);
				hWnd->ShowImage(Img_CannyROI_CH1, hWnd->GetDlgItem(IDC_IMAGE_CannyROI_Channel1), 1);
				//取得中心
				CenterPoint = GetCenterPoint(Img_CannyROI_CH1);
				//多邊形近似
				ApproxPoly(Img_CannyROI_CH1, Img_ApproxPoly_Ch1);
				cvCvtColor(Img_ApproxPoly_Ch1, Img_ApproxPoly_Ch3,CV_GRAY2RGB);
				//取得角點
				CornerDetection(Img_ApproxPoly_Ch1, &CornerPoint[0]);
				
				//排序
				sequencePoint(&CornerPoint[0], CenterPoint);
				for (int i=0;i<4;i++)
				{
					cvCircle(Img_ApproxPoly_Ch3, CornerPoint[i],2,CV_RGB(255,0,255),CV_FILLED);
				}
				cvCircle(Img_ApproxPoly_Ch3, CenterPoint, 2, CV_RGB(255, 0, 255), CV_FILLED);
				hWnd->ShowImage(Img_ApproxPoly_Ch3, hWnd->GetDlgItem(IDC_IMAGE_Object),3);
	
				Img2SixAxis(&CornerPoint[0], CenterPoint, &CMFC_CNCSystemDlg::axis6Package.X, &CMFC_CNCSystemDlg::axis6Package.Y, &CMFC_CNCSystemDlg::axis6Package.Z, &CMFC_CNCSystemDlg::axis6Package.theta);
				Sleep(600);
				CMFC_CNCSystemDlg::axis6Package.beProcessed = false;
			
			}
	}
	cvReleaseImage(&Img_ApproxPoly_Ch1);
	cvReleaseImage(&Img_ApproxPoly_Ch3);
	cvReleaseImage(&Img_CannyROI_CH1);
}
void tab3Dlg::SetRoI(IplImage* img_edge)//將工作平台以外畫成黑色Input CH1 Ouput Ch1
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
void tab3Dlg::CornerDetection(IplImage* edge_roi,CvPoint* CornerPoint)
{
	IplImage* src = nullptr;
	src = cvCreateImage(cvGetSize(edge_roi), edge_roi->depth, 1);
	cvCopy( edge_roi, src);


	IplImage *dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, 1);
	IplImage *dst_8U = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

	cvSetZero(dst);
	cvCornerHarris(src, dst, 5, 7);//@harris
	cvConvertScale(dst, dst_8U, 255, 0);
	cvThreshold(dst_8U, dst_8U, 1, 255, CV_THRESH_BINARY);

	//這裡只做Harris由於Harris的角點是範圍的所以才要有HarrisCornerToPoint
	HarrisCornerToPoint(dst_8U,&CornerPoint[0]);
	cvReleaseImage(&src);
	cvReleaseImage(&dst);
	cvReleaseImage(&dst_8U);
}
void tab3Dlg::ApproxPoly(IplImage *img_roi, IplImage *Img_ApproxPoly)
{
	IplImage* src = NULL;
	IplImage* img = NULL;

	CvMemStorage* storage = cvCreateMemStorage(0);
	CvMemStorage* storage1 = cvCreateMemStorage(0);
	CvSeq* contour = 0;
	CvSeq* cont;
	CvSeq* mcont;

	src = cvCreateImage(cvGetSize(img_roi), img_roi->depth, 1);
	cvCopy(img_roi, src);
	IplImage* First = cvCreateImage(cvSize(src->width, src->height), src->depth, 3);
	IplImage* Sec = cvCreateImage(cvSize(src->width, src->height), src->depth, 3);
	IplImage* InsideImage = cvCreateImage(cvSize(src->width, src->height), src->depth, 1);
	
	cvSetZero(First);
	cvSetZero(Sec);

	cvThreshold(src, src, 150, 255, CV_THRESH_BINARY);
	cvFindContours(src, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

	if (contour)
	{
		CvTreeNodeIterator iterator;
		cvInitTreeNodeIterator(&iterator, contour, 1);
		while (0 != (cont = (CvSeq*)cvNextTreeNode(&iterator)))
		{
			cvSetZero(Sec);
			mcont = cvApproxPoly(cont, sizeof(CvContour), storage1, CV_POLY_APPROX_DP, cvContourPerimeter(cont)*0.02, 0);
			cvDrawContours(Sec, mcont, CV_RGB(255,0,0 ), CV_RGB(255, 255, 255), 1, 1, 8, cvPoint(0, 0));
			InsideImage=findinside(Sec);
			if (InsideImage !=nullptr)
			{
				
				cvCopy(InsideImage, Img_ApproxPoly);
			}
			cvCopy(Sec, First);
		}
	}
	cvReleaseImage(&InsideImage);
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&src);
	cvReleaseImage(&img);

	cvReleaseImage(&First);
	cvReleaseImage(&Sec);
}

IplImage * tab3Dlg::findinside(IplImage * Img)
{
	double tmp;
	bool inside = false;
	IplImage *mask = cvCreateImage(cvSize(Img->width, Img->height), Img->depth, 1);
	cvInRangeS(Img, cvScalar(0, 255, 0), cvScalar(255, 255, 255), mask);
	for (int i = 0; i < Img->height; i++) {
		for (size_t j = 0; j <Img->width; j++) {
			tmp = cvGet2D(mask, i, j).val[0];
			if (tmp > 0)inside = true;
		}
	}

	if (inside)
	{
		return mask;
	}
	else
	{
		return nullptr;
	}
	cvReleaseImage(&mask);
	
}

void tab3Dlg::HarrisCornerToPoint(IplImage* Cornerimage, CvPoint* CornerPoint)
{
	IplImage *Sec = cvCreateImage(cvGetSize(Cornerimage), Cornerimage->depth, 3);
	IplImage *First = cvCreateImage(cvGetSize(Cornerimage), Cornerimage->depth, 3);

	CvMemStorage *storage = cvCreateMemStorage(0);
	CvSeq*contours;
	cvFindContours(Cornerimage, storage, &contours, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));

	int CornerCounter = 0;
	for (; contours != 0; contours = contours->h_next)
	{
		cvSetZero(Sec);
		cvDrawContours(Sec, contours, CV_RGB(255, 255, 255), CV_RGB(0, 0, 0), -1, CV_FILLED, 8, cvPoint(0, 0));

		CvPoint corner = GetCenterPoint(Sec);
		CornerPoint[CornerCounter] = corner;
		CornerCounter++;
		cvCopy(Sec, First);
	}

	cvReleaseImage(&Sec);
	cvReleaseImage(&First);
}
void tab3Dlg::ShowImage(IplImage * Image, CWnd * pWnd, int channels)
{
	CDC	*dc = pWnd->GetWindowDC();
	IplImage *Temp = NULL;
	Temp = cvCreateImage(cvSize(320, 265), IPL_DEPTH_8U, channels);
	cvResize(Image, Temp, CV_INTER_LINEAR);

	CvvImage Temp2;
	Temp2.CopyOf(Temp);
	Temp2.Show(*dc, 0, 0, Temp->width, Temp->height);
	cvReleaseImage(&Temp);
	ReleaseDC(dc);
}
void tab3Dlg::Img2SixAxis(CvPoint* CornerPoint, CvPoint CenterPoint, float * pRobotX, float * pRobotY, float * pRobotZ, float* theta)
{
	//用以排序的cornerPoint算角度  (往短邊的角度)
	CvPoint2D32f mediumPoint;
	mediumPoint = cvPoint2D32f(0.5*(CornerPoint[0].x + CornerPoint[3].x), 0.5*(CornerPoint[0].y + CornerPoint[3].y));
	*theta = getDegree(cvPointTo32f(CenterPoint), mediumPoint);
	//center point to Robot pos
	CMFC_CNCSystemDlg mainDlg;
	mainDlg.kinect.Depth2CameraSpace(CenterPoint.x, CenterPoint.y);

	*pRobotX = mainDlg.kinect.CameraY * 1000 - mainDlg.KinectReferencePointXY.y + ROBOT_ORIGIN_OFFSET_X;
	*pRobotY = mainDlg.kinect.CameraX * 1000-mainDlg.KinectReferencePointXY.x+  + ROBOT_ORIGIN_OFFSET_Y;
	*pRobotZ = (mainDlg.tableReferenceDistance[CenterPoint.x][CenterPoint.y] - mainDlg.kinect.CameraZ * 1000);
	GetDlgItem(IDC_EDIT_XPos)->SetWindowText(_T("%.2f", *pRobotX));
	GetDlgItem(IDC_EDIT_YPos)->SetWindowText(_T("%.2f", *pRobotY));
	GetDlgItem(IDC_EDIT_ZPos)->SetWindowText(_T("%.2f", *pRobotZ));
	GetDlgItem(IDC_EDIT_theta)->SetWindowText(_T("%.2f",*theta));
}
float tab3Dlg::getDegree(CvPoint2D32f first, CvPoint2D32f second)
{
	float x = second.x - first.x;
	float y = second.y - first.y;
	float m = y / x;
	float degree;

	degree = atan(m);

	return degree * 180 / 3.1415926535897932384626433832;
}

void tab3Dlg::sequencePoint(CvPoint * corner, CvPoint center)
{

	//-----排序 CornerPoint x 小到大(左到右)--------------------
	for (int n = 0; n < 3; n++)//做三次
	{
		for (int i = 0; i < 3; i++)//0跟1比......2跟3比
		{
			if (corner[i].x > corner[i + 1].x)
			{
				//交換
				CvPoint temp;
				temp = corner[i];
				corner[i] = corner[i + 1];
				corner[i + 1] = temp;
			}
		}
	}
	//-------------------------------------------------------


	//特殊狀況 偵測到梯形---------
	if (corner[0].y >corner[1].y && corner[3].y >corner[2].y)
	{
		CvPoint temp_23change;
		temp_23change = corner[2];
		corner[2] = corner[3];
		corner[3] = temp_23change;
	}
	if (corner[1].y >corner[0].y && corner[2].y >corner[3].y)
	{
		CvPoint temp_23change;
		temp_23change = corner[2];
		corner[2] = corner[3];
		corner[3] = temp_23change;
	}
	//---------------------------

	//算出外圈的點 供推方塊使用
	CvPoint2D32f mediumPoint[4];
	mediumPoint[0] = cvPoint2D32f(0.5*(corner[0].x + corner[1].x), 0.5*(corner[0].y + corner[1].y));
	mediumPoint[1] = cvPoint2D32f(0.5*(corner[1].x + corner[3].x), 0.5*(corner[1].y + corner[3].y));
	mediumPoint[2] = cvPoint2D32f(0.5*(corner[3].x + corner[2].x), 0.5*(corner[3].y + corner[2].y));
	mediumPoint[3] = cvPoint2D32f(0.5*(corner[2].x + corner[0].x), 0.5*(corner[2].y + corner[0].y));
	float length[4];
	length[0] = sqrt(pow((center.x - mediumPoint[0].x), 2) + pow((center.y - mediumPoint[0].y), 2));
	length[1] = sqrt(pow((center.x - mediumPoint[1].x), 2) + pow((center.y - mediumPoint[1].y), 2));
	length[2] = sqrt(pow((center.x - mediumPoint[2].x), 2) + pow((center.y - mediumPoint[2].y), 2));
	length[3] = sqrt(pow((center.x - mediumPoint[3].x), 2) + pow((center.y - mediumPoint[3].y), 2));


	const int radius1 = 35;
	


	//2 3 對調 使 點 順時針照順序
	CvPoint temp;
	temp = corner[2];
	corner[2] = corner[3];
	corner[3] = temp;

	//0 1 要是長邊
	float distance1, distance2;
	distance1 = pow((corner[1].x - corner[0].x), 2) + pow((corner[1].y - corner[0].y), 2);
	distance2 = pow((corner[3].x - corner[0].x), 2) + pow((corner[3].y - corner[0].y), 2);
	if (distance1 < distance2)
	{
		CvPoint temp;
		temp = corner[1];
		corner[1] = corner[2];
		corner[2] = corner[3];
		corner[3] = corner[0];
		corner[0] = temp;

		CvPoint2D32f tempOut;
		
	}



	//轉成順時針
	int Array1[] = { corner[1].x - corner[0].x ,corner[1].y - corner[0].y };
	int Array2[] = { corner[3].x - corner[0].x ,corner[3].y - corner[0].y };
	int cross = Array1[0] * Array2[1] - Array1[1] * Array2[0];

	if (cross < 0)
	{
		CvPoint temp;
		temp = corner[0];
		corner[0] = corner[1];
		corner[1] = temp;

		temp = corner[2];
		corner[2] = corner[3];
		corner[3] = temp;

	}
}

