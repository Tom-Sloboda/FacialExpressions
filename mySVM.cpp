#include "mySVM.h"


mySVM::mySVM()
{

}

mySVM::~mySVM()
{
}

void mySVM::multiclassExample()
{
	// Data for visual representation
	const int WIDTH = 512, HEIGHT = 512, NTRAINING_SAMPLES = 100;
	float FRAC_LINEAR_SEP = 0.9f;
	Mat I = Mat::zeros(HEIGHT, WIDTH, CV_8UC3);

	//--------------------- 1. Set up training data randomly ---------------------------------------
	Mat trainData(2 * NTRAINING_SAMPLES, 2, CV_32FC1);
	Mat labels(2 * NTRAINING_SAMPLES, 1, CV_32FC1);

	RNG rng(100); // Random value generation class

	// Set up the linearly separable part of the training data
	int nLinearSamples = (int)(FRAC_LINEAR_SEP * NTRAINING_SAMPLES);

	// Generate random points for the class 1
	Mat trainClass = trainData.rowRange(0, nLinearSamples);
	// The x coordinate of the points is in [0, 0.4)
	Mat c = trainClass.colRange(0, 1);
	rng.fill(c, RNG::UNIFORM, Scalar(1), Scalar(0.4 * WIDTH));
	// The y coordinate of the points is in [0, 1)
	c = trainClass.colRange(1, 2);
	rng.fill(c, RNG::UNIFORM, Scalar(1), Scalar(HEIGHT));

	// Generate random points for the class 2
	trainClass = trainData.rowRange(2 * NTRAINING_SAMPLES - nLinearSamples, 2 * NTRAINING_SAMPLES);
	// The x coordinate of the points is in [0.6, 1]
	c = trainClass.colRange(0, 1);
	rng.fill(c, RNG::UNIFORM, Scalar(0.6*WIDTH), Scalar(WIDTH));
	// The y coordinate of the points is in [0, 1)
	c = trainClass.colRange(1, 2);
	rng.fill(c, RNG::UNIFORM, Scalar(1), Scalar(HEIGHT));

	//------------------ Set up the non-linearly separable part of the training data ---------------

	// Generate random points for the classes 1 and 2
	trainClass = trainData.rowRange(nLinearSamples, 2 * NTRAINING_SAMPLES - nLinearSamples);
	// The x coordinate of the points is in [0.4, 0.6)
	c = trainClass.colRange(0, 1);
	rng.fill(c, RNG::UNIFORM, Scalar(0.4*WIDTH), Scalar(0.6*WIDTH));
	// The y coordinate of the points is in [0, 1)
	c = trainClass.colRange(1, 2);
	rng.fill(c, RNG::UNIFORM, Scalar(1), Scalar(HEIGHT));

	//------------------------- Set up the labels for the classes ---------------------------------
	labels.rowRange(0, NTRAINING_SAMPLES).setTo(1);  // Class 1
	labels.rowRange(NTRAINING_SAMPLES, 2 * NTRAINING_SAMPLES).setTo(2);  // Class 2

	//------------------------ 2. Set up the support vector machines parameters --------------------
	CvSVMParams params;
	params.svm_type = cv::SVM::C_SVC;
	params.C = 0.1;
	params.kernel_type = cv::SVM::LINEAR;
	params.term_crit = TermCriteria(CV_TERMCRIT_ITER, (int)1e7, 1e-6);

	//------------------------ 3. Train the svm ----------------------------------------------------
	cout << "Starting training process" << endl;
	CvSVM svm;
	svm.train(trainData, labels, Mat(), Mat(), params);
	cout << "Finished training process" << endl;

	//------------------------ 4. Show the decision regions ----------------------------------------
	Vec3b green(0, 100, 0), blue(100, 0, 0);
	for (int i = 0; i < I.rows; ++i)
		for (int j = 0; j < I.cols; ++j)
		{
			Mat sampleMat = (Mat_<float>(1, 2) << i, j);
			float response = svm.predict(sampleMat);

			if (response == 1)    I.at<Vec3b>(j, i) = green;
			else if (response == 2)    I.at<Vec3b>(j, i) = blue;
		}

	//----------------------- 5. Show the training data --------------------------------------------
	int thick = -1;
	int lineType = 8;
	float px, py;
	// Class 1
	for (int i = 0; i < NTRAINING_SAMPLES; ++i)
	{
		px = trainData.at<float>(i, 0);
		py = trainData.at<float>(i, 1);
		circle(I, Point((int)px, (int)py), 3, Scalar(0, 255, 0), thick, lineType);
	}
	// Class 2
	for (int i = NTRAINING_SAMPLES; i <2 * NTRAINING_SAMPLES; ++i)
	{
		px = trainData.at<float>(i, 0);
		py = trainData.at<float>(i, 1);
		circle(I, Point((int)px, (int)py), 3, Scalar(255, 0, 0), thick, lineType);
	}

	//------------------------- 6. Show support vectors --------------------------------------------
	thick = 2;
	lineType = 8;
	int x = svm.get_support_vector_count();

	for (int i = 0; i < x; ++i)
	{
		const float* v = svm.get_support_vector(i);
		circle(I, Point((int)v[0], (int)v[1]), 6, Scalar(128, 128, 128), thick, lineType);
	}

	imwrite("result.png", I);                      // save the Image
	imshow("SVM for Non-Linear Training Data", I); // show it to the user
	waitKey(0);
}

