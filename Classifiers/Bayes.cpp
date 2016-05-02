#include "Bayes.h"



Bayes::Bayes()
{
}


Bayes::~Bayes()
{
}

void Bayes::train(cv::Mat& trainingData, cv::Mat& trainingLabels)
{
	assert(trainingData.rows == trainingLabels.rows);
	cout << "\nWould you like to load Bayes.xml? y/n\n";
	std::string input;
	cin >> input;
	if (input == "y") {
		bayes = StatModel::load<NormalBayesClassifier>("Bayes.xml");
	}
	if (bayes == NULL)
	{
		cout << "Starting training\n";
		bayes = NormalBayesClassifier::create();
		bayes->train(trainingData, ml::SampleTypes::ROW_SAMPLE, convertFloatToIntMat(trainingLabels));
		//bayes->train(trainingData, ml::SampleTypes::ROW_SAMPLE, trainingLabels);
		bayes->save("Bayes.xml");
		cout << "Training finished\n";
	}
}

void Bayes::predict(Mat &testData, Mat &testLabels)
{
	assert(testData.rows == testData.rows);
	cv::Mat predicted(testLabels.rows, 1, CV_32F);
	for (int i = 0; i < testData.rows; i++)
	{
		const cv::Mat sample = testData.row(i);
		Mat output;
		bayes->predict(sample, output);
		predicted.at<float>(i, 0) = output.at<float>(0, 0);
		//cout << predicted.at<float>(i, 0) << endl;
		//cout << testClasses.at<float>(i, 0) << endl;
		//printMat(output);
		//int wait; cin >> wait;
	}
	//printMat(predicted);
	cout << "Accuracy_{BAYES} = " << evaluate(predicted, testLabels) << endl;
}

float Bayes::predict(cv::Mat &testData)
{
	return bayes->predict(testData);
}
