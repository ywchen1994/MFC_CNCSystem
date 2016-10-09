#include "Kinect2Capture.h"

float Kinect2Capture::CameraX = 0;
float Kinect2Capture::CameraY = 0;
float Kinect2Capture::CameraZ = 0;


Kinect2Capture::Kinect2Capture()
{
	fColorOpened = false;
	fDepthOpened = false;
	fInfraredOpened = false;

	colorROI = cvRect(0, 0, 1920, 1080);
	depthROI = cvRect(0, 0, 512, 424);
	
	//set out put size
	colorSize = cvSize(1920, 1080);
	depthSize = cvSize(512, 424);
}






Kinect2Capture::~Kinect2Capture()
{
	delete[] pPointArray;//FH
	delete[]pDepthPoints;
	//Close
	//pColorFrameReader->Release();
	//pColorFrameReader = nullptr;

	//pDepthFrameReader->Release();
	//pDepthFrameReader = nullptr;

	//pInfraredFrameReader->Release();
	//pInfraredFrameReader = nullptr;

	//pSensor->Close();
	//pSensor->Release();
	//pSensor = nullptr;
	
}







void Kinect2Capture::Open(bool rgb, bool depth, bool Infrared)
{
	pSensor = nullptr;
	if (GetDefaultKinectSensor(&pSensor) != S_OK)MessageBox(L"Get Sensor failed");
	if (pSensor->Open() != S_OK)MessageBox(L"Can't open sensor");

	/**********************************************************************************************************/

	if (rgb)
	{
		IColorFrameSource* pColorFrameSource = nullptr;
		if (pSensor->get_ColorFrameSource(&pColorFrameSource) != S_OK)MessageBox(L"Can't get color frame source");

		//Get frame description
		 iWidth = 0;
		 iHeight = 0;
		IFrameDescription* pFrameDescription = nullptr;
		if (pColorFrameSource->get_FrameDescription(&pFrameDescription) == S_OK)
		{
			pFrameDescription->get_Width(&iWidth);
			pFrameDescription->get_Height(&iHeight);
			uColorPointNum = iWidth*iHeight;//1920*1080個彩色畫素點
			
			pCSPoints = new CameraSpacePoint[uColorPointNum];
		}
		pFrameDescription->Release();
		pFrameDescription = nullptr;


		//get frame reader
		pColorFrameReader = nullptr;
		if (pColorFrameSource->OpenReader(&pColorFrameReader) != S_OK)MessageBox(L"Can't get color frame reader");

		//release Frame source
		pColorFrameSource->Release();
		pColorFrameSource = nullptr;

		//Prepare OpenCV data
		mColorImg = cv::Mat(iHeight, iWidth, CV_8UC4);
		uColorBufferSize = iHeight * iWidth * 4 * sizeof(BYTE);

		//checkOpen(because it will delay few second to AcquireLatestFrame)
		bool checkOpen = false;
		while (checkOpen == false)
		{
			pColorFrame = nullptr;
			if (pColorFrameReader->AcquireLatestFrame(&pColorFrame) == S_OK)
			{
				if (pColorFrame->CopyConvertedFrameDataToArray(uColorBufferSize, mColorImg.data, ColorImageFormat_Bgra) == S_OK)
					checkOpen = true;
				else
				{
					MessageBox(L"Data copy error");
					break;
				}
				pColorFrame->Release();
			}
		}
		fColorOpened = true;
	}

	/**********************************************************************************************************/

	if (depth)
	{
		
		IDepthFrameSource* pFrameSource = nullptr;
		if (pSensor->get_DepthFrameSource(&pFrameSource) != S_OK)MessageBox(L"Can't get depth frame source");

		//Get frame description
		iDepthWidth = 0;
		iDepthHeight = 0;
		IFrameDescription* pFrameDescription = nullptr;
		if (pFrameSource->get_FrameDescription(&pFrameDescription) == S_OK)
		{
			pFrameDescription->get_Width(&iDepthWidth);
			pFrameDescription->get_Height(&iDepthHeight);
			uDepthPointNum= iDepthHeight*iDepthWidth;//512*424個深度畫素
			pDepthPoints = new UINT16[uDepthPointNum];//建立一個動態矩陣用來存16bits深度影像值
			pDSPoints = new CameraSpacePoint[uDepthPointNum];
		}
		

		pFrameDescription->Release();
		pFrameDescription = nullptr;

		//get some dpeth only meta
		UINT16 uDepthMin = 0;
		uDepthMax = 0;
		pFrameSource->get_DepthMinReliableDistance(&uDepthMin);
		pFrameSource->get_DepthMaxReliableDistance(&uDepthMax);

		//get frame reader
		pDepthFrameReader = nullptr;
		if (pFrameSource->OpenReader(&pDepthFrameReader) != S_OK)MessageBox(L"Can't get color frame reader");

		
		//perpare OpenCV
		
		mDepthImg = cv::Mat(iDepthHeight, iDepthWidth, CV_16UC1);
		mDepthImg8bit = cv::Mat(iDepthHeight, iDepthWidth, CV_8UC1);

		//release Frame source
		pFrameSource->Release();
		pFrameSource = nullptr;

		pCoordinateMapper = nullptr;/*初始化座標轉換指標*/
		if (pSensor->get_CoordinateMapper(&pCoordinateMapper) != S_OK)MessageBox(L"get_CoordinateMapper failed");
		fDepthOpened = true;
	}

	/**********************************************************************************************************/

	if (Infrared)
	{
		IInfraredFrameSource* pFrameSource = nullptr;
		if (pSensor->get_InfraredFrameSource(&pFrameSource) != S_OK)MessageBox(L"Can't get depth frame source");

		//Get frame description
		iInfraredWidth = 0;
		iInfraredHeight = 0;
		IFrameDescription* pFrameDescription = nullptr;
		if (pFrameSource->get_FrameDescription(&pFrameDescription) == S_OK)
		{
			pFrameDescription->get_Width(&iInfraredWidth);
			pFrameDescription->get_Height(&iInfraredHeight);
		}
	
		pFrameDescription->Release();
		pFrameDescription = nullptr;

		//get frame reader
		pInfraredFrameReader = nullptr;
		if (pFrameSource->OpenReader(&pInfraredFrameReader) != S_OK)MessageBox(L"Can't get color frame reader");

		//release Frame source
		pFrameSource->Release();
		pFrameSource = nullptr;

		fInfraredOpened = true;
	}
}

