#include <math.h>
#include <ft2build.h>
#include <algorithm>
#include <cmath>
#include "renderer.h"


// font
FT_Face face;


bool Font::Load(const char *filename, int pixelsize) {
    FT_Library ft;
    if(FT_Init_FreeType(&ft))
        return false;
    if(FT_New_Face(ft, filename, 0, &face))
        return false;
    FT_Set_Pixel_Sizes(face,0,pixelsize);
    for(unsigned char c=0;c<240;c++) {
        if(FT_Load_Char(face,c,FT_LOAD_RENDER))
            continue;
        FT_GlyphSlot slot = face->glyph;
        Character &ch = glyph[c];
        ch.width = slot->bitmap.width;
        ch.height = slot->bitmap.rows;
        ch.bearingX = slot->bitmap_left;
        ch.bearingY = slot->bitmap_top;
        ch.advance = slot->advance.x;
        int size = ch.width * ch.height;
        ch.bitmap = new unsigned char[size];
        memcpy(ch.bitmap,
               slot->bitmap.buffer,
               size);
        // Opret OpenGL texture for dette tegn
        glGenTextures(1, &ch.TextureID);
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            ch.width,
            ch.height,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            ch.bitmap
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    return true;
}


// Lav projection matrix texture

bool Renderer::Init(int width, int height) {
    screenWidth  = width;
    screenHeight = height;
    const char* vertexShader = R"(
    #version 330 core
    layout(location=0) in vec2 position;
    layout(location=1) in vec2 texcoord;
    layout(location=2) in vec4 color;
    uniform mat4 projection;
    out vec2 TexCoord;
    out vec4 Color;
    void main() {
        gl_Position =
            projection *
            vec4(position,0,1);
        gl_PointSize = 2.0;      // <-- used for draw dot
        TexCoord = texcoord;
        Color = color;
    }
    )";
    const char* fragmentShader = R"(
    #version 330 core
    in vec2 TexCoord;
    in vec4 Color;
    uniform sampler2D texture0;
    uniform bool textMode;
    uniform bool useTexture;
    out vec4 FragColor;
    void main() {
        vec4 tex = texture(texture0, TexCoord);
        if(textMode)
        {
            FragColor = vec4(Color.rgb, tex.r * Color.a);
        }
        else if(useTexture)
        {
            FragColor = tex * Color;
        }
        else
        {
            FragColor = Color;
        }
    }
    )";
    //
    // Lav shader program
    //
    shader.Load(vertexShader,fragmentShader);
    //
    // VAO
    //
    glGenVertexArrays(1,&vao);
    //
    // VBO
    //
    glGenBuffers(1,&vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    //
    // plads til vertices
    //
    glBufferData(GL_ARRAY_BUFFER,sizeof(Vertex) * 60000,nullptr,GL_DYNAMIC_DRAW);

    //
    // position x,y
    //
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)0);
    glEnableVertexAttribArray(0);
    //

    // texcoord
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    // color r,g,b,a
    //
    glVertexAttribPointer(2,4,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(4*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(2);
    //
    // Alpha blending
    //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);            // Normal
    glEnable(GL_PROGRAM_POINT_SIZE);
    //
    // Projection matrix
    //
    float projection[16];
    for(int i=0;i<16;i++) projection[i]=0;
    //
    // OpenGL matrix:
    //
    // x: 0 -> width
    // y: 0 -> height
    //
    projection[0] = 2.0f / width;
    projection[5] = -2.0f / height;
    projection[10] = -1.0f;
    projection[12] = -1.0f;
    projection[13] = 1.0f;
    projection[15] = 1.0f;

    //
    // send matrix til shader
    //
    shader.Use();
    GLint projectionLoc = glGetUniformLocation(shader.ID,"projection");
    glUniformMatrix4fv(projectionLoc,1,GL_FALSE,projection);
    glUseProgram(0);
    return true;
}



int Renderer::HitTest(float mx, float my) {
    for (auto it = clickableItems.rbegin(); it != clickableItems.rend(); ++it) {
        if (mx >= it->x &&
            mx <= it->x + it->w &&
            my >= it->y &&
            my <= it->y + it->h)
        {
            return it->id;
        }
    }
    return -1;
}


void Renderer::Begin() {
    vertices.clear();
    clickableItems.clear();
}


void Renderer::End() {
    Flush();
}


void MakeProjection(float width,float height,float* m) {
    for(int i=0;i<16;i++) m[i]=0;
    m[0]= 2.0f/width;
    m[5]= -2.0f/height;
    m[10]=-1.0f;
    m[12]=-1.0f;
    m[13]=1.0f;
    m[15]=1.0f;
}


// ****************************************************************************************
//
// return tekst length in pixel
//
// ****************************************************************************************


float Renderer::GetTextWidth(Font* font, const std::string& text) {
    if (!font)
        return 0.0f;
    float width = 0.0f;
    for (unsigned char c : text) {
        if (c >= std::size(font->glyph))
            continue;
        const Character& ch = font->glyph[c];
        width += static_cast<float>(ch.advance >> 6);
    }
    return width;
}



// NEW code

// draw trangle
// works

void Renderer::AddTriangle(float x1, float y1, float x2, float y2, float x3, float y3, float r, float g, float b, float a) {
    // 1. Tjek om tilstanden ændrer sig - hvis ja, flush det forrige batch
    if (currentPrimitive != GL_TRIANGLES || currentTexture != 0 || currentText) {
        Flush();
        currentPrimitive = GL_TRIANGLES;
        currentTexture = 0;
        currentText = false;
    }

    // 2. Tilføj trekanters verticer
    Vertex v;
    v.u = 0.0f;
    v.v = 0.0f;
    v.r = r;
    v.g = g;
    v.b = b;
    v.a = a;

    v.x = x1; v.y = y1;
    vertices.push_back(v);

    v.x = x2; v.y = y2;
    vertices.push_back(v);

    v.x = x3; v.y = y3;
    vertices.push_back(v);
}



// line
// virker 
// ny fra google

void Renderer::AddThickLine(float x1, float y1, float x2, float y2, float width, float r, float g, float b, float a) {
    // 1. Flush hvis enten tekstur, tekst ELLER primitive type har ændret sig
    if (currentTexture != 0 || currentText || currentPrimitive != GL_TRIANGLES) {
        Flush();
        currentText = false;
        currentTexture = 0;
        currentPrimitive = GL_TRIANGLES;
    }
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = sqrtf(dx * dx + dy * dy);
    if (len < 0.0001f)
        return;
    dx /= len;
    dy /= len;
    // Normalvektor (halv bredde)
    float nx = -dy * width * 0.5f;
    float ny =  dx * width * 0.5f;
    // Firkantens 4 hjørner:
    // A = Start Top,   B = Slut Top
    // D = Start Bund, C = Slut Bund
    float ax = x1 + nx, ay = y1 + ny; // Corner A
    float bx = x2 + nx, by = y2 + ny; // Corner B
    float cx = x2 - nx, cy = y2 - ny; // Corner C
    float dx2 = x1 - nx, dy2 = y1 - ny; // Corner D
    // 2. Tegn to trekanter med mod-uret (CCW) winding order:
    // Trekant 1: A -> D -> C
    AddTriangle(ax, ay, dx2, dy2, cx, cy, r, g, b, a);
    // Trekant 2: A -> C -> B
    AddTriangle(ax, ay, cx, cy, bx, by, r, g, b, a);
}




// addquad

void Renderer::AddQuad(const Point3D& p1, const Point3D& p2, const Point3D& p3, const Point3D& p4,float r,float g,float b,float a) {
    AddTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, r, g, b, a);
    AddTriangle(p1.x, p1.y, p3.x, p3.y, p4.x, p4.y, r, g, b, a);
}



