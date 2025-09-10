#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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


// camera settings
glm::vec3 camPos = glm::vec3(0.0f, 1.5f, 2.0f);
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float fov = 45.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float speed = 2.5f;

bool isRightMousebutton = false;

// camera controlls from one of my older projects
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!isRightMousebutton) return;

    float xoffset = float(xpos) - lastX;
    float yoffset = lastY - float(ypos);
    lastX = float(xpos);
    lastY = float(ypos);

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camFront = glm::normalize(front);
}

// process keyboard input
void processInput(GLFWwindow* window)
{
    float currentSpeed = speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camPos += currentSpeed * camFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camPos -= currentSpeed * camFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camPos -= glm::normalize(glm::cross(camFront, camUp)) * currentSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camPos += glm::normalize(glm::cross(camFront, camUp)) * currentSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camPos += currentSpeed * camUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camPos -= currentSpeed * camUp;
}

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
    glfwSetCursorPosCallback(window, mouse_callback);

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
        // to calculate speed of the camera and also we can use it to show the current fps
        float currentFrame = float(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        bool rightPressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        if (rightPressed != isRightMousebutton) {
            isRightMousebutton = rightPressed;
            glfwSetInputMode(window, GLFW_CURSOR, rightPressed ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            if (rightPressed) {
                double xpos, ypos;
                glfwGetCursorPos(window, &xpos, &ypos);
                lastX = float(xpos);
                lastY = float(ypos);
            }
        }

        processInput(window);

        // compute the ray traced image
        rayTracer.render(camPos, camPos + camFront, camUp);

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
