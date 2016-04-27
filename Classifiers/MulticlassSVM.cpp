#include "MulticlassSVM.h"


MulticlassSVM::MulticlassSVM()
{

}

MulticlassSVM::~MulticlassSVM()
{
}


void MulticlassSVM::train(std::vector<std::vector<float>> trainingData, std::vector<float> trainingLabels, float C, float gamma)
{
	cv::Mat trainingDataMat = vectorToMat(trainingData);
	cv::Mat trainingLabelsMat = convertFloatToIntMat(vectorToMat(trainingLabels));
	printMat(trainingLabelsMat);
	int wait; cin >> wait;
#ifndef DEBUG
	cout << "\nWould you like to load MulticlassSVM.xml? y/n\n";
	std::string input;
	cin >> input;
	if (input == "y") {
		svm = StatModel::load<SVM>("SVM.xml");
	}

	if (svm == NULL)
#endif // !DEBUG
	{
		std::cout << "Start training\n";
		svm = SVM::create();
		svm->setType(ml::SVM::C_SVC);
		svm->setKernel(ml::SVM::RBF);
		svm->setGamma(gamma);
		svm->setC(C);
		svm->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 100, 0.1));
		Ptr<ml::TrainData> tData = ml::TrainData::create(trainingDataMat, ml::SampleTypes::ROW_SAMPLE, trainingLabelsMat);
		svm->train(tData);
		svm->save("SVM.xml");
		std::cout << "Finished training\n";
	}
}

void MulticlassSVM::predict(std::vector<std::vector<float>> testData, std::vector<float> testLabels, std::vector<float> &result)
{
	int successfullyPredicted = 0;
	for (int i = 0; i < testLabels.size(); i++)
	{
		Mat res;
		cv::Mat testDataMat(1, testData[i].size(), CV_32FC1, testData[i].data());
		float predictedLabel = predict(testDataMat);
		result.push_back(predictedLabel);
		std::cout << "Predicted: " << classToEmotion(predictedLabel) << "\n";
		std::cout << "Actual: " << classToEmotion(testLabels[i]) << ((predictedLabel == testLabels[i])? " << Success\n" : "\n");
		
		if (floor(predictedLabel) == floor(testLabels[i]))
		{
			successfullyPredicted += 1;
		}
		
	}
	float precision = ((float)successfullyPredicted / (testLabels.size()));
	
}

float MulticlassSVM::predict(Mat testData)
{
		float predictedLabel = svm->predict(testData);
		return predictedLabel;	
}