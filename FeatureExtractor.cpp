#include "FeatureExtractor.h"

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



std::vector<dlib::rectangle> FeatureExtractor::detectFaces(cv_image<bgr_pixel> cimg)
{
	return detector(cimg);
}

std::vector<dlib::rectangle> FeatureExtractor::detectFaces(array2d<rgb_pixel> *cimg)
{
	return detector(*cimg);
}

std::vector<full_object_detection> FeatureExtractor::detectFeatures(cv_image<bgr_pixel> cimg, std::vector<dlib::rectangle> faces)
{
	std::vector<full_object_detection> shapes;
	if (faces.size() < 1)
	{
		cout << endl << "No face detected in detectFeatures()" << endl;
		faces.push_back(dlib::rectangle(cimg.nc(), cimg.nr()));
	}
	for (unsigned long i = 0; i < faces.size(); ++i)
		shapes.push_back(pose_model(cimg, faces[i]));
	return shapes;
}

std::vector<full_object_detection> FeatureExtractor::detectFeatures(array2d<rgb_pixel> *cimg, std::vector<dlib::rectangle> faces)
{
	std::vector<full_object_detection> shapes;
	if (faces.size() < 1)
	{
		cout << endl << "faces.size() < 1 in detectFeatures()" << endl;
		faces.push_back(dlib::rectangle(cimg->nc(), cimg->nr()));
	}
	for (unsigned long i = 0; i < faces.size(); ++i)
		shapes.push_back(pose_model(*cimg, faces[i]));
	return shapes;
}

std::vector<float> FeatureExtractor::getFlattened(std::vector<dlib::full_object_detection> shapes)
{
	std::vector<float> flattened;
	for (unsigned long i = 0; i < shapes.at(0).num_parts(); i++)
	{
		flattened.push_back(shapes.at(0).part(i).x());
		flattened.push_back(shapes.at(0).part(i).y());
	}
	return flattened;
}

std::vector<float> FeatureExtractor::getDifference(std::vector<float> shapes1, std::vector<float> shapes2)
{
	std::vector<float> difference;
	for (int i = 0; i < shapes1.size(); i++)
	{
		difference.push_back(shapes1[i]-shapes2[i]);
	}
	return difference;
}

string FeatureExtractor::getFlattenedStr(std::vector<float> flattened)
{
	stringstream flt;
	for (int i = 0; i < flattened.size(); i ++)
	{
		flt << flattened.at(i);
		flt << " ";
	}
	return flt.str();
}

void FeatureExtractor::showFlattned(std::vector<float> flattened)
{
	for (int i = 0; i < flattened.size(); i++)
	{
		cout << flattened[i] << " ";
	}
	cout << "\n";
}