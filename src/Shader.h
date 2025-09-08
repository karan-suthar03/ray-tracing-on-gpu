#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <iostream>

class Shader
{
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    
    Shader(const char* computePath);
    
    void use() const;
    void setInt(const std::string& name, int value) const;
    
    void dispatchCompute(GLuint numGroupsX, GLuint numGroupsY, GLuint numGroupsZ) const;
    
private:
    GLuint ID;
    bool computeShader;
};

#endif // SHADER_H
