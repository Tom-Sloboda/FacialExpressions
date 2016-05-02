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

ImgPreprocessor::ImgPreprocessor()
{
}

ImgPreprocessor::~ImgPreprocessor()
{
}

//rotate, crop and resize a single @face
void ImgPreprocessor::align(Face &face)
{
	float angle1 = atan2f(face.RIGHT_EYE_OUTER_Y - face.LEFT_EYE_OUTER_Y, face.RIGHT_EYE_OUTER_X - face.LEFT_EYE_OUTER_X);
	rotate(face, Point2f(face.LEFT_EYE_OUTER_X, face.LEFT_EYE_OUTER_Y), angle1*(180 / pi));
	//transform(face);
	crop(face, 0.1);
	resize(face, 300, 300);
}
//calculates the distance between @p1 and @p2
float ImgPreprocessor::distance(Point2f p1, Point2f p2)
{
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}
//aligns @face with @neutralFace
void ImgPreprocessor::align(Face &face, Face &neutralFace)
{
	//The rotation/scale might seem redundant with transpositionAlign(), however it does appear to reduce some variability
	float angle1 = atan2f(face.RIGHT_EYE_OUTER_Y - face.LEFT_EYE_OUTER_Y, face.RIGHT_EYE_OUTER_X - face.LEFT_EYE_OUTER_X);
	float scale = distance(Point2f(neutralFace.RIGHT_EYE_OUTER_X,	neutralFace.RIGHT_EYE_OUTER_Y), Point2f(neutralFace.LEFT_EYE_OUTER_X,	neutralFace.LEFT_EYE_OUTER_Y))
				/ distance(Point2f(face.RIGHT_EYE_OUTER_X,			face.RIGHT_EYE_OUTER_Y),		Point2f(face.LEFT_EYE_OUTER_X,			face.LEFT_EYE_OUTER_Y));
	rotate(face, Point2f(face.LEFT_EYE_OUTER_X, face.LEFT_EYE_OUTER_Y), angle1*(180 / pi) , scale);
	crop(face, 0.1);
	resize(face, 300, 300);
	transpositionAlign(face, neutralFace);
}

//rotates
void ImgPreprocessor::rotate(Face &face, cv::Point2f center_of_rotation, float angle, float scale)
{
	Mat rot_mat;
	rot_mat = getRotationMatrix2D(center_of_rotation, angle, scale);
	warpAffine(face.mat, face.mat, rot_mat, cv::Size(face.mat.size().width*scale, face.mat.size().height*scale));
	transformLandmarks(face, rot_mat);
}

//crops the @face out of the image and leaves @area_around_face_percentage multiplied by width and height from each side
void ImgPreprocessor::crop(Face &face, float area_around_face_percentage)
{
	if (face.mat.cols == 0) {
		cout << "Error reading file in crop()" << endl;
		return;
	}
	float minX = face.landmarks[0], maxX=face.landmarks[0], minY=face.landmarks[1], maxY=face.landmarks[1];
	
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
	float height = maxY - minY;
	float width = maxX - minX;
	minX -= width*area_around_face_percentage;
	minY -= height*area_around_face_percentage;
	maxX += width*area_around_face_percentage;
	maxY += height*area_around_face_percentage;
	if (minX < 0) minX = 0;
	if (minY < 0) minY = 0;
	if (maxX > face.mat.cols) maxX = face.mat.cols;
	if (maxY > face.mat.rows) maxY = face.mat.rows;
	if ((maxX - minX) <= 0 ) maxX = minX;
	if ((maxY - minY) <= 0 ) maxY = minY;
	face.mat = face.mat(cv::Rect(minX, minY, (maxX-minX), (maxY - minY)));
	
	for (int i = 0; i < face.landmarks.size(); i += 2)
	{
		face.landmarks[i] -= minX;
		face.landmarks[i + 1] -= minY;
	}
}
//changes the dimensions of the image with scaling
void ImgPreprocessor::resize(Face &face, int width, int height)
{
	if (face.mat.cols == 0) {
		cout << "Error reading file in resize()" << endl;
		return;
	}
	
	float fx = (float)height / face.mat.rows;
	float fy = (float)height / face.mat.rows;
	cv::resize(face.mat, face.mat, cv::Size(), fx, fy);

	for (int i = 0; i < face.landmarks.size(); i += 2)
	{
		face.landmarks[i] *= fx;
		face.landmarks[i + 1] *= fy;
	}
	
	reshape(face, width, height);
}

//changes the dimension of the Mat without scaling the image
void ImgPreprocessor::reshape(Face &face, int width, int height)
{
	int max_width = (width > face.mat.size().width) ? width : face.mat.size().width;
	int max_height = (height > face.mat.size().height) ? height : face.mat.size().height;
	Mat reshapedMat = Mat::zeros(max_height, max_width, face.mat.type());
	face.mat.copyTo(reshapedMat(Rect(0, 0, face.mat.size().width, face.mat.size().height)));
	face.mat = reshapedMat;
	face.mat = face.mat(cv::Rect(0, 0, width, height));
}

//Transform @face1 to be more like @face2
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

