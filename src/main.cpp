#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// ImGui includes
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

const int SCR_WIDTH = 1280;
const int SCR_HEIGHT = 720;

int main()
{
    // Initialize GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ImGui Test Window", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Print OpenGL info
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    
    // Customize the style for a more professional look
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.ScrollbarRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(4, 3);
    style.ItemSpacing = ImVec2(8, 4);
    style.ItemInnerSpacing = ImVec2(4, 4);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll events
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Get the main viewport size
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_size = viewport->WorkSize;
        
        // Define panel dimensions
        const float left_panel_width = 300.0f;
        const float right_panel_width = 300.0f;
        const float viewport_width = work_size.x - left_panel_width - right_panel_width;
        
        // Left Panel - Tools and Controls
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(left_panel_width, work_size.y));
        ImGui::Begin("Tools & Controls", nullptr, 
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        
        // Tools section
        if (ImGui::CollapsingHeader("Scene Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
            static float camera_speed = 2.5f;
            static float fov = 45.0f;
            static float exposure = 1.0f;
            
            ImGui::SliderFloat("Camera Speed", &camera_speed, 0.1f, 10.0f);
            ImGui::SliderFloat("Field of View", &fov, 10.0f, 120.0f);
            ImGui::SliderFloat("Exposure", &exposure, 0.1f, 5.0f);
            
            if (ImGui::Button("Reset Camera", ImVec2(-1, 0))) {
                // Reset camera logic will go here
            }
        }
        
        if (ImGui::CollapsingHeader("Ray Tracing Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            static int max_bounces = 8;
            static int samples_per_pixel = 16;
            static bool use_denoising = true;
            
            ImGui::SliderInt("Max Bounces", &max_bounces, 1, 32);
            ImGui::SliderInt("Samples/Pixel", &samples_per_pixel, 1, 256);
            ImGui::Checkbox("Use Denoising", &use_denoising);
        }
        
        if (ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
            ImGui::Text("GPU: N/A"); // Will be filled with actual GPU info later
        }
        
        ImGui::End();
        
        // Right Panel - Scene Hierarchy and Properties
        ImGui::SetNextWindowPos(ImVec2(left_panel_width + viewport_width, 0));
        ImGui::SetNextWindowSize(ImVec2(right_panel_width, work_size.y));
        ImGui::Begin("Scene & Properties", nullptr, 
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        
        if (ImGui::CollapsingHeader("Scene Hierarchy", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Scene tree will go here
            if (ImGui::TreeNode("Camera")) {
                ImGui::Text("Position: (0.0, 1.5, 2.0)");
                ImGui::Text("Rotation: (0.0, 0.0, 0.0)");
                ImGui::TreePop();
            }
            
            if (ImGui::TreeNode("Lights")) {
                if (ImGui::TreeNode("Main Light")) {
                    static float light_color[3] = {1.0f, 1.0f, 1.0f};
                    static float light_intensity = 1.0f;
                    
                    ImGui::ColorEdit3("Color", light_color);
                    ImGui::SliderFloat("Intensity", &light_intensity, 0.0f, 10.0f);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
            
            if (ImGui::TreeNode("Objects")) {
                ImGui::Text("Sphere 1");
                ImGui::Text("Cube 1");
                ImGui::Text("Plane 1");
                ImGui::TreePop();
            }
        }
        
        if (ImGui::CollapsingHeader("Material Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
            static float roughness = 0.5f;
            static float metallic = 0.0f;
            static float ior = 1.45f;
            static float material_color[3] = {0.8f, 0.2f, 0.2f};
            
            ImGui::ColorEdit3("Base Color", material_color);
            ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f);
            ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f);
            ImGui::SliderFloat("IOR", &ior, 1.0f, 3.0f);
        }
        
        ImGui::End();
        
        // Viewport area (empty for now - this is where your ray tracer will render)
        // We'll draw a simple placeholder rectangle to show the viewport area
        ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
        ImVec2 viewport_min = ImVec2(left_panel_width, 0);
        ImVec2 viewport_max = ImVec2(left_panel_width + viewport_width, work_size.y);
        
        // Draw viewport border
        draw_list->AddRect(viewport_min, viewport_max, IM_COL32(100, 100, 100, 255), 0.0f, 0, 2.0f);
        
        // Draw placeholder text in viewport
        ImVec2 text_size = ImGui::CalcTextSize("Viewport Area\n(Ray Tracer will render here)");
        ImVec2 text_pos = ImVec2(
            viewport_min.x + (viewport_width - text_size.x) * 0.5f,
            viewport_min.y + (work_size.y - text_size.y) * 0.5f
        );
        draw_list->AddText(text_pos, IM_COL32(150, 150, 150, 255), "Viewport Area\n(Ray Tracer will render here)");

        // Rendering
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
