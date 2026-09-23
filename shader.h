#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>

class Shader {
public:
    GLuint ID;
    bool Load(const char* vertexSource, const char* fragmentSource);
    void Use();
    void SetFloat(const char* name,float value);
    void SetInt(const char* name,int value);
};

#endif

