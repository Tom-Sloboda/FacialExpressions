#pragma once
#include "../FeatureExtractor.h"
#include "../Loader.h"

using namespace std;
using namespace dlib;
using namespace cv;

class Classifiers
{
public:
	Classifiers();
	~Classifiers();

	std::string classToEmotion(int n);


	void train(std::vector<std::vector<float>> trData, std::vector<float> trLabels);
	float predict(std::vector<std::vector<float>> teData, std::vector<float> teLabels);


	void go(std::vector<std::vector<float>> trData, std::vector<float> trLabels, std::vector<std::vector<float>> teData, std::vector<float> teLabels);
	void svm(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses);
	void mlp(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses);
	void knn(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses, int K);
	void bayes(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses);
	void decisiontree(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses);
};

