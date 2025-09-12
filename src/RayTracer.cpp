#include "RayTracer.h"
#include <vector>
#include <iostream>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

RayTracer::RayTracer(GLuint width, GLuint height)
    : width(width), height(height), frameCount(0), prevCamPos(0.0f), prevCamTarget(0.0f), prevCamUp(0.0f), spheresChanged(true), meshChanged(true)
{
    spheres = {
        //{{0.0f, 0.0f, 0.0f}, 0.5f, {1.0f, 0.0f, 0.0f}, 0}, // Lambertian
        // {{2.0f, 2.0f, 0.0f}, 1.0f, {10.0f, 10.0f, 10.0f}, 1}, // Light (bright white)
        //{{0.0f, -100.5f, 0.0f}, 100.0f, {0.5f, 0.5f, 0.5f}, 0} // Lambertian
    };

    mesh.clear();
    
    createCornellBox();
    Material cubeMaterial = {{0.5f, 0.8f, 0.3f}, 0};
    loadOBJIndexed("cube.OBJ", cubeMaterial);

    Material bunnyMaterial = {{0.0f, 1.0f, 1.0f}, 0};
    loadOBJIndexed("bunny2.obj", bunnyMaterial);

    setupTexture();
    setupShader();
    setupSSBO();
    setupIndexedSSBOs();
}

RayTracer::~RayTracer()
{
    glDeleteTextures(1, &outputTexture);
    glDeleteBuffers(1, &ssbo);
    glDeleteBuffers(1, &verticesSSBO);
    glDeleteBuffers(1, &triangleDataSSBO);
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
    updateIndexedSSBOs();

    computeShader->use();

    // passing camera uniforms
    computeShader->setVec3("camPos", cameraPos);
    computeShader->setVec3("camTarget", cameraTarget);
    computeShader->setVec3("camUp", cameraUp);
    computeShader->setInt("frameCount", frameCount);
    computeShader->setVec2("resolution", glm::vec2(width, height));
    computeShader->setInt("numSpheres", static_cast<int>(spheres.size()));
    computeShader->setInt("numTriangles", static_cast<int>(mesh.getTriangleCount()));

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

void RayTracer::setupIndexedSSBOs() {
    verticesData.clear();
    for (const auto& vertex : mesh.vertices) {
        verticesData.push_back(vertex.x);
        verticesData.push_back(vertex.y);
        verticesData.push_back(vertex.z);
    }

    triangleData.clear();
    for (size_t i = 0; i < mesh.normals.size(); ++i) {

        triangleData.push_back(float(mesh.indices[i * 3]));
        triangleData.push_back(float(mesh.indices[i * 3 + 1]));
        triangleData.push_back(float(mesh.indices[i * 3 + 2]));

        triangleData.push_back(mesh.normals[i].x);
        triangleData.push_back(mesh.normals[i].y);
        triangleData.push_back(mesh.normals[i].z);

        triangleData.push_back(mesh.materials[i].color.x);
        triangleData.push_back(mesh.materials[i].color.y);
        triangleData.push_back(mesh.materials[i].color.z);

        triangleData.push_back(float(mesh.materials[i].type));
    }

    glGenBuffers(1, &verticesSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, verticesSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, verticesData.size() * sizeof(float), 
                 verticesData.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, verticesSSBO);

    glGenBuffers(1, &triangleDataSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleDataSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, triangleData.size() * sizeof(float), 
                 triangleData.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, triangleDataSSBO);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    meshChanged = false;
}

void RayTracer::updateIndexedSSBOs() {
    if (meshChanged) {
        verticesData.clear();
        for (const auto& vertex : mesh.vertices) {
            verticesData.push_back(vertex.x);
            verticesData.push_back(vertex.y);
            verticesData.push_back(vertex.z);
        }

        triangleData.clear();
        for (size_t i = 0; i < mesh.normals.size(); ++i) {
            triangleData.push_back(float(mesh.indices[i * 3]));
            triangleData.push_back(float(mesh.indices[i * 3 + 1]));
            triangleData.push_back(float(mesh.indices[i * 3 + 2]));
            triangleData.push_back(mesh.normals[i].x);
            triangleData.push_back(mesh.normals[i].y);
            triangleData.push_back(mesh.normals[i].z);
            triangleData.push_back(mesh.materials[i].color.x);
            triangleData.push_back(mesh.materials[i].color.y);
            triangleData.push_back(mesh.materials[i].color.z);
            triangleData.push_back(float(mesh.materials[i].type));
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, verticesSSBO);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, verticesData.size() * sizeof(float), verticesData.data());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleDataSSBO);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, triangleData.size() * sizeof(float), triangleData.data());

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        meshChanged = false;
    }
}

