#include "Render.hpp"
#include "Game.h"

Render::Render(unsigned int w, unsigned int h, std::string title) {
    InitGLFW(w, h, title);
    InitGLEW();
}

void Render::InitGLFW(unsigned int w, unsigned int h, std::string title) {

    if (!glfwInit()) {
        const char* desc;
        glfwGetError(&desc);
        throw std::runtime_error(std::string("GLFW init failed: ") + desc);
    }

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(w, h, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        const char* desc;
        glfwGetError(&desc);
        throw std::runtime_error(std::string("GLFW init failed: ") + desc);
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
}

void Render::InitGLEW() {
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to init GLEW\n");
    }

    int w, h;
    glfwGetWindowSize(window, &w, &h);

    glViewport(0, 0, w, h);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Render::Update() {
    if (GetGameState() == GameState::MainMenu) {
        // draw, and input ip?
    }
}