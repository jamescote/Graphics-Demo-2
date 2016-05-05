// ==========================================================================
// Image File Reading Support Functions
//  - requires the Magick++ development libraries: http://www.imagemagick.org
//  - or the FreeImage library: http://freeimage.sourceforge.net
//
// You may use this code (or not) however you see fit for your work.
//
// Set the #defines below to choose the image library you have installed on
// your system, that you will be using for your assignment. Then compile and
// link this source file with your project.
//
// Authors: Sonny Chan, Alex Brown
//          University of Calgary
// Date:    January-February 2016
// ==========================================================================

#include <string>
#include <iostream>

#include "EnvSpec.h"

#ifdef USING_LINUX
#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#endif

#ifdef USING_WINDOWS
#include <GLAD/glad.h>
#endif

#include <GLFW/glfw3.h>

#include "ImageReader.h"

using namespace std;

// --------------------------------------------------------------------------
// ImageMagick implementation of the InitializeTexture() function
#ifdef USING_LINUX

#include <Magick++.h>

bool InitializeTexture(MyTexture *texture, const string &imageFileName)
{
    Magick::Image myImage;
    
    // try to read the provided image file
    try {
        myImage.read(imageFileName);
    }
    catch (Magick::Error &error) {
        cout << "Magick++ failed to read image " << imageFileName << endl;
        cout << "ERROR: " << error.what() << endl;
        return false;
    }
    
    // store the image width and height into the texture structure
    texture->width = myImage.columns();
    texture->height = myImage.rows();

    // create a Magick++ pixel cache from the image for direct access to data
    Magick::Pixels pixelCache(myImage);
    Magick::PixelPacket *pixels;
    pixels = pixelCache.get(0, 0, texture->width, texture->height);
    
    // determine the number of stored bytes per pixel channel in the cache
    GLenum channelDataType;
    switch (sizeof(Magick::Quantum)) {
        case 4:     channelDataType = GL_UNSIGNED_INT;      break;
        case 2:     channelDataType = GL_UNSIGNED_SHORT;    break;
        default:    channelDataType = GL_UNSIGNED_BYTE;
    }
    
    // create a texture name to associate our image data with
    if (!texture->textureName)
        glGenTextures(1, &texture->textureName);
    
    // bind the texture as a "rectangle" to access using image pixel coordinates
    glBindTexture(GL_TEXTURE_2D, texture->textureName);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // send image pixel data to OpenGL texture memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height,
                 0, GL_BGRA, channelDataType, pixels);
    
    // unbind this texture
    glBindTexture(GL_TEXTURE_RECTANGLE, 0);
    
    return true;
}

#endif
// --------------------------------------------------------------------------
// FreeImage implementation of the InitializeTexture() function
#ifdef USING_WINDOWS

#include "FreeImage.h"

bool InitializeTexture(MyTexture *mytex, const string &imageFileName)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib = nullptr;
	BYTE* bits = nullptr;

	fif = FreeImage_GetFileType(imageFileName.c_str(), 0);
	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(imageFileName.c_str());
	if (fif == FIF_UNKNOWN)
		return false;

	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, imageFileName.c_str());
	if (!dib)
		return false;

	bits = FreeImage_GetBits(dib);
	mytex->width = FreeImage_GetWidth(dib);
	mytex->height = FreeImage_GetHeight(dib);
	if ((bits == 0) || (mytex->width == 0) || (mytex->height == 0))
		return false;

	glGenTextures(1, &mytex->textureName);
	glBindTexture(GL_TEXTURE_2D, mytex->textureName);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mytex->width, mytex->height,
		0, GL_BGR, GL_UNSIGNED_BYTE, bits);

	FreeImage_Unload(dib);

	return true;
}

#endif
// --------------------------------------------------------------------------
