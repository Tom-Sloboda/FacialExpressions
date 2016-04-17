#include "Capture.h"



Capture::Capture()
{
}


Capture::~Capture()
{
}
/*
cv::Mat Capture::getScreenCap(int x, int y, int height, int width)
{
	cv::Mat img(480, 640, CV_8UC3);
	glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
	glPixelStorei(GL_PACK_ROW_LENGTH, img.step / img.elemSize());
	glReadPixels(0, 0, img.cols, img.rows, GL_BGR_EXT, GL_UNSIGNED_BYTE, img.data);
	cv::Mat flipped(img);
	cv::flip(img, flipped, 0);
	cv::imwrite("snapshot.png", img);
	return cv::Mat();
}
*/
Mat Capture::hwnd2mat(HWND hwnd) 
{
	Mat src = screenshot();
	RECT clientrect;
	GetClientRect(hwnd, &clientrect);

	POINT pt = { 0,0 };	MapWindowPoints(hwnd, NULL, &pt, 1);	OffsetRect(&clientrect, pt.x, pt.y);

	src = src(cv::Rect(clientrect.left, clientrect.top, clientrect.right - clientrect.left, clientrect.bottom - clientrect.top));
	return src;
}

Mat Capture::screenshot()
{
	Mat src;
	// get the device context of the screen
	HDC hScreenDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	// and a device context to put it in
	HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

	int width = GetDeviceCaps(hScreenDC, HORZRES);
	int height = GetDeviceCaps(hScreenDC, VERTRES);

	// maybe worth checking these are positive values
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
	src.create(height, width, CV_8UC4);
	// get a new bitmap
	SelectObject(hMemoryDC, hBitmap);

	BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);

	BITMAPINFOHEADER  bi;
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	GetDIBits(hMemoryDC, hBitmap, 0, height, src.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow
	// clean up
	DeleteDC(hMemoryDC);
	DeleteDC(hScreenDC);

	//TODO: rewrite
	//Mat out(src.rows, src.cols, CV_32FC1);
	//src.convertTo(out, CV_32FC1);
	//src.assignTo(out);
	//cv::transform(src, src, cv::Matx12f(1, 1));
	return src;
}