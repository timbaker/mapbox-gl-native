#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#ifndef NOMINMAX
#define NOMINMAX // min() and max() macros conflict with std::min() and std::max()
#endif
#include <windows.h>

#if MBGL_USE_GLES2
    #define GL_GLEXT_PROTOTYPES
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#else
    #include <glad/glad.h>
	#define GL_BLEND_COLOR 0x8005

//    #define GL_GLEXT_PROTOTYPES
//    #include <GL/gl.h>
//    #include "glext.h"
#endif
