#include "RayTracer.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <chrono>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

RayTracer::RayTracer(GLuint width, GLuint height)
    : width(width), height(height), frameCount(0), prevCamPos(0.0f), prevCamTarget(0.0f), prevCamUp(0.0f), spheresChanged(true), trianglesChanged(true), bvhChanged(true)
{
    spheres = {
        //{{0.0f, 0.0f, 0.0f}, 0.5f, {1.0f, 0.0f, 0.0f}, 0}, // Lambertian
        // {{2.0f, 2.0f, 0.0f}, 1.0f, {10.0f, 10.0f, 10.0f}, 1}, // Light (bright white)
        //{{0.0f, -100.5f, 0.0f}, 100.0f, {0.5f, 0.5f, 0.5f}, 0} // Lambertian
    };

    Triangle leftWall1, leftWall2, rightWall1, rightWall2, backWall1, backWall2, floor1, floor2, ceiling1, ceiling2, frontWall1, frontWall2;
    
    //// Left wall (x = -3, normal pointing right) - 2 triangles
    //leftWall1.v0 = glm::vec3(-3.0f, -3.0f, -3.0f);
    //leftWall1.v1 = glm::vec3(-3.0f, 3.0f, -3.0f);
    //leftWall1.v2 = glm::vec3(-3.0f, 3.0f, 3.0f);
    //leftWall1.normal = glm::vec3(1.0f, 0.0f, 0.0f);
    //leftWall1.material = {{0.8f, 0.2f, 0.2f}, 0}; // Red, Lambertian
    //
    //leftWall2.v0 = glm::vec3(-3.0f, -3.0f, -3.0f);
    //leftWall2.v1 = glm::vec3(-3.0f, 3.0f, 3.0f);
    //leftWall2.v2 = glm::vec3(-3.0f, -3.0f, 3.0f);
    //leftWall2.normal = glm::vec3(1.0f, 0.0f, 0.0f);
    //leftWall2.material = {{0.8f, 0.2f, 0.2f}, 0}; // Red, Lambertian
    //
    //// Right wall (x = 3, normal pointing left) - 2 triangles
    //rightWall1.v0 = glm::vec3(3.0f, -3.0f, -3.0f);
    //rightWall1.v1 = glm::vec3(3.0f, 3.0f, 3.0f);
    //rightWall1.v2 = glm::vec3(3.0f, 3.0f, -3.0f);
    //rightWall1.normal = glm::vec3(-1.0f, 0.0f, 0.0f);
    //rightWall1.material = {{0.2f, 0.8f, 0.2f}, 0}; // Green, Lambertian
    //
    //rightWall2.v0 = glm::vec3(3.0f, -3.0f, -3.0f);
    //rightWall2.v1 = glm::vec3(3.0f, -3.0f, 3.0f);
    //rightWall2.v2 = glm::vec3(3.0f, 3.0f, 3.0f);
    //rightWall2.normal = glm::vec3(-1.0f, 0.0f, 0.0f);
    //rightWall2.material = {{0.2f, 0.8f, 0.2f}, 0}; // Green, Lambertian
    
    // Back wall (z = -3, normal pointing forward) - 2 triangles
    backWall1.v0 = glm::vec3(-3.0f, -3.0f, -3.0f);
    backWall1.v1 = glm::vec3(3.0f, 3.0f, -3.0f);
    backWall1.v2 = glm::vec3(-3.0f, 3.0f, -3.0f);
    backWall1.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    backWall1.material = {{0.2f, 0.2f, 0.8f}, 0}; // Blue, Lambertian
    
    backWall2.v0 = glm::vec3(-3.0f, -3.0f, -3.0f);
    backWall2.v1 = glm::vec3(3.0f, -3.0f, -3.0f);
    backWall2.v2 = glm::vec3(3.0f, 3.0f, -3.0f);
    backWall2.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    backWall2.material = {{0.2f, 0.2f, 0.8f}, 0}; // Blue, Lambertian
    
    // Floor (y = -3, normal pointing up) - 2 triangles
    floor1.v0 = glm::vec3(-3.0f, -3.0f, -3.0f);
    floor1.v1 = glm::vec3(-3.0f, -3.0f, 3.0f);  // Swapped v1 and v2
    floor1.v2 = glm::vec3(3.0f, -3.0f, 3.0f);   // Swapped v1 and v2
    floor1.normal = glm::vec3(0.0f, 1.0f, 0.0f); // Pointing up
    floor1.material = {{0.8f, 0.8f, 0.8f}, 0}; // Gray, Lambertian
    
    floor2.v0 = glm::vec3(-3.0f, -3.0f, -3.0f);
    floor2.v1 = glm::vec3(3.0f, -3.0f, 3.0f);   // Swapped v1 and v2
    floor2.v2 = glm::vec3(3.0f, -3.0f, -3.0f);  // Swapped v1 and v2
    floor2.normal = glm::vec3(0.0f, 1.0f, 0.0f); // Pointing up
    floor2.material = {{0.8f, 0.8f, 0.8f}, 0}; // Gray, Lambertian
    
    // Ceiling (y = 3, normal pointing down) - 2 triangles
    ceiling1.v0 = glm::vec3(-3.0f, 3.0f, -3.0f);
    ceiling1.v1 = glm::vec3(3.0f, 3.0f, 3.0f);  // Swapped v1 and v2
    ceiling1.v2 = glm::vec3(-3.0f, 3.0f, 3.0f); // Swapped v1 and v2
    ceiling1.normal = glm::vec3(0.0f, -1.0f, 0.0f); // Pointing down
    ceiling1.material = {{0.8f, 0.8f, 0.8f}, 0}; // Gray, Lambertian
    
    ceiling2.v0 = glm::vec3(-3.0f, 3.0f, -3.0f);
    ceiling2.v1 = glm::vec3(3.0f, 3.0f, -3.0f);  // Back to original
    ceiling2.v2 = glm::vec3(3.0f, 3.0f, 3.0f); // Back to original
    ceiling2.normal = glm::vec3(0.0f, -1.0f, 0.0f); // Pointing down
    ceiling2.material = {{0.8f, 0.8f, 0.8f}, 0}; // Gray, Lambertian

    //Triangle light1, light2;
    //light1.v0 = glm::vec3(-1.0f, 2.99f, -1.0f);
    //light1.v1 = glm::vec3(1.0f, 2.99f, -1.0f);
    //light1.v2 = glm::vec3(1.0f, 2.99f, 1.0f);
    //light1.normal = glm::vec3(0.0f, -1.0f, 0.0f); // Pointing down
    //light1.material = {{10.0f, 10.0f, 10.0f}, 1}; // Bright white, Light

    //light2.v0 = glm::vec3(-1.0f, 2.99f, -1.0f);
    //light2.v1 = glm::vec3(1.0f, 2.99f, 1.0f);
    //light2.v2 = glm::vec3(-1.0f, 2.99f, 1.0f);
    //light2.normal = glm::vec3(0.0f, -1.0f, 0.0f); // Pointing down
    //light2.material = {{10.0f, 10.0f, 10.0f}, 1}; // Bright white, Light

    //// Front wall (z = 3, normal pointing backward) - 2 triangles
    //frontWall1.v0 = glm::vec3(-3.0f, -3.0f, 3.0f);
    //frontWall1.v1 = glm::vec3(-3.0f, 3.0f, 3.0f);
    //frontWall1.v2 = glm::vec3(3.0f, 3.0f, 3.0f);
    //frontWall1.normal = glm::vec3(0.0f, 0.0f, -1.0f);
    //frontWall1.material = {{0.8f, 0.8f, 0.2f}, 0}; // Yellow, Lambertian

    //frontWall2.v0 = glm::vec3(-3.0f, -3.0f, 3.0f);
    //frontWall2.v1 = glm::vec3(3.0f, 3.0f, 3.0f);
    //frontWall2.v2 = glm::vec3(3.0f, -3.0f, 3.0f);
    //frontWall2.normal = glm::vec3(0.0f, 0.0f, -1.0f);
    //frontWall2.material = {{0.8f, 0.8f, 0.2f}, 0}; // Yellow, Lambertian

    triangles = {
        //leftWall1, leftWall2,
        //rightWall1, rightWall2,
        backWall1, backWall2,
        floor1, floor2,
        ceiling1, ceiling2,
        //frontWall1, frontWall2,
        //light1, light2
    };

    for (auto& tri : triangles) {
		tri.v0.y += 3.0f;
		tri.v1.y += 3.0f;
		tri.v2.y += 3.0f;
	}

    // Load the cube OBJ file
    Material cubeMaterial = {{0.5f, 0.8f, 0.3f}, 0};
    if (!loadOBJ("cube.OBJ", cubeMaterial)) {
        std::cerr << "Failed to load cube.OBJ" << std::endl;
    }

    Material bunnyMaterial = { {0.9f,0.0f, 0.0f}, 0 };
    if (!loadOBJ("bunny.obj", bunnyMaterial)) {
        std::cerr << "Failed to load bunny.obj" << std::endl;
    }

    setupTexture();
    setupShader();
    setupSSBO();
    setupTrianglesSSBO();
    
    // bvh only after all triangles are loaded
    buildBVH();
    setupBVHSSBO();
    setupBVHIndicesSSBO();
}

