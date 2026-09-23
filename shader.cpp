#include "shader.h"
#include <iostream>


bool Shader::Load(const char* vs, const char* fs) {
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex,1,&vs,nullptr);
    glCompileShader(vertex);
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment,1,&fs,nullptr);
    glCompileShader(fragment);
    ID = glCreateProgram();
    glAttachShader(ID,vertex);
    glAttachShader(ID,fragment);
    glLinkProgram(ID);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return true;
}



void Shader::Use() {
    glUseProgram(ID);
}



void Shader::SetFloat(const char* name,float value) {
    glUniform1f(
        glGetUniformLocation(ID,name),
        value);
}



void Shader::SetInt(const char* name,int value) {
    glUniform1i(glGetUniformLocation(ID,name), value);
}

