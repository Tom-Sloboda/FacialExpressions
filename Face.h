#pragma once

#include <dlib/opencv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include "FeatureExtractor.h"

using namespace std;
using namespace dlib;
using namespace cv;

class Face
{
public:
	//Face();
	Face(FeatureExtractor *FE, Mat img, float label);
	Face::Face(FeatureExtractor *FE, std::string img, float label);
	~Face();

	void showLandmarks();
	Mat getLandmarkOverlay();
	void calcLandmarks(FeatureExtractor *FE);

	Mat mat;
	array2d<rgb_pixel> img;
	std::vector<dlib::full_object_detection> shape;
	std::vector<float> landmarks;
	float label;
};

