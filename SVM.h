#pragma once

#include "FeatureExtractor.h"
#include "Loader.h"

using namespace std;
using namespace dlib;

class SVM
{
public:
	SVM();
	~SVM();


	float** samples;
	float* labels;
};

