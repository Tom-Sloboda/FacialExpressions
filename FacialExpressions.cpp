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
#include "Capture.h"
#include "GUI.h"
#include "Classifiers/Classifiers.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#define DEBUG

using namespace dlib;
using namespace std;
using namespace cv;

//int main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
		GUI GUI;
		GUI.console();
		FeatureExtractor FE;
		ImgPreprocessor IP;
		Loader LDR(&FE, &IP);
		Capture CAP;
		Classifiers CLS;
		std::vector<std::vector<float>> trainingData;
		std::vector<float> trainingLabels;
		//std::cout << "1";
		int wait;
		
		int countArr[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
		for (int i = 0; i < LDR.labels.size(); i++)
		{
			countArr[(int)(LDR.labels[i])]++;
		}
		for (int i = 0; i < 9; i++)
		{
			cout << i << " - " << countArr[i] << endl;
		}

		int INDEX = LDR.labels.size()/2;
		for (int i = 0; i < LDR.labels.size()- INDEX; i++)
		{
			if (LDR.labels[i] != 8)//&&(LDR.labels[i] != 0))
			{
				trainingData.push_back(LDR.data[i]);
				trainingLabels.push_back(LDR.labels[i]);
			}
		}
		
		std::vector<std::vector<float>> testData;
		std::vector<float> testLabels;
		//std::cout << "2";
		for (int i = LDR.labels.size() - INDEX; i < LDR.labels.size(); i++)
		{
			if (LDR.labels[i] != 8)// && (LDR.labels[i] != 0))
			{
				testData.push_back(LDR.data[i]);
				testLabels.push_back(LDR.labels[i]);
			}
		}

		//CLS.train(trainingData, trainingLabels);
		//cout << "ALL CLASSIFIERS TRAINED\n";


		std::vector<float> res;
		CLS.multiSvm.train(trainingData, trainingLabels);
		CLS.multiSvm.predict(testData, testLabels, res);



		Mat testDataMat = CLS.vectorToMat(testData);
		for (int i = 0; i < testLabels.size(); i++)
		{
			float res = CLS.predict(testDataMat.row(i));
			cout << "Predicted: " << res << "Actual: " << testLabels[i] << endl;
		}
		//Mat resMat;// (testLabels.size(), 8, CV_32FC1);
		//float precision = SVM.go(trainingData, trainingLabels, testData, testLabels);
		//Mat mplLabelMat(vec.size(), vec[0].size(), CV_32FC1);
		//CLS.bayes.go(CLS.vectorToMat(trainingData), CLS.vectorToMat(trainingLabels), CLS.vectorToMat(testData), CLS.vectorToMat(testLabels));
		//CLS.bayes.train(CLS.vectorToMat(trainingData), CLS.vectorToMat(trainingLabels));
		//CLS.bayes.predict(CLS.vectorToMat(testData), CLS.vectorToMat(testLabels));

		//CLS.mlp.train(CLS.vectorToMat(trainingData), CLS.vectorToMat(trainingLabels));
		//CLS.mlp.predict(CLS.vectorToMat(testData), CLS.vectorToMat(testLabels), resMat);
		////CLS.mlp.go(CLS.vectorToMat(trainingData), CLS.vectorToMat(trainingLabels), CLS.vectorToMat(testData), CLS.vectorToMat(testLabels));
		//std::vector<std::vector<float>> resVec;
		//CLS.linSVM.train(trainingData, trainingLabels);
		//CLS.linSVM.predict(testData, testLabels, resVec);
		////CLS.go(trainingData, trainingLabels, testData, testLabels);
		//cout << "Success rate: " << setprecision(2) << precision << "\n";
		
		HWND hwnd = GUI.createScrnCapWnd(hInstance);
		Mat neutralFace, otherFace, combined;
		array2d<rgb_pixel> dneutralFace, dotherFace;
		Face *neutral = NULL, *other=NULL;
		while (hwnd != NULL)
		{
			MSG msg;
			if (GetMessage(&msg, NULL, 0, 0) > 0)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (msg.message == WM_KEYUP) 
			{
				cout << msg.wParam << endl;
				if (msg.wParam == 'N')
				{
					neutralFace = CAP.hwnd2mat(hwnd);
					cv_image<rgb_alpha_pixel> cv_neutralFace = (cv_image<rgb_alpha_pixel>)neutralFace;
					assign_image(dneutralFace, cv_neutralFace);
					namedWindow("Neutral Face", WINDOW_AUTOSIZE);
					imshow("Neutral Face", neutralFace);
					waitKey(10);
					std::vector<dlib::rectangle> faces = FE.detectFaces(&dneutralFace);
					if (faces.size() > 0)
					{
						neutral = new Face(&FE, neutralFace, &dneutralFace, 0.0);
						IP.align(*neutral);
						imshow("Neutral Face", neutral->getLandmarkOverlay());
						waitKey(10);
					}
					//cout << "Number of faces = " << faces.size() <<endl;
				}
				if (msg.wParam == ' ')
				{
					otherFace = CAP.hwnd2mat(hwnd);
					cv_image<rgb_alpha_pixel> cv_otherFace = (cv_image<rgb_alpha_pixel>)otherFace;
					assign_image(dotherFace, cv_otherFace);
					namedWindow("Other Face", WINDOW_AUTOSIZE);
					imshow("Other Face", otherFace);
					waitKey(10);
					std::vector<dlib::rectangle> faces = FE.detectFaces(&dotherFace);
					if (faces.size() > 0)
					{
						other = new Face(&FE, otherFace, &dotherFace, 0.0);
						imshow("Other Face", other->getLandmarkOverlay());
						waitKey(10);
						if (neutral != NULL) {
							IP.align(*other, *neutral);
							imshow("Neutral Face", neutral->getLandmarkOverlay());
							imshow("Other Face", other->getLandmarkOverlay());

							int max_width = (neutral->mat.size().width > other->mat.size().width) ? neutral->mat.size().width : other->mat.size().width;
							int max_height = (neutral->mat.size().height > other->mat.size().height) ? neutral->mat.size().height : other->mat.size().height;
							Mat combined = Mat::zeros(max_height, max_width, other->mat.type());

							Mat reshapedMat = Mat::zeros(max_height, max_width, other->mat.type());
							neutral->mat.copyTo(reshapedMat(Rect(0, 0, neutral->mat.size().width, neutral->mat.size().height)));
							neutral->mat = reshapedMat.clone();

							reshapedMat = Mat::zeros(max_height, max_width, other->mat.type());
							other->mat.copyTo(reshapedMat(Rect(0, 0, other->mat.size().width, other->mat.size().height)));
							other->mat = reshapedMat.clone();

							cv::addWeighted(neutral->getLandmarkOverlay(), 0.5, other->getLandmarkOverlay(), 0.5, 0.0, combined);
							imshow("Combined", combined);
							waitKey(10);
							std::vector<float> testData = FE.getDifference(neutral->landmarks, other->landmarks);
							cv::Mat testDataMat(1, testData.size(), CV_32FC1, testData.data());
							//cout << SVM.classToEmotion(SVM.svm->predict(testDataMat)) << endl;
						}
					}
				}
			}
		}
		cin >> wait;
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
		int wait;
		cin >> wait;
	}

}
