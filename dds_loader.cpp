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

#include "dds_loader.h"

#ifndef max
static int
max (int a, int b)
{
  return ((a > b) ? a : b);
}
#endif

static struct gl_texture_t *
ReadDDSFile (const char *filename)
{
  struct DDSurfaceDesc ddsd;
  struct gl_texture_t *texinfo;
  FILE *fp;
  char magic[4];
  int mipmapFactor;
  long bufferSize, curr, end;

  /* Open the file */
  fp = fopen (filename, "rb");
  if (!fp)
    {
      fprintf (stderr, "error: couldn't open \"%s\"!\n", filename);
      return NULL;
    }

  /* Read magic number and check if valid .dds file */
  fread (&magic, sizeof (char), 4, fp);

  if (strncmp (magic, "DDS ", 4) != 0)
    {
      fprintf (stderr, "the file \"%s\" doesn't appear to be"
	       "a valid .dds file!\n", filename);
      fclose (fp);
      return NULL;
    }

  /* Get the surface descriptor */
  fread (&ddsd, sizeof (ddsd), 1, fp);

  texinfo = (struct gl_texture_t *)
    calloc (sizeof (struct gl_texture_t), 1);
  texinfo->width = ddsd.width;
  texinfo->height = ddsd.height;
  texinfo->numMipmaps = ddsd.mipMapLevels;

  switch (ddsd.format.fourCC)
    {
    case FOURCC_DXT1:
      /* DXT1's compression ratio is 8:1 */
      texinfo->format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
      texinfo->internalFormat = 3;
      mipmapFactor = 2;
      break;

    case FOURCC_DXT3:
      /* DXT3's compression ratio is 4:1 */
      texinfo->format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
      texinfo->internalFormat = 4;
      mipmapFactor = 4;
      break;

    case FOURCC_DXT5:
      /* DXT5's compression ratio is 4:1 */
      texinfo->format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
      texinfo->internalFormat = 4;
      mipmapFactor = 4;
      break;

    default:
      /* Bad fourCC, unsupported or bad format */
      fprintf (stderr, "the file \"%s\" doesn't appear to be"
	       "compressed using DXT1, DXT3, or DXT5! [%i]\n",
	       filename, ddsd.format.fourCC);
      free (texinfo);
      fclose (fp);
      return NULL;
    }

  /* Calculate pixel data size */
  curr = ftell (fp);
  fseek (fp, 0, SEEK_END);
  end = ftell (fp);
  fseek (fp, curr, SEEK_SET);
  bufferSize = end - curr;

  /* Read pixel data with mipmaps */
  texinfo->texels = (GLubyte *)malloc (bufferSize * sizeof (GLubyte));
  fread (texinfo->texels, sizeof (GLubyte), bufferSize, fp);

  /* Close the file */
  fclose (fp);
  return texinfo;
}

GLuint
loadDDSTexture (const char *filename)
{
  struct gl_texture_t *compressed_texture = NULL;
  GLsizei mipWidth, mipHeight, mipSize;
  int blockSize, offset;
  GLuint tex_id = 0;
  GLint mip;

  /* Read texture from file */
  compressed_texture = ReadDDSFile (filename);

  if (compressed_texture && compressed_texture->texels)
    {
      /* Generate new texture */
      glGenTextures (1, &compressed_texture->id);
      glBindTexture (GL_TEXTURE_2D, compressed_texture->id);

      /* Setup some parameters for texture filters and mipmapping */
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      mipWidth = compressed_texture->width;
      mipHeight = compressed_texture->height;
      blockSize = (compressed_texture->format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
      offset = 0;

      /* Upload mipmaps to video memory */
      for (mip = 0; mip < compressed_texture->numMipmaps; ++mip)
	{
	  mipSize = ((mipWidth + 3) / 4) * ((mipHeight + 3) / 4) * blockSize;

	  glCompressedTexImage2D (GL_TEXTURE_2D, mip, compressed_texture->format,
				  mipWidth, mipHeight, 0, mipSize,
				  compressed_texture->texels + offset);

	  mipWidth = max (mipWidth >> 1, 1);
	  mipHeight = max (mipHeight >> 1, 1);

	  offset += mipSize;
	}

      tex_id = compressed_texture->id;

      /* Opengl has its own copy of pixels */
      free (compressed_texture->texels);
      free (compressed_texture);
    }

  return tex_id;
}


