#include "ImgPreprocessor.h"

#define LEFT_EYE_OUTER_X landmarks[72]
#define LEFT_EYE_OUTER_Y landmarks[73]
#define LEFT_EYE_INNER_X landmarks[78]
#define LEFT_EYE_INNER_Y landmarks[79]
#define RIGHT_EYE_OUTER_X landmarks[90]
#define RIGHT_EYE_OUTER_Y landmarks[91]
#define RIGHT_EYE_INNER_X landmarks[84]
#define RIGHT_EYE_INNER_Y landmarks[85]
#define NOSE1_X landmarks[60]
#define NOSE1_Y landmarks[61]
#define NOSE2_X landmarks[56]
#define NOSE2_Y landmarks[57]

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
	//float angle1 = atan2f(face.RIGHT_EYE_OUTER_Y - face.LEFT_EYE_OUTER_Y, face.RIGHT_EYE_OUTER_X - face.LEFT_EYE_OUTER_X);
	transform(face);
	crop(face);
	resize(face);
}

void ImgPreprocessor::align(Face &face1, Face &face2)
{
	float angle1 = atan2f(face1.RIGHT_EYE_OUTER_Y - face1.LEFT_EYE_OUTER_Y, face1.RIGHT_EYE_OUTER_X - face1.LEFT_EYE_OUTER_X);
	//float angle2 = atan2f(face2.RIGHT_EYE_OUTER_Y - face2.LEFT_EYE_OUTER_Y, face2.RIGHT_EYE_OUTER_X - face2.LEFT_EYE_OUTER_X);
	rotate(face1, Point2f(face1.LEFT_EYE_OUTER_X, face1.LEFT_EYE_OUTER_Y), angle1*(180/pi));
	//rotate(face2, Point2f(face2.LEFT_EYE_OUTER_X, face2.LEFT_EYE_OUTER_Y), angle2*(180/pi));

	//perspectiveTransform(face1, face2);
	//transform(face1, face2);
	crop(face1);
	crop(face2);
	resize(face1, 300, 300);
	resize(face2, 300, 300);
}

void ImgPreprocessor::rotate(Face &face, cv::Point2f center_of_rotation, float angle, float scale)
{
	Mat rot_mat(2, 3, CV_32FC1);
	rot_mat = getRotationMatrix2D(center_of_rotation, angle, scale);
	warpAffine(face.mat, face.mat, rot_mat, face.mat.size());
	transformLandmarks(face, rot_mat);
}

void ImgPreprocessor::crop(Face &face, float area_around_face_percentage)
{
	int minX = face.landmarks[0], maxX=face.landmarks[0], minY=face.landmarks[1], maxY=face.landmarks[1];
	
	for (int i = 0; i < face.landmarks.size(); i+=2)
	{
		if (face.landmarks[i] < minX)
			minX = face.landmarks[i];
		if (face.landmarks[i] > maxX)
			maxX = face.landmarks[i];
		if (face.landmarks[i+1] < minY)
			minY = face.landmarks[i+1];
		if (face.landmarks[i+1] > maxY)
			maxY = face.landmarks[i+1];
	}
	int height = maxY - minY;
	int width = maxX - minX;
	minX -= width*area_around_face_percentage;
	minY -= height*area_around_face_percentage;
	maxX += width*area_around_face_percentage;
	maxY += height*area_around_face_percentage;
	if (minX < 0)
	{
		cout << "minX < 0" << endl;
		minX = 0;
	}
	if (minY < 0)
	{
		cout << "minY < 0" << endl;
		minY = 0;
	}
	if (maxX > face.mat.cols)
	{
		cout << "maxX > face.mat.cols" << endl;
		maxX = face.mat.cols;
	}
	if (maxY > face.mat.rows)
	{
		cout << "maxY > face.mat.rows" << endl;
		maxY = face.mat.rows;
	}
	if ((maxX - minX) <= 0 )
	{
		cout << "(maxX - minX) <= 0" << endl;
		maxX = minX;
	}
	if ((maxY - minY) <= 0 )
	{
		cout << "(maxY - minY) <= 0" << endl;
		maxY = minY;
	}
	face.mat = face.mat(cv::Rect(minX, minY, (maxX-minX), (maxY - minY)));
	
	for (int i = 0; i < face.landmarks.size(); i += 2)
	{
		face.landmarks[i] -= minX;
		face.landmarks[i + 1] -= minY;
	}
}

void ImgPreprocessor::resize(Face &face, int width, int height)
{
	float fx = (float)width / face.mat.cols;
	float fy = (float)height / face.mat.rows;
	cv::resize(face.mat, face.mat, cv::Size(width, height));

	for (int i = 0; i < face.landmarks.size(); i += 2)
	{
		face.landmarks[i] *= fx;
		face.landmarks[i + 1] *= fy;
	}
}

