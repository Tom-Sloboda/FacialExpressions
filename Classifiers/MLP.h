#pragma once
#include "BaseClassifier.h"
#include "../FeatureExtractor.h"
#include "../Loader.h"

using namespace std;
using namespace dlib;
using namespace cv;
using namespace cv::ml;

class MLP:public BaseClassifier
{
public:
	MLP();
	~MLP();

	void train(cv::Mat& trainingData, cv::Mat trainingLabels);
	void predict(Mat &testData, Mat &testLabels, Mat &result);
	std::vector<float> predict(Mat &testData, float threshold = 0.5);

private:
		Ptr<ml::ANN_MLP> mlp;
};
