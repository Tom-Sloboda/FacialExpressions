#pragma once
#include "Face.h"
#include "FeatureExtractor.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/video/video.hpp>
#include <math.h>
#include <cmath>

class ImgPreprocessor
{
public:
	ImgPreprocessor();
	~ImgPreprocessor();

	void align(Face &face);
	void align(Face &face1, Face &face2);
	float distance(Point2f p1, Point2f p2);
	void rotate(Face &face, cv::Point2f center_of_rotation, float angle, float scale = 1.0);
	void transform(Face &face1, Face &face2);
	void transformEEC(Face &face1, Face &face2, int warp_mode = MOTION_HOMOGRAPHY);
	void transpositionAlign(Face &face1, Face &face2);
	void transformLandmarks(Face &face, Mat warp_mat);
	void crop(Face &face, float area_around_face_percentage = 0.2);
	void resize(Face &face, int width = 100, int height = 100);
	void reshape(Face &face, int width = 100, int height = 100);
	void perspectiveTransform(Face &face1, Face &face2);
	void transformPerspectiveLandmarks(Face &face, Mat perspective_mat);
};