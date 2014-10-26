#pragma once

#ifndef _OPENGGL_H_
#define _OPENGGL_H_

HGLRC InitializeOpenGL( HDC hdcWindow );
void  ShutdownOpenGL( HGLRC& renderingContext );

#endif