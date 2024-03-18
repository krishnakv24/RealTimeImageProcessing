#include "pch.h"
#include "hwCameraManger.h"


ChwCameraManger::ChwCameraManger()
{
}

ChwCameraManger::~ChwCameraManger()
{
}

void ChwCameraManger::IntiCameraModules(long CameraMModuleCount)
{
	m_ptrCamreGraber = make_shared<ChwOpenCVImageGraber>();
	m_ptrCamreGraber->InitCameraModules(CameraMModuleCount);
}


void ChwCameraManger::SnapGrab(long CameraIndex, cv::Mat &GrabImage)
{
	cv::Mat LocalMat;
	m_ptrCamreGraber->CaptureImage(CameraIndex, LocalMat);
	cv::flip(LocalMat, LocalMat, 1);
	GrabImage = LocalMat;
}
