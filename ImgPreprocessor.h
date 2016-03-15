#pragma once
#include "Face.h"
#include "opencv2/imgproc/imgproc.hpp"



class ImgPreprocessor
{
public:
	ImgPreprocessor();
	~ImgPreprocessor();

	void align(Face *face1, Face *face2);
};

