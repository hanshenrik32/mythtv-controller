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


extern mFont font12;  // 12px font
extern mFont font18;  // 18px font
extern mFont font24;  // 24px font

// #define MAX_CHARS 128
#define MAX_CHARS 255
Character characters[MAX_CHARS];

// FreeType skrifttypehåndtering
FT_Library ft;
FT_Face face;


// *****************************************************************************************

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
// Load font og opret teksturer for hvert tegn
// Returnere en mFont struktur med alle tegn og deres teksturer
//
// ****************************************************************************************

mFont loadFont(FT_Face face, int pixelSize) {
  mFont font;
  font.size = pixelSize;
  FT_Set_Pixel_Sizes(face, 0, pixelSize);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  for (unsigned int c = 0; c < 128; c++) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      continue;
    }
    flipBitmap(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_LUMINANCE,
      face->glyph->bitmap.width,
      face->glyph->bitmap.rows,
      0,
      GL_LUMINANCE,
      GL_UNSIGNED_BYTE,
      face->glyph->bitmap.buffer
    );
    // 🔥 vigtigt for pæn tekst
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    Character character_1 = {
      texture,
      face->glyph->bitmap.width,
      face->glyph->bitmap.rows,
      face->glyph->bitmap_left,
      face->glyph->bitmap_top,
      (unsigned int)face->glyph->advance.x
    };
    font.characters.insert(std::pair<unsigned int, Character>(c, character_1));
  }
  return font;
}

// ****************************************************************************************
//
// Funktion to draw text in screen.
//
// ****************************************************************************************/*

void drawText(mFont& font, const char* text, float x, float y, float scale, int color) {
  for (size_t i = 0; i < strlen(text); i++) {
    unsigned char c = (unsigned char)text[i];
    if (font.characters.find(c) == font.characters.end())
     continue;
    Character ch = font.characters[c];
    float xpos = floor(x + ch.bearingX * scale);
    float ypos = floor(y - (ch.height - ch.bearingY) * scale);
    float w = ch.width * scale;
    float h = ch.height * scale;
    glBindTexture(GL_TEXTURE_2D, ch.texture);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(xpos, ypos);
    glTexCoord2f(1, 0); glVertex2f(xpos + w, ypos);
    glTexCoord2f(1, 1); glVertex2f(xpos + w, ypos + h);
    glTexCoord2f(0, 1); glVertex2f(xpos, ypos + h);
    glEnd();
    x += (ch.advance >> 6) * scale;
  }
  glEnable(GL_BLEND);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
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

    FT_Set_Pixel_Sizes(face, 0, 16); // Sæt skriftstørrelse default FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Deaktiver bytejustering

    // Indlæs alle ASCII-tegn
    for (unsigned char c = 0; c < MAX_CHARS; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER | FT_LOAD_NO_HINTING)) {
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

    font12 = loadFont(face, 16);
    font18 = loadFont(face, 18);
    font24 = loadFont(face, 64);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
  

    return 1;
}



// ****************************************************************************************
//
// Funktion to draw lines of text in screen.
//
// ****************************************************************************************
void drawLinesOfText(const std::string& text, float x, float y, float scale,int maxWidth,int maxlines,int color,bool center) {
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
        if (center) formattext = fmt::format("{:^{}s}",currentLine,maxWidth);
        else formattext = fmt::format("{}",currentLine);
        drawText(font12,formattext.c_str(), x, y + yoffset, scale, color);
        currentLine = word; // Start a new line with the current word
        linecount++;
        yoffset-=18.0f;
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
      if (center) formattext = fmt::format("{:^{}s}",currentLine,maxWidth);
      else formattext = fmt::format("^{}",currentLine,maxWidth);
      drawText(font12,formattext.c_str(), x, y + yoffset, scale, color);
    }
  }
}

#endif