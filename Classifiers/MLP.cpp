#include "MLP.h"



MLP::MLP()
{
}


MLP::~MLP()
{
}


void MLP::train(Ptr<ml::TrainData> trainingData)
{
	cv::Mat layers = cv::Mat(6, 1, CV_32SC1);
	layers.row(0) = cv::Scalar(trainingData->getTrainSamples().size().width);
	layers.row(1) = cv::Scalar(20);
	layers.row(2) = cv::Scalar(20);
	layers.row(3) = cv::Scalar(20);
	layers.row(4) = cv::Scalar(20);
	layers.row(5) = cv::Scalar(trainingData->getClassLabels().size().height);

	mlp = ml::ANN_MLP::create();
	CvTermCriteria criteria;
	criteria.max_iter = 100;
	criteria.epsilon = 0.00001f;
	criteria.type = CV_TERMCRIT_ITER | CV_TERMCRIT_EPS;
	mlp->setTrainMethod(ml::ANN_MLP::BACKPROP);
	mlp->setBackpropWeightScale(0.05f);
	mlp->setBackpropMomentumScale(0.05f);
	mlp->setTermCriteria(criteria);

	// train
	mlp->train(trainingData);
}

float MLP::predict(std::vector<float> testData)
{
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

/******************************************************************************/
// global definitions (for speed and ease of use)

#define NUMBER_OF_TRAINING_SAMPLES 797
#define ATTRIBUTES_PER_SAMPLE 256
#define NUMBER_OF_TESTING_SAMPLES 796

#define NUMBER_OF_CLASSES 10

// N.B. classes are integer handwritten digits in range 0-9

/******************************************************************************/

// loads the sample database from file (which is a CSV text file)

int read_data_from_csv(const char* filename, Mat data, Mat classes,
	int n_samples)
{

	int classlabel; // the class label
	float tmpf;

	// if we can't read the input file then return 0
	FILE* f = fopen(filename, "r");
	if (!f)
	{
		printf("ERROR: cannot read file %s\n", filename);
		return 0; // all not OK
	}

	// for each sample in the file

	for (int line = 0; line < n_samples; line++)
	{

		// for each attribute on the line in the file

		for (int attribute = 0; attribute < (ATTRIBUTES_PER_SAMPLE + 1); attribute++)
		{
			if (attribute < ATTRIBUTES_PER_SAMPLE)
			{

				// first 256 elements (0-255) in each line are the attributes

				fscanf(f, "%f,", &tmpf);
				data.at<float>(line, attribute) = tmpf;

			}
			else if (attribute == ATTRIBUTES_PER_SAMPLE)
			{

				// attribute 256 is the class label {0 ... 9}

				fscanf(f, "%i,", &classlabel);
				classes.at<float>(line, classlabel) = 1.0;
			}
		}
	}

	fclose(f);

	return 1; // all OK
}

/******************************************************************************/

int main(int argc, char** argv)
{
	// lets just check the version first

	printf("OpenCV version %s (%d.%d.%d)\n",
		CV_VERSION,
		CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION);

	// define training data storage matrices (one for attribute examples, one
	// for classifications)

	Mat training_data = Mat(NUMBER_OF_TRAINING_SAMPLES, ATTRIBUTES_PER_SAMPLE, CV_32FC1);
	Mat training_classifications = Mat(NUMBER_OF_TRAINING_SAMPLES, NUMBER_OF_CLASSES, CV_32FC1);

	// define testing data storage matrices

	Mat testing_data = Mat(NUMBER_OF_TESTING_SAMPLES, ATTRIBUTES_PER_SAMPLE, CV_32FC1);
	Mat testing_classifications = Mat::zeros(NUMBER_OF_TESTING_SAMPLES, NUMBER_OF_CLASSES, CV_32FC1);

	// define classification output vector

	Mat classificationResult = Mat(1, NUMBER_OF_CLASSES, CV_32FC1);
	Point max_loc = Point(0, 0);

	// load training and testing data sets

	if (read_data_from_csv(argv[1], training_data, training_classifications, NUMBER_OF_TRAINING_SAMPLES) &&
		read_data_from_csv(argv[2], testing_data, testing_classifications, NUMBER_OF_TESTING_SAMPLES))
	{
		// define the parameters for the neural network (MLP)

		// set the network to be 3 layer 256->10->10
		// - one input node per attribute in a sample
		// - 10 hidden nodes
		// - one output node per class

		// note that the OpenCV neural network (MLP) implementation does not
		// support categorical variables explicitly.
		// So, instead of the output class label, we will use
		// a binary vector of {0,0 ... 1,0,0} components (one element by class)
		// for training and therefore, MLP will give us a vector of "probabilities"
		// at the prediction stage - the highest probability can be accepted
		// as the "winning" class label output by the network

		int layers_d[] = { ATTRIBUTES_PER_SAMPLE, 10,  NUMBER_OF_CLASSES };
		Mat layers = Mat(1, 3, CV_32SC1);
		layers.at<int>(0, 0) = layers_d[0];
		layers.at<int>(0, 1) = layers_d[1];
		layers.at<int>(0, 2) = layers_d[2];

		// create the network using a sigmoid function with alpha and beta
		// parameters 0.6 and 1 specified respectively (refer to manual)

		CvANN_MLP* nnetwork = new CvANN_MLP;
		nnetwork->create(layers, CvANN_MLP::SIGMOID_SYM, 0.6, 1);

		// set the training parameters

		CvANN_MLP_TrainParams params = CvANN_MLP_TrainParams(

			// terminate the training after either 1000
			// iterations or a very small change in the
			// network wieghts below the specified value

			cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 1000, 0.000001),

			// use backpropogation for training

			CvANN_MLP_TrainParams::BACKPROP,

			// co-efficents for backpropogation training
			// (refer to manual)

			0.1,
			0.1);

		// train the neural network (using training data)

		printf("\nUsing training database: %s\n", argv[1]);

		int iterations = nnetwork->train(training_data, training_classifications, Mat(), Mat(), params);

		printf("Training iterations: %i\n\n", iterations);

		// perform classifier testing and report results

		Mat test_sample;
		int correct_class = 0;
		int wrong_class = 0;
		int false_positives[NUMBER_OF_CLASSES] = { 0,0,0,0,0,0,0,0,0,0 };

		printf("\nUsing testing database: %s\n\n", argv[2]);

		for (int tsample = 0; tsample < NUMBER_OF_TESTING_SAMPLES; tsample++)
		{

			// extract a row from the testing matrix

			test_sample = testing_data.row(tsample);

			// run neural network prediction

			nnetwork->predict(test_sample, classificationResult);

			// The NN gives out a vector of probabilities for each class
			// We take the class with the highest "probability"
			// for simplicity (but we really should also check separation
			// of the different "probabilities" in this vector - what if
			// two classes have very similar values ?)

			minMaxLoc(classificationResult, 0, 0, 0, &max_loc);

			printf("Testing Sample %i -> class result (digit %d)\n", tsample, max_loc.x);

			// if the corresponding location in the testing classifications
			// is not "1" (i.e. this is the correct class) then record this

			if (!(testing_classifications.at<float>(tsample, max_loc.x)))
			{
				// if they differ more than floating point error => wrong class

				wrong_class++;

				false_positives[(int)max_loc.x]++;

			}
			else
			{

				// otherwise correct

				correct_class++;
			}
		}

		printf("\nResults on the testing database: %s\n"
			"\tCorrect classification: %d (%g%%)\n"
			"\tWrong classifications: %d (%g%%)\n",
			argv[2],
			correct_class, (double)correct_class * 100 / NUMBER_OF_TESTING_SAMPLES,
			wrong_class, (double)wrong_class * 100 / NUMBER_OF_TESTING_SAMPLES);

		for (int i = 0; i < NUMBER_OF_CLASSES; i++)
		{
			printf("\tClass (digit %d) false postives 	%d (%g%%)\n", i,
				false_positives[i],
				(double)false_positives[i] * 100 / NUMBER_OF_TESTING_SAMPLES);
		}

		// all OK : main returns 0

		return 0;
	}

	// not OK : main returns -1

	return -1;
}
/******************************************************************************/