uint32_t RayTracer::addVertex(const glm::vec3& vertex) {
    const float epsilon = 1e-4f; // small threshold to consider vertices identical
    for (size_t i = 0; i < mesh.vertices.size(); ++i) {
        if (glm::length(mesh.vertices[i] - vertex) < epsilon) {
            return static_cast<uint32_t>(i);
        }
    }
    
    mesh.vertices.push_back(vertex);
    return static_cast<uint32_t>(mesh.vertices.size() - 1);
}

void RayTracer::addTriangle(uint32_t i0, uint32_t i1, uint32_t i2, 
                           const glm::vec3& normal, const Material& material) {
    mesh.indices.push_back(i0);
    mesh.indices.push_back(i1);
    mesh.indices.push_back(i2);
    mesh.normals.push_back(normal);
    mesh.materials.push_back(material);
    meshChanged = true;
}

void RayTracer::createCornellBox() {
    std::vector<glm::vec3> roomVertices = {
        {-3.0f, 0.0f, -3.0f}, // 0: bottom-back-left
        { 3.0f, 0.0f, -3.0f}, // 1: bottom-back-right
        { 3.0f, 0.0f,  3.0f}, // 2: bottom-front-right
        {-3.0f, 0.0f,  3.0f}, // 3: bottom-front-left
        {-3.0f, 6.0f, -3.0f}, // 4: top-back-left
        { 3.0f, 6.0f, -3.0f}, // 5: top-back-right
        { 3.0f, 6.0f,  3.0f}, // 6: top-front-right
        {-3.0f, 6.0f,  3.0f}  // 7: top-front-left
    };
    
    std::vector<uint32_t> indices;
    for (const auto& vertex : roomVertices) {
        indices.push_back(addVertex(vertex));
    }
    
    Material floorMat = {{0.8f, 0.8f, 0.8f}, 0};
    addTriangle(indices[0], indices[2], indices[1], {0,1,0}, floorMat);
    addTriangle(indices[0], indices[3], indices[2], {0,1,0}, floorMat);
    
    Material ceilingMat = {{0.8f, 0.8f, 0.8f}, 0};
    addTriangle(indices[4], indices[5], indices[6], {0,-1,0}, ceilingMat);
    addTriangle(indices[4], indices[6], indices[7], {0,-1,0}, ceilingMat);
    
    Material leftMat = {{0.8f, 0.2f, 0.2f}, 0};
    addTriangle(indices[0], indices[4], indices[7], {1,0,0}, leftMat);
    addTriangle(indices[0], indices[7], indices[3], {1,0,0}, leftMat);
    
    Material rightMat = {{0.2f, 0.8f, 0.2f}, 0};
    addTriangle(indices[1], indices[2], indices[6], {-1,0,0}, rightMat);
    addTriangle(indices[1], indices[6], indices[5], {-1,0,0}, rightMat);
    
    Material backMat = {{0.2f, 0.2f, 0.8f}, 0};
    addTriangle(indices[0], indices[1], indices[5], {0,0,1}, backMat);
    addTriangle(indices[0], indices[5], indices[4], {0,0,1}, backMat);
    
    //Material frontMat = {{0.8f, 0.8f, 0.2f}, 0};
    //addTriangle(indices[3], indices[7], indices[6], {0,0,-1}, frontMat);
    //addTriangle(indices[3], indices[6], indices[2], {0,0,-1}, frontMat);
    
    std::vector<glm::vec3> lightVertices = {
        {-1.0f, 5.99f, -1.0f},
        { 1.0f, 5.99f, -1.0f},
        { 1.0f, 5.99f,  1.0f},
        {-1.0f, 5.99f,  1.0f}
    };
    
    std::vector<uint32_t> lightIndices;
    for (const auto& vertex : lightVertices) {
        lightIndices.push_back(addVertex(vertex));
    }
    
    Material lightMat = {{10.0f, 10.0f, 10.0f}, 1};
    addTriangle(lightIndices[0], lightIndices[2], lightIndices[1], {0,-1,0}, lightMat);
    addTriangle(lightIndices[0], lightIndices[3], lightIndices[2], {0,-1,0}, lightMat);
    
}

bool RayTracer::loadOBJIndexed(const std::string& filename, const Material& material) {
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

            std::vector<uint32_t> triIndices;
            for (int v = 0; v < 3; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                glm::vec3 vertex(
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                );
                triIndices.push_back(addVertex(vertex));
            }

            const glm::vec3& v0 = mesh.vertices[triIndices[0]];
            const glm::vec3& v1 = mesh.vertices[triIndices[1]];
            const glm::vec3& v2 = mesh.vertices[triIndices[2]];
            glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
            addTriangle(triIndices[0], triIndices[1], triIndices[2], normal, material);
            index_offset += fv;
        }
    }

    std::cout << "Loaded " << mesh.getTriangleCount() << " triangles from " << filename << std::endl;
    return true;
}

bool RayTracer::loadOBJ(const std::string& filename, const Material& material) {
    return loadOBJIndexed(filename, material);
}
