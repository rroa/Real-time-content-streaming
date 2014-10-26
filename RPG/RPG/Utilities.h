#pragma once

#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <string>
#include <sstream>
#include <iostream>

#include "BitmapFont.h"

template <class T>
bool from_string(T& t, 
	const std::string& s, 
	std::ios_base& (*f)(std::ios_base&))
{
	std::istringstream iss(s);
	return !(iss >> f >> t).fail();
}

template <class T>
std::shared_ptr< T > WeightedRandom ( std::map<int, std::shared_ptr< T >> items )  
{   
	double random_weight;  
	double sum_of_weight = 0;


	/* Calculate the sum of weights */ 
	for ( auto i = items.begin(); i != items.end(); ++i ) {
		sum_of_weight += i->first;
	}

	/* Choose a random number in the range [0,1) */	
	double g = rand() / ( (double) RAND_MAX + 1.0 );
	random_weight = g * sum_of_weight;

	/* Find a random number wrt its weight */
	int temp_total = 0;

	for ( auto i = items.begin(); i != items.end(); ++i )
	{
		temp_total += i->first;

		if (random_weight < temp_total)
		{
			return i->second;
		} 
	}   
	return nullptr; /* Oops, we could not find a random number */
}


// SHADERS
//
GLuint LoadShaders( const char * shaderName );
GLuint LoadShaders( const std::string& shaderName );

// FONTS
//
bool		 LoadFontXML( const char* pszFilename, const char* pszTextureFileName, Solidus::BitmapFont& bitmapFont );
bool		 LoadFontXML( const std::string& pszFilename, const char* pszTextureFileName, Solidus::BitmapFont& bitmapFont );
const float  CalcStringPixelWidth( Solidus::BitmapFont& bitmapFont, const char* text );
const float  CalcStringPixelWidth( Solidus::BitmapFont& bitmapFont, const std::string& text );
//bool   LoadFontMono( const char * pszFilename, float cellSpace = 16.0f );
//bool   LoadFontMono( const std::string& pszFilename, float cellSpace = 16.0f );

// COLORS
//
unsigned int ToRGBA( float r, float g, float b, float a = 1.0f );
void		 FromRGBA( float& r, float& g, float& b, float& a, unsigned int rgba );

namespace Drawing
{
	void ApplyTransformations( float x = 0.0f, float y = 0.0f, float angle = 0.0f, float scaleX = 0.0f, float scaleY = 0.0f );
}

// UUID
std::string NewUUID();

#endif