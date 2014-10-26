// main.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

// ################################################
// SOLIDUS INCLUDES
// ################################################
#include "OpenGL.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "World.h"
#include "Settings.h"
#include "Utilities.h"
#include "Console.h"
#include "TextManager.h"
#include "JobManager.h"

#define MAX_LOADSTRING 100

namespace
{
	// Global Variables:
	HINSTANCE hInst;								// current instance
	TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
	TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
	const double DESIRED_FRAME_RATE = 60.0;
	const double DESIRED_SECONDS_PER_FRAME = 1.0 / DESIRED_FRAME_RATE;
	const double DESIRED_CLOCKS_PER_FRAME = (double) Solidus::SecondsToClocks( DESIRED_SECONDS_PER_FRAME );	
	std::unique_ptr< Solidus::World > g_App;

	int                 g_windowWidth;
	int                 g_windowHeight;
	bool                g_isFullScreen;	
	HWND                g_hWnd;
}

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
HWND				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void KeyWordInput( UINT message, WPARAM wParam );
void ToggleFullScreen();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.	
	HRESULT hResult;	
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_RPG, szWindowClass, MAX_LOADSTRING);
	
	ATOM atom = MyRegisterClass( hInstance );
	hResult = atom ? S_OK : E_FAIL;

	// Perform application initialization:
	//
	g_hWnd = InitInstance(hInstance, nCmdShow);
	if ( !g_hWnd )
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RPG));

	// Setup rendering.
	//
	HDC hDC = GetDC( g_hWnd );
	HGLRC hRenderingContext = InitializeOpenGL( hDC );

	RECT clientRect;
	GetClientRect( g_hWnd, &clientRect );	

	// Loading engine settings
	//
	Solidus::Settings::Create( "app.xml ");

	// Creating Console
	//
	Solidus::BitmapFont font;
	LoadFontXML( "MainFont_EN.FontDef.xml", "MainFont_EN_00.png", font );
	Solidus::TextManager::Create( font );
	Solidus::ConsoleSystem::Create( clientRect.right, clientRect.bottom );

	g_App.reset( new Solidus::World( g_hWnd, hDC, DESIRED_SECONDS_PER_FRAME, clientRect.right, clientRect.bottom ));

	// Main message loop:
	//
	Solidus::SystemClock nextUpdateTime = Solidus::GetSystemClocks();
	bool executeMainLoop = true;
	while ( executeMainLoop )
	{
		MSG msg;
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{	
			if( msg.message == WM_QUIT )
			{
				break;
			}

			if ( !TranslateAccelerator( msg.hwnd, hAccelTable, &msg ) )
			{
				TranslateMessage( &msg);
				DispatchMessage( &msg );
			}
		}
		else
		{    
			// UPDATE FRAME
			if( Solidus::GetSystemClocks() >= nextUpdateTime )
			{
				nextUpdateTime = Solidus::GetSystemClocks() + (Solidus::SystemClock) DESIRED_CLOCKS_PER_FRAME;				

				// Job Manager
				Solidus::JobManager::Instance().Update();

				// Input
				//
				g_App->UpdateInput(); // Note this is not the input manager

				// Update
				//
				g_App->UpdateSimulation();				

				// Draw
				//
				g_App->Draw();
			}			
		}
	}

	// Shutdown
	//
	ShutdownOpenGL( hRenderingContext );

	ReleaseDC( g_hWnd, hDC );

	return SUCCEEDED( hResult ) ? EXIT_SUCCESS : EXIT_FAILURE;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDC_RPG));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= Solidus::Settings::Instance().ShowWindowMenu() ? MAKEINTRESOURCE(IDC_RPG) : NULL;//
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return NULL;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return hWnd;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	// Handling mouse position
	//
	keyboardPollEvent(); // WndProc is invoked by PeekMessage	
	switch (message)
	{	
	case WM_MOUSEWHEEL:		
		onMouseWheelEvent( GET_WHEEL_DELTA_WPARAM( wParam ) > 0 ? 1 : -1 );
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		switch( wParam )
		{
		case VK_ESCAPE:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		default:
			KeyWordInput( message, wParam );
			break;
		}		
		break;
	case WM_CHAR:	
		switch( wParam )
		{
		case 0x46:
		case 0x66:
			if( !Solidus::ConsoleSystem::Instance().IsVisibile() )
			{ 
				ToggleFullScreen();
				break;
			}			
		default:
			KeyWordInput( message, wParam );
			break;
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		switch (wParam) 
		{ 
		case VK_LEFT: 			
			Solidus::ConsoleSystem::Instance().MoveCursorLeft();
			break; 
		case VK_RIGHT: 			
			Solidus::ConsoleSystem::Instance().MoveCursorRight();
			break; 
		case VK_UP: 
			Solidus::ConsoleSystem::Instance().PreviousCommand();
			break; 
		case VK_DOWN: 
			Solidus::ConsoleSystem::Instance().NextCommand();			
			break;
		case VK_PRIOR:
			if( Solidus::ConsoleSystem::Instance().IsVisibile() )
			{
				Solidus::ConsoleSystem::Instance().UpdateInput( static_cast< const unsigned char> ( wParam ));
			}
			break;
		case VK_NEXT:
			if( Solidus::ConsoleSystem::Instance().IsVisibile() )
			{
				Solidus::ConsoleSystem::Instance().UpdateInput( static_cast< const unsigned char> ( wParam ));
			}
			break;
		default: 
			onKeyboardReleasedEvent( static_cast< const unsigned char> ( wParam ));
			break; 
		}
		break;	
	case WM_LBUTTONUP:
		break;
	case WM_RBUTTONUP:
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{		
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;	
	case WM_SIZE:
		g_windowWidth = static_cast<int>(LOWORD(lParam));
		g_windowHeight = static_cast<int>(HIWORD(lParam));

		g_App->OnResize( g_windowWidth, g_windowHeight );

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void KeyWordInput( UINT message, WPARAM wParam )
{	
	if( Solidus::ConsoleSystem::Instance().IsVisibile() )
	{
		switch( message )
		{
		case WM_CHAR:
			Solidus::ConsoleSystem::Instance().UpdateInput( static_cast< const unsigned char> ( wParam ));
			break;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			switch( wParam )
			{
			case 0xC0:
				Solidus::ConsoleSystem::Instance().SetVisibility( false );
				break;
			}
			break;
		}
	}
	else
	{
		switch( message )
		{
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			if( wParam == 0xC0 )
			{
				Solidus::ConsoleSystem::Instance().SetVisibility( true );
			}
			else
			{
				onKeyboardDownEvent( static_cast< const unsigned char> ( wParam ));
			}			
			break;
		}
	}
}

void ToggleFullScreen()
{
	static DWORD savedExStyle;
	static DWORD savedStyle;
	static RECT rcSaved;

	g_isFullScreen = !g_isFullScreen;

	if (g_isFullScreen)
	{
		// Moving to full screen mode.

		savedExStyle = GetWindowLong(g_hWnd, GWL_EXSTYLE);
		savedStyle = GetWindowLong(g_hWnd, GWL_STYLE);
		GetWindowRect(g_hWnd, &rcSaved);

		SetWindowLong(g_hWnd, GWL_EXSTYLE, 0);
		SetWindowLong(g_hWnd, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

		g_windowWidth = GetSystemMetrics(SM_CXSCREEN);
		g_windowHeight = GetSystemMetrics(SM_CYSCREEN);

		SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0,
			g_windowWidth, g_windowHeight, SWP_SHOWWINDOW);
	}
	else
	{
		// Moving back to windowed mode.

		SetWindowLong(g_hWnd, GWL_EXSTYLE, savedExStyle);
		SetWindowLong(g_hWnd, GWL_STYLE, savedStyle);
		SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

		g_windowWidth = rcSaved.right - rcSaved.left;
		g_windowHeight = rcSaved.bottom - rcSaved.top;

		SetWindowPos(g_hWnd, HWND_NOTOPMOST, rcSaved.left, rcSaved.top,
			g_windowWidth, g_windowHeight, SWP_SHOWWINDOW);
	}
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
