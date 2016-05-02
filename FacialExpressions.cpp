#include "FeatureExtractor.h"
#include "Loader.h"
#include "Capture.h"
#include "GUI.h"
#include "Classifiers/Classifiers.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>


using namespace dlib;
using namespace std;
using namespace cv;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
		GUI GUI;
		//allocates console
		GUI.console();
		FeatureExtractor FE;
		ImgPreprocessor IP;
		//Loads and preprocesses the dataseta
		Loader LDR(&FE, &IP);
		Capture CAP;
		Classifiers CLS;
		std::vector<std::vector<float>> trainingData;
		std::vector<float> trainingLabels;
		int wait;
		
		//counts the number of images of different classes loaded
		int countArr[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
		for (int i = 0; i < LDR.labels.size(); i++)
		{
			countArr[(int)(LDR.labels[i])]++;
		}
		for (int i = 0; i < 9; i++)
		{
			cout << i << " - " << countArr[i] << endl;
		}

		//divides the loaded data in half between training and test data
		int INDEX = LDR.labels.size()/2;
		for (int i = 0; i < LDR.labels.size()- INDEX; i++)
		{
			if (LDR.labels[i] != 8)
			{
				trainingData.push_back(LDR.data[i]);
				trainingLabels.push_back(LDR.labels[i]);
			}
		}
		
		std::vector<std::vector<float>> testData;
		std::vector<float> testLabels;
		for (int i = LDR.labels.size() - INDEX; i < LDR.labels.size(); i++)
		{
			if (LDR.labels[i] != 8)
			{
				testData.push_back(LDR.data[i]);
				testLabels.push_back(LDR.labels[i]);
			}
		}
		//trains all of the classification algorithms
		CLS.train(trainingData, trainingLabels);

		//goes through testData to give calculate prediction accuracy
		int successfullyPredicted = 0;
		Mat testDataMat = CLS.vectorToMat(testData);
		for (int i = 0; i < testLabels.size(); i++)
		{
			float res = CLS.predict(testDataMat.row(i));
			cout << "Predicted: " << CLS.classToEmotion(res) << "Actual: " << CLS.classToEmotion(testLabels[i])  << "\n\n";
			if (res == testLabels[i]) successfullyPredicted++;
		}
		cout << "Precision: " << (float)successfullyPredicted/testDataMat.rows << "\n\n";

		//creates viewfinder window
		HWND hwnd = GUI.createScrnCapWnd(hInstance);

		Mat neutralFace, otherFace, combined;
		array2d<rgb_pixel> dneutralFace, dotherFace;
		Face *neutral = NULL, *other=NULL;
		//main application loop
		while (hwnd != NULL)
		{
			//check for keypress messages
			MSG msg;
			if (GetMessage(&msg, NULL, 0, 0) > 0)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (msg.message == WM_KEYUP) 
			{
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
							//calculates the required height and width for the Mat which will combine both of the faces
							int max_width = (neutral->mat.size().width > other->mat.size().width) ? neutral->mat.size().width : other->mat.size().width;
							int max_height = (neutral->mat.size().height > other->mat.size().height) ? neutral->mat.size().height : other->mat.size().height;
							Mat combined = Mat::zeros(max_height, max_width, other->mat.type());
							//make both Mats the same size
							Mat reshapedMat = Mat::zeros(max_height, max_width, other->mat.type());
							neutral->mat.copyTo(reshapedMat(Rect(0, 0, neutral->mat.size().width, neutral->mat.size().height)));
							neutral->mat = reshapedMat.clone();

							reshapedMat = Mat::zeros(max_height, max_width, other->mat.type());
							other->mat.copyTo(reshapedMat(Rect(0, 0, other->mat.size().width, other->mat.size().height)));
							other->mat = reshapedMat.clone();

							//creates the overlayed images
							cv::addWeighted(neutral->getLandmarkOverlay(), 0.5, other->getLandmarkOverlay(), 0.5, 0.0, combined);
							imshow("Combined", combined);
							waitKey(10);
							std::vector<float> testData = FE.getDifference(neutral->landmarks, other->landmarks);
							cv::Mat testDataMat(1, testData.size(), CV_32FC1, testData.data());
							
							cout << CLS.classToEmotion(CLS.predict(testDataMat)) << endl << endl;
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