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
	
	void multiclassExample();
	void simpleExample();

	float go(std::vector<std::vector<float>> trainingData, std::vector<float> trainingLabels, std::vector<std::vector<float>> testData, std::vector<float> testLabels);

	float** samples;
	float* labels;
};