void mySVM::simpleExample()
{
		// Data for visual representation
		int width = 512, height = 512;
		Mat image = Mat::zeros(height, width, CV_8UC3);

		// Set up training data
		float labels[4] = { 1.0, -1.0, -1.0, -1.0 };
		Mat labelsMat(4, 1, CV_32FC1, labels);

		float trainingData[4][2] = { { 501, 10 },{ 255, 10 },{ 501, 255 },{ 10, 501 } };
		Mat trainingDataMat(4, 2, CV_32FC1, trainingData);

		// Set up SVM's parameters
		CvSVMParams params;
		params.svm_type = CvSVM::C_SVC;
		params.kernel_type = CvSVM::LINEAR;
		params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

		// Train the SVM
		CvSVM SVM;
		SVM.train(trainingDataMat, labelsMat, Mat(), Mat(), params);

		Vec3b green(0, 255, 0), blue(255, 0, 0);
		// Show the decision regions given by the SVM
		for (int i = 0; i < image.rows; ++i)
			for (int j = 0; j < image.cols; ++j)
			{
				Mat sampleMat = (Mat_<float>(1, 2) << j, i);
				float response = SVM.predict(sampleMat);

				if (response == 1)
					image.at<Vec3b>(i, j) = green;
				else if (response == -1)
					image.at<Vec3b>(i, j) = blue;
			}

		// Show the training data
		int thickness = -1;
		int lineType = 8;
		circle(image, Point(501, 10), 5, Scalar(0, 0, 0), thickness, lineType);
		circle(image, Point(255, 10), 5, Scalar(255, 255, 255), thickness, lineType);
		circle(image, Point(501, 255), 5, Scalar(255, 255, 255), thickness, lineType);
		circle(image, Point(10, 501), 5, Scalar(255, 255, 255), thickness, lineType);

		// Show support vectors
		thickness = 2;
		lineType = 8;
		int c = SVM.get_support_vector_count();

		for (int i = 0; i < c; ++i)
		{
			const float* v = SVM.get_support_vector(i);
			circle(image, Point((int)v[0], (int)v[1]), 6, Scalar(128, 128, 128), thickness, lineType);
		}

		//imwrite("result.png", image);        // save the image

		imshow("SVM Simple Example", image); // show it to the user
		waitKey(0);
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
	CvSVM svm;
	std::cout << "Start training\n";
	//svm.load("SVM.xml");
	//if (svm.get_support_vector_count() == 0) {
		CvSVMParams params;
		params.kernel_type = CvSVM::RBF;
		params.svm_type = CvSVM::C_SVC;
		params.gamma = 1.13906e-05; // 0.000001;
		params.C = 24.879; //5000;
		//params.nu = 0.1;
		//params.p = 1.1;
		params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 0.1);

		svm.train(trainingDataMat, trainingLabelsMat, cv::Mat(), cv::Mat(), params);
		
		//svm.train_auto(trainingDataMat, trainingLabelsMat, cv::Mat(), cv::Mat(), params);
		//params = svm.get_params();
		svm.save("SVM.xml");
	//}
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
		std::cout << "Predicted: " << predictedLabel << "\n";
		std::cout << "Actual: " << testLabels[i] << "\n";
		
		if (floor(predictedLabel) == floor(testLabels[i]))
		{
			successfullyPredicted += 1;
			std::cout << "\nSuccess\n\n";
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