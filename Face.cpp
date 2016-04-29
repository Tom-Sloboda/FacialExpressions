#include "Face.h"



Face::Face(){}

Face::Face(FeatureExtractor *FE, Mat img, float label)
{
	Face::mat = img;
	assign_image(Face::img, cv_image<uchar>(img));
	Face::faceBoxes = FE->detectFaces(dlib::cv_image<bgr_pixel>(img));
	Face::shape = FE->detectFeatures(dlib::cv_image<bgr_pixel>(img), Face::faceBoxes);
	Face::landmarks = FE->getFlattened(shape);
	Face::label = label;
}

Face::Face(FeatureExtractor *FE, Mat mat, array2d<rgb_pixel> *img, float label)
{
	assign_image(Face::img, *img);
	Face::mat = mat;
	Face::faceBoxes = FE->detectFaces(img);
	Face::shape = FE->detectFeatures(img, Face::faceBoxes);
	Face::landmarks = FE->getFlattened(shape);
	Face::label = label;
}

Face::Face(FeatureExtractor *FE, std::string img, float label)
{
	load_png(Face::img, img);
	Face::mat = imread(img, CV_LOAD_IMAGE_COLOR);
	Face::faceBoxes = FE->detectFaces(&(Face::img));
	Face::shape = FE->detectFeatures(&(Face::img), Face::faceBoxes);
	Face::landmarks = FE->getFlattened(shape);
	Face::label = label;
}

void Face::calcLandmarks(FeatureExtractor *FE)
{
	assign_image(Face::img, cv_image<bgr_pixel>(Face::mat));
	Face::faceBoxes = FE->detectFaces(dlib::cv_image<bgr_pixel>(Face::mat));
	Face::shape = FE->detectFeatures(dlib::cv_image<bgr_pixel>(Face::mat), Face::faceBoxes);
	Face::landmarks = FE->getFlattened(shape);
	Face::label = label;
}

void Face::showLandmarks()
{
	Mat landmark_overlay = Face::mat;
	namedWindow(to_string(Face::label), WINDOW_AUTOSIZE);// Create a window for display.
	for (int i = 0; i < Face::landmarks.size(); i += 2)
	{
		putText(landmark_overlay, to_string(i/2), cvPoint(Face::landmarks[i], Face::landmarks[i+1]), FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(100, 200, 250), 1, CV_AA);
	}
	imshow(to_string(Face::label), landmark_overlay);
	waitKey(0);
	return;
}

Mat Face::getLandmarkOverlay()
{
	Mat landmark_overlay; 
	Face::mat.copyTo(landmark_overlay);
	for (int i = 0; i < Face::landmarks.size(); i += 2)
	{
		putText(landmark_overlay, to_string(i/2), cvPoint(Face::landmarks[i], Face::landmarks[i + 1]), FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(100, 200, 250), 1, CV_AA);
	}
	return landmark_overlay;
}


Face::~Face()
{
}
