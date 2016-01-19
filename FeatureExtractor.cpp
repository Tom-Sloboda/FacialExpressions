#include "FeatureExtractor.h"

using namespace dlib;
using namespace std;

FeatureExtractor::FeatureExtractor()
{
	try 
	{
		// Load face detection and pose estimation models.
		detector = get_frontal_face_detector();
		deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;
	}
	catch (serialization_error& e)
	{
		cout << "You need dlib's default face landmarking model file to run this example." << endl;
		cout << "You can get it from the following URL: " << endl;
		cout << "   http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
		cout << endl << e.what() << endl;
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
}


FeatureExtractor::~FeatureExtractor()
{
}

std::vector<full_object_detection> FeatureExtractor::detectFeatures(cv_image<bgr_pixel> cimg, std::vector<rectangle> faces)
{
	std::vector<full_object_detection> shapes;
	for (unsigned long i = 0; i < faces.size(); ++i)
		shapes.push_back(pose_model(cimg, faces[i]));
	return shapes;
}

std::vector<rectangle> FeatureExtractor::detectFaces(cv_image<bgr_pixel> cimg)
{
	return detector(cimg);
}

std::vector<full_object_detection> FeatureExtractor::detectFeatures(array2d<rgb_pixel> *cimg, std::vector<rectangle> faces)
{
	std::vector<full_object_detection> shapes;
	for (unsigned long i = 0; i < faces.size(); ++i)
		shapes.push_back(pose_model(*cimg, faces[i]));
	return shapes;
}

std::vector<rectangle> FeatureExtractor::detectFaces(array2d<rgb_pixel> *cimg)
{
	return detector(*cimg);
}


std::vector<float> FeatureExtractor::getFlattened(std::vector<dlib::full_object_detection> shapes)
{
	//cout << "!getFlattened() - SIZE=" << shapes.at(0).num_parts() << "\n";

	std::vector<float> flattened;
	for (unsigned long i = 0; i < shapes.at(0).num_parts(); i++)
	{
		flattened.push_back(shapes.at(0).part(i).x());
		flattened.push_back(shapes.at(0).part(i).y());
		//cout << "!XY - " << shapes.at(0).part(i).x() << " " << shapes.at(0).part(i).y() << "\n";
	}
	return flattened;
}

string FeatureExtractor::getFlattenedStr(std::vector<float> flattened)
{
	//string out = "";
	stringstream flt;
	for (int i = 0; i < flattened.size(); i ++)
	{
		flt << flattened.at(i);
		flt << " ";
		//out.append(flt.str()+" ");
	}
	return flt.str();
}

void FeatureExtractor::showFlattned(std::vector<float> flattened)
{
	//cout << "!showFlattned() - SIZE=" << flattened.size() << "\n";

	for (int i = 0; i < flattened.size(); i++)
	{
		cout << flattened[i] << " ";
	}
	cout << "\n";
}