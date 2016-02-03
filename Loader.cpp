#include "Loader.h"



Loader::Loader()
{
	loadProgress();
	std::vector<std::string> file_list;
	FindFilesRecursively(label_dir.c_str(), _T("*.txt"), &file_list);
	if (file_list.size() != label_list.size())
	{
		label_list = file_list;
		for each (string path in label_list) img_list.push_back(getImgPath(path));

		FeatureExtractor FE;
		array2d<rgb_pixel> cimg;
		for (int i = 0; i < img_list.size(); i++)
		{
			//cout << "!IMG - " << img_list[i] << "\n";
			std::vector<float> img1, img2;
			load_png(cimg, img_list[i]);
			img1 = FE.getFlattened(FE.detectFeatures(&cimg, FE.detectFaces(&cimg)));

			string neutral_img_name = img_list[i];
			//cout << "!IMG1 - " << neutral_img_name << "\n";
			neutral_img_name[neutral_img_name.size() - 5] = '1';
			neutral_img_name[neutral_img_name.size() - 6] = '0';
			//cout << "!IMG2 - " << neutral_img_name << "\n";
			load_png(cimg, neutral_img_name);
			img2 = FE.getFlattened(FE.detectFeatures(&cimg, FE.detectFaces(&cimg)));
			data.push_back(FE.getDifference(img2, img1));
			//FE.showFlattned(data[i]);
			//cout << "\n!!!! - " << FeatureExtractor::getFlattenedStr(data[i]) << "\n";

			/*
			for (int i = 0; i < (68 * 2); i += 2)
			{
			std::cout << "{" << flat[i] << ", " << flat[i + 1] << "}\n";
			}
			*/
			//FE.showFlattned(std::vector<float>(data.at(i), data.at(i)+sizeof data.at(i)[0]*68*2));
			labels.push_back(readLabel(label_list.at(i)));
			//cout << labels.at(i) << "\n";
			cout << i << "/" << img_list.size() <<"\n";
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
			label_list.push_back(line);
			//cout << line << "\n";

			getline(savefile, line);
			img_list.push_back(line);
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
	for (int i = 0; i < label_list.size(); i++)
	{
		savefile << label_list[i] << "\n";
		savefile << img_list[i] << "\n";
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