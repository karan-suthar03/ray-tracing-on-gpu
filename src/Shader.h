#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <iostream>
#include <glm/glm.hpp>

class Shader
{
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    
    Shader(const char* computePath);
    
    void use() const;
    void setInt(const std::string& name, int value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    
    void dispatchCompute(GLuint numGroupsX, GLuint numGroupsY, GLuint numGroupsZ) const;
    
    GLuint getID() const { return ID; }
    
private:
    GLuint ID;
    bool computeShader;
};

#endif // SHADER_H
