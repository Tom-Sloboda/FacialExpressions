#include "BaseClassifier.h"



BaseClassifier::BaseClassifier()
{
}


BaseClassifier::~BaseClassifier()
{
}

cv::Mat BaseClassifier::vectorToMat(std::vector<std::vector<float>> vec)
{
	cv::Mat mat(vec.size(), vec[0].size(), CV_32FC1);
	for (int i = 0; i < vec.size(); i++)
	{
		for (int j = 0; j < vec[0].size(); j++)
		{
			mat.at<float>(i, j) = vec.data()[i][j];
		}
	}
	return mat;
}

cv::Mat BaseClassifier::vectorToMat(std::vector<float> vec)
{
	cv::Mat mat((vec.size()), 1, CV_32FC1);
	for (int i = 0; i < vec.size(); i++)
	{
		mat.at<float>(i, 0) = vec[i];
	}
	return mat;
}

float BaseClassifier::evaluate(cv::Mat& predicted, cv::Mat& actual)
{
	assert(predicted.rows == actual.rows);
	int t = 0;
	int f = 0;
	for (int i = 0; i < actual.rows; i++)
	{
		float p = predicted.at<float>(i, 0);
		float a = actual.at<float>(i, 0);
		if ((p == a) || (abs(p - a) < 0.4))
		{
			t++;
		}
		else
		{
			f++;
		}
	}
	return (t * 1.0) / (t + f);
}

std::vector<std::vector<float>> BaseClassifier::getAllDataOfClass(std::vector<std::vector<float>> data, std::vector<float> labels, float outputClass)
{
	std::vector<std::vector<float>> outVector;
	for (int i = 0; i < labels.size(); i++)
	{
		if (labels[i] == outputClass)
		{
			outVector.push_back(data[i]);
		}
	}
	return outVector;
}

//Sets all of the labels equal to classLabel to 1 and all the other ones to -1
Mat BaseClassifier::convertMultiClassToSingleClassLabels(Mat labels, float classLabel)
{
	Mat singleClassLabels(labels.rows, labels.cols, CV_32SC1);
	for (int j = 0; j < labels.rows; j++)
	{
		singleClassLabels.at<float>(j, 0) = (labels.at<float>(j, 0) == classLabel) ? 1 : -1;
	}
	//printMat(singleClassLabels);
	return singleClassLabels;
}


void BaseClassifier::printMat(Mat mat)
{
	for (int i = 0; i < mat.rows; i++)
	{
		for (int j = 0; j < mat.cols; j++)
		{
			cout << mat.at<float>(i, j) << " ";
		}
		cout << endl;
	}
}

std::string BaseClassifier::classToEmotion(int n)
{
	switch (n)
	{
	case 1:
		return "Anger";
	case 2:
		return "Contempt";
	case 3:
		return "Disgust";
	case 4:
		return "Fear";
	case 5:
		return "Happiness";
	case 6:
		return "Sadness";
	case 7:
		return "Surprise";
	case 0:
		return "Neutral";
	default:
		return "Undefined";
	}
}

Mat BaseClassifier::convertFloatToIntMat(Mat mat)
{
	Mat intMat(mat.rows, mat.cols, CV_32SC1);
	for (int j = 0; j <  mat.cols; j++)
	{
		for (int i = 0; i < mat.rows; i++)
		{
			intMat.at<float>(i, j) = (int)(mat.at<float>(i, j));
		}
	}
	return intMat;
}

float BaseClassifier::getHighestProbability(Mat probability)
{
	int maxIndex = 0; float maxValue = 0;
	for (int j = 0; j < 8; j++)
	{
		if (probability.at<float>(0, j) > maxValue)
		{
			maxIndex = j;
			maxValue = probability.at<float>(0, j);
		}
	}
	return maxIndex;
}
