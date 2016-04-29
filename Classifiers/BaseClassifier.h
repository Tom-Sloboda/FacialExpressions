#pragma once
#include "../Loader.h"

using namespace std;
using namespace dlib;
using namespace cv;

class BaseClassifier
{
public:
	BaseClassifier();
	~BaseClassifier();

	static std::string classToEmotion(int n);
	static cv::Mat vectorToMat(std::vector<std::vector<float>> vec);
	static cv::Mat vectorToMat(std::vector<float> vec);
	static cv::Mat vectorToMat(std::vector<int> vec);
	static float evaluate(cv::Mat& predicted, cv::Mat& actual);
	static void printMat(Mat mat);
	static std::vector<std::vector<float>> getAllDataOfClass(std::vector<std::vector<float>> data, std::vector<float> labels, float outputClass);
	static Mat convertMultiClassToSingleClassLabels(Mat labels, float classLabel);
	static Mat convertFloatToIntMat(Mat mat);
	static float getHighestProbability(Mat mat);


};

