#pragma once
#pragma comment(lib, "shlwapi.lib")
#include "FeatureExtractor.h"
#include "ImgPreprocessor.h"
#include "Face.h"

#include <dlib/image_io.h>
#include <fstream>
#include <streambuf>
#include "shlwapi.h"
#include <tchar.h>

#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/range/algorithm/count.hpp>

using namespace std;
using namespace dlib;
using namespace boost::filesystem;

class Loader
{
public:
	Loader();
	Loader(FeatureExtractor *FE);
	~Loader();

	//void findFilesRecursively(LPCTSTR lpFolder, LPCTSTR lpFilePattern, std::vector<std::string> *file_list);
	void findFilesRecursively(const path &directory, const std::string extension, std::vector<boost::filesystem::path> *file_list);
	void Loader::getAllDirectories(const path &directory, std::vector<std::string> *img_dir_list);
	float readLabel(string path);
	string Loader::getImgPath(string path);
	void saveProgress(string filename = "save.txt");
	void loadProgress(string filename = "save.txt");
	std::vector<float> Loader::strToFloatVec(string in);
	void randomizeData();

	FeatureExtractor *FE;
	std::vector<std::vector<float>> data;
	std::vector<float> labels;
	std::vector<std::string> img_path_list, label_path_list;
	//string img_dir = "C:\\Users\\dell490\\Downloads\\CK_dataset\\CK+\\extended-cohn-kanade-images\\cohn-kanade-images";
	string img_dir = "C:/Users/dell490/Downloads/CK_dataset/CK+/extended-cohn-kanade-images/cohn-kanade-images";
	//string label_dir = "C:\\Users\\dell490\\Downloads\\CK_dataset\\CK+\\Emotion_labels\\Emotion";
	string label_dir = "C:/Users/dell490/Downloads/CK_dataset/CK+/Emotion_labels/Emotion";

	std::vector<Face> faces;
};

