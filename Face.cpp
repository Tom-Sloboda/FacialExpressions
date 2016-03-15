#include "Face.h"



//Face::Face(){}

Face::Face(FeatureExtractor *FE, Mat img, float label )
{
	Face::mat = img;
	assign_image(Face::img, cv_image<uchar>(img));
	Face::shape = FE->detectFeatures(dlib::cv_image<bgr_pixel>(img), FE->detectFaces(dlib::cv_image<bgr_pixel>(img)));
	Face::landmarks = FE->getFlattened(shape);
	Face::label = label;
}

Face::Face(FeatureExtractor *FE, std::string img, float label)
{
	cout << img << endl;
	load_png(Face::img, img);
	Face::mat = imread(img, CV_LOAD_IMAGE_COLOR);
	/*
	if (!Face::mat.data)                              // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return;
	}

	namedWindow("Display window", WINDOW_AUTOSIZE);// Create a window for display.
	imshow("Display window", Face::mat);                   // Show our image inside it.

	waitKey(0);                                          // Wait for a keystroke in the window
	*/
	Face::shape = FE->detectFeatures(&(Face::img), FE->detectFaces(&(Face::img)));
	Face::calcLandmarks(FE);
	Face::label = label;
}

void Face::calcLandmarks(FeatureExtractor *FE)
{
	Face::landmarks = FE->getFlattened(Face::shape);
}

void Face::showLandmarks()
{
	Mat landmark_overlay = Face::mat;
	namedWindow(to_string(Face::label), WINDOW_AUTOSIZE);// Create a window for display.
	for (int i = 0; i < Face::landmarks.size(); i += 2)
	{
		putText(landmark_overlay, to_string(i), cvPoint(Face::landmarks[i], Face::landmarks[i+1]), FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(100, 200, 250), 1, CV_AA);
	}
	imshow(to_string(Face::label), landmark_overlay);
	waitKey(0);
	return;
}

Mat Face::getLandmarkOverlay()
{
	Mat landmark_overlay = Face::mat;
	//namedWindow(to_string(Face::label), WINDOW_AUTOSIZE);// Create a window for display.
	for (int i = 0; i < Face::landmarks.size(); i += 2)
	{
		putText(landmark_overlay, to_string(i), cvPoint(Face::landmarks[i], Face::landmarks[i + 1]), FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(100, 200, 250), 1, CV_AA);
	}
	//imshow(to_string(Face::label), landmark_overlay);
	return landmark_overlay;
}


Face::~Face()
{
}