void Kinect2Capture::Close()
{
	pColorFrameReader->Release();
	pColorFrameReader = nullptr;

	pDepthFrameReader->Release();
	pDepthFrameReader = nullptr;

	pInfraredFrameReader->Release();
	pInfraredFrameReader = nullptr;

	pSensor->Close();
	pSensor->Release();
	pSensor = nullptr;

	pCoordinateMapper->Release();/*釋放座標轉換指標*/
	pCoordinateMapper = nullptr;
	delete []pCSPoints;
	delete []pDSPoints;
	
}

void Kinect2Capture::setColorROISize(CvRect ROI, CvSize size)
{
	colorROI = ROI;
	colorSize = size;
}
void Kinect2Capture::setDepthROISize(CvRect ROI, CvSize size)
{
	depthROI = ROI;
	depthSize = size;
}

IplImage* Kinect2Capture::RGBAImage()
{
	if (fColorOpened == false) { MessageBox(L"Need Open"); return NULL; }
	bool copyDone = false;
	IplImage* pImg = nullptr;
	pColorFrame = nullptr;

	if (pColorFrameReader->AcquireLatestFrame(&pColorFrame) == S_OK)
	{
		//Copy to OpenCV image
		if (pColorFrame->CopyConvertedFrameDataToArray(uColorBufferSize, mColorImg.data, ColorImageFormat_Bgra) == S_OK)
		{
			pImg = cvCloneImage(&(IplImage)mColorImg);
			copyDone = true;
			//cvSetImageROI(pImg, colorROI);
			//cvResize(pImg, cvCreateImage(colorSize, IPL_DEPTH_8U, 4), CV_INTER_AREA);
		}
		else
			MessageBox(L"Data copy error");

		pColorFrame->Release();
	}
	if (copyDone)
		return pImg;
	else
		return NULL;
}

