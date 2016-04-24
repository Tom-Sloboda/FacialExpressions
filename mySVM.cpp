#include "mySVM.h"


mySVM::mySVM()
{

}

mySVM::~mySVM()
{
}


float mySVM::go(std::vector<std::vector<float>> trainingData, std::vector<float> trainingLabels, std::vector<std::vector<float>> testData, std::vector<float> testLabels)
{
	int successfullyPredicted = 0;

	cv::Mat trainingDataMat(trainingData.size(), trainingData[0].size(), CV_32FC1);
	cv::Mat trainingLabelsMat((trainingLabels.size()), 1, CV_32SC1);
	for (int i = 0; i < trainingData.size(); i++)
	{
		trainingLabelsMat.at<int>(i) = (int)trainingLabels[i];
		for (int j = 0; j < trainingData[0].size(); j++)
		{
			trainingDataMat.at<float>(i, j) = trainingData.data()[i][j];
		}
	}
	cout << "\nWould you like to load SVM.xml? y/n\n";
	std::string input;
	cin >> input;
	if (input == "y") {
		svm = StatModel::load<SVM>("SVM.xml");
	}
	if (svm==NULL)
	{
		std::cout << "Start training\n";
		//svm.load("SVM.xml");
		//if (svm.get_support_vector_count() == 0) {
		svm = SVM::create();
		svm->setType(ml::SVM::C_SVC);
		svm->setKernel(ml::SVM::RBF);
		svm->setGamma(3.375e-06);
		svm->setC(83.9666);
		svm->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 100, 0.1));
		Ptr<ml::TrainData> tData = ml::TrainData::create(trainingDataMat, ml::SampleTypes::ROW_SAMPLE, trainingLabelsMat);

		svm->train(tData);

		//svm.train_auto(trainingDataMat, trainingLabelsMat, cv::Mat(), cv::Mat(), params);
		//params = svm.get_params();
		svm->save("SVM.xml");
	}
	//}
	FeatureExtractor FE;
	for (int i = 0; i < testLabels.size(); i++)
	{
		cv::Mat testDataMat(1, testData[i].size(), CV_32FC1, testData[i].data());
		float predictedLabel = svm->predict(testDataMat);
		//std::cout << "Predicted: " << classToEmotion(predictedLabel) << "\n";
		//std::cout << "Actual: " << classToEmotion(testLabels[i]) << ((predictedLabel == testLabels[i])? " << Success\n" : "\n");
		
		if (floor(predictedLabel) == floor(testLabels[i]))
		{
			successfullyPredicted += 1;
		}
		
	}
	float precision = ((float)successfullyPredicted / (testLabels.size()));
	return precision;
}

float mySVM::go(std::vector<std::vector<float>> trainingData, std::vector<float> trainingLabels, std::vector<std::vector<float>> testData, std::vector<float> testLabels, float C, float gamma)
{
	int successfullyPredicted = 0;

	cv::Mat trainingDataMat(trainingData.size(), trainingData[0].size(), CV_32FC1);
	cv::Mat trainingLabelsMat((trainingLabels.size()), 1, CV_32SC1);
	for (int i = 0; i < trainingData.size(); i++)
	{
		trainingLabelsMat.at<int>(i) = (int)trainingLabels[i];
		for (int j = 0; j < trainingData[0].size(); j++)
		{
			trainingDataMat.at<float>(i, j) = trainingData.data()[i][j];
		}
	}

	std::cout << "Start training\n";
	//svm.load("SVM.xml");
	//if (svm.get_support_vector_count() == 0) {
	svm = ml::SVM::create();
	svm->setType(ml::SVM::C_SVC);
	svm->setKernel(ml::SVM::RBF);
	svm->setGamma(gamma);
	svm->setC(C);
	svm->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 100, 0.1));
	Ptr<ml::TrainData> tData = ml::TrainData::create(trainingDataMat, ml::SampleTypes::ROW_SAMPLE, trainingLabelsMat);

	svm->train(tData);

	//svm.train_auto(trainingDataMat, trainingLabelsMat, cv::Mat(), cv::Mat(), params);
	//params = svm.get_params();
	svm->save("SVM.xml");
	//}
	FeatureExtractor FE;
	for (int i = 0; i < testLabels.size(); i++)
	{
		cv::Mat testDataMat(1, testData[i].size(), CV_32FC1, testData[i].data());
		float predictedLabel = svm->predict(testDataMat);
		std::cout << "Predicted: " << classToEmotion(predictedLabel) << "\n";
		std::cout << "Actual: " << classToEmotion(testLabels[i]) << ((predictedLabel == testLabels[i]) ? " << Success\n" : "\n");

		if (floor(predictedLabel) == floor(testLabels[i]))
		{
			successfullyPredicted += 1;
		}

	}
	float precision = ((float)successfullyPredicted / (testLabels.size()));
	return precision;
}

float mySVM::go_auto(std::vector<std::vector<float>> trainingData, std::vector<float> trainingLabels, std::vector<std::vector<float>> testData, std::vector<float> testLabels, float start_C, float end_C, float start_gamma, float end_gamma)
{
	float best_C = start_C;
	float best_gamma = start_gamma;
	float best_precision = 0;

	float inc_C = (end_C - start_C) / 100;
	float inc_gamma = (end_gamma - start_gamma) / 100;
	cout << "inc_C: " << inc_C << " inc_gamma: " << inc_gamma << endl;
	cin.get();
	for (float C = start_C; C < end_C; C *= 1.5)
	{
		for (float gamma = start_gamma; gamma < end_gamma; gamma *= 1.5)
		{
			float precision = go(trainingData, trainingLabels, testData, testLabels, C, gamma);
			cout << "Precision: " << precision << " C: " << C << " gamma: " << gamma << endl;
			if (precision > best_precision)
			{
				best_C = C;
				best_gamma = gamma;
				best_precision = precision;
			}
		}
	}
	cout << "++Precision: " << best_precision << " C: " << best_C << " gamma: " << best_gamma << endl;
	return best_precision;
}

std::string mySVM::classToEmotion(int n)
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
		return "Happyness";
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