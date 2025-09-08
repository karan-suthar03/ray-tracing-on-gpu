#include "Shader.h"
#include <fstream>
#include <sstream>

// doe vertex and fragment shader just to show the texture on the screen
Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    computeShader = false;

    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    std::stringstream vShaderStream, fShaderStream;
    
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    
    vShaderFile.close();
    fShaderFile.close();
    
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
    
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    
    //compile
    GLuint vertex, fragment;
    
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

// compute shader
Shader::Shader(const char* computePath)
{
    computeShader = true;
    
    std::string computeCode;
    std::ifstream cShaderFile;
    
    cShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    
    cShaderFile.open(computePath);
    std::stringstream cShaderStream;
    
    cShaderStream << cShaderFile.rdbuf();
    
    cShaderFile.close();
    
    computeCode = cShaderStream.str();
    
    const char* cShaderCode = computeCode.c_str();
    
    GLuint compute;
    
    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &cShaderCode, NULL);
    glCompileShader(compute);
    
    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);
    
    glDeleteShader(compute);
}

void Shader::use() const
{
    glUseProgram(ID);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

// utility function to set vec3 uniforms in the shader,
// it is being used for camera positions but will be used in other things obviously
// why im wrting so many comments though
void Shader::setVec3(const std::string &name, const glm::vec3& value) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}

void Shader::dispatchCompute(GLuint numGroupsX, GLuint numGroupsY, GLuint numGroupsZ) const
{    
    glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
}
