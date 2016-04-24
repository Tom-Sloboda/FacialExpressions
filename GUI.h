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
	//static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void console();

	void show();

};

