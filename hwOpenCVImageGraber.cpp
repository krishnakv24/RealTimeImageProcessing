#include "pch.h"
#include "hwOpenCVImageGraber.h"

ChwOpenCVImageGraber::ChwOpenCVImageGraber()
{
}

ChwOpenCVImageGraber::~ChwOpenCVImageGraber()
{
	for (int i = 0; i < m_vecCameraList.size(); ++i)
	{
		m_vecCameraList[i].release();
	}
}

bool ChwOpenCVImageGraber::InitCameraModules(long CameraCount)
{
	for (int i = 0; i < CameraCount; ++i)
	{
		cv::VideoCapture cap(i);
		m_vecCameraList.push_back(cap);
		if (!m_vecCameraList[i].isOpened())  
		{
			return false;
		}
	}
	return true;
}

bool ChwOpenCVImageGraber::CaptureImage(long CameraIndex,cv::Mat &ImageGrabed)
{
	return m_vecCameraList[CameraIndex].read(ImageGrabed);
}

