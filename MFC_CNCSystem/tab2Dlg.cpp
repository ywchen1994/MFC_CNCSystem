// tab2Dlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MFC_CNCSystem.h"
#include "tab2Dlg.h"
#include "afxdialogex.h"
#include "Aria.h"
#include"MFC_CNCSystemDlg.h"

using namespace zbar;
using namespace cv;
// tab2Dlg 對話方塊

ArTcpConnection con;
ArSerialConnection serial;
ArRobot robot;
ArTime start;
bool tab2Dlg::SetStation1 = false;
bool tab2Dlg::SetStation2 = false;
CvPoint tab2Dlg::QRInWebPos = {0};
CvPoint tab2Dlg::QRInKinectPos = {0};
IMPLEMENT_DYNAMIC(tab2Dlg, CDialogEx)

tab2Dlg::tab2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_tab2, pParent)
{

}

tab2Dlg::~tab2Dlg()
{
}

void tab2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE_WebCamLive, m_Img_WebCamLive);
	DDX_Control(pDX, IDC_Image_KinectLive, m_Img_KinectLive);
}


BEGIN_MESSAGE_MAP(tab2Dlg, CDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_connect, &tab2Dlg::OnBnClickedButtonconnect)
	ON_BN_CLICKED(IDC_BUTTON_SetPos1, &tab2Dlg::OnBnClickedButtonSetpos1)
	ON_BN_CLICKED(IDC_BUTTON_SetPos2, &tab2Dlg::OnBnClickedButtonSetpos2)
	ON_BN_CLICKED(IDC_BUTTON_Go, &tab2Dlg::OnBnClickedButtonGo)
	ON_BN_CLICKED(IDC_BUTTON_Forward, &tab2Dlg::OnBnClickedButtonForward)
	ON_BN_CLICKED(IDC_BUTTON_Backward, &tab2Dlg::OnBnClickedButtonBackward)
	ON_BN_CLICKED(IDC_BUTTON_Stop, &tab2Dlg::OnBnClickedButtonStop)
END_MESSAGE_MAP()


// tab2Dlg 訊息處理常式
BOOL tab2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    m_Img_WebCamLive.SetWindowPos(NULL, 10, 10, 320, 240, SWP_SHOWWINDOW);
	m_Img_KinectLive.SetWindowPos(NULL, 10+320, 10 , 480, 270, SWP_SHOWWINDOW);
	return TRUE;
}

