#include "AdaBoost.h"



AdaBoost::AdaBoost()
{
}


AdaBoost::~AdaBoost()
{
}

void AdaBoost::train(std::vector<std::vector<float>> trainingData, std::vector<float> trainingLabels)
{
	cv::Mat trainingDataMat = vectorToMat(trainingData);
	cv::Mat trainingLabelsMat = vectorToMat(trainingLabels);

	cout << "\nWould you like to load AdaBoost[0-7].xml? y/n\n";
	std::string input;
	cin >> input;
	if (input == "y") {
		for (int i = 0; i < 8; i++)
		{
			Ptr<ml::Boost> tmpBoost = StatModel::load<Boost>("AdaBoost" + to_string(i) + ".xml");
			if (tmpBoost  != NULL)
			{
				boost.push_back(tmpBoost);
			}
			else {
				for (Ptr<ml::Boost> j : boost)
				{
					j->empty();
				}
				break;
			}
		}
	}
	if (boost.size() < 8)
	{
		std::cout << "Start training\n";
		for (int i = 0; i < 8; i++)
		{
			Ptr<ml::Boost> tmpBoost  = Boost::create();
			Ptr<ml::TrainData> tData = ml::TrainData::create(trainingDataMat, ml::SampleTypes::ROW_SAMPLE, convertMultiClassToSingleClassLabels(trainingLabelsMat, i));
			tmpBoost ->train(tData);
			tmpBoost ->save("AdaBoost" + to_string(i) + ".xml");
			boost.push_back(tmpBoost);
		}
		std::cout << "Finished training\n";
	}
}

void AdaBoost::predict(std::vector<std::vector<float>> testData, std::vector<float> testLabels, std::vector<std::vector<float>> &result)
{
	Mat predictions = Mat::zeros(testLabels.size(), 8, CV_32FC1);
	for (int i = 0; i < testLabels.size(); i++)
	{
		cv::Mat testDataMat(1, testData[i].size(), CV_32FC1, testData[i].data());
		std::vector<float> res;
		for (int j = 0; j < boost.size(); j++)
		{
			predictions.at<float>(i, j) = boost[j]->predict(testDataMat);
			if (predictions.at<float>(i, j) > 0)
				res.push_back(j);
		}
		//printMat(predictions.row(i));
		if (res.size() == 0) res.push_back(8);
		for (int k : res) cout << k;
		cout << endl;
		int wait; cin >> wait;
		result.push_back(res);
	}
}

std::vector<float> AdaBoost::predict(Mat testData)
{
	Mat predictions = Mat::zeros(1, 8, CV_32FC1);
	std::vector<float> result;
	for (int j = 0; j < boost.size(); j++)
	{
		predictions.at<float>(0, j) = boost[j]->predict(testData);
		if (predictions.at<float>(0, j) > 0)
			result.push_back(j);
	}
	if (result.size() == 0) result.push_back(8);
	//for (int k : res) cout << k;
	//cout << endl;
	//int wait; cin >> wait;
	return result;
}
