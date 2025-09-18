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

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
    
    AABB() : min(1e30f), max(-1e30f) {}
    AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}
    
    void expand(const glm::vec3& point) {
        min = glm::min(min, point);
        max = glm::max(max, point);
    }
    
    void expand(const AABB& box) {
        min = glm::min(min, box.min);
        max = glm::max(max, box.max);
    }
    
    glm::vec3 center() const {
        return (min + max) * 0.5f;
    }
    
    float surfaceArea() const {
        glm::vec3 extent = max - min;
        return 2.0f * (extent.x * extent.y + extent.y * extent.z + extent.z * extent.x);
    }
};

struct BVHNode {
    AABB bounds;
    int leftChild;   // Index to left child node -1 if leaf
    int rightChild;  // Index to right child node -1 if leaf
    int firstTriIndex; // Index of first triangle in leaf nodes
    int triCount;    // Number of triangles in leaf nodes

    BVHNode() : leftChild(-1), rightChild(-1), firstTriIndex(0), triCount(0) {}
    
    bool isLeaf() const {
        return leftChild == -1 && rightChild == -1;
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

    void setTriangles(const std::vector<Triangle>& newTriangles);

    const std::vector<Triangle>& getTriangles() const { return triangles; }

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

    std::vector<Triangle> triangles;
    std::vector<float> trianglesData;
    GLuint trianglesSSBO;
    bool trianglesChanged;

    std::vector<BVHNode> bvhNodes;
    std::vector<int> triangleIndices;
    std::vector<float> bvhData;
    std::vector<float> bvhIndicesData;
    GLuint bvhSSBO;
    GLuint bvhIndicesSSBO;
    bool bvhChanged;

    void setupTexture();
    void setupShader();
    void setupSSBO();
    void updateSSBO();
    void setupTrianglesSSBO();
    void updateTrianglesSSBO();
    
    AABB computeTriangleAABB(const Triangle& tri);
    glm::vec3 computeTriangleCentroid(const Triangle& tri);
    void buildBVH();
    int buildBVHRecursive(int start, int end, std::vector<int>& indices, const std::vector<glm::vec3>& centroids);
    void setupBVHSSBO();
    void updateBVHSSBO();
    void setupBVHIndicesSSBO();
    void updateBVHIndicesSSBO();
};

#endif // RAY_TRACER_H