// used by analog clock

void Renderer::DrawHand(float cx,float cy,float length,float angle,float width,float r,float g,float b,float a) {
    float PI=3.14159265359;
    float x2 = cx + cosf(angle - PI/2) * length;
    float y2 = cy + sinf(angle - PI/2) * length;
    AddThickLine(cx,cy,x2,y2,width,r,g,b,a);
}


// color rectangle
// virker ikke

void Renderer::AddRect(float x,float y,float w,float h,float r,float g,float b,float a) {
    currentPrimitive = GL_TRIANGLES;
    currentText = false;
    currentTexture=0;
    Vertex v[6]= {
    {x,y,r,g,b,a},
    {x+w,y,r,g,b,a},
    {x+w,y+h,r,g,b,a},

    {x,y,r,g,b,a},
    {x+w,y+h,r,g,b,a},
    {x,y+h,r,g,b,a}
    };
    for(int i=0;i<6;i++) vertices.push_back(v[i]);
}




// texture rectangle
// works

void Renderer::AddTextureRect(int id,GLuint texture,float x,float y,float w,float h,float r,float g,float b,float a) {
    currentText = false;
    /*
    if(!vertices.empty() && currentTexture != texture) {
        Flush();              // tegn den gamle texture først
    }
    */
    if(!vertices.empty() && (currentTexture != texture || currentText != false)) {
        Flush();
    }
    currentPrimitive = GL_TRIANGLES;
    currentTexture = texture;
    id = id;
    Vertex v[6]={
    {x,y,0,1,r,g,b,a},
    {x+w,y,1,1,r,g,b,a},
    {x+w,y+h,1,0,r,g,b,a},

    {x,y,0,1,r,g,b,a},
    {x+w,y+h,1,0,r,g,b,a},
    {x,y+h,0,0,r,g,b,a}
    };
    for(int i=0;i<6;i++) vertices.push_back(v[i]);
    clickableItems.push_back({id,x,y,w,h});
}


