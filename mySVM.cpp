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
	cv::Mat trainingLabelsMat((trainingLabels.size()), 1, CV_32FC1, trainingLabels.data());
	
	//cout << "\n Training Data\n";
	//cout << trainingData[0].size() << " " << trainingData.size() << endl;
	for (int i = 0; i < trainingData.size(); i++)
	{
		//cout << "\n#" << i << "\n";
		for (int j = 0; j < trainingData[0].size(); j++)
		{
			trainingDataMat.at<float>(i, j) = trainingData.data()[i][j];
			//cout << trainingData.data()[i][j] << " ";
		}
		//cout << endl;
	}
	//cin.get();
	/*
	cout << "\n Training Mat\n";
	cout << trainingDataMat.size().width << " " << trainingDataMat.size().height << endl;
	for (int i = 0; i < trainingDataMat.size().height; i++)
	{
		cout << "\n#" << i << "\n";
		for (int j = 0; j < trainingDataMat.size().width; j++)
			cout << trainingDataMat.at<float>(i, j) << " ";
		cout << endl;
	}
	//cin.get();
	*/

	std::cout << "Start training\n";
	//svm.load("SVM.xml");
	//if (svm.get_support_vector_count() == 0) {
		CvSVMParams params;
		params.kernel_type = CvSVM::RBF;
		params.svm_type = CvSVM::C_SVC;
		params.gamma = 3.375e-06; // 0.000001;
		params.C = 83.9666; //5000;
		//params.nu = 0.1;
		//params.p = 1.1;
		params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 0.1);

		svm.train(trainingDataMat, trainingLabelsMat, cv::Mat(), cv::Mat(), params);
		
		//svm.train_auto(trainingDataMat, trainingLabelsMat, cv::Mat(), cv::Mat(), params);
		//params = svm.get_params();
		svm.save("SVM.xml");
	//}
		FeatureExtractor FE;
	for (int i = 0; i < testLabels.size(); i++)
	{
		cv::Mat testDataMat(1, testData[i].size(), CV_32FC1, testData[i].data());
		/*
		cout << "\n Test Mat\n";
		cout << testDataMat.size().width << " " << testDataMat.size().height << endl;
		for (int i = 0; i < testDataMat.size().height; i++)
		{
			cout << "\n#" << i << "\n";
			for (int j = 0; j < testDataMat.size().width; j++)
				cout << testDataMat.at<float>(i, j) << " ";
			cout << endl;
		}
		//cin.get();
		*/
		float predictedLabel = svm.predict(testDataMat);
		std::cout << "Predicted: " << classToEmotion(predictedLabel) << "\n";
		std::cout << "Actual: " << classToEmotion(testLabels[i]) << ((predictedLabel == testLabels[i])? " << Success\n" : "\n");
		
		if (floor(predictedLabel) == floor(testLabels[i]))
		{
			successfullyPredicted += 1;
			//std::cout << " << Success\n";
		}
		
	}

	//std::cout << "successfullyPredicted: " << successfullyPredicted << "\n";
	//std::cout << "testLabels.size(): " << testLabels.size() << "\n";
	//std::cout << "precision: " << ((float)successfullyPredicted / (float)(testLabels.size())) << "\n";

	float precision = ((float)successfullyPredicted / (testLabels.size()));
	//std::cout << fixed << setprecision(2) << "precision: " << ((float)successfullyPredicted / (testLabels.size())) << "\n";

	return precision;
}

float mySVM::go(std::vector<std::vector<float>> trainingData, std::vector<float> trainingLabels, std::vector<std::vector<float>> testData, std::vector<float> testLabels, float C, float gamma)
{
	int successfullyPredicted = 0;

	cv::Mat trainingDataMat(trainingData.size(), trainingData[0].size(), CV_32FC1);
	cv::Mat trainingLabelsMat((trainingLabels.size()), 1, CV_32FC1, trainingLabels.data());

	for (int i = 0; i < trainingData.size(); i++)
	{
		for (int j = 0; j < trainingData[0].size(); j++)
		{
			trainingDataMat.at<float>(i, j) = trainingData.data()[i][j];
		}
	}
	CvSVM svm;
	CvSVMParams params;
	params.kernel_type = CvSVM::RBF;
	params.svm_type = CvSVM::C_SVC;
	params.gamma = gamma;
	params.C = C;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 0.1);
	svm.train(trainingDataMat, trainingLabelsMat, cv::Mat(), cv::Mat(), params);
	for (int i = 0; i < testLabels.size(); i++)
	{
		cv::Mat testDataMat(1, testData[i].size(), CV_32FC1, testData[i].data());
		float predictedLabel = svm.predict(testDataMat);
		//std::cout << "Predicted: " << predictedLabel << "\n";
		//std::cout << "Actual: " << testLabels[i] << "\n";

		if (floor(predictedLabel) == floor(testLabels[i]))
		{
			successfullyPredicted += 1;
			//std::cout << "\nSuccess\n\n";
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