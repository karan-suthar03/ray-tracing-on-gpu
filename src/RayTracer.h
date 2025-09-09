#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include "Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Sphere {
    glm::vec3 center;
    float radius;
    glm::vec3 color;
    int materialType; // 0 = Lambertian, 1 = Light
};

class RayTracer {
public:
    RayTracer(GLuint width, GLuint height);
    ~RayTracer();

    void render(const glm::vec3& cameraPos,
        const glm::vec3& cameraTarget,
        const glm::vec3& cameraUp);
    
    // Get the texture containing the rendered image
    GLuint getOutputTexture() const { return outputTexture; }

    // Update spheres data (only when changed)
    void setSpheres(const std::vector<Sphere>& newSpheres) {
        spheres = newSpheres;
        spheresChanged = true;
    }

    // Get current spheres
    const std::vector<Sphere>& getSpheres() const { return spheres; }

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

    std::vector<Sphere> spheres;
    std::vector<float> spheresData;
    GLuint ssbo;
    bool spheresChanged;

    void setupTexture();
    void setupShader();
    void setupSSBO();
    void updateSSBO();
};

#endif // RAY_TRACER_H
