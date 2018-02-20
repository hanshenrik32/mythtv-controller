#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jpeglib.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL.h>
#include <jpeglib.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>


// load jpg png tga


int loadTexture ( char *filename ) {
  GLuint retval;
  SDL_Surface *sdlimage;
  void *raw;
  int w, h, i, j, bpp;
  Uint8 *srcPixel, *dstPixel;
  Uint32 truePixel;
  GLenum errorCode;

//printf("Filename %s \n ",filename);

  sdlimage = IMG_Load(filename);

  if ( !sdlimage ) {
//    printf("SDL_Image load error: %s on %s\n", IMG_GetError(),filename);
    return 0;
  }
  if ( sdlimage->format->BytesPerPixel < 2 ) {
    printf("Bad image -- not true color! %s\n",filename);
    SDL_FreeSurface (sdlimage);
    return 0;
  }

  w = sdlimage->w;
  h = sdlimage->h;

  raw = (void *)malloc( w * h * 4 );
  dstPixel = (Uint8 *)raw;

  SDL_LockSurface( sdlimage );

  bpp = sdlimage->format->BytesPerPixel;

  for ( i = h-1 ; i >= 0 ; i-- ) {
    for ( j = 0 ; j < w ; j++ ) {
      srcPixel = (Uint8 *)sdlimage->pixels + i * sdlimage->pitch + j * bpp;
      switch (bpp) {
        case 1:
          truePixel = *srcPixel;
          break;

        case 2:
          truePixel = *(Uint16 *)srcPixel;
          break;

        case 3:
          if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            truePixel = srcPixel[0] << 16 | srcPixel[1] << 8 | srcPixel[2];
          } else {
            truePixel = srcPixel[0] | srcPixel[1] << 8 | srcPixel[2] << 16;
          }
          break;

        case 4:
          truePixel = *(Uint32 *)srcPixel;
          break;

        default:
          printf("Image bpp of %d unusable\n", bpp);
          SDL_UnlockSurface (sdlimage);
          SDL_FreeSurface (sdlimage);
          free (raw);
          return 0;
      }

      SDL_GetRGBA( truePixel, sdlimage->format, &(dstPixel[0]), &(dstPixel[1]), &(dstPixel[2]), &(dstPixel[3]));
      dstPixel++;
      dstPixel++;
      dstPixel++;
      dstPixel++;
    }
  }

  SDL_UnlockSurface( sdlimage );
  SDL_FreeSurface( sdlimage );

  while ( glGetError() ) { ; }

  glGenTextures( 1, &retval );
  glBindTexture( GL_TEXTURE_2D, retval );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  errorCode = glGetError();
  if ( errorCode != 0 ) {
    if ( errorCode == GL_OUT_OF_MEMORY ) {
      printf("Out of texture memory!\n");
    }

    glDeleteTextures (1, &retval);
    free (raw);
    return 0;
  }

  gluBuild2DMipmaps( GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, (Uint8 *)raw);

  errorCode = glGetError();
  if ( errorCode != 0 ) {
    if ( errorCode == GL_OUT_OF_MEMORY ) {
      printf("Out of texture memory!\n");
    }

    glDeleteTextures (1, &retval);
    free (raw);
    return 0;
  }
  return retval;
}

