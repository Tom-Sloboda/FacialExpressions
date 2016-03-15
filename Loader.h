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

using namespace std;
using namespace dlib;

class Loader
{
public:
	Loader();
	~Loader();

	void FindFilesRecursively(LPCTSTR lpFolder, LPCTSTR lpFilePattern, std::vector<std::string> *file_list);
	float readLabel(string path);
	string Loader::getImgPath(string path);
	void saveProgress();
	void loadProgress();
	std::vector<float> Loader::strToFloatVec(string in);

	std::vector<std::vector<float>> data;
	std::vector<float> labels;
	std::vector<std::string> img_list, label_list;
	//string img_dir = "C:\\Users\\dell490\\Downloads\\CK_dataset\\CK+\\extended-cohn-kanade-images\\cohn-kanade-images";
	string img_dir = "C:/Users/dell490/Downloads/CK_dataset/CK+/extended-cohn-kanade-images/cohn-kanade-images";
	//string label_dir = "C:\\Users\\dell490\\Downloads\\CK_dataset\\CK+\\Emotion_labels\\Emotion";
	string label_dir = "C:/Users//dell490/Downloads/CK_dataset/CK+/Emotion_labels/Emotion";

	std::vector<Face> faces;
};

