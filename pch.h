// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

//Additional Added
#include <GdiPlus.h>
#include <string>
#include <vector>
#include <queue>
#include <memory>
#include <filesystem>
#include <map>
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")


#include "opencv2/opencv.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgcodecs/imgcodecs_c.h"
#include "opencv2/imgproc/types_c.h"
#pragma warning(default:)

using namespace std;
using namespace cv;
using namespace Gdiplus;

#endif //PCH_H
