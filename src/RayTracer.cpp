#include "RayTracer.h"
#include <vector>
#include <iostream>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

RayTracer::RayTracer(GLuint width, GLuint height)
    : width(width), height(height), frameCount(0), prevCamPos(0.0f), prevCamTarget(0.0f), prevCamUp(0.0f), spheresChanged(true), trianglesChanged(true)
{
    spheres = {
        //{{0.0f, 0.0f, 0.0f}, 0.5f, {1.0f, 0.0f, 0.0f}, 0}, // Lambertian
        // {{2.0f, 2.0f, 0.0f}, 1.0f, {10.0f, 10.0f, 10.0f}, 1}, // Light (bright white)
        //{{0.0f, -100.5f, 0.0f}, 100.0f, {0.5f, 0.5f, 0.5f}, 0} // Lambertian
    };

    Triangle leftWall1, leftWall2, rightWall1, rightWall2, backWall1, backWall2, floor1, floor2, ceiling1, ceiling2, frontWall1, frontWall2;
    
    // Left wall (x = -3, normal pointing right) - 2 triangles
    leftWall1.v0 = glm::vec3(-3.0f, -3.0f, -3.0f);
    leftWall1.v1 = glm::vec3(-3.0f, 3.0f, -3.0f);
    leftWall1.v2 = glm::vec3(-3.0f, 3.0f, 3.0f);
    leftWall1.normal = glm::vec3(1.0f, 0.0f, 0.0f);
    leftWall1.material = {{0.8f, 0.2f, 0.2f}, 0}; // Red, Lambertian
    
    leftWall2.v0 = glm::vec3(-3.0f, -3.0f, -3.0f);
    leftWall2.v1 = glm::vec3(-3.0f, 3.0f, 3.0f);
    leftWall2.v2 = glm::vec3(-3.0f, -3.0f, 3.0f);
    leftWall2.normal = glm::vec3(1.0f, 0.0f, 0.0f);
    leftWall2.material = {{0.8f, 0.2f, 0.2f}, 0}; // Red, Lambertian
    
    // Right wall (x = 3, normal pointing left) - 2 triangles
    rightWall1.v0 = glm::vec3(3.0f, -3.0f, -3.0f);
    rightWall1.v1 = glm::vec3(3.0f, 3.0f, 3.0f);
    rightWall1.v2 = glm::vec3(3.0f, 3.0f, -3.0f);
    rightWall1.normal = glm::vec3(-1.0f, 0.0f, 0.0f);
    rightWall1.material = {{0.2f, 0.8f, 0.2f}, 0}; // Green, Lambertian
    
    rightWall2.v0 = glm::vec3(3.0f, -3.0f, -3.0f);
    rightWall2.v1 = glm::vec3(3.0f, -3.0f, 3.0f);
    rightWall2.v2 = glm::vec3(3.0f, 3.0f, 3.0f);
    rightWall2.normal = glm::vec3(-1.0f, 0.0f, 0.0f);
    rightWall2.material = {{0.2f, 0.8f, 0.2f}, 0}; // Green, Lambertian
    
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

    Triangle light1, light2;
    light1.v0 = glm::vec3(-1.0f, 2.99f, -1.0f);
    light1.v1 = glm::vec3(1.0f, 2.99f, -1.0f);
    light1.v2 = glm::vec3(1.0f, 2.99f, 1.0f);
    light1.normal = glm::vec3(0.0f, -1.0f, 0.0f); // Pointing down
    light1.material = {{10.0f, 10.0f, 10.0f}, 1}; // Bright white, Light

    light2.v0 = glm::vec3(-1.0f, 2.99f, -1.0f);
    light2.v1 = glm::vec3(1.0f, 2.99f, 1.0f);
    light2.v2 = glm::vec3(-1.0f, 2.99f, 1.0f);
    light2.normal = glm::vec3(0.0f, -1.0f, 0.0f); // Pointing down
    light2.material = {{10.0f, 10.0f, 10.0f}, 1}; // Bright white, Light

    // Front wall (z = 3, normal pointing backward) - 2 triangles
    frontWall1.v0 = glm::vec3(-3.0f, -3.0f, 3.0f);
    frontWall1.v1 = glm::vec3(-3.0f, 3.0f, 3.0f);
    frontWall1.v2 = glm::vec3(3.0f, 3.0f, 3.0f);
    frontWall1.normal = glm::vec3(0.0f, 0.0f, -1.0f);
    frontWall1.material = {{0.8f, 0.8f, 0.2f}, 0}; // Yellow, Lambertian

    frontWall2.v0 = glm::vec3(-3.0f, -3.0f, 3.0f);
    frontWall2.v1 = glm::vec3(3.0f, 3.0f, 3.0f);
    frontWall2.v2 = glm::vec3(3.0f, -3.0f, 3.0f);
    frontWall2.normal = glm::vec3(0.0f, 0.0f, -1.0f);
    frontWall2.material = {{0.8f, 0.8f, 0.2f}, 0}; // Yellow, Lambertian

    triangles = {
        leftWall1, leftWall2,
        rightWall1, rightWall2,
        backWall1, backWall2,
        floor1, floor2,
        ceiling1, ceiling2,
        frontWall1, frontWall2,
        light1, light2
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

    //Material bunnyMaterial = { {1.0f,1.0f, 1.0f}, 0 };
    //if (!loadOBJ("bunny2.obj", bunnyMaterial)) {
    //    std::cerr << "Failed to load bunny.obj" << std::endl;
    //}

    setupTexture();
    setupShader();
    setupSSBO();
    setupTrianglesSSBO();
}

RayTracer::~RayTracer()
{
    glDeleteTextures(1, &outputTexture);
    glDeleteBuffers(1, &ssbo);
    glDeleteBuffers(1, &trianglesSSBO);
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

    computeShader->use();

    // passing camera uniforms
    computeShader->setVec3("camPos", cameraPos);
    computeShader->setVec3("camTarget", cameraTarget);
    computeShader->setVec3("camUp", cameraUp);
    computeShader->setInt("frameCount", frameCount);
    computeShader->setVec2("resolution", glm::vec2(width, height));
    computeShader->setInt("numSpheres", static_cast<int>(spheres.size()));
    computeShader->setInt("numTriangles", static_cast<int>(triangles.size()));

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
    std::cout << "Loaded " << triangles.size() << " triangles from " << filename << std::endl;
    
    return true;
}
