#pragma once

#include "../FeatureExtractor.h"
#include "../Loader.h"

using namespace std;
using namespace dlib;
using namespace cv;
using namespace cv::ml;

class MLP
{
public:
	MLP();
	~MLP();

	Ptr<ml::ANN_MLP> mlp;

	void train(Ptr<ml::TrainData> trainingData);
	float predict(Ptr<ml::TrainData> testData);
	float predict(cv::Mat testData);
	float predict(std::vector<float> testData);

};

