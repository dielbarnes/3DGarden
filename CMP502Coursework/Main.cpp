//
// Main.cpp
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#include "App.h"

// Entry point
int WINAPI WinMain(
				HINSTANCE hInstance, 
				HINSTANCE hPrevInstance, 
				PSTR pCmdLine,				// Address of command line string for the application
				int iCmdShow)				// Controls how the window is to be shown
{
	App* pApp = new App(hInstance);
	if (pApp->Initialize())
	{
		pApp->Run();
	}

	SAFE_DELETE(pApp);

	return 0;
}
