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
	float go(std::vector<std::vector<float>> trainingData, std::vector<float> trainingLabels, std::vector<std::vector<float>> testData, std::vector<float> testLabels, float C, float gamma);
	float go_auto(std::vector<std::vector<float>> trainingData, std::vector<float> trainingLabels, std::vector<std::vector<float>> testData, std::vector<float> testLabels, float start_C = 1e-7, float end_C = 1e7, float start_gamma = 1e-7, float end_gamma = 1e7);
	float** samples;
	float* labels;
};

