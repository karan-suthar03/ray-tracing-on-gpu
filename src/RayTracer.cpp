#include "RayTracer.h"
#include <vector>

RayTracer::RayTracer(GLuint width, GLuint height)
    : width(width), height(height), frameCount(0), prevCamPos(0.0f), prevCamTarget(0.0f), prevCamUp(0.0f), spheresChanged(true), trianglesChanged(true)
{
    spheres = {
        //{{0.0f, 0.0f, 0.0f}, 0.5f, {1.0f, 0.0f, 0.0f}, 0}, // Lambertian
        // {{2.0f, 2.0f, 0.0f}, 1.0f, {10.0f, 10.0f, 10.0f}, 1}, // Light (bright white)
        //{{0.0f, -100.5f, 0.0f}, 100.0f, {0.5f, 0.5f, 0.5f}, 0} // Lambertian
    };

    Triangle leftWall1, leftWall2, rightWall1, rightWall2, backWall1, backWall2, floor1, floor2, ceiling1, ceiling2;
    
    // Left wall (x = -3, normal pointing right) - 2 triangles
    leftWall1.v0 = glm::vec3(-3.0f, -3.0f, -3.0f);
    leftWall1.v1 = glm::vec3(-3.0f, 3.0f, -3.0f);
    leftWall1.v2 = glm::vec3(-3.0f, 3.0f, 3.0f);
    leftWall1.normal = glm::vec3(1.0f, 0.0f, 0.0f);
    leftWall1.color = glm::vec3(0.8f, 0.2f, 0.2f); // Red
    leftWall1.materialType = 0;
    
    leftWall2.v0 = glm::vec3(-3.0f, -3.0f, -3.0f);
    leftWall2.v1 = glm::vec3(-3.0f, 3.0f, 3.0f);
    leftWall2.v2 = glm::vec3(-3.0f, -3.0f, 3.0f);
    leftWall2.normal = glm::vec3(1.0f, 0.0f, 0.0f);
    leftWall2.color = glm::vec3(0.8f, 0.2f, 0.2f); // Red
    leftWall2.materialType = 0;
    
    // Right wall (x = 3, normal pointing left) - 2 triangles
    rightWall1.v0 = glm::vec3(3.0f, -3.0f, -3.0f);
    rightWall1.v1 = glm::vec3(3.0f, 3.0f, 3.0f);
    rightWall1.v2 = glm::vec3(3.0f, 3.0f, -3.0f);
    rightWall1.normal = glm::vec3(-1.0f, 0.0f, 0.0f);
    rightWall1.color = glm::vec3(0.2f, 0.8f, 0.2f); // Green
    rightWall1.materialType = 0;
    
    rightWall2.v0 = glm::vec3(3.0f, -3.0f, -3.0f);
    rightWall2.v1 = glm::vec3(3.0f, -3.0f, 3.0f);
    rightWall2.v2 = glm::vec3(3.0f, 3.0f, 3.0f);
    rightWall2.normal = glm::vec3(-1.0f, 0.0f, 0.0f);
    rightWall2.color = glm::vec3(0.2f, 0.8f, 0.2f); // Green
    rightWall2.materialType = 0;
    
    // Back wall (z = -3, normal pointing forward) - 2 triangles
    backWall1.v0 = glm::vec3(-3.0f, -3.0f, -3.0f);
    backWall1.v1 = glm::vec3(3.0f, 3.0f, -3.0f);
    backWall1.v2 = glm::vec3(-3.0f, 3.0f, -3.0f);
    backWall1.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    backWall1.color = glm::vec3(0.2f, 0.2f, 0.8f); // Blue
    backWall1.materialType = 0;
    
    backWall2.v0 = glm::vec3(-3.0f, -3.0f, -3.0f);
    backWall2.v1 = glm::vec3(3.0f, -3.0f, -3.0f);
    backWall2.v2 = glm::vec3(3.0f, 3.0f, -3.0f);
    backWall2.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    backWall2.color = glm::vec3(0.2f, 0.2f, 0.8f); // Blue
    backWall2.materialType = 0;
    
    // Floor (y = -3, normal pointing up) - 2 triangles
    floor1.v0 = glm::vec3(-3.0f, -3.0f, -3.0f);
    floor1.v1 = glm::vec3(-3.0f, -3.0f, 3.0f);  // Swapped v1 and v2
    floor1.v2 = glm::vec3(3.0f, -3.0f, 3.0f);   // Swapped v1 and v2
    floor1.normal = glm::vec3(0.0f, 1.0f, 0.0f); // Pointing up
    floor1.color = glm::vec3(0.8f, 0.8f, 0.8f); // Gray
    floor1.materialType = 0;
    
    floor2.v0 = glm::vec3(-3.0f, -3.0f, -3.0f);
    floor2.v1 = glm::vec3(3.0f, -3.0f, 3.0f);   // Swapped v1 and v2
    floor2.v2 = glm::vec3(3.0f, -3.0f, -3.0f);  // Swapped v1 and v2
    floor2.normal = glm::vec3(0.0f, 1.0f, 0.0f); // Pointing up
    floor2.color = glm::vec3(0.8f, 0.8f, 0.8f); // Gray
    floor2.materialType = 0;
    
    // Ceiling (y = 3, normal pointing down) - 2 triangles
    ceiling1.v0 = glm::vec3(-3.0f, 3.0f, -3.0f);
    ceiling1.v1 = glm::vec3(3.0f, 3.0f, 3.0f);  // Swapped v1 and v2
    ceiling1.v2 = glm::vec3(-3.0f, 3.0f, 3.0f); // Swapped v1 and v2
    ceiling1.normal = glm::vec3(0.0f, -1.0f, 0.0f); // Pointing down
    ceiling1.color = glm::vec3(0.8f, 0.8f, 0.8f); // Gray
    ceiling1.materialType = 0;
    
    ceiling2.v0 = glm::vec3(-3.0f, 3.0f, -3.0f);
    ceiling2.v1 = glm::vec3(3.0f, 3.0f, -3.0f);  // Back to original
    ceiling2.v2 = glm::vec3(3.0f, 3.0f, 3.0f); // Back to original
    ceiling2.normal = glm::vec3(0.0f, -1.0f, 0.0f); // Pointing down
    ceiling2.color = glm::vec3(0.8f, 0.8f, 0.8f); // Gray
    ceiling2.materialType = 0;

    Triangle light1, light2;
    light1.v0 = glm::vec3(-1.0f, 2.99f, -1.0f);
    light1.v1 = glm::vec3(1.0f, 2.99f, -1.0f);
    light1.v2 = glm::vec3(1.0f, 2.99f, 1.0f);
    light1.normal = glm::vec3(0.0f, -1.0f, 0.0f); // Pointing down
    light1.color = glm::vec3(10.0f, 10.0f, 10.0f); // Bright white
    light1.materialType = 1; // Light

    light2.v0 = glm::vec3(-1.0f, 2.99f, -1.0f);
    light2.v1 = glm::vec3(1.0f, 2.99f, 1.0f);
    light2.v2 = glm::vec3(-1.0f, 2.99f, 1.0f);
    light2.normal = glm::vec3(0.0f, -1.0f, 0.0f); // Pointing down
    light2.color = glm::vec3(10.0f, 10.0f, 10.0f); // Bright white
    light2.materialType = 1; // Light

    triangles = {leftWall1, leftWall2, rightWall1, rightWall2, backWall1, backWall2, floor1, floor2, ceiling1, ceiling2, light1, light2};

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
        spheresData.push_back(s.color.x);
        spheresData.push_back(s.color.y);
        spheresData.push_back(s.color.z);
        spheresData.push_back(float(s.materialType)); // material type
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
            spheresData.push_back(s.color.x);
            spheresData.push_back(s.color.y);
            spheresData.push_back(s.color.z);
            spheresData.push_back(float(s.materialType));
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
        trianglesData.push_back(t.color.x);
        trianglesData.push_back(t.color.y);
        trianglesData.push_back(t.color.z);
        // material type
        trianglesData.push_back(float(t.materialType));
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
            trianglesData.push_back(t.color.x);
            trianglesData.push_back(t.color.y);
            trianglesData.push_back(t.color.z);
            // material type
            trianglesData.push_back(float(t.materialType));
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, trianglesSSBO);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, trianglesData.size() * sizeof(float), trianglesData.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        trianglesChanged = false;
    }
}
