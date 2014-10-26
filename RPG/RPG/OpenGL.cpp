#include "stdafx.h"
#include "OpenGL.h"

HGLRC InitializeOpenGL( HDC hdcWindow )
{
	assert( hdcWindow );

	// Set the window pixel format
	//
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor = {0};

	pixelFormatDescriptor.nSize = sizeof( pixelFormatDescriptor );
	pixelFormatDescriptor.nVersion = 1;

	pixelFormatDescriptor.dwFlags = 
		PFD_DRAW_TO_WINDOW | 
		PFD_SUPPORT_OPENGL | 
		PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.dwLayerMask = PFD_MAIN_PLANE;
	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits = 32;
	pixelFormatDescriptor.cDepthBits = 0;	// No depth buffer for now.

	int pixelFormat = ChoosePixelFormat( hdcWindow, &pixelFormatDescriptor );
	assert( pixelFormat != 0 );
	SetPixelFormat( hdcWindow, pixelFormat, &pixelFormatDescriptor );

	// Create the OpenGL render context
	//
	HGLRC renderingContext = wglCreateContext( hdcWindow );
	wglMakeCurrent( hdcWindow, renderingContext );

	return renderingContext;
}

void ShutdownOpenGL( HGLRC& renderingContext )
{
	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( renderingContext );
	renderingContext = 0;
}