RayTracer::~RayTracer()
{
    glDeleteTextures(1, &outputTexture);
    glDeleteBuffers(1, &ssbo);
    glDeleteBuffers(1, &trianglesSSBO);
    glDeleteBuffers(1, &bvhSSBO);
    glDeleteBuffers(1, &bvhIndicesSSBO);
    delete computeShader;
}

void RayTracer::render(const glm::vec3& cameraPos,
    const glm::vec3& cameraTarget,
    const glm::vec3& cameraUp)
{
    // Reset frame count if camera has moved
    if (cameraPos != prevCamPos || cameraTarget != prevCamTarget || cameraUp != prevCamUp) {
        frameCount = 0;
    }
    prevCamPos = cameraPos;
    prevCamTarget = cameraTarget;
    prevCamUp = cameraUp;

    updateSSBO();
    updateTrianglesSSBO();
    updateBVHSSBO();
    updateBVHIndicesSSBO();

    computeShader->use();

    // passing camera uniforms
    computeShader->setVec3("camPos", cameraPos);
    computeShader->setVec3("camTarget", cameraTarget);
    computeShader->setVec3("camUp", cameraUp);
    computeShader->setInt("frameCount", frameCount);
    computeShader->setVec2("resolution", glm::vec2(width, height));
    computeShader->setInt("numSpheres", static_cast<int>(spheres.size()));
    computeShader->setInt("numTriangles", static_cast<int>(triangles.size()));
    computeShader->setInt("numBVHNodes", static_cast<int>(bvhNodes.size()));

    // we are going to make worker groups with each of them containing 16 * 16 threads as defined in the compute shader
    // we are adding 15 to ensure we round up when the dimensions are not multiples of 16
	// coordinates (id's which we are using as pixel cordinates) are not in the bounds of the size of the screen then the shader will automatically discard them
	// as written in the compute shader
    GLuint workGroupsX = (width + 15) / 16;
    GLuint workGroupsY = (height + 15) / 16;
    computeShader->dispatchCompute(workGroupsX, workGroupsY, 1);

	// this is the barrier to ensure that the writes to the image have finished before we use it
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    frameCount++;
}

