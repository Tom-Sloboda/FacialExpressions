#include "ImgPreprocessor.h"

#define LEFT_EYE_OUTER_X landmarks[72]
#define LEFT_EYE_OUTER_Y landmarks[73]
#define LEFT_EYE_INNER_X landmarks[78]
#define LEFT_EYE_INNER_Y landmarks[79]
#define RIGHT_EYE_OUTER_X landmarks[90]
#define RIGHT_EYE_OUTER_Y landmarks[91]
#define RIGHT_EYE_INNER_X landmarks[84]
#define RIGHT_EYE_INNER_Y landmarks[85]
#define NOSE_X landmarks[54]
#define NOSE_Y landmarks[55]


ImgPreprocessor::ImgPreprocessor()
{
}


ImgPreprocessor::~ImgPreprocessor()
{
}

void ImgPreprocessor::align(Face *face1, Face *face2)
{
	float angle1 = atan2f(face1->RIGHT_EYE_OUTER_Y - face1->LEFT_EYE_OUTER_Y, face1->RIGHT_EYE_OUTER_X - face1->LEFT_EYE_OUTER_X);
	float angle2 = atan2f(face2->RIGHT_EYE_OUTER_Y - face2->LEFT_EYE_OUTER_Y, face2->RIGHT_EYE_OUTER_X - face2->LEFT_EYE_OUTER_X);

}