//new alignment algorithm introduced in OpenCV 3.0, it's not very effective
void ImgPreprocessor::transformEEC(Face &face1, Face &face2, int warp_mode)
{
	// Convert images to gray scale;
	Mat im1_gray, im2_gray;
	cvtColor(face1.mat, im1_gray, CV_BGR2GRAY);
	cvtColor(face2.mat, im2_gray, CV_BGR2GRAY);
	Mat warp_matrix;
	if (warp_mode == MOTION_HOMOGRAPHY)
		warp_matrix = Mat::eye(3, 3, CV_32FC1);
	else
		warp_matrix = Mat::eye(2, 3, CV_32FC1);
	int number_of_iterations = 500;
	double termination_eps = 1e-7;
	TermCriteria criteria(TermCriteria::COUNT + TermCriteria::EPS, number_of_iterations, termination_eps);
	findTransformECC(im2_gray, im1_gray, warp_matrix, warp_mode, criteria);
	Mat im1_aligned;
	if (warp_mode != MOTION_HOMOGRAPHY)
		// Use warpAffine for Translation, Euclidean and Affine
		warpAffine(face1.mat, face1.mat, warp_matrix, face1.mat.size(), INTER_LINEAR + WARP_INVERSE_MAP);
	else
		// Use warpPerspective for Homography
		warpPerspective(face1.mat, face1.mat, warp_matrix, face1.mat.size(), INTER_LINEAR + WARP_INVERSE_MAP);

	ImgPreprocessor::transformPerspectiveLandmarks(face1, warp_matrix);
}

//transposes, scales and rotates @face to align it with @neutralFace
void ImgPreprocessor::transpositionAlign(Face & face, Face & neutralFace)
{
	long double M_PI = 3.141592653589793238L;
	Point2f srcTri[3];
	srcTri[0] = Point2f(face.RIGHT_EYE_OUTER_X, face.RIGHT_EYE_OUTER_Y);
	srcTri[1] = Point2f(face.LEFT_EYE_OUTER_X, face.LEFT_EYE_OUTER_Y); 
	srcTri[2] = Point2f(cos(60 * pi / 180.0) * (srcTri[0].x - srcTri[1].x) - sin(60 * pi / 180.0) * (srcTri[0].y - srcTri[1].y) + srcTri[1].x,
						sin(60 * pi / 180.0) * (srcTri[0].x - srcTri[1].x) + cos(60 * pi / 180.0) * (srcTri[0].y - srcTri[1].y) + srcTri[1].y);
	Point2f dstTri[3];
	dstTri[0] = Point2f(neutralFace.RIGHT_EYE_OUTER_X, neutralFace.RIGHT_EYE_OUTER_Y); 
	dstTri[1] = Point2f(neutralFace.LEFT_EYE_OUTER_X, neutralFace.LEFT_EYE_OUTER_Y);
	dstTri[2] = Point2f(cos(60 * pi / 180.0) * (dstTri[0].x - dstTri[1].x) - sin(60 * pi / 180.0) * (dstTri[0].y - dstTri[1].y) + dstTri[1].x,
						sin(60 * pi / 180.0) * (dstTri[0].x - dstTri[1].x) + cos(60 * pi / 180.0) * (dstTri[0].y - dstTri[1].y) + dstTri[1].y);
	Mat warp_mat(2, 3, CV_32FC1);
	warp_mat = getAffineTransform(srcTri, dstTri);
	warpAffine(face.mat, face.mat, warp_mat, face.mat.size());
	ImgPreprocessor::transformLandmarks(face, warp_mat);
}
//transforms the landmark vector after the transformation has been applied to the Mat
void ImgPreprocessor::transformLandmarks(Face &face, Mat warp_mat)
{
	std::vector<float> new_landmarks;
	for (int i = 0; i<face.landmarks.size(); i+=2) {

		Mat point_original(3, 1, CV_64FC1);// warp_mat.type());
		point_original.at<double>(0, 0) = face.landmarks[i];
		point_original.at<double>(1, 0) = face.landmarks[i+1];
		point_original.at<double>(2, 0) = 1;

		Mat result(2, 1, CV_64FC1);//warp_mat.type());

		gemm(warp_mat, point_original, 1.0, cv::Mat(), 0.0, result);

		new_landmarks.push_back(result.at<double>(0, 0));
		new_landmarks.push_back(result.at<double>(1, 0));
	}
	face.landmarks = new_landmarks;
}

//applies perspective transformation to face1 to align it with face2
void ImgPreprocessor::perspectiveTransform(Face &face1, Face &face2)
{
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

//transforms the landmark vector after the transformation has been applied to the Mat, used in perspectiveTransform()
void ImgPreprocessor::transformPerspectiveLandmarks(Face &face, Mat perspective_mat)
{
	std::vector<float> new_landmarks;
	for (int i = 0; i<face.landmarks.size(); i += 2) {

		Mat point_original(3, 1, perspective_mat.type());
		point_original.at<float>(0, 0) = face.landmarks[i];
		point_original.at<float>(1, 0) = face.landmarks[i + 1];
		point_original.at<float>(2, 0) = 1;

		Mat result(3, 1, perspective_mat.type());

		gemm(perspective_mat, point_original, 1.0, cv::Mat(), 0.0, result);

		Point point_result(cvRound(result.at<float>(0, 0)/result.at<float>(2, 0)), cvRound(result.at<float>(1, 0) / result.at<float>(2, 0)));

		new_landmarks.push_back(point_result.x);
		new_landmarks.push_back(point_result.y);
	}
	face.landmarks = new_landmarks;
}