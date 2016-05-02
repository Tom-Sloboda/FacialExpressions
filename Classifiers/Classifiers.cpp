#include "Classifiers.h"



Classifiers::Classifiers()
{
	mlp = MLP();
	multiSvm = MulticlassSVM();
	linSVM = LinearSVM();
	bayes = Bayes();
	boost = AdaBoost();
}


Classifiers::~Classifiers()
{
}




void Classifiers::train(std::vector<std::vector<float>> trainingData, std::vector<float> trainingLabels)
{
	assert(trainingData.size() == trainingLabels.size());
	multiSvm.train(trainingData, trainingLabels);
	boost.train(trainingData, trainingLabels);
	bayes.train(vectorToMat(trainingData), vectorToMat(trainingLabels));
	mlp.train(vectorToMat(trainingData), vectorToMat(trainingLabels));
	linSVM.train(trainingData, trainingLabels);
}

void Classifiers::predict(std::vector<std::vector<float>> testData, std::vector<float> testLabels)
{
	assert(testData.size() == testLabels.size());
	Mat testDataMat = vectorToMat(testData);
	std::vector<std::vector<float>> resultOverall;
	std::vector<std::vector<float>> result0;
	std::vector<std::vector<float>> result1;
	std::vector<std::vector<float>> result2;
	std::vector<std::vector<float>> result3;
	std::vector<std::vector<float>> result4;
	std::vector<std::vector<float>> result5;
	std::vector<std::vector<float>> result6;
	std::vector<std::vector<float>> result7;
	for (int i = 0; i < testLabels.size(); i++)
	{
		resultOverall.push_back(std::vector<float>());
		resultOverall[i].push_back(testLabels[i]);
		std::vector<float> resAdaBoost = boost.predict(testDataMat.row(i));
		if (resAdaBoost.size() == 1) {
			resultOverall[i].push_back(resAdaBoost[0]);
		}
		else {
			resultOverall[i].push_back(-1);
		}

		float resBayes = bayes.predict(testDataMat.row(i));
		resultOverall[i].push_back(resBayes);

		std::vector<float> resLinSVM = linSVM.predict(testDataMat.row(i));
		if (resLinSVM.size() == 1) {
			resultOverall[i].push_back(resLinSVM[0]);
		}
		else {
			resultOverall[i].push_back(-1);
		}

		std::vector<float> resMLP = mlp.predict(testDataMat.row(i));
		if (resMLP.size() == 1) {
			resultOverall[i].push_back(resMLP[0]);
		}
		else {
			resultOverall[i].push_back(-1);
		}

		float resMultiSVM = multiSvm.predict(testDataMat.row(i));
		resultOverall[i].push_back(resMultiSVM);

		std::vector<float> result = { 0,0,0,0,0,0,0,0 };

		for (float i : resAdaBoost)
			result[(int)i]++;

		result[(int)resBayes]++;

		for (float i : resLinSVM)
			result[(int)i]++;

		for (float i : resMLP)
			result[(int)i]++;

		result[(int)resMultiSVM]++;
		
		resultOverall[i].push_back(getHighestProbability(vectorToMat(result)));
	}
	printMat(vectorToMat(resultOverall));
}

float Classifiers::predict(Mat testData)
{
	std::vector<float> resAdaBoost = boost.predict(testData);
	cout << "AdaBoost: ";
	for (float i :resAdaBoost) 
		cout << classToEmotion(i) << " ";
	cout << endl;

	float resBayes = bayes.predict(testData);
	cout << "Bayes: " << classToEmotion(resBayes) << endl;

	std::vector<float> resLinSVM = linSVM.predict(testData);
	cout << "LinearSVM: ";
	for (float i : resLinSVM)
		cout << classToEmotion(i) << " ";
	cout << endl;

	std::vector<float> resMLP = mlp.predict(testData);
	cout << "MLP: ";
	for (float i : resMLP)
		cout << classToEmotion(i) << " ";
	cout << endl;

	float resMultiSVM = multiSvm.predict(testData);
	cout << "MultiSVM: " << classToEmotion(resMultiSVM) << endl;

	std::vector<float> result = { 0,0,0,0,0,0,0,0 };

	for (float i : resAdaBoost)
		result[(int)i]++;

	result[(int)resBayes]++;

	for (float i : resLinSVM)
		result[(int)i]++;

	for (float i : resMLP)
		result[(int)i]++;

	result[(int)resMultiSVM]++;

	cout << "Overall: " << classToEmotion(getHighestProbability(vectorToMat(result))) << endl;
	return getHighestProbability(vectorToMat(result));
}