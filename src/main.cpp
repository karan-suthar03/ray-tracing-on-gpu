#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// ImGui includes
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "RayTracer.h"

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;
constexpr float LEFT_PANEL_WIDTH = 300.0f;
constexpr float RIGHT_PANEL_WIDTH = 300.0f;

struct CameraState {
    glm::vec3 position{0.0f, 0.0f, 5.0f};
    glm::vec3 target{0.0f, 0.0f, 0.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    float speed = 0.1f;
} g_camera;

GLFWwindow* createWindow();
void setupImGui(GLFWwindow* window);
void renderUI(RayTracer* rayTracer);
void renderSceneControlsPanel();
void renderScenePropertiesPanel(float viewportWidth);
void renderViewport(RayTracer* rayTracer, float viewportWidth, float viewportHeight);

int main() {
    glfwInit();
    GLFWwindow* window = createWindow();
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    setupImGui(window);
    
    std::unique_ptr<RayTracer> rayTracer = std::make_unique<RayTracer>(SCREEN_WIDTH, SCREEN_HEIGHT);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        renderUI(rayTracer.get());
        
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
GLFWwindow* createWindow() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ray Tracing", nullptr, nullptr);
    glfwSwapInterval(1);
    return window;
}

void setupImGui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.ScrollbarRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(4, 3);
    style.ItemSpacing = ImVec2(8, 4);
    style.ItemInnerSpacing = ImVec2(4, 4);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");
}

void renderUI(RayTracer* rayTracer) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 workSize = viewport->WorkSize;
    
    const float viewportWidth = workSize.x - LEFT_PANEL_WIDTH - RIGHT_PANEL_WIDTH;
    
    renderSceneControlsPanel();
    renderScenePropertiesPanel(viewportWidth);
    renderViewport(rayTracer, viewportWidth, workSize.y);
}

void renderSceneControlsPanel() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 workSize = viewport->WorkSize;
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(LEFT_PANEL_WIDTH, workSize.y));
    ImGui::Begin("Tools & Controls", nullptr, 
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
                ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    
    ImGui::End();
}

void renderScenePropertiesPanel(float viewportWidth) {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 workSize = viewport->WorkSize;
    
    ImGui::SetNextWindowPos(ImVec2(LEFT_PANEL_WIDTH + viewportWidth, 0));
    ImGui::SetNextWindowSize(ImVec2(RIGHT_PANEL_WIDTH, workSize.y));
    ImGui::Begin("Scene & Properties", nullptr, 
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
                ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    
    ImGui::End();
}

void renderViewport(RayTracer* rayTracer, float viewportWidth, float viewportHeight) {
    static float topSectionHeight = viewportHeight * 0.25f;
    static float bottomSectionHeight = viewportHeight * 0.25f;
    
    const float minSectionHeight = 50.0f;
    const float splitterHeight = 8.0f;
    
    float middleSectionHeight = viewportHeight - topSectionHeight - bottomSectionHeight - 2 * splitterHeight;
    
    ImGui::SetNextWindowPos(ImVec2(LEFT_PANEL_WIDTH, 0));
    ImGui::SetNextWindowSize(ImVec2(viewportWidth, viewportHeight));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport", nullptr, 
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
                ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    
    // Top Section
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    ImGui::BeginChild("TopSection", ImVec2(viewportWidth, topSectionHeight), true, ImGuiWindowFlags_None);
    ImVec2 textSize = ImGui::CalcTextSize("Top Section");
    ImVec2 textPos = ImVec2((viewportWidth - textSize.x) * 0.5f, (topSectionHeight - textSize.y) * 0.5f);
    ImGui::SetCursorPos(textPos);
    ImGui::TextDisabled("Top Section");
    ImGui::EndChild();
    ImGui::PopStyleColor();
    
    // Top Splitter
    ImGui::Button("##TopSplitter", ImVec2(viewportWidth, splitterHeight));
    if (ImGui::IsItemActive()) {
        topSectionHeight += ImGui::GetIO().MouseDelta.y;
        topSectionHeight = std::max(minSectionHeight, topSectionHeight);
    }
    
    // Middle Section
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    ImGui::BeginChild("MiddleSection", ImVec2(viewportWidth, middleSectionHeight), true, ImGuiWindowFlags_None);
    
    if (rayTracer) {
        rayTracer->render(g_camera.position, g_camera.target, g_camera.up);
        GLuint renderedTexture = rayTracer->getOutputTexture();
        
        if (renderedTexture > 0) {
            float aspectRatio = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
            float imageWidth = viewportWidth - 20.0f;
            float imageHeight = imageWidth / aspectRatio;
            
            if (imageHeight > middleSectionHeight - 20.0f) {
                imageHeight = middleSectionHeight - 20.0f;
                imageWidth = imageHeight * aspectRatio;
            }
            
            ImVec2 imagePos = ImVec2((viewportWidth - imageWidth) * 0.5f, (middleSectionHeight - imageHeight) * 0.5f);
            ImGui::SetCursorPos(imagePos);
            
            ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(renderedTexture)), 
                        ImVec2(imageWidth, imageHeight));
        }
    }
    
    ImGui::EndChild();
    ImGui::PopStyleColor();
    
    // Bottom Splitter
    ImGui::Button("##BottomSplitter", ImVec2(viewportWidth, splitterHeight));
    if (ImGui::IsItemActive()) {
        bottomSectionHeight -= ImGui::GetIO().MouseDelta.y;
        bottomSectionHeight = std::max(minSectionHeight, bottomSectionHeight);
    }
    
    // Bottom Section
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    ImGui::BeginChild("BottomSection", ImVec2(viewportWidth, bottomSectionHeight), true, ImGuiWindowFlags_None);
    textSize = ImGui::CalcTextSize("Bottom Section");
    textPos = ImVec2((viewportWidth - textSize.x) * 0.5f, (bottomSectionHeight - textSize.y) * 0.5f);
    ImGui::SetCursorPos(textPos);
    ImGui::TextDisabled("Bottom Section");
    ImGui::EndChild();
    ImGui::PopStyleColor();
    
    ImGui::End();
    ImGui::PopStyleVar();
}
