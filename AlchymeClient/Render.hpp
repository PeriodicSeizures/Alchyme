#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <string>

class Render {
	GLFWwindow* window;

    void InitGLFW(unsigned int w, unsigned int h, std::string title);
    void InitGLEW();

    void HandleEvents();

public:
    Render(unsigned int w, unsigned int h, std::string title);

    void Update();
};