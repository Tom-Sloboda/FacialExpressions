#pragma once
#include "BaseClassifier.h"


using namespace std;
using namespace dlib;
using namespace cv;
using namespace cv::ml;

class AdaBoost : public BaseClassifier
{
public:
	AdaBoost();
	~AdaBoost();

	void train(std::vector<std::vector<float>> trainingData, std::vector<float> trainingLabels);
	void predict(std::vector<std::vector<float>> testData, std::vector<float> testLabels, std::vector<std::vector<float>> &result);
	std::vector<float> predict(Mat testData);

private:
	std::vector<Ptr<ml::Boost>> boost;
};

