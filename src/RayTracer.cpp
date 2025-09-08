#include "RayTracer.h"

RayTracer::RayTracer(GLuint width, GLuint height)
    : width(width), height(height)
{
    setupTexture();
    setupShader();
}

RayTracer::~RayTracer()
{
    glDeleteTextures(1, &outputTexture);
    delete computeShader;
}

void RayTracer::render()
{
    computeShader->use();
	// we are going to make worker groups with each of them containing 16 * 16 threads as defined in the compute shader
    // we are adding 15 to ensure we round up when the dimensions are not multiples of 16
	// coordinates (id's which we are using as pixel cordinates) are not in the bounds of the size of the screen then the shader will automatically discard them
	// as written in the compute shader
    GLuint workGroupsX = (width + 15) / 16;
    GLuint workGroupsY = (height + 15) / 16;
    computeShader->dispatchCompute(workGroupsX, workGroupsY, 1);
    
	// this is the barrier to ensure that the writes to the image have finished before we use it
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
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
