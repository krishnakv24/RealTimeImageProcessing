#pragma once


class ChwOpenCVImageGraber
{
public:
	ChwOpenCVImageGraber();
	~ChwOpenCVImageGraber();
public:
	bool InitCameraModules(long CameraCount);
	bool CaptureImage(long CameraIndex, cv::Mat &ImageGrabed);
private:
	vector<cv::VideoCapture> m_vecCameraList;
};

