#ifndef CLASSIFIERS_H_
#define CLASSIFIERS_H_
#pragma once
#include "../FeatureExtractor.h"
#include "../Loader.h"
#include "MLP.h"
#include "MulticlassSVM.h"
#include "BaseClassifier.h"
#include "LinearSVM.h"
#include "Bayes.h"
#include "AdaBoost.h"

#define DEBUG


class Classifiers:public BaseClassifier
{
public:
	Classifiers();
	~Classifiers();

	MLP mlp;
	MulticlassSVM multiSvm;
	LinearSVM linSVM;
	Bayes bayes;
	AdaBoost boost;

	void train(std::vector<std::vector<float>> trainingData, std::vector<float> trainingLabels);
	void predict(std::vector<std::vector<float>> testData, std::vector<float> testLabels);


	float predict(Mat testData);
private:

	//void go(std::vector<std::vector<float>> trData, std::vector<float> trLabels, std::vector<std::vector<float>> teData, std::vector<float> teLabels);
	//void svm(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses);
	//void mlp(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses);
	//void knn(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses, int K);
	//void bayes(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses);
	//void decisiontree(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses);
};

#endif
