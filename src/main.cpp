#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#include "Shader.h"
#include "RayTracer.h"

const GLuint SCR_WIDTH = 800;
const GLuint SCR_HEIGHT = 600;

// just a full size triangles with full texture coordinates
float quadVertices[] = {
    // vertexes    - cordinates of textures
    -1.0f,  1.0f,     0.0f, 1.0f,
    -1.0f, -1.0f,     0.0f, 0.0f,
     1.0f, -1.0f,     1.0f, 0.0f,
     1.0f,  1.0f,     1.0f, 1.0f
};
GLuint quadIndices[] = {
    0, 1, 2,
    2, 3, 0
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Ray Tracer on GPU", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // for displaying the texture
    Shader displayShader("shaders/quad.vert", "shaders/quad.frag");
    
    RayTracer rayTracer(SCR_WIDTH, SCR_HEIGHT);
    
    GLuint quadVAO, quadVBO, quadEBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &quadEBO);
    
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    displayShader.use();
    displayShader.setInt("computeResult", 0);

    // we have a long way figuring out how to enable randomness, though i saw some mathods using bit manupilations in the shaders
    // we can create a suedo random number in the sahder though 
    // also we can pass a random seed from the cpu when we're calling the render function but i dont think we need that true randomness
    // so we'll stick with suedo random number i guess with using hashfunction or whaterver the magic it does

    // main render loop
    while (!glfwWindowShouldClose(window))
    {
		// rendering the image and updating the outputTexture in the rayTracer object
        rayTracer.render();

        glClear(GL_COLOR_BUFFER_BIT);
        

        displayShader.use();
        glActiveTexture(GL_TEXTURE0);
        // rendering the object simply
        glBindTexture(GL_TEXTURE_2D, rayTracer.getOutputTexture());
        glBindVertexArray(quadVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &quadEBO);

    glfwTerminate();
    return 0;
}
