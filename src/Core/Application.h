#pragma once

#include "../Game/Universe.h"
#include "../Game/Quad.h"
#include "../OpenGL/Framebuffer.h"
#include "../Game/Camera.h"
#include "Window.h"


class Application
{
public:
	static constexpr int START_WIDTH = 800;
	static constexpr int START_HEIGHT = 600;

	Application();
	~Application();

	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	void run();

private:
	void handleInputs();
	static void mouse_callback(GLFWwindow *window, double xpos, double ypos);

	Window m_Window{ "Planets LESS GO!", START_WIDTH, START_HEIGHT };
	inline static Camera cam{glm::vec3(0, 0, 150)};

	Framebuffer *m_FBdown, *m_FBup;
	Quad *quad;
	Universe *universe;
};