void ImgPreprocessor::transform(Face &face)
{
	Point2f srcTri[3];
	srcTri[0] = Point2f(face.LEFT_EYE_OUTER_X, face.LEFT_EYE_OUTER_Y);
	srcTri[1] = Point2f(face.NOSE2_X, face.NOSE2_Y);
	srcTri[2] = Point2f(face.RIGHT_EYE_OUTER_X, face.RIGHT_EYE_OUTER_Y);
	Point2f dstTri[3];
	dstTri[0] = Point2f(ImgPreprocessor::faceTemplate->LEFT_EYE_OUTER_X, ImgPreprocessor::faceTemplate->LEFT_EYE_OUTER_Y);
	dstTri[1] = Point2f(ImgPreprocessor::faceTemplate->NOSE2_X, ImgPreprocessor::faceTemplate->NOSE2_Y);
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
	srcTri[1] = Point2f(face1.NOSE1_X, face1.NOSE1_Y);
	srcTri[2] = Point2f(face1.RIGHT_EYE_OUTER_X, face1.RIGHT_EYE_OUTER_Y);
	Point2f dstTri[3];
	dstTri[0] = Point2f(face2.LEFT_EYE_OUTER_X, face2.LEFT_EYE_OUTER_Y);
	dstTri[1] = Point2f(face2.NOSE1_X, face2.NOSE1_Y);
	dstTri[2] = Point2f(face2.RIGHT_EYE_OUTER_X, face2.RIGHT_EYE_OUTER_Y);
	Mat warp_mat(2, 3, CV_32FC1);
	//Mat warp_dst = Mat::zeros(face.mat.rows, face.mat.cols, face.mat.type());
	warp_mat = getAffineTransform(srcTri, dstTri);
	//warpAffine(face.mat, warp_dst, warp_mat, warp_dst.size());
	warpAffine(face1.mat, face1.mat, warp_mat, face1.mat.size());
	//warp_dst.assignTo(face.mat);
	ImgPreprocessor::transformLandmarks(face1, warp_mat);
}

void ImgPreprocessor::transformEEC(Face &face1, Face &face2, int warp_mode)
{

	Mat warp_mat;
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

void ImgPreprocessor::perspectiveTransform(Face &face1, Face &face2)
{
	/*
	Point2f srcPoints[4];
	srcPoints[0] = Point2f(face1.landmarks[72], face1.landmarks[2]);
	srcPoints[1] = Point2f(face1.landmarks[2], face1.landmarks[2]);
	srcPoints[2] = Point2f(face1.landmarks[2], face1.landmarks[2]);
	srcPoints[3] = Point2f(face1.landmarks[2], face1.landmarks[2]);
	Point2f dstPoints[4];
	dstPoints[0] = Point2f(face2.landmarks[2], face2.landmarks[2]);
	dstPoints[1] = Point2f(face2.landmarks[2], face2.landmarks[2]);
	dstPoints[2] = Point2f(face2.landmarks[2], face2.landmarks[2]);
	dstPoints[3] = Point2f(face2.landmarks[2], face2.landmarks[2]);
	*/
	/*
	Point2f srcPoints[4];
	srcPoints[0] = Point2f(face1.landmarks[72], face1.landmarks[73]);
	srcPoints[1] = Point2f(face1.landmarks[2], face1.landmarks[3]);
	srcPoints[2] = Point2f(face1.landmarks[30], face1.landmarks[31]);
	srcPoints[3] = Point2f(face1.landmarks[90], face1.landmarks[91]);
	Point2f dstPoints[4];
	dstPoints[0] = Point2f(face2.landmarks[72], face2.landmarks[73]);
	dstPoints[1] = Point2f(face2.landmarks[2], face2.landmarks[3]);
	dstPoints[2] = Point2f(face2.landmarks[30], face2.landmarks[31]);
	dstPoints[3] = Point2f(face2.landmarks[90], face2.landmarks[91]);
	*/
	///*
	Point2f srcPoints[4];
	srcPoints[0] = Point2f(face1.landmarks[0], face1.landmarks[1]);
	srcPoints[1] = Point2f(face1.landmarks[2], face1.landmarks[3]);
	srcPoints[2] = Point2f(face1.landmarks[30], face1.landmarks[31]);
	srcPoints[3] = Point2f(face1.landmarks[32], face1.landmarks[33]);
	Point2f dstPoints[4];
	dstPoints[0] = Point2f(face2.landmarks[0], face2.landmarks[1]);
	dstPoints[1] = Point2f(face2.landmarks[2], face2.landmarks[3]);
	dstPoints[2] = Point2f(face2.landmarks[30], face2.landmarks[31]);
	dstPoints[3] = Point2f(face2.landmarks[32], face2.landmarks[33]);
	//*/
	Mat perspective_mat = getPerspectiveTransform(srcPoints, dstPoints);
	warpPerspective(face1.mat, face1.mat, perspective_mat, face1.mat.size());
	ImgPreprocessor::transformPerspectiveLandmarks(face1, perspective_mat);
}

void ImgPreprocessor::transformPerspectiveLandmarks(Face &face, Mat perspective_mat)
{
	/*
	std::vector<float> new_landmarks;
	int x, y;
	for (int i = 0; i<face.landmarks.size(); i += 2) {
		x = face.landmarks[i];
		y = face.landmarks[i + 1];

		new_landmarks.push_back();
		new_landmarks.push_back(point_result.y);
	}
	face.landmarks = new_landmarks;
	*/
	std::vector<float> new_landmarks;
	for (int i = 0; i<face.landmarks.size(); i += 2) {

		Mat point_original(3, 1, CV_64FC1);
		point_original.at<double>(0, 0) = face.landmarks[i];
		point_original.at<double>(1, 0) = face.landmarks[i + 1];
		point_original.at<double>(2, 0) = 1;

		Mat result(3, 1, CV_64FC1);

		gemm(perspective_mat, point_original, 1.0, cv::Mat(), 0.0, result);

		Point point_result(cvRound(result.at<double>(0, 0)/result.at<double>(2, 0)), cvRound(result.at<double>(1, 0) / result.at<double>(2, 0)));

		new_landmarks.push_back(point_result.x);
		new_landmarks.push_back(point_result.y);
	}
	face.landmarks = new_landmarks;
}