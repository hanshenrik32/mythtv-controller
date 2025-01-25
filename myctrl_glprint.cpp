#ifndef MYCTRL_GLPRINT
#define MYCTRL_GLPRINT

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
#include <iostream>
#include <sstream>
#include <fmt/format.h>

#include "myctrl_glprint.h"

// #define MAX_CHARS 128
#define MAX_CHARS 255
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
            GL_LUMINANCE, // Use luminance instead of red
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_LUMINANCE, // Input format as luminance
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
// Funktion to draw text in screen.
//
// ****************************************************************************************
void drawText(const char *text, float x, float y, float scale,int color) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    // Set text color to white
    switch(color) {
        case 0: glColor4f(0.8f, 0.8f, 0.8f, 1.0f);          // gray
                glColor3f(0.8f, 0.8f, 0.8f);
                break;
        case 1: glColor4f(0.8f, 0.8f, 0.8f, 1.0f);           // gray
                glColor3f(0.8f, 0.8f, 0.8f);
                break;
        case 2: glColor4f(0.8f, 1.0f, 0.4f, 1.0f);          // red
                glColor3f(0.8f, 1.0f, 0.4f);
                break;
        case 3: glColor4f(1.0f, 0.8f, 0.4f, 1.0f);
                glColor3f(1.0f, 0.8f, 0.4f);
                break;
        case 4: glColor4f(1.0f, 0.0f, 0.0f, 1.0f);           // red
                glColor3f(1.0f, 0.0f, 0.0f);
                break;
        case 5: glColor4f(0.5f, 0.5f, 0.5f, 1.0f);           // gray 1
                glColor3f(0.5f, 0.5f, 0.5f);
                break;
        case 15:glColor4f(1.0f, 1.0f, 1.0f, 1.0f);          // white
                glColor3f(1.0f, 1.0f, 1.0f);
                break;
        default:glColor4f(0.8f, 0.8f, 0.8f, 1.0f);
                glColor3f(0.8f, 0.8f, 0.8f);

    };
    // Ensure texture environment mode is set to replace
    // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
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
    // glDisable(GL_BLEND);
    glEnable(GL_BLEND);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}


// ****************************************************************************************
//
// Funktion to draw lines of text in screen.
//
// ****************************************************************************************
void drawLinesOfText(const std::string& text, float x, float y, float scale,int maxWidth,int maxlines,int color) {
    std::istringstream stream(text);
    std::string word;
    std::string currentLine;
    std::string formattext;
    float yoffset=0.0f;
    int linecount=0;
    while (stream >> word) {
        // Check if adding the word exceeds the maximum width then print the line
        if (currentLine.length() + word.length() + 1 > maxWidth) {
            if (currentLine.length()>0) {
                formattext = fmt::v8::format("{:^24}",currentLine);
                drawText(formattext.c_str(), x, y + yoffset, scale, color);
                currentLine = word; // Start a new line with the current word
                linecount++;
                yoffset-=20.0f;
            } else {
                currentLine = word; // Start a new line with the current word
            }
        } else {
            if (!currentLine.empty()) {
                currentLine += " "; // Add a space before the next word
            }
            currentLine += word; // Add the word to the current line
        }
        if (linecount>maxlines) break;
    }
    // Print any remaining text in the current line
    if (linecount<=maxlines) {
        if (!currentLine.empty()) {
            if (currentLine.length()>maxWidth) currentLine.resize(maxWidth);
            formattext = fmt::v8::format("{:^24}",currentLine);
            drawText(formattext.c_str(), x, y + yoffset, scale, color);
        }
    }
}

#endif