// Draw lines
// Works
/*

void Renderer::AddFilledLine(float x1,float y1, float x2,float y2, float width, float r,float g,float b,float a) {
    if(currentTexture != 0 || currentText) {
        Flush();
        currentText=false;
        currentTexture=0;
    }
    if(currentPrimitive != GL_TRIANGLES) {
        Flush();
        currentPrimitive = GL_TRIANGLES;
    }   
    currentText = false;
    currentTexture = 0;
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = sqrtf(dx*dx + dy*dy);
    if(len <= 0.001f)
        return;
    float nx = -dy / len;
    float ny =  dx / len;
    float half = width * 0.5f;
    float xoff = nx * half;
    float yoff = ny * half;
    Vertex v[6];
    // hjørner
    float xA = x1 + xoff;
    float yA = y1 + yoff;
    float xB = x1 - xoff;
    float yB = y1 - yoff;
    float xC = x2 - xoff;
    float yC = y2 - yoff;
    float xD = x2 + xoff;
    float yD = y2 + yoff;
    // trekant 1
    v[0] = {xA,yA,0,0,r,g,b,a};
    v[1] = {xB,yB,0,0,r,g,b,a};
    v[2] = {xC,yC,0,0,r,g,b,a};
    // trekant 2
    v[3] = {xA,yA,0,0,r,g,b,a};
    v[4] = {xC,yC,0,0,r,g,b,a};
    v[5] = {xD,yD,0,0,r,g,b,a};
    for(int i=0;i<6;i++)
        vertices.push_back(v[i]);
}

*/


// Draw text
// works

void Renderer::AddText(Font *font, float x, float y, const std::string &text, float r,float g,float b,float a) {
    float xpos = x;
    for(unsigned char c : text) {
        if (c >=128)
            continue;
        Character &ch = font->glyph[c];
        if (!vertices.empty() && (!currentText || currentTexture != ch.TextureID)) {
            Flush();
        }       
        currentPrimitive = GL_TRIANGLES;
        currentText = true;
        currentTexture = ch.TextureID;
        float xpos2 = xpos + ch.bearingX;
        float ypos2 = y - ch.bearingY;
        float w = ch.width;
        float h = ch.height;
        Vertex v[6] ={
            {xpos2,   ypos2,     0,0,r,g,b,a},
            {xpos2+w, ypos2,     1,0,r,g,b,a},
            {xpos2+w, ypos2+h,   1,1,r,g,b,a},

            {xpos2,   ypos2,     0,0,r,g,b,a},
            {xpos2+w, ypos2+h,   1,1,r,g,b,a},
            {xpos2,   ypos2+h,   0,1,r,g,b,a}
        };
        for(int i=0;i<6;i++)
            vertices.push_back(v[i]);
        // FreeType Advance er i 1/64 pixel
        xpos += (ch.advance >> 6);

    }
}


