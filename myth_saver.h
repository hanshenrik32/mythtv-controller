#ifndef MYCTRL_SAVER
#define MYCTRL_SAVER

#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "myctrl_music.h"

// screen saver type in config file

// analog     urtype=ANALOG - Analog ur
// digital    urtype=DIGITAL - Float digital clock
// 3D2        urtype=SAVER3D2 - giver rotate box
// 3D         urtype=SAVER3D -  giver wave rotate box
// PICTURE3D  urtype=PICTURE3D - OK
// MUSICMETER urtype=MUSICMETER - OK



class spectrum_class {
  public:
    float music_spectrum[128] = {0};
 };


class musicmeter_class : public spectrum_class {
  private:
    float rot;    
    float peak[512]={0};
    float barHeight[512]={0};
    public: 
    void DrawPlasmaVortex(float cx, float cy,float radius,float bass);
    void DrawOuterRingWithLines(float r);  
    void DrawAudioRing();
    void DrawClockRing(float radius);
    // new
    void DrawOuterRingClock(float r);
    void renderScene();    
};

void drawPlasma(int width, int height);
void AddGlowLine(float x1, float y1, float x2, float y2,float width,float r, float g, float b);

#endif
