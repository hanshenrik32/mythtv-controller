//
// Screen savers
//

#include <GL/glew.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cmath>
#include <vector>
#include <GL/gl.h>
#include <algorithm>
#include <iostream>
#include <chrono>


#include "renderer.h"
#include "myth_saver.h"


extern FILE *logfile;
extern char configpicturepath[256];
extern GLuint screensaverbox;
extern GLuint newstuf_icon;
extern GLuint ring_background;
extern int orgwinsizex,orgwinsizey;
extern musicoversigt_class musicoversigt;
extern Renderer renderer;
extern float spectrum[];                                                           // used for spectium



float rot = 0.0f;

#define NUM_BARS 128
// #define PI 3.14159265358979323846

// *****************************************************************************************
//
// Plasma
//
// *****************************************************************************************


void drawPlasma(int width, int height) {
  static float plasmaTime = 0.0f;
  float music = 0.0f;
  // tag gennemsnit af spectrum
  for(int i=0;i<64;i++)
      music += spectrum[i+24];
  music /= 512.0f;
  // begræns så det ikke eksploderer
  music = fminf(music, 0.5f);
  plasmaTime += 0.020f + music * 0.05f;
  for(int y = 0; y < height; y+=2) {
    for(int x = 0; x < width; x+=2) {
      float nx = (float)x / width;
      float ny = (float)y / height;
      float wave =
        sinf(nx * (10.0f + music*20.0f) + plasmaTime) +
        cosf(ny * (10.0f + music*20.0f) + plasmaTime) +
        sinf((nx+ny) * 10.0f + plasmaTime) +
        cosf(sqrtf(nx*nx+ny*ny) * 15.0f + plasmaTime);
      float v = (wave+4.0f)*0.25f;
      float r = 0.5f + 0.5f*sinf(v*6.28f + music*5);
      float g = 0.5f + 0.5f*sinf(v*6.28f + 2.0f + music*5);
      float b = 0.5f + 0.5f*sinf(v*6.28f + 4.0f + music*5);
      // boost farver med musik
      r *= 0.8f + music;
      g *= 0.8f + music;
      b *= 0.8f + music;
      renderer.AddPoint(
          x,y,
          r,g,b,
          1.0f);
    }
  }
}



// ****************************************************************************************
//
// audio spectrum ring 3D
//
// ****************************************************************************************


Point3D Project3D(Point3D p) {
  float cameraAngle = 35.0f * M_PI / 180.0f;
  float cameraDistance = 700.0f;
  float z = p.z * cosf(cameraAngle) - p.y * sinf(cameraAngle);
  float y = p.z * sinf(cameraAngle) + p.y * cosf(cameraAngle);
  float scale = cameraDistance / (cameraDistance + z);
  return {
    960 + p.x * scale,
    540 - y * scale
  };
}



// ****************************************************************************************
//
// outher ring
//
// ****************************************************************************************


void musicmeter_class::DrawOuterRingWithLines(float r) {
  // Tegn ring i samme 3D-plan som søjlerne
  Point3D center3D = {
      0.0f,
      0.0f,
      0.0f
  };
  Point3D center2D = Project3D(center3D);
  Point3D q1;
  Point3D q2;
  for (int i = 0; i < 120; i++) {
    float a1 = (float) i / 120.0f * 2.0f * M_PI;
    float a2 = (float)(i + 1) / 120.0f * 2.0f * M_PI;
    Point3D p1 = {
    cosf(a1) * r,
    0.0f,
    sinf(a1) * r
    };

    Point3D p2 = {
        cosf(a2) * r,
        0.0f,
        sinf(a2) * r
    };
    q1 = Project3D(p1);
    q2 = Project3D(p2);
    AddGlowLine( q1.x, q1.y, q2.x, q2.y, 1.0f, 0.2f, 1.0f, 1.0f);
  }
}


// In use

