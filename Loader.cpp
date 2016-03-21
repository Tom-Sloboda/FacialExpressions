#include "Loader.h"
#include "Face.h"

//TODO: Rewrite this
Loader::Loader()
{
	loadProgress();
	std::vector<std::string> file_list;
	FindFilesRecursively(label_dir.c_str(), _T("*.txt"), &file_list);
	//std::cout << "Done loading";
	if (file_list.size() != label_path_list.size())
	{
		img_path_list.clear();
		label_path_list.clear();
		//image_window  win;
		Mat f1, f2, combined;
		namedWindow("Processing", WINDOW_AUTOSIZE);
		namedWindow("Face1", WINDOW_AUTOSIZE);
		namedWindow("Face2", WINDOW_AUTOSIZE);
		label_path_list = file_list;
		for each (string path in label_path_list) 
			img_path_list.push_back(getImgPath(path));

		FeatureExtractor FE;
		ImgPreprocessor IP(&FE);
		for (int i = 0; i < img_path_list.size(); i++)
		{
			//Get face with full expression
			Face face1(&FE, img_path_list[i], readLabel(label_path_list.at(i)));

			//Get neutral face
			string neutral_img_name = img_path_list[i];
			neutral_img_name[neutral_img_name.size() - 5] = '1';
			neutral_img_name[neutral_img_name.size() - 6] = '0';
			Face face2(&FE, neutral_img_name, 0);

			//win.clear_overlay();
			//win.set_image(face1.img);
			//win.add_overlay(render_face_detections(face1.shape));
			//win.add_overlay(render_face_detections(face2.shape));

			IP.align(face1, face2);

			f1 = face1.getLandmarkOverlay();
			f2 = face2.getLandmarkOverlay();
			cv::addWeighted(f1, 0.5, f2, 0.5, 0.0, combined);
			imshow("Processing", combined);
			imshow("Face1", f1);
			imshow("Face2", f2);
			//waitKey(0);
			data.push_back(FE.getDifference(face2.landmarks, face1.landmarks));
			labels.push_back(readLabel(label_path_list.at(i)));
			cout << i << "/" << img_path_list.size() << "\n";

		}
		cout << labels.size() << "\n";
		cout << data.size() << "\n";
		saveProgress();
	}
}


Loader::~Loader()
{
}

string Loader::getImgPath(string path)
{
	string img_name;
	path.erase(0, label_dir.size());
	//cout << path << "\n";
	char* split = strtok((char*)(path.c_str()), "_");
	img_name = split;
	img_name.append("_");
	split = strtok(NULL, "_");
	img_name.append(split);
	img_name.append("_");
	split = strtok(NULL, "_");
	img_name.append(split);
	img_name.append(".png");
	return img_dir + img_name;
}

float Loader::readLabel(string path)
{
	std::ifstream t(path);
	//cout << path;
	std::stringstream buffer;
	buffer << t.rdbuf();
	float label = 0.0;
	//buffer >> label;
	label = stof(buffer.str());
	return label;
}

void Loader::FindFilesRecursively(LPCTSTR lpFolder, LPCTSTR lpFilePattern, std::vector<std::string> *file_list)
{
	TCHAR szFullPattern[MAX_PATH];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile;
	// first we are going to process any subdirectories
	PathCombine(szFullPattern, lpFolder, _T("*"));
	hFindFile = FindFirstFile(szFullPattern, &FindFileData);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(FindFileData.cFileName, _T(".")) != 0 && _tcscmp(FindFileData.cFileName, _T("..")) != 0)
				{
					// found a subdirectory; recurse into it
					PathCombine(szFullPattern, lpFolder, FindFileData.cFileName);
					FindFilesRecursively(szFullPattern, lpFilePattern, file_list);
				}
			}
		} while (FindNextFile(hFindFile, &FindFileData));
		FindClose(hFindFile);
	}
	// now we are going to look for the matching files
	PathCombine(szFullPattern, lpFolder, lpFilePattern);
	hFindFile = FindFirstFile(szFullPattern, &FindFileData);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				// found a file; do something with it
				PathCombine(szFullPattern, lpFolder, FindFileData.cFileName);
				//_tprintf_s(_T("%s\n"), szFullPattern);
				file_list->push_back(szFullPattern);
			}
		} while (FindNextFile(hFindFile, &FindFileData));
		FindClose(hFindFile);
	}
}

void Loader::loadProgress()
{
	//cout << "!!!loadProgress()\n";
	string line;
	ifstream savefile("save.txt");
	int i = 0;
	if (savefile.is_open())
	{
		//cout << "!!!file open\n";
		while (getline(savefile, line))
		{
			//cout << ++i << "/327" << "\n";
			label_path_list.push_back(line);
			//cout << line << "\n";

			getline(savefile, line);
			img_path_list.push_back(line);
			//cout << line << "\n";

			getline(savefile, line);
			labels.push_back(stof(line));
			//cout << line << "\n";

			getline(savefile, line);
			std::vector<float> fltVec = strToFloatVec(line);
			data.push_back(fltVec);
			//cout << line << "\n";
			getline(savefile, line);
		}
	}
	savefile.close();
	//cout << "???loadProgress()\n";
}

void Loader::saveProgress()
{
	ofstream savefile;
	savefile.open("save.txt");
	for (int i = 0; i < label_path_list.size(); i++)
	{
		savefile << label_path_list[i] << "\n";
		savefile << img_path_list[i] << "\n";
		savefile << labels[i] << "\n";
		savefile << FeatureExtractor::getFlattenedStr(data[i]) << "\n\n";
	}
	savefile.close();
}

std::vector<float> Loader::strToFloatVec(string in)
{
	//cout << "!!!strToFloatVec - " << in << "\n";
	char* split = strtok((char*)(in.c_str()), " ");
	std::vector<float> out;
	while (split != NULL)
	{
		//cout << "!!!strToFloatVec - " << split << " " << stof(split) << "\n";
		out.push_back(stof(split));
		split = strtok(NULL, " ");
	}
	//cout << "???strToFloatVec \n";
	return out;
}