//
// opengl fast print
//
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <string>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "myctrl_glprint.h"

#define MAX_CHARS 128
Character characters[MAX_CHARS];

// FreeType skrifttypehåndtering
FT_Library ft;
FT_Face face;

void flipBitmap(unsigned char *buffer, int width, int height) {
    for (int row = 0; row < height / 2; row++) {
        for (int col = 0; col < width; col++) {
            // Byt pixeldata mellem øverste og nederste række
            unsigned char temp = buffer[row * width + col];
            buffer[row * width + col] = buffer[(height - row - 1) * width + col];
            buffer[(height - row - 1) * width + col] = temp;
        }
    }
}


// ****************************************************************************************
//
// opengl text print (fast)
// 
// Initialisering af FreeType og indlæsning af skrifttype
//
// ****************************************************************************************

int initFreeType(const char *fontPath) {
    if (FT_Init_FreeType(&ft)) {
        fprintf(stderr, "Kunne ikke initialisere FreeType.\n");
        return 0;
    }

    if (FT_New_Face(ft, fontPath, 0, &face)) {
        fprintf(stderr, "Kunne ikke indlæse skrifttype: %s\n", fontPath);
        return 0;
    }

    FT_Set_Pixel_Sizes(face, 0, 48); // Sæt skriftstørrelse

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Deaktiver bytejustering

    // Indlæs alle ASCII-tegn
    for (unsigned char c = 0; c < MAX_CHARS; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            fprintf(stderr, "Kunne ikke indlæse tegn '%c'.\n", c);
            continue;
        }
        // ****************************************************************************
        // flip font for my x y z setup. to get it working, else font is up side down.
        //
        flipBitmap(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows);
        //
        // ****************************************************************************


        // Opret tekstur for hvert tegn
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_R8, // Internal format (3 color channels)
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED, // Input format (3 color channels)
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // Teksturopsætning
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Gem tegnets data
        characters[c].texture = texture;
        characters[c].width = face->glyph->bitmap.width;
        characters[c].height = face->glyph->bitmap.rows;
        characters[c].bearingX = face->glyph->bitmap_left;
        characters[c].bearingY = face->glyph->bitmap_top;
        characters[c].advance = face->glyph->advance.x >> 6;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return 1;
}

// ****************************************************************************************
//
// Funktion til at tegne tekst
//
// ****************************************************************************************
void drawText(const char *text, float x, float y, float scale) {
    glEnable(GL_BLEND);
    // default disabled
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // my stuf
    // glBlendFunc(GL_ONE, GL_ONE);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);

    // Tegn hvert tegn
    for (size_t i = 0; i < strlen(text); i++) {
        char c = text[i];
        Character ch = characters[c];

        float xpos = x + ch.bearingX * scale;
        float ypos = y - (ch.height - ch.bearingY) * scale;

        float w = ch.width * scale;
        float h = ch.height * scale;

        // Tegn et kvadrat for teksturen
        glBindTexture(GL_TEXTURE_2D, ch.texture);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex2f(xpos, ypos);
        glTexCoord2f(1.0, 0.0); glVertex2f(xpos + w, ypos);
        glTexCoord2f(1.0, 1.0); glVertex2f(xpos + w, ypos + h);
        glTexCoord2f(0.0, 1.0); glVertex2f(xpos, ypos + h);
        glEnd();

        x += (ch.advance) * scale; // Flyt til næste tegn
    }

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}


