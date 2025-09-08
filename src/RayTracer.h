#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include "Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

class RayTracer {
public:
    RayTracer(GLuint width, GLuint height);
    ~RayTracer();

    void render(const glm::vec3& cameraPos,
        const glm::vec3& cameraTarget,
        const glm::vec3& cameraUp);
    
    // Get the texture containing the rendered image
    GLuint getOutputTexture() const { return outputTexture; }

private:
    GLuint width;
    GLuint height;
    GLuint outputTexture;
    Shader* computeShader;

    void setupTexture();
    void setupShader();
};

#endif // RAY_TRACER_H
