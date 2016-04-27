#pragma once
#include "BaseClassifier.h"
#include "../FeatureExtractor.h"
#include "../Loader.h"

#ifndef MLP_H_
#define MLP_H_

using namespace std;
using namespace dlib;
using namespace cv;
using namespace cv::ml;

class MLP:public BaseClassifier
{
public:
	MLP();
	~MLP();

	Ptr<ml::ANN_MLP> mlp;

	void train(cv::Mat& trainingData, cv::Mat trainingLabels);
	float probabilityToClass(cv::Mat mat);
	void predict(Mat &testData, Mat &testLabels, Mat &result);
	std::vector<float> predict(Mat &testData);
	void go(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses);
};

#endif