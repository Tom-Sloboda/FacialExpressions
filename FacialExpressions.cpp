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
#include "mySVM.h"

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
		
		Loader LDR;
		mySVM SVM;

		std::vector<std::vector<float>> trainingData;
		std::vector<float> trainingLabels;
		//std::cout << "1";
		int INDEX = 220;
		for (int i = 0; i < LDR.labels.size()- INDEX; i++)
		{
			trainingData.push_back(LDR.data[i]);
			trainingLabels.push_back(LDR.labels[i]);
		}
		
		std::vector<std::vector<float>> testData;
		std::vector<float> testLabels;
		//std::cout << "2";
		for (int i = LDR.labels.size() - INDEX; i < LDR.labels.size(); i++)
		{
			testData.push_back(LDR.data[i]);
			testLabels.push_back(LDR.labels[i]);
		}

		float precision = SVM.go(trainingData, trainingLabels, testData, testLabels);
		//float precision = SVM.go_auto(trainingData, trainingLabels, testData, testLabels);
		//cout << "Success rate: " << SVM.go(trainingData, trainingLabels, testData, testLabels) << "\n";
		cout << "Success rate: " << setprecision(2) << precision << "\n";

	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}

}

