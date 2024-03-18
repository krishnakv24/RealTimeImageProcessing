#pragma once
#include "ImageCtrl.h"
class CuiCamImageControl :public CImageCtrl
{
public:
	CuiCamImageControl();
	virtual ~CuiCamImageControl();
public:
	void SetMatImage(cv::Mat& Image);
	Bitmap GetBitMap(cv::Mat inputImage);
	cv::Mat GetNowUiMatImage() { return m_NowUiMat; }
private:
	Bitmap * CopyMatToBmp(cv::Mat i_Mat);
	cv::Mat m_NowUiMat;
};

