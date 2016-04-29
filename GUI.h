#pragma once
#include <windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>


using namespace std;

class GUI
{
public:
	GUI();
	~GUI();

	HWND createScrnCapWnd(HINSTANCE hInstance);
	void console();
};