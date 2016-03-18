#include "ImgPreprocessor.h"

#define LEFT_EYE_OUTER_X landmarks[72]
#define LEFT_EYE_OUTER_Y landmarks[73]
#define LEFT_EYE_INNER_X landmarks[78]
#define LEFT_EYE_INNER_Y landmarks[79]
#define RIGHT_EYE_OUTER_X landmarks[90]
#define RIGHT_EYE_OUTER_Y landmarks[91]
#define RIGHT_EYE_INNER_X landmarks[84]
#define RIGHT_EYE_INNER_Y landmarks[85]
#define NOSE_X landmarks[56]
#define NOSE_Y landmarks[57]


ImgPreprocessor::ImgPreprocessor(FeatureExtractor *FE)
{
 	ImgPreprocessor::FE = FE;
	ImgPreprocessor::faceTemplate = new Face(FE, "C:/Users/dell490/Downloads/CK_dataset/CK+/extended-cohn-kanade-images/cohn-kanade-images/S037/001/S037_001_00000001.png", 0);
	float angle = atan2f(ImgPreprocessor::faceTemplate->RIGHT_EYE_OUTER_Y - ImgPreprocessor::faceTemplate->LEFT_EYE_OUTER_Y, ImgPreprocessor::faceTemplate->RIGHT_EYE_OUTER_X - ImgPreprocessor::faceTemplate->LEFT_EYE_OUTER_X);
	rotate(*(ImgPreprocessor::faceTemplate), Point2f(ImgPreprocessor::faceTemplate->LEFT_EYE_OUTER_X, ImgPreprocessor::faceTemplate->LEFT_EYE_OUTER_Y), angle*(180 / pi), 1.5);
}


ImgPreprocessor::~ImgPreprocessor()
{
}

void ImgPreprocessor::align(Face &face)
{
	float angle1 = atan2f(face.RIGHT_EYE_OUTER_Y - face.LEFT_EYE_OUTER_Y, face.RIGHT_EYE_OUTER_X - face.LEFT_EYE_OUTER_X);
	transform(face);
}

void ImgPreprocessor::align(Face &face1, Face &face2)
{
	float angle1 = atan2f(face1.RIGHT_EYE_OUTER_Y - face1.LEFT_EYE_OUTER_Y, face1.RIGHT_EYE_OUTER_X - face1.LEFT_EYE_OUTER_X);
	float angle2 = atan2f(face2.RIGHT_EYE_OUTER_Y - face2.LEFT_EYE_OUTER_Y, face2.RIGHT_EYE_OUTER_X - face2.LEFT_EYE_OUTER_X);
	rotate(face1, Point2f(face1.LEFT_EYE_OUTER_X, face1.LEFT_EYE_OUTER_Y), angle1*(180/pi));
	rotate(face2, Point2f(face2.LEFT_EYE_OUTER_X, face2.LEFT_EYE_OUTER_Y), angle2*(180/pi));
	transform(face1, face2);
}

void ImgPreprocessor::rotate(Face &face, cv::Point2f center_of_rotation, float angle, float scale)
{
	Mat rot_mat(2, 3, CV_32FC1);
	rot_mat = getRotationMatrix2D(center_of_rotation, angle, scale);
	warpAffine(face.mat, face.mat, rot_mat, face.mat.size());
	transformLandmarks(face, rot_mat);
}

void ImgPreprocessor::transform(Face &face)
{
	Point2f srcTri[3];
	srcTri[0] = Point2f(face.LEFT_EYE_OUTER_X, face.LEFT_EYE_OUTER_Y);
	srcTri[1] = Point2f(face.NOSE_X, face.NOSE_Y);
	srcTri[2] = Point2f(face.RIGHT_EYE_OUTER_X, face.RIGHT_EYE_OUTER_Y);
	Point2f dstTri[3];
	dstTri[0] = Point2f(ImgPreprocessor::faceTemplate->LEFT_EYE_OUTER_X, ImgPreprocessor::faceTemplate->LEFT_EYE_OUTER_Y);
	dstTri[1] = Point2f(ImgPreprocessor::faceTemplate->NOSE_X, ImgPreprocessor::faceTemplate->NOSE_Y);
	dstTri[2] = Point2f(ImgPreprocessor::faceTemplate->RIGHT_EYE_OUTER_X, ImgPreprocessor::faceTemplate->RIGHT_EYE_OUTER_Y);
	Mat warp_mat(2, 3, CV_32FC1);
	warp_mat = getAffineTransform(srcTri, dstTri);
	warpAffine(face.mat, face.mat, warp_mat, face.mat.size());
	ImgPreprocessor::transformLandmarks(face, warp_mat);
}

//Transform face1 to be more like face2
void ImgPreprocessor::transform(Face &face1, Face &face2)
{
	Point2f srcTri[3];
	srcTri[0] = Point2f(face1.LEFT_EYE_OUTER_X, face1.LEFT_EYE_OUTER_Y);
	srcTri[1] = Point2f(face1.NOSE_X, face1.NOSE_Y);
	srcTri[2] = Point2f(face1.RIGHT_EYE_OUTER_X, face1.RIGHT_EYE_OUTER_Y);
	Point2f dstTri[3];
	dstTri[0] = Point2f(face2.LEFT_EYE_OUTER_X, face2.LEFT_EYE_OUTER_Y);
	dstTri[1] = Point2f(face2.NOSE_X, face2.NOSE_Y);
	dstTri[2] = Point2f(face2.RIGHT_EYE_OUTER_X, face2.RIGHT_EYE_OUTER_Y);
	Mat warp_mat(2, 3, CV_32FC1);
	//Mat warp_dst = Mat::zeros(face.mat.rows, face.mat.cols, face.mat.type());
	warp_mat = getAffineTransform(srcTri, dstTri);
	//warpAffine(face.mat, warp_dst, warp_mat, warp_dst.size());
	warpAffine(face1.mat, face1.mat, warp_mat, face1.mat.size());
	//warp_dst.assignTo(face.mat);
	ImgPreprocessor::transformLandmarks(face1, warp_mat);
}

void ImgPreprocessor::transformLandmarks(Face &face, Mat warp_mat)
{
	std::vector<float> new_landmarks;
	for (int i = 0; i<face.landmarks.size(); i+=2) {

		Mat point_original(3, 1, CV_64FC1);
		point_original.at<double>(0, 0) = face.landmarks[i];
		point_original.at<double>(1, 0) = face.landmarks[i+1];
		point_original.at<double>(2, 0) = 1;

		Mat result(2, 1, CV_64FC1);

		gemm(warp_mat, point_original, 1.0, cv::Mat(), 0.0, result);

		Point point_result(cvRound(result.at<double>(0, 0)), cvRound(result.at<double>(1, 0)));

		new_landmarks.push_back(point_result.x); 
		new_landmarks.push_back(point_result.y);
	}
	face.landmarks = new_landmarks;
}