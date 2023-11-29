#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_main.h>
#include <IL/il.h>
#include <GL/gl.h>

// Load png files and return (texture OpenglId)

GLuint loadpng_texture(char *filename) {
    int success;
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    ILuint texid; /* ILuint is a 32bit unsigned integer. Variable texid will be used to store image name. */
//    GLuint tex;
    GLuint image;
    ilGenImages(1, &texid);						/* Generation of one image name */
    ilBindImage(texid);							/* Binding of image name */
    success = ilLoadImage(filename);					/* Loading of image ".png" */
    if (success) {
        success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); 
        /* Convert every colour component into unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
        if (!(success)) {
         printf("loadpng file error %s\n",filename);
         return(0);
        }
    }
    glGenTextures(1, &image);						/* Texture name generation */
    glBindTexture(GL_TEXTURE_2D, image);				/* Binding of texture name */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),ilGetInteger(IL_IMAGE_HEIGHT), 0, 
                 ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData()); /* Texture specification */
    ilDeleteImages(1, &texid); /* Because we have already copied image data into texture data we can release memory used by image. */
    return(image);
}

GLuint save_texture_dds(ILuint image,char *filename) {
    char temp[1024];
    strcpy(temp,filename);
    strcat(temp,".dds");
    ilSave(IL_DDS,temp);
    return {};
}

