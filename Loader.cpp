#include "Loader.h"
#include "Face.h"

//TODO: Rewrite this
Loader::Loader()
{
	cout << "\nWould you like to load existing data? y/n\n";
	std::string input;
	cin >> input;
	if (input == "y") {
		loadProgress();
		std::cout << "Done loading\n";
	}
	else
	{
		std::vector<std::string> file_list;
		findFilesRecursively(label_dir.c_str(), ".txt", &file_list);
		img_path_list.clear();
		label_path_list.clear();
		labels.clear();
		data.clear();
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
			string num; 
			num += img_path_list[i][img_path_list[i].size() - 6];
			num += img_path_list[i][img_path_list[i].size() - 5];
			int num_of_images_in_sequence = stoi(num);

			//Get neutral face
			string neutral_img_path = img_path_list[i];
			neutral_img_path[neutral_img_path.size() - 5] = '1';
			neutral_img_path[neutral_img_path.size() - 6] = '0';
			Face neutral_face(&FE, neutral_img_path, 0);

			for (int j = round(num_of_images_in_sequence / 2); j < num_of_images_in_sequence; j++)
			{
				string current_img_path = img_path_list[i];
				current_img_path[current_img_path.size() - 6] = to_string(j/10)[0];
				current_img_path[current_img_path.size() - 5] = to_string(j%10)[0];
				Face face1(&FE, current_img_path, readLabel(label_path_list.at(i)));

				IP.align(face1, neutral_face);
				/*
				f1 = face1.getLandmarkOverlay();
				f2 = neutral_face.getLandmarkOverlay();
				cv::addWeighted(f1, 0.5, f2, 0.5, 0.0, combined);
				imshow("Processing", combined);
				imshow("Face1", f1);
				imshow("Face2", f2);
				waitKey(0);
				*/
				data.push_back(FE.getDifference(neutral_face.landmarks, face1.landmarks));
				labels.push_back(readLabel(label_path_list.at(i)));
				cout << i << "/" << img_path_list.size() << "\n";
			}
		}
		cout << labels.size() << "\n";
		cout << data.size() << "\n";
		saveProgress();
	}
	randomizeData();
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

void Loader::findFilesRecursively(const path &directory, const std::string extension, std::vector<std::string> *file_list)
{
	if (!exists(directory)) 
	{
		cout << "Directory doesn't exist/n";
		return;
	}
	recursive_directory_iterator end_itr; // default construction yields past-the-end
	for (auto&& x : recursive_directory_iterator(directory)) {
		if (x.path().filename().extension() == extension)
		{
			//cout << x.path().string() << endl;
			file_list->push_back(x.path().string());
		}
	}
}

/*
void Loader::findFilesRecursively(LPCTSTR lpFolder, LPCTSTR lpFilePattern, std::vector<std::string> *file_list)
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
*/

void Loader::loadProgress(string filename)
{
	string line;
	std::ifstream savefile(filename);
	int i = 0;
	if (savefile.is_open())
	{
		while (std::getline(savefile, line))
		{
			//label_path_list.push_back(line);

			//std::getline(savefile, line);
			//img_path_list.push_back(line);

			//std::getline(savefile, line);
			labels.push_back(stof(line));

			std::getline(savefile, line);
			std::vector<float> fltVec = strToFloatVec(line);
			data.push_back(fltVec);
			std::getline(savefile, line);
		}
	}
	savefile.close();
}


void Loader::saveProgress(string filename)
{
	std::ofstream savefile;
	savefile.open(filename);
	for (int i = 0; i < Loader::labels.size(); i++)
	{
		//savefile << label_path_list[i] << "\n";
		//savefile << img_path_list[i] << "\n";
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

void Loader::randomizeData()
{
	srand(time(NULL));
	int tmp_label;
	int j;
	for (int i = 0; i < labels.size(); i++)
	{
		j = std::rand() % labels.size();
		tmp_label = labels[i];
		labels[i] = labels[j];
		labels[j] = tmp_label;
		data[i].swap(data[j]);
	}
}