// video texture rectangle
// works

void Renderer::AddVideoTextureRect(int id,GLuint texture,float x,float y,float w,float h,float r,float g,float b,float a) {
    currentText = false;
    if(!vertices.empty() && currentTexture != texture) {
        Flush();              // tegn den gamle texture først
    }
    currentPrimitive = GL_TRIANGLES;
    currentTexture = texture;
    id = id;
    Vertex v[6]={
    {x,y,0,0,r,g,b,a},
    {x+w,y,1,0,r,g,b,a},
    {x+w,y+h,1,1,r,g,b,a},

    {x,y,0,0,r,g,b,a},
    {x+w,y+h,1,1,r,g,b,a},
    {x,y+h,0,1,r,g,b,a}
    };
    for(int i=0;i<6;i++) vertices.push_back(v[i]);
    clickableItems.push_back({id,x,y,w,h});
}


// draw points
// works

void Renderer::AddPoint(float x, float y, float r,float g,float b,float a) {
    if(!vertices.empty() && currentPrimitive != GL_POINTS)
        Flush();
    currentPrimitive = GL_POINTS;
    currentTexture = 0;
    currentText = false;
    if(vertices.size() >= 50000)
        Flush();
    vertices.push_back({
        x,
        y,
        0.0f,
        0.0f,
        r,g,b,a
    });
}

//
// works
// 


void Renderer::DrawRawLines(const std::vector<TextureVertex>& lineVertices,GLenum primitive,float lineWidth,GLenum srcBlend, GLenum dstBlend) {
    if (lineVertices.empty())
        return;
    // Tegn først eventuel eksisterende batch
    Flush();
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(srcBlend, dstBlend);
    shader.Use();
    glUniform1i(
        glGetUniformLocation(shader.ID, "textMode"),
        GL_FALSE
    );
    glUniform1i(
        glGetUniformLocation(shader.ID, "useTexture"),
        GL_FALSE
    );
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        lineVertices.size() * sizeof(TextureVertex),
        lineVertices.data()
    );
    glLineWidth(lineWidth);
    glDrawArrays(
        primitive,
        0,
        static_cast<GLsizei>(lineVertices.size())
    );
    glLineWidth(1.0f);
    glBindVertexArray(0);
}

// med texture samt id

void Renderer::AddRoundedTextureRect(int id, GLuint texture,float x, float y, float w, float h,float radius,float r, float g, float b, float a) {
    if (w <= 0.0f || h <= 0.0f)
      return;
    radius = std::clamp(radius, 0.0f, std::min(w, h) * 0.5f);
    // Tegn eksisterende vertices med deres oprindelige tilstand.
    if (!vertices.empty() && (currentTexture != texture || currentText || currentPrimitive != GL_TRIANGLES)) {
      Flush();
    }
    currentText = false;
    currentPrimitive = GL_TRIANGLES;
    currentTexture = texture;
    // Samme UV-retning som din AddTextureRect().
    auto makeVertex = [&](float px, float py) -> Vertex {
      return Vertex{
        px, py,
        (px - x) / w,
        1.0f - (py - y) / h,
        r, g, b, a
      };
    };
    if (radius == 0.0f) {
      Vertex tl = makeVertex(x,     y);
      Vertex tr = makeVertex(x + w, y);
      Vertex br = makeVertex(x + w, y + h);
      Vertex bl = makeVertex(x,     y + h);
      vertices.push_back(tl);
      vertices.push_back(tr);
      vertices.push_back(br);
      vertices.push_back(tl);
      vertices.push_back(br);
      vertices.push_back(bl);
    } else {
      constexpr float PI = 3.14159265359f;
      constexpr int segments = 16;
      constexpr int pointCount = 4 * (segments + 1);
      Vertex outline[pointCount];
      int count = 0;
      auto corner = [&](float cx, float cy, float startAngle) {
        for (int i = 0; i <= segments; ++i) {
          float angle = startAngle + (PI * 0.5f) * float(i) / float(segments);
          outline[count++] = makeVertex( cx + std::cos(angle) * radius, cy + std::sin(angle) * radius);
        }
      };
      // Konturen rundt med koordinatsystemets top til venstre.
      corner(x + radius,     y + radius,     PI);
      corner(x + w - radius, y + radius,     PI * 1.5f);
      corner(x + w - radius, y + h - radius, 0.0f);
      corner(x + radius,     y + h - radius, PI * 0.5f);
      Vertex center = makeVertex(x + w * 0.5f, y + h * 0.5f);
      // Trekanter uden overlap, så alpha også virker korrekt.
      for (int i = 0; i < count; ++i) {
        vertices.push_back(center);
        vertices.push_back(outline[i]);
        vertices.push_back(outline[(i + 1) % count]);
      }
    }
    clickableItems.push_back({id, x, y, w, h});
}