void musicmeter_class::DrawOuterRingClock(float r) {
  const int ringSegments = 120;
  std::vector<TextureVertex> ringVertices;
  std::vector<TextureVertex> minuteVertices;
  std::vector<TextureVertex> hourVertices;
  ringVertices.reserve(ringSegments + 1);
  minuteVertices.reserve(48 * 2);
  hourVertices.reserve(12 * 2);
  // =====================================================
  // 1. RINGEN
  // =====================================================
  for (int i = 0; i <= ringSegments; i++) {
      float a =
          static_cast<float>(i) /
          static_cast<float>(ringSegments) *
          2.0f * static_cast<float>(M_PI);
      Point3D p = {
          cosf(a) * r,
          0.0f,
          sinf(a) * r
      };
      Point3D q = Project3D(p);
      ringVertices.push_back({
          q.x, q.y,
          0.0f, 0.0f,
          0.20f, 0.45f, 1.0f, 0.80f
      });
  }

  // =====================================================
  // 2. 60 MARKERINGER
  // =====================================================

  for (int i = 0; i < 60; i++) {
    float a = static_cast<float>(i) / 60.0f * 2.0f * static_cast<float>(M_PI);
    bool isHourMark = ((i % 5) == 0);
    float innerRadius;
    float outerRadius;
    if (isHourMark) {
        // Lange timestreger
        innerRadius = r + 38.0f;
        outerRadius = r + 3.0f;
    } else {
        // Korte minutstreger
        innerRadius = r + 18.0f;
        outerRadius = r + 3.0f;
    }
    Point3D p1 = {
        cosf(a) * innerRadius,
        0.0f,
        sinf(a) * innerRadius
    };
    Point3D p2 = {
        cosf(a) * outerRadius,
        0.0f,
        sinf(a) * outerRadius
    };
    Point3D q1 = Project3D(p1);
    Point3D q2 = Project3D(p2);
    if (isHourMark) {
      hourVertices.push_back({ q1.x, q1.y, 0.0f, 0.0f,  0.75f, 0.90f, 1.0f, 1.0f});
      hourVertices.push_back({ q2.x, q2.y, 0.0f, 0.0f,  0.75f, 0.90f, 1.0f, 1.0f});
    } else {
      minuteVertices.push_back({q1.x, q1.y, 0.0f, 0.0f,  0.20f, 0.50f, 1.0f, 0.75f});
      minuteVertices.push_back({q2.x, q2.y, 0.0f, 0.0f,  0.20f, 0.50f, 1.0f, 0.75f});
    }
  }
  // =====================================================
  // 3. SEND TO SHADER
  // =====================================================
  renderer.DrawRawLines(ringVertices, GL_LINE_STRIP, 2.0f, GL_SRC_ALPHA, GL_ONE);
  renderer.DrawRawLines(minuteVertices, GL_LINES, 1.0f, GL_SRC_ALPHA, GL_ONE);
  renderer.DrawRawLines(hourVertices, GL_LINES, 3.0f, GL_SRC_ALPHA, GL_ONE);
  // Tilbage til normal blending
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}



// **********************************************************************************
//
// vortex
//
// **********************************************************************************

void musicmeter_class::DrawPlasmaVortex(float cx,float cy, float radius, float bass) {
  static float t = 0.0f;
  t += 0.025f;
  bass = log1pf(spectrum[0])/2;
  renderer.Flush();
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  const int rings    = 30;
  const int segments = 100;
  float pulse = 1.0f + (bass/10) * 0.25f;
  for (int r = 0; r < rings; r++) {
    float rn = (float)r / (float)rings;
    // Sort hul i centrum
    float rr = radius * (0.25f + rn * 0.75f) * pulse;
    for (int i = 0; i < segments; i++) {
      float a1 = (float)i / (float)segments * 2.0f * M_PI;
      float a2 = (float)(i + 1) / (float)segments * 2.0f * M_PI;
      // Vortex rotation
      float twist = t * 1.5f * (1.0f - rn); 
      // Plasma bølger
      float wave1 = sinf(a1 * 4.0f + t * 2.0f + rn * 12.0f);
      float wave2 = sinf(a1 * 9.0f - t * 3.0f + rn * 20.0f);
      float distortion = wave1 * 0.06f + wave2 * 0.025f;
      float r1 = rr * (1.0f + distortion);
      // beregn distortion separat til næste punkt
      float wave1b = sinf(a2 * 4.0f + t * 2.0f + rn * 12.0f);
      float wave2b = sinf(a2 * 9.0f - t * 3.0f + rn * 20.0f);
      float distortion2 = wave1b * 0.06f +  wave2b * 0.025f;
      float r2 = rr * (1.0f + distortion2);
      // ==========================================
      // WORLD 3D POSITION
      // SAMME SYSTEM SOM DINE SØJLER
      // ==========================================
      Point3D p1;
      p1.x = cosf(a1 + twist) * r1;
      p1.y = 0.0f;
      p1.z = sinf(a1 + twist) * r1;
      Point3D p2;
      p2.x = cosf(a2 + twist) * r2;
      p2.y = 0.0f;
      p2.z = sinf(a2 + twist) * r2;
      // Samme projection som bars
      Point3D q1 = Project3D(p1);
      Point3D q2 = Project3D(p2);
      // ==========================================
      // BLUE -> PURPLE
      // ==========================================
      float p = rn;
      float red = 0.10f + p * 0.70f;
      float green = 0.20f - p * 0.15f;
      float blue = 1.0f;
      float brightness = 0.5f + 0.5f * sinf(a1 * 6.0f - t * 4.0f + rn * 15.0f);
      red   *= brightness;
      green *= brightness;
      blue  *= brightness;
      float alpha = 0.25f + brightness * 0.5f;
      // Glow
      renderer.AddThickLine(q1.x, q1.y, q2.x, q2.y, 8.0f, red, green, blue, alpha * 0.15f);
      renderer.AddThickLine(q1.x, q1.y, q2.x, q2.y, 3.0f, red, green, blue, alpha * 0.45f);
      renderer.AddThickLine(q1.x, q1.y, q2.x, q2.y, 1.2f, red, green, blue, alpha );
    }
  }

  float r = radius * 0.25f;
  DrawOuterRingWithLines(r);
  float radius2 = radius  + 1320;
  r = radius2 * 0.25f;
  DrawOuterRingClock(r);
}




