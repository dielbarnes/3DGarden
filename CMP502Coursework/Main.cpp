//
// Main.cpp
// Copyright © 2018 Diel Barnes. All rights reserved.
//

#include "System.h"

// Entry point
int WINAPI WinMain(
				HINSTANCE hInstance, 
				HINSTANCE hPrevInstance, 
				PSTR pCmdLine,				// Address of command line string for the application
				int iCmdShow)				// Controls how the window is to be shown
{
	System* pSystem = new System(hInstance);
	if (pSystem->Initialize())
	{
		pSystem->Run();
	}

	SAFE_DELETE(pSystem);

	return 0;
}
