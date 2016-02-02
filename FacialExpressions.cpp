// The contents of this file are in the public domain. See LICENSE_FOR_EXAMPLE_PROGRAMS.txt
/*

This example program shows how to find frontal human faces in an image and
estimate their pose.  The pose takes the form of 68 landmarks.  These are
points on the face such as the corners of the mouth, along the eyebrows, on
the eyes, and so forth.


This example is essentially just a version of the face_landmark_detection_ex.cpp
example modified to use OpenCV's VideoCapture object to read from a camera instead
of files.


Finally, note that the face detector is fastest when compiled with at least
SSE2 instructions enabled.  So if you are using a PC with an Intel or AMD
chip then you should enable at least SSE2 instructions.  If you are using
cmake to compile this program you can enable them by using one of the
following commands when you create the build project:
cmake path_to_dlib_root/examples -DUSE_SSE2_INSTRUCTIONS=ON
cmake path_to_dlib_root/examples -DUSE_SSE4_INSTRUCTIONS=ON
cmake path_to_dlib_root/examples -DUSE_AVX_INSTRUCTIONS=ON
This will set the appropriate compiler options for GCC, clang, Visual
Studio, or the Intel compiler.  If you are using another compiler then you
need to consult your compiler's manual to determine how to enable these
instructions.  Note that AVX is the fastest but requires a CPU from at least
2011.  SSE4 is the next fastest and is supported by most current machines.
*/

#include "FeatureExtractor.h"
#include "Loader.h"
#include "SVM.h"

#include <opencv2/highgui/highgui.hpp>

using namespace dlib;
using namespace std;
using namespace cv;

int main()
{
	try
	{
		//cv::VideoCapture cap(0);
		//image_window win;

		// Grab and process frames until the main window is closed by the user.
		//while (!win.is_closed())

			/*
			//cv::Mat temp;
			//cap >> temp;
			//cv_image<bgr_pixel> cimg(temp);
			
			array2d<rgb_pixel> cimg;
			load_png(cimg, "C:/Users/dell490/Downloads/CK_dataset/CK+/extended-cohn-kanade-images/cohn-kanade-images/S010/001/S010_001_00000014.png");
			//load_png(cimg, "C:\\Users\\dell490\\Downloads\\CK_dataset\\CK+\\extended-cohn-kanade-images\\cohn-kanade-images\\S010\\001\\S010_001_00000014.png");
			//pyramid_up(cimg);

			FeatureExtractor FE;
			std::vector<full_object_detection> shapes = FE.detectFeatures(&cimg, FE.detectFaces(&cimg));
			
			float* flat = (FE.getFlattened(shapes));

			for (int i = 0; i < (68*2); i+=2)
			{
				std::cout << "{" << flat[i] << ", " << flat[i+1] << "}\n";
			}
			//cout << flat;

			// Display it all on the screen
			win.clear_overlay();
			win.set_image(cimg);
			win.add_overlay(render_face_detections(shapes));
			*/


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
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
}

