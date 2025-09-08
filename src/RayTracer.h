#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include "Shader.h"
#include <glad/glad.h>

class RayTracer {
public:
    RayTracer(GLuint width, GLuint height);
    ~RayTracer();

    void render();
    
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
