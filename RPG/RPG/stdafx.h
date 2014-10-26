// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOMINMAX

// Windows Header Files:
#include <windows.h>

//
#pragma  comment (lib, "rpcrt4.lib")

// C RunTime Header Files
#include <tchar.h>
#include <cassert>
#include <memory>

// OPENGL
#include <gl/glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#pragma  comment (lib, "glew32.lib")
#pragma  comment (lib, "opengl32.lib")
#pragma  comment (lib, "glu32.lib")

// Profiling
//
#include "Profiler.h"