// uden texture og id

void Renderer::AddRoundedRect(float x, float y,float width, float height,float radius,float r, float g, float b, float a) {
    if (width <= 0.0f || height <= 0.0f)
      return;
    radius = std::clamp(radius, 0.0f, std::min(width, height) * 0.5f);
    // Almindelig firkant gennem din AddQuad.
    auto rect = [&](float px, float py, float w, float h) {
      if (w <= 0.0f || h <= 0.0f)
        return;
      AddQuad(
        Point3D{px,     py,     0.0f},
        Point3D{px + w, py,     0.0f},
        Point3D{px + w, py + h, 0.0f},
        Point3D{px,     py + h, 0.0f},
        r, g, b, a
      );
    };

    if (radius == 0.0f) {
      rect(x, y, width, height);
      return;
    }
    // Tre rektangler uden overlap.
    rect(x + radius, y, width - radius * 2.0f, height);
    rect(x, y + radius,radius, height - radius * 2.0f);
    rect(x + width - radius, y + radius,radius, height - radius * 2.0f);
    constexpr float PI = 3.14159265359f;
    constexpr int segments = 16;  // Segmenter pr. hjørne
    auto corner = [&](float cx, float cy, float startAngle) {
      Point3D center{cx, cy, 0.0f};
      for (int i = 0; i < segments; ++i) {
        float angle1 = startAngle + (PI * 0.5f) * float(i) / float(segments);
        float angle2 = startAngle + (PI * 0.5f) * float(i + 1) / float(segments);
        Point3D p1{
          cx + std::cos(angle1) * radius,
          cy + std::sin(angle1) * radius,
          0.0f
        };
        Point3D p2{
          cx + std::cos(angle2) * radius,
          cy + std::sin(angle2) * radius,
          0.0f
        };
        // Gentaget sidste punkt laver en trekant via AddQuad.
        AddQuad(center, p1, p2, center, r, g, b, a);
      }
    };
    corner(x + radius,         y + radius,          PI);
    corner(x + width - radius, y + radius,          PI * 1.5f);
    corner(x + width - radius, y + height - radius,  0.0f);
    corner(x + radius,         y + height - radius, PI * 0.5f);
}


void Renderer::Flush() {
    if(vertices.empty())
        return;
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    vertices.size()*sizeof(TextureVertex),
                    vertices.data());
    shader.Use();
    glActiveTexture(GL_TEXTURE0);
    if(currentTexture)
        glBindTexture(GL_TEXTURE_2D,currentTexture);
    else
        glBindTexture(GL_TEXTURE_2D,0);
    glUniform1i(glGetUniformLocation(shader.ID,"textMode"),currentText ? GL_TRUE : GL_FALSE);
    glUniform1i(glGetUniformLocation(shader.ID,"useTexture"),currentTexture ? GL_TRUE : GL_FALSE);
    glBindVertexArray(vao);
    glDrawArrays(currentPrimitive, 0, vertices.size());
    glBindVertexArray(0);
    vertices.clear();
}


