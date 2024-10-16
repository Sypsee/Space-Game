#include "Window.h"

#include <iostream>

Window::Window(const char* title, const int width, const int height)
	:m_WindowRes{width, height}
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_Window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (m_Window == NULL)
	{
		std::cerr << "Error while creating glfwWindow!" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(m_Window);
    glfwSetWindowUserPointer(m_Window, this);
    glfwSetFramebufferSizeCallback(m_Window, framebufferResizeCallback);
}

Window::~Window()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void Window::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    auto myWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    myWindow->m_FramebufferResized = true;
    myWindow->m_WindowRes.x = width;
    myWindow->m_WindowRes.y = height;
}