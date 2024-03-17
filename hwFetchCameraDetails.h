#pragma once
#include <dshow.h>
#pragma comment(lib, "strmiids")

class ChwFetchCameraDetails
{
public:
	ChwFetchCameraDetails();
	~ChwFetchCameraDetails();

public:
	long FetchCameraHardwareData();
	HRESULT EnumerateDevices(REFGUID category, IEnumMoniker ** ppEnum);
private:
	vector<wstring> m_vecConnctedCamera;
};

