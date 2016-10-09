
#pragma once
#include "afxwin.h"
#include"Kinect.h"
#include "opencv.hpp"


class Kinect2Capture :public CWnd
{
public:
	Kinect2Capture();
	~Kinect2Capture();



	//Open(RGBImage, DepthImage, InfraredImage)
	void Open(bool rgb, bool depth, bool Infrared);

	void Close();
	IKinectSensor* pSensor;

	void setColorROISize(CvRect ROI, CvSize size);
	void setDepthROISize(CvRect ROI, CvSize size);

	IplImage* RGBAImage();
	bool fColorOpened;
	IColorFrameReader* pColorFrameReader;
	IColorFrame* pColorFrame;
	cv::Mat mColorImg;
	UINT uColorBufferSize;
	CvRect colorROI;
	CvSize colorSize;


	IplImage* DepthImage();
	cv::Mat DepthImageM();
	bool fDepthOpened;
	IDepthFrameReader* pDepthFrameReader;
	int iDepthWidth, iDepthHeight;
	UINT16 uDepthMax;
	cv::Mat mDepthImg;
	cv::Mat mDepthImg8bit;
	CvRect depthROI;
	CvSize depthSize;


	IplImage* InfraredImage();
	bool fInfraredOpened;
	IInfraredFrameReader* pInfraredFrameReader;
	int iInfraredWidth, iInfraredHeight;


	/************FH***************/
	void Depth2CameraSpace( int Dpixelx, int Dpixely);
	void Color2CameraSpace( int RGBpixelx, int RGBpixely);
	ICoordinateMapper* pCoordinateMapper;
	UINT uDepthPointNum;
	UINT uColorPointNum;
	UINT16*				pDepthPoints = new UINT16[uDepthPointNum];
	DepthSpacePoint*	pPointArray = new DepthSpacePoint[uColorPointNum];
	CameraSpacePoint* pCSPoints = nullptr;//用來儲存彩色影像到攝影機座標的指標
	CameraSpacePoint* pDSPoints = nullptr;//用來儲存深度影像到攝影機座標的指標
	int iHeight;
	int iWidth;

static	float CameraX;
static	float CameraY;
static	float CameraZ;


};

