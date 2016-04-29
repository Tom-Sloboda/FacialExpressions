#pragma once
#include "BaseClassifier.h"
#include "../FeatureExtractor.h"
#include "../Loader.h"

using namespace std;
using namespace dlib;
using namespace cv;
using namespace cv::ml;


class MulticlassSVM:public BaseClassifier
{
public:
	MulticlassSVM();
	~MulticlassSVM();

	void train(std::vector<std::vector<float>> trainingData, std::vector<float> trainingLabels, float C = 83.9666, float gamma = 3.375e-06);
	void predict(std::vector<std::vector<float>> testData, std::vector<float> testLabels, std::vector<float> &result );
	float predict(Mat testData);

private:
	Ptr<ml::SVM> svm;
};
