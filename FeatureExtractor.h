#pragma once
#include <dlib/opencv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>

using namespace std;
using namespace dlib;
using namespace cv;

class FeatureExtractor
{
public:
	FeatureExtractor();
	~FeatureExtractor();
	std::vector<dlib::full_object_detection> detectFeatures(dlib::cv_image<dlib::bgr_pixel> cimg, std::vector<dlib::rectangle> faces);
	std::vector<dlib::full_object_detection> detectFeatures(dlib::array2d<dlib::rgb_pixel> *cimg, std::vector<dlib::rectangle> faces);
	
	std::vector<dlib::rectangle> detectFaces(dlib::cv_image<dlib::bgr_pixel> cimg);
	std::vector<dlib::rectangle> detectFaces(dlib::array2d<dlib::rgb_pixel> *cimg);

	std::vector<float> getFlattened(std::vector<dlib::full_object_detection> shapes);
	std::vector<float> getDifference(std::vector<float> shapes1, std::vector<float> shapes2);
	static std::string getFlattenedStr(std::vector<float> flattned);
	void showFlattned(std::vector<float> flattened);

	dlib::frontal_face_detector detector;
	dlib::shape_predictor pose_model;
};