void RayTracer::setTriangles(const std::vector<Triangle>& newTriangles) {
    triangles = newTriangles;
    trianglesChanged = true;
    buildBVH();
    bvhChanged = true;
}

void RayTracer::setupTexture()
{
    // this is just to create the texture with the size and bind to slot 0
	// for further use in the compute shader
    glGenTextures(1, &outputTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, outputTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}

void RayTracer::setupShader()
{
    computeShader = new Shader("shaders/raytracer.comp");
}

void RayTracer::setupSSBO()
{
    spheresData.clear();
    for (const auto& s : spheres) {
        spheresData.push_back(s.center.x);
        spheresData.push_back(s.center.y);
        spheresData.push_back(s.center.z);
        spheresData.push_back(s.radius);
        spheresData.push_back(s.material.color.x);
        spheresData.push_back(s.material.color.y);
        spheresData.push_back(s.material.color.z);
        spheresData.push_back(float(s.material.type)); // material type
    }
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, spheresData.size() * sizeof(float), spheresData.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    spheresChanged = false;
}

void RayTracer::updateSSBO()
{
    if (spheresChanged) {
        spheresData.clear();
        for (const auto& s : spheres) {
            spheresData.push_back(s.center.x);
            spheresData.push_back(s.center.y);
            spheresData.push_back(s.center.z);
            spheresData.push_back(s.radius);
            spheresData.push_back(s.material.color.x);
            spheresData.push_back(s.material.color.y);
            spheresData.push_back(s.material.color.z);
            spheresData.push_back(float(s.material.type));
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, spheresData.size() * sizeof(float), spheresData.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        spheresChanged = false;
    }
}

void RayTracer::setupTrianglesSSBO()
{
    trianglesData.clear();
    for (const auto& t : triangles) {
        // v0
        trianglesData.push_back(t.v0.x);
        trianglesData.push_back(t.v0.y);
        trianglesData.push_back(t.v0.z);
        // v1
        trianglesData.push_back(t.v1.x);
        trianglesData.push_back(t.v1.y);
        trianglesData.push_back(t.v1.z);
        // v2
        trianglesData.push_back(t.v2.x);
        trianglesData.push_back(t.v2.y);
        trianglesData.push_back(t.v2.z);
        // normal
        trianglesData.push_back(t.normal.x);
        trianglesData.push_back(t.normal.y);
        trianglesData.push_back(t.normal.z);
        // color
        trianglesData.push_back(t.material.color.x);
        trianglesData.push_back(t.material.color.y);
        trianglesData.push_back(t.material.color.z);
        // material type
        trianglesData.push_back(float(t.material.type));
    }
    glGenBuffers(1, &trianglesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, trianglesSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, trianglesData.size() * sizeof(float), trianglesData.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, trianglesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    trianglesChanged = false;
}

void RayTracer::updateTrianglesSSBO()
{
    if (trianglesChanged) {
        trianglesData.clear();
        for (const auto& t : triangles) {
            // v0
            trianglesData.push_back(t.v0.x);
            trianglesData.push_back(t.v0.y);
            trianglesData.push_back(t.v0.z);
            // v1
            trianglesData.push_back(t.v1.x);
            trianglesData.push_back(t.v1.y);
            trianglesData.push_back(t.v1.z);
            // v2
            trianglesData.push_back(t.v2.x);
            trianglesData.push_back(t.v2.y);
            trianglesData.push_back(t.v2.z);
            // normal
            trianglesData.push_back(t.normal.x);
            trianglesData.push_back(t.normal.y);
            trianglesData.push_back(t.normal.z);
            // color
            trianglesData.push_back(t.material.color.x);
            trianglesData.push_back(t.material.color.y);
            trianglesData.push_back(t.material.color.z);
            // material type
            trianglesData.push_back(float(t.material.type));
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, trianglesSSBO);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, trianglesData.size() * sizeof(float), trianglesData.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        trianglesChanged = false;
    }
}

bool RayTracer::loadOBJ(const std::string& filename, const Material& material) {
    tinyobj::attrib_t attrib{};
    std::vector<tinyobj::shape_t> shapes{};
    std::vector<tinyobj::material_t> materials{};
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str());

    if (!warn.empty()) {
        std::cout << "Warning: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << "Error: " << err << std::endl;
        return false;
    }

    if (!ret) {
        std::cerr << "Failed to load OBJ file: " << filename << std::endl;
        return false;
    }

    // Process each shape
    for (const auto& shape : shapes) {
        size_t index_offset = 0;
        // Process each face
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            int fv = shape.mesh.num_face_vertices[f];
            if (fv != 3) {
                // Skip non-triangles for now
                index_offset += fv;
                continue;
            }

            // Get indices for the three vertices
            tinyobj::index_t idx0 = shape.mesh.indices[index_offset + 0];
            tinyobj::index_t idx1 = shape.mesh.indices[index_offset + 1];
            tinyobj::index_t idx2 = shape.mesh.indices[index_offset + 2];

            // Get vertex positions
            glm::vec3 v0(attrib.vertices[3 * idx0.vertex_index + 0],
                        attrib.vertices[3 * idx0.vertex_index + 1],
                        attrib.vertices[3 * idx0.vertex_index + 2]);
            glm::vec3 v1(attrib.vertices[3 * idx1.vertex_index + 0],
                        attrib.vertices[3 * idx1.vertex_index + 1],
                        attrib.vertices[3 * idx1.vertex_index + 2]);
            glm::vec3 v2(attrib.vertices[3 * idx2.vertex_index + 0],
                        attrib.vertices[3 * idx2.vertex_index + 1],
                        attrib.vertices[3 * idx2.vertex_index + 2]);

            // // Scale the model
            // float scale = 1.0f;
            // if (filename == "bunny.obj") {
            //     scale = 5.0f;
            // } else if (filename == "cube.OBJ") {
            //     scale = 2.0f;
            // }
            // v0 *= scale;
            // v1 *= scale;
            // v2 *= scale;


            // Get or compute normal
            glm::vec3 normal(0.0f);
            if (idx0.normal_index >= 0 && idx1.normal_index >= 0 && idx2.normal_index >= 0) {
                // Use provided normals
                glm::vec3 n0(attrib.normals[3 * idx0.normal_index + 0],
                            attrib.normals[3 * idx0.normal_index + 1],
                            attrib.normals[3 * idx0.normal_index + 2]);
                glm::vec3 n1(attrib.normals[3 * idx1.normal_index + 0],
                            attrib.normals[3 * idx1.normal_index + 1],
                            attrib.normals[3 * idx1.normal_index + 2]);
                glm::vec3 n2(attrib.normals[3 * idx2.normal_index + 0],
                            attrib.normals[3 * idx2.normal_index + 1],
                            attrib.normals[3 * idx2.normal_index + 2]);
                // Average the normals
                normal = glm::normalize((n0 + n1 + n2) / 3.0f);
            } else {
                // Compute normal from vertices
                glm::vec3 edge1 = v1 - v0;
                glm::vec3 edge2 = v2 - v0;
                normal = glm::normalize(glm::cross(edge1, edge2));
            }

            // Create triangle
            Triangle tri;
            tri.v0 = v0;
            tri.v1 = v1;
            tri.v2 = v2;
            tri.normal = normal;
            tri.material = material;

            // Add to triangles vector
            triangles.push_back(tri);

            index_offset += fv;
        }
    }

    trianglesChanged = true;
    // buildBVH();
    // bvhChanged = true;
    std::cout << "Loaded " << triangles.size() << " triangles from " << filename << std::endl;
    
    return true;
}

AABB RayTracer::computeTriangleAABB(const Triangle& tri) {
    AABB aabb;
    aabb.expand(tri.v0);
    aabb.expand(tri.v1);
    aabb.expand(tri.v2);
    return aabb;
}

glm::vec3 RayTracer::computeTriangleCentroid(const Triangle& tri) {
    return (tri.v0 + tri.v1 + tri.v2) / 3.0f;
}

void RayTracer::buildBVH() {
    if (triangles.empty()) {
        bvhNodes.clear();
        triangleIndices.clear();
        bvhChanged = true;
        return;
    }

    std::cout << "Building BVH for " << triangles.size() << " triangles..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();

    triangleIndices.resize(triangles.size());
    std::iota(triangleIndices.begin(), triangleIndices.end(), 0);

    std::vector<glm::vec3> centroids(triangles.size());
    for (size_t i = 0; i < triangles.size(); i++) {
        centroids[i] = computeTriangleCentroid(triangles[i]);
    }

    
    bvhNodes.clear();
    bvhNodes.reserve(2 * triangles.size());

    buildBVHRecursive(0, static_cast<int>(triangles.size()), triangleIndices, centroids);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    bvhChanged = true;
    std::cout << bvhNodes.size() << " nodes in " << duration.count() << "ms" << std::endl;
}

int RayTracer::buildBVHRecursive(int start, int end, std::vector<int>& indices, const std::vector<glm::vec3>& centroids) {
    int nodeIndex = static_cast<int>(bvhNodes.size());
    bvhNodes.emplace_back();
    BVHNode& node = bvhNodes[nodeIndex];

    for (int i = start; i < end; i++) {
        const Triangle& tri = triangles[indices[i]];
        AABB triAABB = computeTriangleAABB(tri);
        node.bounds.expand(triAABB);
    }

    int triCount = end - start;
    
    if (triCount <= 8) {
        node.firstTriIndex = start;
        node.triCount = triCount;
        return nodeIndex;
    }

    // Find the longest axis and split at median
    // Suggestion: we can make a surface based spilit function which is way more optimized
    glm::vec3 extent = node.bounds.max - node.bounds.min;
    int splitAxis = 0;
    if (extent.y > extent.x) splitAxis = 1;
    if (extent.z > extent[splitAxis]) splitAxis = 2;

    // Sort triangles by centroid along the split axis
    std::sort(indices.begin() + start, indices.begin() + end, 
              [&centroids, splitAxis](int a, int b) {
                  return centroids[a][splitAxis] < centroids[b][splitAxis];
              });

    int split = start + triCount / 2;

    node.leftChild = buildBVHRecursive(start, split, indices, centroids);
    node.rightChild = buildBVHRecursive(split, end, indices, centroids);

    return nodeIndex;
}

void RayTracer::setupBVHSSBO() {
    bvhData.clear();
    
    for (const auto& node : bvhNodes) {
        // AABB min
        bvhData.push_back(node.bounds.min.x);
        bvhData.push_back(node.bounds.min.y);
        bvhData.push_back(node.bounds.min.z);
        bvhData.push_back(0.0f); // padding
        
        // AABB max
        bvhData.push_back(node.bounds.max.x);
        bvhData.push_back(node.bounds.max.y);
        bvhData.push_back(node.bounds.max.z);
        bvhData.push_back(0.0f); // padding
        
        // Node data
        bvhData.push_back(static_cast<float>(node.leftChild));
        bvhData.push_back(static_cast<float>(node.rightChild));
        bvhData.push_back(static_cast<float>(node.firstTriIndex));
        bvhData.push_back(static_cast<float>(node.triCount));
    }

    glGenBuffers(1, &bvhSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bvhData.size() * sizeof(float), bvhData.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, bvhSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    bvhChanged = false;
}

// i dont think we're ever going to use this function now but maybe in future
void RayTracer::updateBVHSSBO() {
    if (bvhChanged) {
        bvhData.clear();
        
        // Serialize BVH nodes
        for (const auto& node : bvhNodes) {
            // AABB min
            bvhData.push_back(node.bounds.min.x);
            bvhData.push_back(node.bounds.min.y);
            bvhData.push_back(node.bounds.min.z);
            bvhData.push_back(0.0f); // padding
            
            // AABB max
            bvhData.push_back(node.bounds.max.x);
            bvhData.push_back(node.bounds.max.y);
            bvhData.push_back(node.bounds.max.z);
            bvhData.push_back(0.0f); // padding
            
            // Node data
            bvhData.push_back(static_cast<float>(node.leftChild));
            bvhData.push_back(static_cast<float>(node.rightChild));
            bvhData.push_back(static_cast<float>(node.firstTriIndex));
            bvhData.push_back(static_cast<float>(node.triCount));
        }

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhSSBO);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bvhData.size() * sizeof(float), bvhData.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        bvhChanged = false;
    }
}

void RayTracer::setupBVHIndicesSSBO() {
    bvhIndicesData.clear();
    
    for (int index : triangleIndices) {
        bvhIndicesData.push_back(static_cast<float>(index));
    }

    glGenBuffers(1, &bvhIndicesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhIndicesSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bvhIndicesData.size() * sizeof(float), bvhIndicesData.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, bvhIndicesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void RayTracer::updateBVHIndicesSSBO() {
    if (bvhChanged) {
        bvhIndicesData.clear();
        
        for (int index : triangleIndices) {
            bvhIndicesData.push_back(static_cast<float>(index));
        }

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhIndicesSSBO);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bvhIndicesData.size() * sizeof(float), bvhIndicesData.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
}
