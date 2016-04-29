#pragma once
#include "../FeatureExtractor.h"
#include "../Loader.h"
#include "MLP.h"
#include "MulticlassSVM.h"
#include "BaseClassifier.h"
#include "LinearSVM.h"
#include "Bayes.h"
#include "AdaBoost.h"

using namespace std;
using namespace dlib;
using namespace cv;
using namespace cv::ml;

class Classifiers:public BaseClassifier
{
public:
	Classifiers();
	~Classifiers();

	void train(std::vector<std::vector<float>> trainingData, std::vector<float> trainingLabels);
	void predict(std::vector<std::vector<float>> testData, std::vector<float> testLabels);
	float predict(Mat testData);

private:
	MLP mlp;
	MulticlassSVM multiSvm;
	LinearSVM linSVM;
	Bayes bayes;
	AdaBoost boost;
};
