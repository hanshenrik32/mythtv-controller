/*
 * dds.c -- dds texture loader
 * last modification: aug. 14, 2007
 *
 * Copyright (c) 2005-2007 David HENRY
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * gcc -Wall -ansi -lGL -lGLU -lglut dds.c -o dds
 */

#ifndef _WIN32
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif /* GL_GLEXT_PROTOTYPES */
#endif /* _WIN32 */

#include <GL/glut.h>
#include <GL/glext.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2D = NULL;
#endif /* _WIN32 */


/* OpenGL texture info */
struct gl_texture_t
{
  GLsizei width;
  GLsizei height;

  GLenum format;
  GLint internalFormat;
  GLuint id;

  GLubyte *texels;

  GLint numMipmaps;
};

/* DirectDraw's structures */
struct DDPixelFormat
{
  GLuint size;
  GLuint flags;
  GLuint fourCC;
  GLuint bpp;
  GLuint redMask;
  GLuint greenMask;
  GLuint blueMask;
  GLuint alphaMask;
};

struct DDSCaps
{
  GLuint caps;
  GLuint caps2;
  GLuint caps3;
  GLuint caps4;
};

struct DDColorKey
{
  GLuint lowVal;
  GLuint highVal;
};

struct DDSurfaceDesc
{
  GLuint size;
  GLuint flags;
  GLuint height;
  GLuint width;
  GLuint pitch;
  GLuint depth;
  GLuint mipMapLevels;
  GLuint alphaBitDepth;
  GLuint reserved;
  GLuint surface;

  struct DDColorKey ckDestOverlay;
  struct DDColorKey ckDestBlt;
  struct DDColorKey ckSrcOverlay;
  struct DDColorKey ckSrcBlt;

  struct DDPixelFormat format;
  struct DDSCaps caps;

  GLuint textureStage;
};

#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
  (GLuint)( \
    (((GLuint)(GLubyte)(ch3) << 24) & 0xFF000000) | \
    (((GLuint)(GLubyte)(ch2) << 16) & 0x00FF0000) | \
    (((GLuint)(GLubyte)(ch1) <<  8) & 0x0000FF00) | \
     ((GLuint)(GLubyte)(ch0)        & 0x000000FF) )

#define FOURCC_DXT1 MAKEFOURCC('D', 'X', 'T', '1')
#define FOURCC_DXT3 MAKEFOURCC('D', 'X', 'T', '3')
#define FOURCC_DXT5 MAKEFOURCC('D', 'X', 'T', '5')

GLuint loadDDSTexture (const char *filename);
