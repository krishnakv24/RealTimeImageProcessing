#include "pch.h"
#include "uiCamImageControl.h"
CRITICAL_SECTION ImageUiUpdateLocker;
CuiCamImageControl::CuiCamImageControl()
{
	InitializeCriticalSection(&ImageUiUpdateLocker);
}

CuiCamImageControl::~CuiCamImageControl()
{
}

void CuiCamImageControl::SetMatImage(cv::Mat& Image)
{
	if (Image.data)
	{
		m_NowUiMat = Image;
		SetImage(CopyMatToBmp(m_NowUiMat));
	}
}



Bitmap* CuiCamImageControl::CopyMatToBmp(cv::Mat i_Mat)
{
	PixelFormat e_Format;
	switch (i_Mat.channels())
	{
	case 1: e_Format = PixelFormat8bppIndexed; break;
	case 3: e_Format = PixelFormat24bppRGB;    break;
	case 4: e_Format = PixelFormat32bppARGB;   break;
	default: throw L"Image format not supported.";
	}

	// Create Bitmap with own memory
	Bitmap* pi_Bmp = new Bitmap(i_Mat.cols, i_Mat.rows, e_Format);

	BitmapData i_Data;
	Gdiplus::Rect k_Rect(0, 0, i_Mat.cols, i_Mat.rows);
	if (Ok != pi_Bmp->LockBits(&k_Rect, ImageLockModeWrite, e_Format, &i_Data))
	{
		delete pi_Bmp;
		throw L"Error locking Bitmap.";
	}

	if (i_Mat.elemSize1() == 1)
	{
		EnterCriticalSection(&ImageUiUpdateLocker);
			int size = i_Mat.total() * i_Mat.elemSize();
			BYTE* u8_Dst = (BYTE*)i_Data.Scan0;
			std::memcpy(u8_Dst, i_Mat.data, size * sizeof(byte));
		LeaveCriticalSection(&ImageUiUpdateLocker);
	}
	else
	{
		int s32_Type;
		switch (i_Mat.channels())
		{
		case 1: s32_Type = CV_8UC1; break;
		case 3: s32_Type = CV_8UC3; break;
		default: throw L"Image format not supported.";
		}

		CvMat i_Dst;
		cvInitMatHeader(&i_Dst, i_Mat.rows, i_Mat.cols, s32_Type, i_Data.Scan0, i_Data.Stride);

		CvMat i_Img = i_Mat;
		cvConvertImage(&i_Img, &i_Dst, 0);
	}

	pi_Bmp->UnlockBits(&i_Data);

	if (e_Format == PixelFormat8bppIndexed)
	{
		CByteArray i_Arr;
		i_Arr.SetSize(sizeof(ColorPalette) + 256 * sizeof(ARGB));
		ColorPalette* pk_Palette = (ColorPalette*)i_Arr.GetData();

		pk_Palette->Count = 256;
		pk_Palette->Flags = PaletteFlagsGrayScale;

		ARGB* pk_Color = &pk_Palette->Entries[0];
		for (int i = 0; i < 256; i++)
		{
			pk_Color[i] = Color::MakeARGB(255, (BYTE)i, (BYTE)i, (BYTE)i);
		}

		if (Ok != pi_Bmp->SetPalette(pk_Palette))
		{
			delete pi_Bmp;
			throw L"Error setting Gray scale palette.";
		}
	}
	return pi_Bmp;

}
