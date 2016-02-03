#pragma once

#include "FeatureExtractor.h"
#include "Loader.h"

using namespace std;
using namespace dlib;
using namespace cv;


class mySVM
{
public:
	mySVM();
	~mySVM();


	float** samples;
	float* labels;
};