IplImage * Kinect2Capture::DepthImage()
{
	if (fDepthOpened == false) { MessageBox(L"Need Open"); return NULL; }
	bool fGet = false;
	IplImage* pImg = nullptr;
	IDepthFrame* pFrame = nullptr;
	if (pDepthFrameReader->AcquireLatestFrame(&pFrame) == S_OK)
	{
		//copy the depth map to image
		
		pFrame->CopyFrameDataToArray(iDepthWidth * iDepthHeight, reinterpret_cast<UINT16*>(mDepthImg.data));
		pDepthPoints = reinterpret_cast<UINT16*>(mDepthImg.data);
	   
		//convert from 16bit to 8bit

		mDepthImg.convertTo(mDepthImg8bit, CV_8U, 255.0f / 1000);
		pImg = cvCloneImage(&(IplImage)mDepthImg8bit);

		//cvSetImageROI(pImg, depthROI);
		//cvResize(pImg, cvCreateImage(depthSize, IPL_DEPTH_8U, 1), CV_INTER_AREA);
		fGet = true;

		pFrame->Release();
	}

	if (fGet)
		return pImg;
	else
		return NULL;
}

IplImage * Kinect2Capture::InfraredImage()
{
	if (fInfraredOpened == false) { MessageBox(L"Need Open"); return NULL; }
	bool accessDone = false;
	IplImage* pImg = nullptr;
	IInfraredFrame* pFrame = nullptr;
	if (pInfraredFrameReader->AcquireLatestFrame(&pFrame) == S_OK)
	{
		UINT	uSize = 0;
		UINT16*	pBuffer = nullptr;
		if (pFrame->AccessUnderlyingBuffer(&uSize, &pBuffer) == S_OK)
		{
			cv::Mat mIRImg(iInfraredHeight, iInfraredWidth, CV_16UC1, pBuffer);
			pImg = cvCloneImage(&(IplImage)mIRImg);
			cvSetImageROI(pImg, depthROI);
			cvResize(pImg, cvCreateImage(depthSize, IPL_DEPTH_16U, 1), CV_INTER_AREA);
			accessDone = true;
		}
		else
			MessageBox(L"Data access error");

		pFrame->Release();
	}

	if (accessDone)
		return pImg;
	else
		return NULL;
}

cv::Mat Kinect2Capture::DepthImageM()
{
	if (fDepthOpened == false) { MessageBox(L"Need Open"); return cv::Mat::zeros(mDepthImg8bit.rows, mDepthImg8bit.cols, CV_8U); }
	IDepthFrame* pFrame = nullptr;
	bool fGet = false;
	while (fGet == false)
	{
		if (pDepthFrameReader->AcquireLatestFrame(&pFrame) == S_OK)
		{
			//copy the depth map to image
			pFrame->CopyFrameDataToArray(iDepthWidth * iDepthHeight, reinterpret_cast<UINT16*>(mDepthImg.data));
			
			//convert from 16bit to 8bit
			mDepthImg.convertTo(mDepthImg8bit, CV_8U, 255.0f / uDepthMax);

			fGet = true;
			pFrame->Release();
		}
	}

	if (fGet)
		return mDepthImg8bit;


}
void Kinect2Capture::Color2CameraSpace( int RGBpixelx, int RGBpixely)
{
	
	if (pCoordinateMapper->MapColorFrameToCameraSpace(uDepthPointNum, pDepthPoints, uColorPointNum, pCSPoints) == S_OK)
	{

		int idx = RGBpixelx + RGBpixely * iWidth;
		const CameraSpacePoint& rPt = pCSPoints[idx];
		 CameraX = rPt.X;
		 CameraY = rPt.Y;
		 CameraZ = rPt.Z;
		
		
	}
}

void  Kinect2Capture::Depth2CameraSpace(int Dpixelx,int Dpixely)
{
	if (pCoordinateMapper->MapDepthFrameToCameraSpace(uDepthPointNum, pDepthPoints, uDepthPointNum, pDSPoints) == S_OK)//這裡的uDepthPointNum指的是攝影機座標的點數
	{
		int depIdx = Dpixelx + Dpixely * iDepthWidth;
			
		const CameraSpacePoint& rPt = pDSPoints[depIdx];
		 CameraX = rPt.X;
		 CameraY = rPt.Y;
		 CameraZ = rPt.Z;	
	}
}
