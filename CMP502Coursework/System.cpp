//
// System.cpp
// Copyright � 2018 Diel Barnes. All rights reserved.
//

#include "System.h"

#pragma region Init

System::System(HINSTANCE hInstance)
	: m_hInstance(hInstance)
{
	m_hMainWindow = nullptr;
	m_pGraphics = nullptr;
	m_pTimer = nullptr;
	g_pApplicationHandle = this;
}

System::~System()
{
	SAFE_DELETE(m_pGraphics)
	SAFE_DELETE(m_pTimer)

	CleanUpMainWindow();
}

bool System::Initialize()
{
	int iScreenWidth = 1024;
	int iScreenHeight = 768;

	if (!InitMainWindow(iScreenWidth, iScreenHeight))
	{
		return false;
	}

	m_pGraphics = new Graphics();
	if (!m_pGraphics->Initialize(iScreenWidth, iScreenHeight, m_hMainWindow))
	{
		return false;
	}

	m_pTimer = new Timer();

	return true;
}

bool System::InitMainWindow(int& iScreenWidth, int& iScreenHeight)
{
	// Register the window class

	WNDCLASS windowClass;
	windowClass.style = CS_HREDRAW | CS_VREDRAW;					// Redraw the entire window if a movement or size adjustment changes the width and height of the client area
	windowClass.lpfnWndProc = MainWindowProc;
	windowClass.cbClsExtra = 0;										// Number of extra bytes to allocate following the window class structure
	windowClass.cbWndExtra = 0;										// Number of extra bytes to allocate following the window instance
	windowClass.hInstance = m_hInstance;
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH); // Class background brush handle
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = m_applicationName;

	RegisterClass(&windowClass);
	
	// Place the window at the center of the screen
	int positionX = (GetSystemMetrics(SM_CXSCREEN) - iScreenWidth) / 2;
	int positionY = (GetSystemMetrics(SM_CYSCREEN) - iScreenHeight) / 2;

	// Create and show the window

	m_hMainWindow = CreateWindowEx(
						0,                    // Extended window style
						m_applicationName,    // Window class
						m_applicationName,    // Window text
						WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,  // Window style
						positionX,
						positionY,
						iScreenWidth,
						iScreenHeight,
						NULL,                 // Parent window    
						NULL,                 // Menu
						m_hInstance,          // Application instance handle
						NULL                  // Additional application data
	);

	if (m_hMainWindow == NULL)
	{
		MessageBox(
			0,								// Owner window of the message box
			"Failed to create window.", 
			"",								// Title
			0);								// Buttons to be displayed
		return false;
	}

	ShowWindow(
		m_hMainWindow, 
		SW_SHOW);		// Activates the window and displays it in its current size and position
	
	return true;
}

#pragma endregion

#pragma region Message Handling

LRESULT CALLBACK MainWindowProc(HWND hWindow, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uiMsg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;

		case WM_KEYUP:
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
				return 0;
			}

		// Pass other messages to the System class message handler
		default:
			return g_pApplicationHandle->MessageHandler(hWindow, uiMsg, wParam, lParam);
	}
}

LRESULT System::MessageHandler(HWND hWindow, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uiMsg)
	{
		case WM_LBUTTONDOWN:
			m_pGraphics->OnMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), hWindow);

		case WM_LBUTTONUP:
			m_pGraphics->OnMouseUp();

		case WM_MOUSEMOVE:
			m_pGraphics->OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}

	return DefWindowProc(hWindow, uiMsg, wParam, lParam);
}

#pragma endregion

#pragma region Run

void System::Run()
{
	// Run the main loop until there is a quit message from the window
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		// Handle window events
		while (PeekMessage(
					&msg, 
					NULL,		// Retrieve messages for any window that belongs to the current thread, and any message on the current thread's message queue whose hwnd value is NULL
					0, 0,		// If wMsgFilterMin and wMsgFilterMax are both zero, all available messages are returned (no range filtering is performed)
					PM_REMOVE)) // Messages are removed from the queue after processing
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		// Update
		m_pTimer->Update();

		// Render
		if (!m_pGraphics->Render(m_pTimer->GetDeltaTime(), m_pTimer->GetFrameTime()))
		{
			break;
		}
	}
}

#pragma endregion

#pragma region CleanUp

void System::CleanUpMainWindow()
{
	DestroyWindow(m_hMainWindow);
	m_hMainWindow = nullptr;

	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = nullptr;

	g_pApplicationHandle = nullptr;
}

#pragma endregion
