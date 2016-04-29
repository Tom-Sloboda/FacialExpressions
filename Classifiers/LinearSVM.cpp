#include "LinearSVM.h"



LinearSVM::LinearSVM()
{
}


LinearSVM::~LinearSVM()
{
}

void LinearSVM::train(std::vector<std::vector<float>> trainingData, std::vector<float> trainingLabels, float C /*= 83.9666*/, float gamma /*= 3.375e-06*/)
{
	cv::Mat trainingDataMat = vectorToMat(trainingData);
	cv::Mat trainingLabelsMat = vectorToMat(trainingLabels);

	cout << "\nWould you like to load LinearSVM[0-7].xml? y/n\n";
	std::string input;
	cin >> input;
	if (input == "y") {
		for (int i = 0; i < 8; i++)
		{
			Ptr<ml::SVM> tmpSVM = StatModel::load<SVM>("LinearSVM" + to_string(i) + ".xml");
			if (tmpSVM != NULL)
			{
				svm.push_back(tmpSVM);
			}
			else {
				for (Ptr<ml::SVM> j : svm)
				{
					j->empty();
				}
				break;
			}
		}
	}
	if (svm.size() < 8)
	{
		std::cout << "Start training\n";
		for (int i = 0; i < 8; i++)
		{
			Ptr<ml::SVM> tmpSVM = SVM::create();
			tmpSVM->setType(ml::SVM::C_SVC);
			tmpSVM->setKernel(ml::SVM::LINEAR);
			tmpSVM->setGamma(gamma);
			tmpSVM->setC(C);
			tmpSVM->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 1000, 0.1));
			//BUG: Something is very wrong here
			Ptr<ml::TrainData> tData = ml::TrainData::create(trainingDataMat, ml::SampleTypes::ROW_SAMPLE, convertMultiClassToSingleClassLabels(trainingLabelsMat, i));
			tmpSVM->trainAuto(tData);
			tmpSVM->save("LinearSVM"+to_string(i)+".xml");
			svm.push_back(tmpSVM);
		}
		std::cout << "Finished training\n";
	}
}

void LinearSVM::predict(std::vector<std::vector<float>> testData, std::vector<float> testLabels, std::vector<std::vector<float>> &result)
{
	Mat predictions = Mat::zeros(testLabels.size(), 8, CV_32FC1);
	for (int i = 0; i < testLabels.size(); i++)
	{
		cv::Mat testDataMat(1, testData[i].size(), CV_32FC1, testData[i].data());
		std::vector<float> res;
		for (int j = 0; j < svm.size(); j++)
		{
			predictions.at<float>(i,j) = svm[j]->predict(testDataMat);
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

std::vector<float> LinearSVM::predict(Mat testData)
{
	Mat predictions = Mat::zeros(1, 8, CV_32FC1);
	std::vector<float> result;
	for (int j = 0; j < svm.size(); j++)
	{
		predictions.at<float>(0, j) = svm[j]->predict(testData);
		if (predictions.at<float>(0, j) > 0)
			result.push_back(j);
	}
	return result;
}
