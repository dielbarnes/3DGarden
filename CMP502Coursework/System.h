//
// System.h
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#ifndef SYSTEM_H
#define SYSTEM_H

#include <windowsx.h>
#include "Graphics.h"
#include "Timer.h"

LRESULT CALLBACK MainWindowProc(
					HWND hWindow, 
					UINT uiMsg, 
					WPARAM wParam, LPARAM lParam); // Additional message information

class System
{
public:
	System(HINSTANCE hInstance);
	~System();

	bool Initialize();
	void Run();

	LRESULT MessageHandler(HWND hWindow, UINT uiMsg, WPARAM wParam, LPARAM lParam);

private:
	LPCTSTR m_applicationName = "App";
	HINSTANCE m_hInstance; // Application instance handle
	HWND m_hMainWindow; // Main window handle

	Graphics* m_pGraphics;
	Timer* m_pTimer;

	bool InitMainWindow(int& iScreenWidth, int& iScreenHeight);
	void CleanUpMainWindow();
};

static System* g_pApplicationHandle; // External pointer to this application

#endif
