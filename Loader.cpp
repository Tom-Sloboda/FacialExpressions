#include "Loader.h"
#include "Face.h"

//TODO: Rewrite this

Loader::Loader()
{
	FeatureExtractor FE;
	Loader::Loader(&FE);
}

Loader::Loader(FeatureExtractor *FE)
{
	Loader::FE = FE;
	cout << "\nWould you like to load existing data? y/n\n";
	std::string input;
	cin >> input;
	if (input == "y") {
		loadProgress();
		std::cout << "Done loading: " << labels.size() << endl;
	}
	else
	{
		std::vector<std::string> directory_list;
		img_path_list.clear();
		label_path_list.clear();
		labels.clear();
		data.clear();
		getAllDirectories(img_dir, &(directory_list));


		//findFilesRecursively(label_dir.c_str(), ".txt", &file_list);

		//image_window  win;
		
		Mat f1, f2, combined;
		namedWindow("Processing", WINDOW_AUTOSIZE);
		namedWindow("Face1", WINDOW_AUTOSIZE);
		namedWindow("Face2", WINDOW_AUTOSIZE);
		
		ImgPreprocessor IP(FE);
		int counter = 0;
		for each (string path in directory_list)
		{
			string imgpath = img_dir + path;
			
			std::vector<boost::filesystem::path> imgpaths;
			findFilesRecursively(imgpath.c_str(), ".png", &imgpaths);
			string labelpath = Loader::label_dir + path + "/" + imgpaths.back().stem().string();
			labelpath.append("_emotion.txt");
			float label = readLabel(labelpath);

			Face neutral_face(FE, imgpaths[0].string(), 0);

			for (int j = round(imgpaths.size() / 2); j < imgpaths.size(); j++)
			{
				Face face(FE, imgpaths[j].string(), 0);
				IP.align(face, neutral_face);
				data.push_back(FE->getDifference(neutral_face.landmarks, face.landmarks));
				
				f1 = face.getLandmarkOverlay();
				f2 = neutral_face.getLandmarkOverlay();
				cv::addWeighted(f1, 0.5, f2, 0.5, 0.0, combined);
				imshow("Processing", combined);
				imshow("Face1", f1);
				imshow("Face2", f2);
				waitKey(1);
				
				labels.push_back(label);
			}

			cout << counter++ << "/" << directory_list.size() << endl;
		}
		/*
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

				data.push_back(FE.getDifference(neutral_face.landmarks, face1.landmarks));
				labels.push_back(readLabel(label_path_list.at(i)));
				cout << i << "/" << img_path_list.size() << "\n";
			}
		}
		*/
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
	if (!exists(path))
		return 8.0;
	std::ifstream t(path);
	//cout << path;
	std::stringstream buffer;
	buffer << t.rdbuf();
	float label = 0.0;
	//buffer >> label;
	label = stof(buffer.str());
	return label;
}

void Loader::findFilesRecursively(const path &directory, const std::string extension, std::vector<boost::filesystem::path> *file_list)
{
	if (!exists(directory)) 
	{
		cout << "Directory doesn't exist - " << directory.string() << endl;
		return;
	}
	recursive_directory_iterator end_itr; // default construction yields past-the-end
	for (auto&& x : recursive_directory_iterator(directory)) {
		if (x.path().filename().extension() == extension)
		{
			//cout << x.path().string() << endl;
			file_list->push_back(x.path());
		}
	}
}

void Loader::getAllDirectories(const path &directory, std::vector<std::string> *img_dir_list)
{
	for (auto&& i : recursive_directory_iterator(directory)) {
		if (is_directory(i.path())) {
			if ((boost::count(i.path().string(), '\\')) >= (boost::count(directory.string(), '\\') + 2))
			{
				string path = i.path().generic_string().substr(directory.size(), i.path().size() - directory.size());
				img_dir_list->push_back(path);
				//cout << path << endl;
			}
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