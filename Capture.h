#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <string>
using namespace std;
using namespace cv;
class Capture
{
public:
	Capture();
	~Capture();

	Mat neutralFace;
	Mat otherFace;

	//cv::Mat getScreenCap(int x, int y, int height, int width);
	Mat hwnd2mat(HWND hwnd);
	Mat screenshot();
};

