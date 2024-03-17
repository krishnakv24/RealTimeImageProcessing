#include "pch.h"
#include "hwFetchCameraDetails.h"


ChwFetchCameraDetails::ChwFetchCameraDetails()
{
}


ChwFetchCameraDetails::~ChwFetchCameraDetails()
{
}


long ChwFetchCameraDetails::FetchCameraHardwareData()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (SUCCEEDED(hr))
	{
		IEnumMoniker *pEnum;
		hr = EnumerateDevices(CLSID_VideoInputDeviceCategory, &pEnum);
		if (SUCCEEDED(hr))
		{
			IMoniker *pMoniker = NULL;
			while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
			{
				IPropertyBag *pPropBag;
				hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
				if (FAILED(hr))
				{
					pMoniker->Release();
					continue;
				}

				VARIANT var;
				VariantInit(&var);
				hr = pPropBag->Read(L"Description", &var, 0);
				if (FAILED(hr))
				{
					hr = pPropBag->Read(L"FriendlyName", &var, 0);
				}

				if (SUCCEEDED(hr))
				{
					std::wstring ws(var.bstrVal, SysStringLen(var.bstrVal));
					m_vecConnctedCamera.push_back(ws);
					VariantClear(&var);
				}
				pMoniker->Release();
			}
			pEnum->Release();
		}
		CoUninitialize();
	}

	return static_cast<long>(m_vecConnctedCamera.size());
}

HRESULT ChwFetchCameraDetails::EnumerateDevices(REFGUID category, IEnumMoniker **ppEnum)
{
	// Create the System Device Enumerator.
	ICreateDevEnum *pDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

	if (SUCCEEDED(hr))
	{
		// Create an enumerator for the category.
		hr = pDevEnum->CreateClassEnumerator(category, ppEnum, 0);
		if (hr == S_FALSE)
		{
			hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
		}
		pDevEnum->Release();
	}
	return hr;
}