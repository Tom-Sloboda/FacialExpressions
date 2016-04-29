#pragma once
#include "../Loader.h"
#include "BaseClassifier.h"

using namespace std;
using namespace dlib;
using namespace cv;
using namespace cv::ml;

class Bayes : public BaseClassifier
{
public:
	Bayes();
	~Bayes();

	void train(cv::Mat& trainingData, cv::Mat& trainingLabels);
	void predict(cv::Mat &testData, cv::Mat &testLabels);
	float predict(cv::Mat &testData);

private:
	Ptr<ml::NormalBayesClassifier> bayes;
};

