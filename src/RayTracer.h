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

    // Frame count for accumulation
    int frameCount;

    // Previous camera parameters to detect movement
    glm::vec3 prevCamPos;
    glm::vec3 prevCamTarget;
    glm::vec3 prevCamUp;

    void setupTexture();
    void setupShader();
};

#endif // RAY_TRACER_H
