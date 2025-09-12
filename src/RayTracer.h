#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include "Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Material {
    glm::vec3 color;
    int type; // 0 = Lambertian, 1 = Light
};

struct Sphere {
    glm::vec3 center;
    float radius;
    Material material;
};

struct Triangle {
    glm::vec3 v0, v1, v2;
    glm::vec3 normal;
    Material material;
};

struct IndexedMesh {
    std::vector<glm::vec3> vertices;
    std::vector<uint32_t> indices;
    std::vector<glm::vec3> normals;
    std::vector<Material> materials;
    
    void clear() {
        vertices.clear();
        indices.clear();
        normals.clear();
        materials.clear();
    }
    
    size_t getTriangleCount() const {
        return indices.size() / 3;
    }
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

    bool loadOBJ(const std::string& filename, const Material& material = {{0.8f, 0.8f, 0.8f}, 0});

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

    // Indexed mesh data
    IndexedMesh mesh;
    std::vector<float> verticesData;
    std::vector<float> triangleData; // indices + normals + materials
    GLuint verticesSSBO;
    GLuint triangleDataSSBO;
    bool meshChanged;


    void setupTexture();
    void setupShader();
    void setupSSBO();
    void updateSSBO();    
    
    void setupIndexedSSBOs();
    void updateIndexedSSBOs();
    uint32_t addVertex(const glm::vec3& vertex);
    void addTriangle(uint32_t i0, uint32_t i1, uint32_t i2, 
                    const glm::vec3& normal, const Material& material);
    void createCornellBox();
    bool loadOBJIndexed(const std::string& filename, const Material& material);
};

#endif // RAY_TRACER_H
