#include "RayTracer.h"
#include <vector>

RayTracer::RayTracer(GLuint width, GLuint height)
    : width(width), height(height), frameCount(0), prevCamPos(0.0f), prevCamTarget(0.0f), prevCamUp(0.0f), spheresChanged(true)
{
    spheres = {
        {{0.0f, 0.0f, 0.0f}, 0.5f, {1.0f, 0.0f, 0.0f}, 0}, // Lambertian
        {{2.0f, 2.0f, 0.0f}, 1.0f, {10.0f, 10.0f, 10.0f}, 1}, // Light (bright white)
        {{0.0f, -100.5f, 0.0f}, 100.0f, {0.5f, 0.5f, 0.5f}, 0} // Lambertian
    };

    setupTexture();
    setupShader();
    setupSSBO();
}

RayTracer::~RayTracer()
{
    glDeleteTextures(1, &outputTexture);
    glDeleteBuffers(1, &ssbo);
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

    computeShader->use();

    // passing camera uniforms
    computeShader->setVec3("camPos", cameraPos);
    computeShader->setVec3("camTarget", cameraTarget);
    computeShader->setVec3("camUp", cameraUp);
    computeShader->setInt("frameCount", frameCount);
    computeShader->setVec2("resolution", glm::vec2(width, height));
    computeShader->setInt("numSpheres", spheres.size());

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
