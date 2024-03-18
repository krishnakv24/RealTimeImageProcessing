#pragma once
#include "hwOpenCVImageGraber.h"
class ChwCameraManger
{
public:
	ChwCameraManger();
	~ChwCameraManger();
public:
	void IntiCameraModules(long CameraMModuleCount);
	void SnapGrab(long CameraIndex, cv::Mat & GrabImage);
private:
	shared_ptr<ChwOpenCVImageGraber> m_ptrCamreGraber;
};

