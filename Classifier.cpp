#include "Classifier.h"



Classifier::Classifier()
{
}


Classifier::~Classifier()
{
}

void Classifier::go(std::vector<std::vector<float>> trData, std::vector<float> trLabels, std::vector<std::vector<float>> teData, std::vector<float> teLabels)
{
	cv::Mat trainingData(trData.size(), trData[0].size(), CV_32FC1);
	cv::Mat trainingLabels((trLabels.size()), 1, CV_32FC1, trLabels.data());

	for (int i = 0; i < trData.size(); i++)
	{
		for (int j = 0; j < trData[0].size(); j++)
		{
			trainingData.at<float>(i, j) = trData.data()[i][j];
		}
	}

	cv::Mat testData(teData.size(), teData[0].size(), CV_32FC1);
	cv::Mat testLabels((teLabels.size()), 1, CV_32FC1, teLabels.data());
	for (int i = 0; i < teData.size(); i++)
	{
		for (int j = 0; j < teData[0].size(); j++)
		{
			testData.at<float>(i, j) = teData.data()[i][j];
		}
	}

	svm(trainingData, trainingLabels, testData, testLabels);
	//mlp(trainingData, trainingLabels, testData, testLabels);
	knn(trainingData, trainingLabels, testData, testLabels, 3);
	bayes(trainingData, trainingLabels, testData, testLabels);
	//decisiontree(trainingData, trainingLabels, testData, testLabels);
}


float evaluate(cv::Mat& predicted, cv::Mat& actual) {	assert(predicted.rows == actual.rows);
	int t = 0;
	int f = 0;
	for (int i = 0; i < actual.rows; i++) 	{
		float p = predicted.at<float>(i, 0);
		float a = actual.at<float>(i, 0);
		if ((p == a) || (abs(p-a) < 0.4) )
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

void Classifier::svm(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses) {
	CvSVMParams param = CvSVMParams();
	/*
	param.svm_type = CvSVM::C_SVC;
	param.kernel_type = CvSVM::RBF; //CvSVM::RBF, CvSVM::LINEAR ...
	param.degree = 0; // for poly
	param.gamma = 20; // for poly/rbf/sigmoid
	param.coef0 = 0; // for poly/sigmoid
	param.C = 7; // for CV_SVM_C_SVC, CV_SVM_EPS_SVR and CV_SVM_NU_SVR
	param.nu = 0.0; // for CV_SVM_NU_SVC, CV_SVM_ONE_CLASS, and CV_SVM_NU_SVR
	param.p = 0.0; // for CV_SVM_EPS_SVR	param.class_weights = NULL; // for CV_SVM_C_SVC
	param.term_crit.type = CV_TERMCRIT_ITER + CV_TERMCRIT_EPS;
	param.term_crit.max_iter = 1000;
	param.term_crit.epsilon = 1e-6;	*/
	param.kernel_type = CvSVM::RBF;
	param.svm_type = CvSVM::C_SVC;
	param.gamma = 3.375e-06;
	param.C = 83.9666; 
	param.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 0.1);
	// SVM training (use train auto for OpenCV>=2.0)
	CvSVM svm(trainingData, trainingClasses, cv::Mat(), cv::Mat(), param);
	cv::Mat predicted(testClasses.rows, 1, CV_32F);
	for (int i = 0; i < testData.rows; i++) 	{		cv::Mat sample = testData.row(i);
		float x = sample.at<float>(0, 0);
		float y = sample.at<float>(0, 1);
		predicted.at<float>(i, 0) = svm.predict(sample);
	}
	cout << "Accuracy_{SVM} = " << evaluate(predicted, testClasses) << endl;
}

void Classifier::mlp(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses){
	cv::Mat layers = cv::Mat(6, 1, CV_32SC1);
	layers.row(0) = cv::Scalar(trainingData.size().width);
	layers.row(1) = cv::Scalar(trainingData.size().width*2);
	layers.row(2) = cv::Scalar(trainingData.size().width*2);
	layers.row(3) = cv::Scalar(trainingData.size().width*2);
	layers.row(4) = cv::Scalar(trainingData.size().width*2);
	layers.row(5) = cv::Scalar(1);

	CvANN_MLP mlp;
	CvANN_MLP_TrainParams params;
	CvTermCriteria criteria;
	criteria.max_iter = 100;
	criteria.epsilon = 0.00001f;
	criteria.type = CV_TERMCRIT_ITER | CV_TERMCRIT_EPS;
	params.train_method = CvANN_MLP_TrainParams::BACKPROP;
	params.bp_dw_scale = 0.05f;
	params.bp_moment_scale = 0.05f;
	params.term_crit = criteria;	mlp.create(layers);
	// train
	mlp.train(trainingData, trainingClasses, cv::Mat(), cv::Mat(), params);
	cv::Mat response(1, 1, CV_32FC1);
	cv::Mat predicted(testClasses.rows, 1, CV_32F);
	for (int i = 0; i < testData.rows; i++) 
	{
		cv::Mat response(1, 1, CV_32FC1);
		cv::Mat sample = testData.row(i);
		mlp.predict(sample, response);
		predicted.at<float>(i, 0) = response.at<float>(0, 0);
	}
	cout << "Accuracy_{MLP} = " << evaluate(predicted, testClasses) << endl;
}

void Classifier::knn(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses, int K)
{
	CvKNearest knn(trainingData, trainingClasses, cv::Mat(), false, K);
	cv::Mat predicted(testClasses.rows, 1, CV_32F);
	for (int i = 0; i < testData.rows; i++) 
	{
		const cv::Mat sample = testData.row(i);
		predicted.at<float>(i, 0) = knn.find_nearest(sample, K);
	}
	cout << "Accuracy_{KNN} = " << evaluate(predicted, testClasses) << endl;
}

void Classifier::bayes(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses){
	CvNormalBayesClassifier bayes(trainingData, trainingClasses);
	cv::Mat predicted(testClasses.rows, 1, CV_32F);
	for (int i = 0; i < testData.rows; i++) 
	{
		const cv::Mat sample = testData.row(i);
		predicted.at<float>(i, 0) = bayes.predict(sample);
	}
	cout << "Accuracy_{BAYES} = " << evaluate(predicted, testClasses) << endl;
}

void Classifier::decisiontree(cv::Mat& trainingData, cv::Mat& trainingClasses, cv::Mat& testData, cv::Mat& testClasses){
	CvDTree dtree;
	cv::Mat var_type(3, 1, CV_8U);
	// define attributes as numerical
	var_type.at<unsigned int>(0, 0) = CV_VAR_NUMERICAL;
	var_type.at<unsigned int>(0, 1) = CV_VAR_NUMERICAL;
	// define output node as numerical
	var_type.at<unsigned int>(0, 2) = CV_VAR_NUMERICAL;
	dtree.train(trainingData, CV_ROW_SAMPLE, trainingClasses, cv::Mat(), cv::Mat(), var_type, cv::Mat(), CvDTreeParams());
	cv::Mat predicted(testClasses.rows, 1, CV_32F);
	for (int i = 0; i < testData.rows; i++) {
		const cv::Mat sample = testData.row(i);
		CvDTreeNode* prediction = dtree.predict(sample);
		predicted.at<float>(i, 0) = prediction->value;
	}
	cout << "Accuracy_{TREE} = " << evaluate(predicted, testClasses) << endl;
}