// **********************************************************************************
//
// Glow line
//
// **********************************************************************************



void AddGlowLine(float x1, float y1, float x2, float y2,float width,float r, float g, float b) {
    // yderste svage glow
    renderer.AddThickLine( x1,y1,x2,y2, width * 4.0f, r,g,b, 0.50f);
    // mellem glow
    renderer.AddThickLine( x1,y1,x2,y2, width * 2.5f, r,g,b, 0.35f);
    // tæt glow
    renderer.AddThickLine( x1,y1,x2,y2, width * 1.5f, r,g,b, 0.2f);
    // selve søjlen
    renderer.AddThickLine( x1,y1,x2,y2, width, r,g,b, 1.0f);
}


  // new DrawBar3D
  // working

  struct Color_bars {
      float r, g, b, a;
  };


void DrawBar3D(float x, float z, float w, float h, float r, float g, float b,float a,int color_part,float rotationX,float rotationY) {
  Color_bars color1;
  Color_bars color2;
  Color_bars color3;
  Color_bars color4;
  Color_bars color5;
  Color_bars color6;
  float d = w * 1.8f;
  float depth = d * 0.5f;
  constexpr float PI = 3.14159265358979323846f;
  // Barens retning langs cirklens tangent.
  float angle = std::atan2(z, x) - PI * 0.5f;
  float ca = std::cos(angle);
  float sa = std::sin(angle);
  // Fælles rotation for hele cirklen.
  // float cr = std::cos(rotation);
  // float sr = std::sin(rotation);
  Point3D p[8] = {
      {-d, 0, -depth},
      { d, 0, -depth},
      { d, 0,  depth},
      {-d, 0,  depth},

      {-d, h, -depth},
      { d, h, -depth},
      { d, h,  depth},
      {-d, h,  depth}
  };
  Point3D q[8];
  float cx = std::cos(rotationX);
  float sx = std::sin(rotationX);
  float cy = std::cos(rotationY);
  float sy = std::sin(rotationY);
  for (int i = 0; i < 8; i++) {
      float localX = p[i].x;
      float localZ = p[i].z;

      // Orientér baren og placér den i cirklen.
      float worldX = x + localX * ca - localZ * sa;
      float worldY = p[i].y;
      float worldZ = z + localX * sa + localZ * ca;

      // Først rotation omkring X-aksen.
      float rotatedY = worldY * cx - worldZ * sx;
      float rotatedZ = worldY * sx + worldZ * cx;

      // Derefter rotation omkring Y-aksen.
      p[i].x = worldX * cy - rotatedZ * sy;
      p[i].y = rotatedY;
      p[i].z = worldX * sy + rotatedZ * cy;

      q[i] = Project3D(p[i]);
  }

  if (color_part==1) {
    unsigned int color = 0x292f2d;
    float r = ((color >> 16) & 0xFF) / 255.0f;
    float g = ((color >>  8) & 0xFF) / 255.0f;
    float b = ( color        & 0xFF) / 255.0f;    
    if ((h>=0.0f) && (h<2.0f)) {
      color1={0.01f, 0.01f, 0.01f, 0.1f};       // top
      color2={0.01f, 0.01f, 0.01f, 0.1f};       // bund
      color2={0.01f, 0.01f, 0.01f, 0.1f};       // bund
      color3={0.03f, 0.03f, 0.03f, 0.1f};       // v side
      color3={0.03f, 0.03f, 0.03f, 0.1f};       // v side
      color4={0.03f, 0.03f, 0.03f, 0.1f};       // h side
      color5={0.03f, 0.03f, 0.03f, 0.1f};       // top
      color6={0.03f, 0.03f, 0.03f, 0.1f};       // front
    } else if ((h>=2.0f) && (h<40.0f)) {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.03f, a};       // v side
      color4={0.03f, 0.03f, 0.03f, a};       // h side
      color5={r, g, b, a};                   // top
      color6={0.01f, 0.01f, 0.01f, a};       // front
    } else if ((h>=20.0f) && (h<40.0f)) {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.03f, a};       // v side
      color4={0.03f, 0.03f, 0.03f, a};       // h side
      color5={r, g, b, a};                   // top
      color6={0.01f, 0.01f, 0.01f, a};       // front
    } else if ((h>=40.0f) && (h<60.0f)) {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.12f, a};       // v side
      color4={0.03f, 0.03f, 0.12f, a};       // h side
      color5={r, g, b, a};                   // top
      color6={0.01f, 0.01f, 0.01f, a};       // front
    } else if ((h>=60.0f) && (h<200.0f)) {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.12f, a};       // v side
      color4={0.03f, 0.03f, 0.12f, a};       // h side
      color5={r, g, b, a};       // top
      color6={0.01f, 0.01f, 0.01f, a};       // front
    } else {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.03f, a};       // v side
      color4={0.03f, 0.03f, 0.03f, a};       // h side
      color5={r, g, b, a};       // top
      color6={0.01f, 0.01f, 0.01f, a};       // front  
    }
  } else if (color_part==2) {
    unsigned int color = 0x292f2d;
    float r = ((color >> 16) & 0xFF) / 255.0f;
    float g = ((color >>  8) & 0xFF) / 255.0f;
    float b = ( color        & 0xFF) / 255.0f;    
    if ((h>=0.0f) && (h<2.0f)) {
      color1={0.01f, 0.01f, 0.01f, 0.1f};       // top
      color2={0.01f, 0.01f, 0.01f, 0.1f};       // bund
      color2={0.01f, 0.01f, 0.01f, 0.1f};       // bund
      color3={0.03f, 0.03f, 0.03f, 0.1f};       // v side
      color3={0.03f, 0.03f, 0.03f, 0.1f};       // v side
      color4={0.03f, 0.03f, 0.03f, 0.1f};       // h side
      color5={0.03f, 0.03f, 0.03f, 0.1f};       // top
      color6={0.03f, 0.03f, 0.03f, 0.1f};       // front
    } else if ((h>=2.0f) && (h<40.0f)) {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.03f, a};       // v side
      color4={0.03f, 0.03f, 0.03f, a};       // h side
      color5={r, g, b, a};       // top
      color6={0.01f, 0.01f, 0.01f, a};       // front
    } else if ((h>=20.0f) && (h<40.0f)) {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.03f, a};       // v side
      color4={0.03f, 0.03f, 0.03f, a};       // h side
      color5={r, g, b, a};       // top
      color6={0.01f, 0.01f, 0.01f, a};       // front
    } else if ((h>=40.0f) && (h<60.0f)) {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.12f, a};       // v side
      color4={0.03f, 0.03f, 0.12f, a};       // h side
      color5={r, g, b, a};       // top
      color6={0.01f, 0.01f, 0.01f, a};       // front
    } else if ((h>=60.0f) && (h<200.0f)) {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.12f, 0.03f, 0.01f, a};       // v side
      color4={0.12f, 0.03f, 0.01f, a};       // h side
      color5={r, g, b, a};       // top
      color6={0.01f, 0.01f, 0.01f, a};       // front
    } else {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.03f, a};       // v side
      color4={0.03f, 0.03f, 0.03f, a};       // h side
      color5={r, g, b, a};       // top
      color6={0.01f, 0.01f, 0.01f, a};       // front  
    }
  } else if (color_part==3) {
    unsigned int color = 0x292f2d;
    float r = ((color >> 16) & 0xFF) / 255.0f;
    float g = ((color >>  8) & 0xFF) / 255.0f;
    float b = ( color        & 0xFF) / 255.0f;    
    if ((h>=0.0f) && (h<2.0f)) {
      color1={0.01f, 0.01f, 0.01f, 0.1f};       // top
      color2={0.01f, 0.01f, 0.01f, 0.1f};       // bund
      color2={0.01f, 0.01f, 0.01f, 0.1f};       // bund
      color3={0.03f, 0.03f, 0.03f, 0.1f};       // v side
      color3={0.03f, 0.03f, 0.03f, 0.1f};       // v side
      color4={0.03f, 0.03f, 0.03f, 0.1f};       // h side
      color5={0.03f, 0.03f, 0.03f, 0.1f};       // top
      color6={0.03f, 0.03f, 0.03f, 0.1f};       // front
    } else if ((h>=2.0f) && (h<40.0f)) {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.03f, a};       // v side
      color4={0.03f, 0.03f, 0.03f, a};       // h side
      color5={0.12f, 0.12f, 0.12f, a};       // top
      color6={0.01f, 0.01f, 0.01f, a};       // front
    } else if ((h>=20.0f) && (h<40.0f)) {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.03f, a};       // v side
      color4={0.03f, 0.03f, 0.03f, a};       // h side
      color5={0.12f, 0.12f, 0.12f, a};       // top
      color6={0.01f, 0.01f, 0.01f, a};       // front
    } else if ((h>=40.0f) && (h<60.0f)) {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.12f, a};       // v side
      color4={0.03f, 0.03f, 0.12f, a};       // h side
      color5={0.12f, 0.12f, 0.24f, a};       // top
      color6={0.01f, 0.01f, 0.01f, a};       // front
    } else if ((h>=60.0f) && (h<200.0f)) {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.12f, a};       // v side
      color4={0.03f, 0.03f, 0.12f, a};       // h side
      color5={0.12f, 0.12f, 0.24f, a};       // top
      color6={0.01f, 0.01f, 0.01f, a};       // front
    } else {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.03f, a};       // v side
      color4={0.03f, 0.03f, 0.03f, a};       // h side
      color5={0.24f, 0.12f, 0.12f, a};       // top
      color6={0.01f, 0.01f, 0.01f, a};       // front  
    }
  } else {
    unsigned int color = 0x292f2d;
    float r = ((color >> 16) & 0xFF) / 255.0f;
    float g = ((color >>  8) & 0xFF) / 255.0f;
    float b = ( color        & 0xFF) / 255.0f;    
    if ((h>=0.0f) && (h<2.0f)) {
      color1={0.01f, 0.01f, 0.01f, 0.1f};       // top
      color2={0.01f, 0.01f, 0.01f, 0.1f};       // bund
      color2={0.01f, 0.01f, 0.01f, 0.1f};       // bund
      color3={0.03f, 0.03f, 0.03f, 0.1f};       // v side
      color3={0.03f, 0.03f, 0.03f, 0.1f};       // v side
      color4={0.03f, 0.03f, 0.03f, 0.1f};       // h side
      color5={0.03f, 0.03f, 0.03f, 0.1f};       // top
      color6={0.03f, 0.03f, 0.03f, 0.1f};       // front
    } else if ((h>=2.0f) && (h<40.0f)) {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.03f, a};       // v side
      color4={0.03f, 0.03f, 0.03f, a};       // h side
      color5={0.12f, 0.12f, 0.12f, a};       // top
      color6={0.01f, 0.01f, 0.01f, a};       // front
    } else if ((h>=20.0f) && (h<40.0f)) {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.03f, a};       // v side
      color4={0.03f, 0.03f, 0.03f, a};       // h side
      color5={0.12f, 0.12f, 0.12f, a};       // top
      color6={0.01f, 0.01f, 0.01f, a};       // front
    } else if ((h>=40.0f) && (h<60.0f)) {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.12f, a};       // v side
      color4={0.03f, 0.03f, 0.12f, a};       // h side
      color5={0.12f, 0.12f, 0.24f, a};       // top
      color6={0.01f, 0.01f, 0.01f, a};       // front
    } else if ((h>=60.0f) && (h<200.0f)) {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.12f, a};       // v side
      color4={0.03f, 0.03f, 0.12f, a};       // h side
      color5={0.12f, 0.12f, 0.24f, a};       // top
      color6={0.01f, 0.01f, 0.01f, a};       // front
    } else {
      color1={0.01f, 0.01f, 0.01f, a};       // top
      color2={0.01f, 0.01f, 0.01f, a};       // bund
      color3={0.03f, 0.03f, 0.03f, a};       // v side
      color4={0.03f, 0.03f, 0.03f, a};       // h side
      color5={0.24f, 0.12f, 0.12f, a};       // top
      color6={0.01f, 0.01f, 0.01f, a};       // front  
    }
  }

  /*
           p[7] -------- p[6]
          /|            /|
         / |           / |
      p[4] -------- p[5] |
       |   |          |  |
       |   |          |  |
       |  p[3] -------|--p[2]
       | /            |  /
       |/             | /
      p[0] -------- p[1]
  */

  renderer.AddQuad(q[2], q[3], q[7], q[6], color6.r, color6.g, color6.b,color6.a); // back
  renderer.AddQuad(q[0], q[1], q[2], q[3], color2.r, color2.g, color2.b,color2.a); // Bund
  renderer.AddQuad(q[0], q[3], q[7], q[4], color3.r, color3.g, color3.b,color3.a); // Venstre side
  renderer.AddQuad(q[2], q[1], q[5], q[6], color4.r, color4.g, color4.b,color4.a); // Højre side
  renderer.AddQuad(q[4], q[7], q[6], q[5], color5.r, color5.g, color5.b,color5.a); // Top
  // renderer.AddQuad(q[1], q[0], q[4], q[5], color1.r, color1.g, color1.b,a); // front
  // renderer.AddQuad(q[0], q[1], q[5], q[4], color1.r, color1.g, color1.b,a); // front
  renderer.AddQuad(q[4], q[5], q[1], q[0],color1.r,color1.g,color1.b,color6.a); // front
 
  // 2. GRÅ KANTER
  
  float lineWidth = 1.0f;
  float edgeR = 0.0f, edgeG = 1.0f, edgeB = 0.4f; // Grå farve (0.0 = sort, 1.0 = hvid)
  printf("Hight = %f \n",h);
  if (color_part==1) {
    unsigned int color = 0x292f2d;
    float r = ((color >> 16) & 0xFF) / 255.0f;
    float g = ((color >>  8) & 0xFF) / 255.0f;
    float b = ( color        & 0xFF) / 255.0f;    
    if ((h>=0.0f) && (h<2.0f)) {
      edgeR = 0.0f, edgeG = 0.0f, edgeB = 0.0f;
    } else if ((h>=2.0f) && (h<4.0f)) {
      edgeR = r, edgeG = g, edgeB = b;
    } else if ((h>=4.0f) && (h<6.0f)) {
      edgeR = r, edgeG = g, edgeB = b;
    } else if ((h>=6.0f) && (h<8.0f)) {
      edgeR = r, edgeG = g, edgeB = b;
    } else if ((h>=8.0f) && (h<10.0f)) {
      edgeR = r, edgeG = g, edgeB = b;
    } else if ((h>=10.0f) && (h<12.0f)) {
      edgeR = r, edgeG = g, edgeB = b;
    } else if ((h>=12.0f) && (h<20.0f)) {
      edgeR = r, edgeG = g, edgeB = b;
    } else if ((h>=20.0f) && (h<40.0f)) {
      edgeR = r, edgeG = g, edgeB = b;
    } else if ((h>=40.0f) && (h<60.0f)) {
      edgeR = r, edgeG = g, edgeB = b;
    } else if ((h>=60.0f) && (h<80.0f)) {
      edgeR = r, edgeG = g, edgeB = b;
    } else if ((h>=80.0f) && (h<100.0f)) {
      edgeR = r, edgeG = g, edgeB = b;
    } else if ((h>=100.0f) && (h<120.0f)) {
      edgeR = r, edgeG = g, edgeB = b;
    } else if ((h>=120.0f) && (h<140.0f)) {
      edgeR = r, edgeG = g, edgeB = b;
    } else if ((h>=140.0f) && (h<160.0f)) {
      edgeR = r, edgeG = g, edgeB = b;
    } else if ((h>=160.0f) && (h<180.0f)) {
      edgeR = r, edgeG = g, edgeB = b;
    } else if ((h>=180.0f) && (h<200.0f)) {
      edgeR = 0.0f, edgeG = 0.5f, edgeB = 1.0f;
    } else {
      edgeR = 0.0f, edgeG = 1.0f, edgeB = 0.4f;
    }
  } else if (color_part==2) {
    if ((h>=0.0f) && (h<2.0f)) {
      edgeR = 0.0f, edgeG = 0.0f, edgeB = 0.0f;
    } else if ((h>=2.0f) && (h<4.0f)) {
      edgeR = 0.4f, edgeG = 0.2f, edgeB = 0.2f;
    } else if ((h>=4.0f) && (h<6.0f)) {
      edgeR = 0.4f, edgeG = 0.4f, edgeB = 0.2f;
    } else if ((h>=6.0f) && (h<8.0f)) {
      edgeR = 0.4f, edgeG = 0.4f, edgeB = 0.2f;
    } else if ((h>=8.0f) && (h<10.0f)) {
      edgeR = 0.4f, edgeG = 0.4f, edgeB = 0.2f;
    } else if ((h>=10.0f) && (h<12.0f)) {
      edgeR = 0.4f, edgeG = 0.4f, edgeB = 0.2f;
    } else if ((h>=20.0f) && (h<40.0f)) {
      edgeR = 0.4f, edgeG = 0.5f, edgeB = 0.2f;
    } else if ((h>=40.0f) && (h<60.0f)) {
      edgeR = 0.4f, edgeG = 0.5f, edgeB = 0.2f;
    } else if ((h>=60.0f) && (h<80.0f)) {
      edgeR = 0.4f, edgeG = 0.5f, edgeB = 0.2f;
    } else if ((h>=80.0f) && (h<100.0f)) {
      edgeR = 0.4f, edgeG = 0.5f, edgeB = 0.2f;
    } else if ((h>=100.0f) && (h<200.0f)) {
      edgeR = 0.3f, edgeG = 0.7f, edgeB = 0.8f;
    } else {
      edgeR = 0.0f, edgeG = 1.0f, edgeB = 0.4f;
    }
  } else if (color_part==3) {
    if ((h>=0.0f) && (h<2.0f)) {
      edgeR = 0.0f, edgeG = 0.0f, edgeB = 0.0f;
    } else if ((h>=2.0f) && (h<4.0f)) {
      edgeR = 0.0f, edgeG = 0.2f, edgeB = 0.4f;
    } else if ((h>=4.0f) && (h<6.0f)) {
      edgeR = 0.0f, edgeG = 0.4f, edgeB = 0.4f;
    } else if ((h>=6.0f) && (h<8.0f)) {
      edgeR = 0.0f, edgeG = 0.5f, edgeB = 0.4f;
    } else if ((h>=8.0f) && (h<10.0f)) {
      edgeR = 0.0f, edgeG = 0.7f, edgeB = 0.4f;
    } else if ((h>=10.0f) && (h<12.0f)) {
      edgeR = 0.0f, edgeG = 0.7f, edgeB = 0.4f;
    } else if ((h>=20.0f) && (h<40.0f)) {
      edgeR = 0.0f, edgeG = 1.0f, edgeB = 0.4f;
    } else if ((h>=40.0f) && (h<60.0f)) {
      edgeR = 0.0f, edgeG = 1.0f, edgeB = 0.6f;
    } else if ((h>=60.0f) && (h<80.0f)) {
      edgeR = 0.0f, edgeG = 0.5f, edgeB = 0.7f;
    } else if ((h>=80.0f) && (h<100.0f)) {
      edgeR = 0.0f, edgeG = 0.5f, edgeB = 0.8f;
    } else if ((h>=100.0f) && (h<200.0f)) {
      edgeR = 0.0f, edgeG = 0.5f, edgeB = 1.0f;
    } else {
      edgeR = 0.0f, edgeG = 1.0f, edgeB = 0.4f;
    }
  } else {
    if ((h>=0.0f) && (h<2.0f)) {
      edgeR = 0.0f, edgeG = 0.0f, edgeB = 0.0f;
    } else if ((h>=2.0f) && (h<4.0f)) {
      edgeR = 0.0f, edgeG = 0.2f, edgeB = 0.4f;
    } else if ((h>=4.0f) && (h<6.0f)) {
      edgeR = 0.0f, edgeG = 0.4f, edgeB = 0.4f;
    } else if ((h>=6.0f) && (h<8.0f)) {
      edgeR = 0.0f, edgeG = 0.5f, edgeB = 0.4f;
    } else if ((h>=8.0f) && (h<10.0f)) {
      edgeR = 0.0f, edgeG = 0.7f, edgeB = 0.4f;
    } else if ((h>=10.0f) && (h<12.0f)) {
      edgeR = 0.0f, edgeG = 0.7f, edgeB = 0.4f;
    } else if ((h>=20.0f) && (h<40.0f)) {
      edgeR = 0.0f, edgeG = 1.0f, edgeB = 0.4f;
    } else if ((h>=40.0f) && (h<60.0f)) {
      edgeR = 0.0f, edgeG = 1.0f, edgeB = 0.6f;
    } else if ((h>=60.0f) && (h<80.0f)) {
      edgeR = 0.0f, edgeG = 0.5f, edgeB = 0.7f;
    } else if ((h>=80.0f) && (h<100.0f)) {
      edgeR = 0.0f, edgeG = 0.5f, edgeB = 0.8f;
    } else if ((h>=100.0f) && (h<200.0f)) {
      edgeR = 0.0f, edgeG = 0.5f, edgeB = 1.0f;
    } else {
      edgeR = 0.0f, edgeG = 1.0f, edgeB = 0.4f;
    }
  }
  renderer.Flush();
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  // bund
  AddGlowLine(q[0].x, q[0].y, q[1].x, q[1].y, lineWidth, edgeR, edgeG, edgeB);
  AddGlowLine(q[1].x, q[1].y, q[2].x, q[2].y, lineWidth, edgeR, edgeG, edgeB);
  AddGlowLine(q[2].x, q[2].y, q[3].x, q[3].y, lineWidth, edgeR, edgeG, edgeB);
  AddGlowLine(q[3].x, q[3].y, q[0].x, q[0].y, lineWidth, edgeR, edgeG, edgeB);
  // Top
  AddGlowLine(q[4].x, q[4].y, q[5].x, q[5].y, lineWidth, edgeR, edgeG, edgeB);
  AddGlowLine(q[5].x, q[5].y, q[6].x, q[6].y, lineWidth, edgeR, edgeG, edgeB);
  AddGlowLine(q[6].x, q[6].y, q[7].x, q[7].y, lineWidth, edgeR, edgeG, edgeB);
  AddGlowLine(q[7].x, q[7].y, q[4].x, q[4].y, lineWidth, edgeR, edgeG, edgeB);
  // Lodrette kanter
  for (int i = 0; i < 4; i++) {
    AddGlowLine(q[i].x, q[i].y, q[i+4].x, q[i+4].y, lineWidth, edgeR, edgeG, edgeB);
  }
  // Vigtigt: tegn batchen mens GL_ONE stadig er aktiv
  renderer.Flush();
  glDepthMask(GL_TRUE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


// main 3d ring

struct BarInstance {
    float x, z, h;
    float r, g, b;
};


void musicmeter_class::DrawAudioRing() {
  float radius = 400.0f;
  int count = NUM_BARS / 5; 
  std::vector<BarInstance> bars;
  bars.reserve(count);
  renderer.AddTextureRect(0,ring_background, 1, 1, 1920, 1080,1,1,1,0.5);
  for (int aantal_bars=0;aantal_bars<6;aantal_bars++) {
    switch(aantal_bars) {
      case 0: radius = 400.0f;
              count = NUM_BARS / 3;
              break;
      case 1: radius = 360.0f;
              count = NUM_BARS / 3;
              break;
      case 2: radius = 320.0f;
              count = NUM_BARS / 3;
              break;
      case 3: radius = 280.0f;
              count = NUM_BARS / 4;
              break;
      case 4: radius = 240.0f;
              count = NUM_BARS / 5;
              break;
      case 5: radius = 200.0f;
              count = NUM_BARS / 5;
              break;
      case 6: radius = 160.0f;
              count = NUM_BARS / 5; 
              break;
      case 7: radius = 120.0f;
              count = NUM_BARS / 6; 
              break;
      case 8: radius = 80.0f;
              count = NUM_BARS / 6; 
              break;
      default:
              break;
    }
    // 1. Beregn positioner og gem i en liste
    // bar 0
    for (int i = 0; i < count; i++) {
      float a = i * 2.0f * M_PI / count;
      float x = cosf(a) * radius;
      float z = sinf(a) * radius;

      float bassReduce = 0.3f + (float)i / (NUM_BARS * 0.7f);
      if(bassReduce > 1.0f)
        bassReduce = 1.0f;

      // float target = sqrtf(spectrum[i] * bassReduce * 128.0f) * 8.0f;
      float target = log1pf((spectrum[i]/2.0f) * bassReduce * 80.0f) * 35.0f;

      // Hurtig op
      if (target > barHeight[i]) {
        barHeight[i] += (target - barHeight[i]) * 0.45f;
      } else {
        // Langsom ned
        barHeight[i] -= 2.5f;          // pixels pr. frame
        if (barHeight[i] < target)
          barHeight[i] = target;
      }
      float h = barHeight[i];
      bars.push_back({x, z, h, 0.2f, 1.0f, 0.2f});
    }
    // 2. Sorter fra bagest til forrest (størst Z tegnes først)
    std::sort(bars.begin(), bars.end(), [](const BarInstance& a, const BarInstance& b) {
      return a.z > b.z;
    });
    static const auto start = std::chrono::steady_clock::now();
    float seconds = std::chrono::duration<float>( std::chrono::steady_clock::now() - start).count();
    constexpr float PI = 3.14159265358979323846f;
    // Sving ±15 grader. En hel svingning tager 8 sekunder.
    float rotation = std::sin(seconds * (2.0f * PI / 16.0f)) * (15.0f * PI / 180.0f);
    // Sving ±15 grader. En hel svingning tager 16 sekunder.
    float rotationY = std::sin(seconds * (2.0f * PI / 32.0f)) * (15.0f * PI / 180.0f);
    for (const auto& bar : bars) {
      DrawBar3D(bar.x, bar.z, 12.0f, bar.h, bar.r, bar.g, bar.b, 1.0f, 2,rotation,rotationY);
      renderer.Flush(); // Tvinger rendereren til at tegne baren i sin helhed før næste tegnes
    }
  }
}

// end 3D ring