void tab2Dlg::Thread_Image_WebCamLive(LPVOID lParam)
{
	CTab2threadParam * Thread_Info = (CTab2threadParam *)lParam;
	tab2Dlg * hWnd = (tab2Dlg *)CWnd::FromHandle((HWND)Thread_Info->hWnd);
	CMFC_CNCSystemDlg mainDlg;
	CvCapture* Capture = cvCaptureFromCAM(0);
	CListBox* m_listBox_msg = (CListBox*)hWnd->GetDlgItem(IDC_LIST_QRcodeInfo);
	Mat Image_WebCamLive;
	while (1)
	{
			
			Image_WebCamLive = cvQueryFrame(Capture);
		
			ImageScanner scanner;
			scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
			
			Mat grey;
			cvtColor(Image_WebCamLive, grey, CV_BGR2GRAY);

			int width = Image_WebCamLive.cols;
			int height = Image_WebCamLive.rows;
			uchar *raw = (uchar *)grey.data;

			// wrap image data  
			Image image(width, height, "Y800", raw, width *height);
			// scan the image for barcodes  
			int n = scanner.scan(image);
			// extract results 
			for (Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
			{
				mainDlg.CarStatement = 1;
				CString data(symbol->get_data().c_str());//QR-codeInfo
				vector<Point> vp;
				int n = symbol->get_location_size();
				for (int i = 0; i < n; i++)
				{
					vp.push_back(Point(symbol->get_location_x(i), symbol->get_location_y(i)));
				}
				RotatedRect r = minAreaRect(vp);
				Point2f pts[4];
				r.points(pts);

				line(Image_WebCamLive, pts[0], pts[1], Scalar(255, 0, 0), 3);
				line(Image_WebCamLive, pts[1], pts[2], Scalar(0, 255, 0), 3);
				line(Image_WebCamLive, pts[2], pts[3], Scalar(0, 0, 255), 3);
				line(Image_WebCamLive, pts[3], pts[0], Scalar(255, 0, 255), 3);

				QRInWebPos=cvPoint(0.5*(pts[0].x+ pts[2].x), 0.5*(pts[0].y + pts[2].y));

				CString msg;
				CString msg2;
				msg = _T("In WebCam.Car No") + data + _T(" Pos:");
				msg2.Format(_T("(x,y) = ( %d,%d)"), QRInWebPos.x, QRInWebPos.y);
				msg = msg + msg2;
				m_listBox_msg->InsertString(0, msg);


				circle(Image_WebCamLive, QRInWebPos,3, Scalar(11, 23, 70));
				if (SetStation1)
				{
					mainDlg.Station1 = QRInWebPos;
					SetStation1 = false;
				}
			}
			
			IplImage* imageWebCamShow = cvCloneImage(&(IplImage)Image_WebCamLive);
			hWnd->ShowImage(imageWebCamShow, hWnd->GetDlgItem(IDC_IMAGE_WebCamLive), 3,cvSize(320,240));
			cvReleaseImage(&imageWebCamShow);
		
	}
	
}

void tab2Dlg::Thread_Image_KinectLive(LPVOID lParam)
{
	CTab2threadParam * Thread_Info = (CTab2threadParam *)lParam;
	tab2Dlg * hWnd = (tab2Dlg *)CWnd::FromHandle((HWND)Thread_Info->hWnd);
	Mat Image_KinectLive;
	CMFC_CNCSystemDlg mainDlg;
	CListBox* m_listBox_msg = (CListBox*)hWnd->GetDlgItem(IDC_LIST_QRcodeInfo);

	while (1)
	{		
       Image_KinectLive = mainDlg.s_Img_RGB;
	   ImageScanner scanner;
	   scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

	   Mat grey;
	   cvtColor(Image_KinectLive, grey, CV_BGR2GRAY);

	   int width = Image_KinectLive.cols;
	   int height = Image_KinectLive.rows;
	   uchar *raw = (uchar *)grey.data;

			// wrap image data  
			Image image(width, height, "Y800", raw, width *height);
			// scan the image for barcodes  
			int n = scanner.scan(image);
			// extract results 
			for (Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
			{
				mainDlg.CarStatement = 2;
				CString data(symbol->get_data().c_str());//QR-codeInfo
				vector<Point> vp;
				int n = symbol->get_location_size();
				for (int i = 0; i < n; i++)
				{
					vp.push_back(Point(symbol->get_location_x(i), symbol->get_location_y(i)));
				}
				RotatedRect r = minAreaRect(vp);
				Point2f pts[4];
				r.points(pts);

				line(Image_KinectLive, pts[0], pts[1], Scalar(255, 0, 0), 3);
				line(Image_KinectLive, pts[1], pts[2], Scalar(0, 255, 0), 3);
				line(Image_KinectLive, pts[2], pts[3], Scalar(0, 0, 255), 3);
				line(Image_KinectLive, pts[3], pts[0], Scalar(255, 0, 255), 3);

			    QRInKinectPos = cvPoint(0.5*(pts[0].x + pts[2].x), 0.5*(pts[0].y + pts[2].y));
				CString msg;
				CString msg2;
				msg = _T("In Kinect.Car No") + data + _T(" Pos:");
				msg2.Format(_T("(x,y) = ( %d,%d)"), QRInKinectPos.x, QRInKinectPos.y);
				msg = msg + msg2;
				m_listBox_msg->InsertString(0, msg);
				if (SetStation2)
				{
					mainDlg.Station2 = QRInKinectPos;
					SetStation2 = false;
				}
				
			}

			IplImage* imageKinectShow = cvCloneImage(&(IplImage)Image_KinectLive);
			hWnd->ShowImage(imageKinectShow, hWnd->GetDlgItem(IDC_Image_KinectLive), 3,cvSize(480,270));
			cvReleaseImage(&imageKinectShow);
	}
}

void tab2Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (point.x>10 && point.x<320+10 && point.y>10 && point.y<240+10)
	{
		m_threadPara.m_case = 0;
		m_threadPara.hWnd = m_hWnd;
		m_lpThread = AfxBeginThread(&tab2Dlg::Tab2threadFun, (LPVOID)&m_threadPara);
	}
	if (point.x>10+320 && point.x<320 +10+480 && point.y>10 && point.y<270 + 10)
	{
		m_threadPara.m_case = 1;
		m_threadPara.hWnd = m_hWnd;
		m_lpThread = AfxBeginThread(&tab2Dlg::Tab2threadFun, (LPVOID)&m_threadPara);
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}
void tab2Dlg::ShowImage(IplImage* Image, CWnd* pWnd, int channels,CvSize size)
{
	CDC	*dc = pWnd->GetWindowDC();
	IplImage *Temp = NULL;
	if (channels != 4) {
		Temp = cvCreateImage(size, IPL_DEPTH_8U, channels);
		cvResize(Image, Temp, CV_INTER_LINEAR);
	}
	if (channels == 4)
	{
		Temp = cvCreateImage(size, IPL_DEPTH_8U, 3);
		IplImage *Temp_transfer = cvCreateImage(size, IPL_DEPTH_8U, 4);
		cvResize(Image, Temp_transfer, CV_INTER_LINEAR);
		cvCvtColor(Temp_transfer, Temp, CV_BGRA2BGR);
		cvReleaseImage(&Temp_transfer);
	}

	CvvImage Temp2;
	Temp2.CopyOf(Temp);
	Temp2.Show(*dc, 0, 0, Temp->width, Temp->height);
	cvReleaseImage(&Temp);
	ReleaseDC(dc);
}
UINT tab2Dlg::Tab2threadFun(LPVOID LParam)
{
	CTab2threadParam* para = (CTab2threadParam*)LParam;
	tab2Dlg* lpview = (tab2Dlg*)(para->m_lpPara);
	para->m_blthreading = TRUE;

	switch (para->m_case)
	{
	case 0:
		lpview->Thread_Image_WebCamLive(LParam);
	case 1:
		lpview->Thread_Image_KinectLive(LParam);
	case 2:
		lpview->Thread_goRobot(LParam);
	default:
		break;
	}

	para->m_blthreading = FALSE;
	para->m_case = 0xFF;
	return 0;

}

void tab2Dlg::OnBnClickedButtonconnect()
{
	Aria::init();
	robot.lock();
	con.setPort("192.168.1.87", 8101);

	if (!con.openSimple())
	{
		printf("Open failed.");
		Aria::shutdown();
	}
	robot.setDeviceConnection(&con);
	if (!robot.blockingConnect())
	{
		printf("Could not connect to robot... exiting\n");
		Aria::shutdown();
	}


	robot.enableMotors();
	//	robot.comInt(ArCommands::ENABLE, 1);
	robot.disableSonar();		                 // Disables the sonar.
	robot.requestEncoderPackets();// Starts a continuous stream of encoder packets.

	robot.runAsync(true);
	robot.unlock();

}

void tab2Dlg::OnBnClickedButtonSetpos1()
{
	SetStation1 = true;
}
void tab2Dlg::OnBnClickedButtonSetpos2()
{
	SetStation2 = true;
	CMFC_CNCSystemDlg mainDlg;
	for (int i = 0; i < 512; i++)//掃描整個平台的高度
		for (int j = 0; j < 424; j++)
			mainDlg.tableReferenceDistance[i][j] = mainDlg.kinect.pDepthPoints[i + 512 * j];//單位是mm
}
void tab2Dlg::Thread_goRobot(LPVOID lParam)
{
	CTab2threadParam * Thread_Info = (CTab2threadParam *)lParam;
	tab2Dlg * hWnd = (tab2Dlg *)CWnd::FromHandle((HWND)Thread_Info->hWnd);
	CMFC_CNCSystemDlg mainDlg;
	while (1)
	{
		if (mainDlg.StationOneStop == false && mainDlg.StationTwoStop == false && !mainDlg.SixAxisDone && !mainDlg.PalletizingDone)//車子在識別區外且未經過第1.2站
		{
			robot.setVel(50);
		}
		if (mainDlg.Station1.y <= QRInWebPos.y && mainDlg.StationOneStop == false)//進入第1站
		{
			robot.setVel(0);
			Sleep(500);
			mainDlg.StationOneStop = true;
			mainDlg.Thread_Server_Palletizing();
		}
		if (mainDlg.PalletizingDone && mainDlg.StationTwoStop == false)
			robot.setVel(30);

		if ( mainDlg.Station2.y <=QRInKinectPos.y && mainDlg.StationTwoStop == false && mainDlg.StationOneStop)
		{
			robot.setVel(0); 
			Sleep(500);
			mainDlg.StationTwoStop = true;
		}
		if (mainDlg.SixAxisDone && mainDlg.StationTwoStop )
			robot.setVel(50);
	}
	
}

void tab2Dlg::OnBnClickedButtonGo()
{
	m_threadPara.m_case = 2;
	m_threadPara.hWnd = m_hWnd;
	m_lpThread = AfxBeginThread(&tab2Dlg::Tab2threadFun, (LPVOID)&m_threadPara);
}

void tab2Dlg::OnBnClickedButtonForward()
{
	robot.setVel(100);
}

void tab2Dlg::OnBnClickedButtonBackward()
{
	robot.setVel(-100);
}

void tab2Dlg::OnBnClickedButtonStop()
{
	robot.setVel(0);
}
