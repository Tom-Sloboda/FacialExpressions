#pragma once
#include "BaseClassifier.h"
#include "../FeatureExtractor.h"
#include "../Loader.h"

#ifndef MULTICLASS_H_
#define MULTICLASS_H_

using namespace std;
using namespace dlib;
using namespace cv;
using namespace cv::ml;


class MulticlassSVM:public BaseClassifier
{
public:
	MulticlassSVM();
	~MulticlassSVM();

	float go(std::vector<std::vector<float>> trainingData, std::vector<float> trainingLabels, std::vector<std::vector<float>> testData, std::vector<float> testLabels);
	Ptr<ml::SVM> svm;

	void train(std::vector<std::vector<float>> trainingData, std::vector<float> trainingLabels, float C = 83.9666, float gamma = 3.375e-06);
	void predict(std::vector<std::vector<float>> testData, std::vector<float> testLabels, std::vector<float> &result );

	float predict(Mat testData);
};

#endif