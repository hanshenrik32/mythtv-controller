#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <vector>
#include "shader.h"

#include <ft2build.h>
#include FT_FREETYPE_H



struct Character {
  GLuint TextureID;
  unsigned char *bitmap;
  int width;
  int height;
  int bearingX;
  int bearingY;
  int advance;
};

class Font {
  public:
    bool Load(const char *filename,int size);
    Character glyph[256];
};

struct ClickableItem {
    int id;
    float x;
    float y;
    float w;
    float h;
};

struct TextureVertex {
    float x;
    float y;
    float u;
    float v;
    float r;
    float g;
    float b;
    float a;
};


struct Vertex {
    float x;
    float y;
    float u;
    float v;
    float r;
    float g;
    float b;
    float a;
  };

struct Batch {
    GLuint texture;
    std::vector<Vertex> vertices;
};

struct Point3D {
    float x;
    float y;
    float z;
};


class Renderer {
  public:
    bool Init(int width, int height);
    void Begin();
    void Flush();
    void AddClockLine(float start,float end,float angle,float r,float g,float b,float a);
    void AddRect(float x,float y,float w,float h,float r,float g,float b,float a);
    void AddTextureRect(int id,GLuint texture,float x,float y,float w,float h,float r,float g,float b,float a);
    void AddVideoTextureRect(int id,GLuint texture,float x,float y,float w,float h,float r,float g,float b,float a);
    void AddText(Font *font,float x,float y,const std::string &text,float r,float g,float b,float a);
    void AddTriangle(float x1,float y1,float x2,float y2,float x3,float y3,float r,float g,float b,float a);
    void DrawHand(float cx,float cy,float length,float angle,float width,float r,float g,float b,float a);
    void AddThickLine(float x1,float y1,float x2,float y2,float width, float r,float g,float b,float a);
    void AddPoint(float x, float y,float r,float g,float b,float a);
    // void AddFilledLine(float x1,float y1, float x2,float y2, float width, float r,float g,float b,float a);
    void AddQuad(const Point3D& p1, const Point3D& p2, const Point3D& p3, const Point3D& p4,float r,float g,float b,float a);
    void AddRoundedRect(float x, float y,float width, float height,float radius,float r, float g, float b, float a);
    void AddRoundedTextureRect(int id, GLuint texture,float x, float y, float w, float h,float radius,float r, float g, float b, float a);
    void DrawRawLines(const std::vector<TextureVertex>& lineVertices,GLenum primitive,float lineWidth,GLenum srcBlend,GLenum dstBlend);
    float GetTextWidth(Font* font, const std::string& text);

    void End();
    // void End2();
    int HitTest(float mx, float my);
  private:
    GLenum currentPrimitive = GL_TRIANGLES;
    bool currentText = false;
    GLint textModeLocation;
    GLuint vao;
    GLuint vbo;
    GLuint currentTexture;
    Shader shader;
    int screenWidth;
    int screenHeight;    
    std::vector<Vertex> vertices;
    std::vector<Batch> batches;
    std::vector<ClickableItem> clickableItems;
    std::vector<Vertex> lineVertices;
};


#endif
