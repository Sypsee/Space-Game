#include "Application.h"

void Application::mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    cam.mouse_callback(xpos, ypos);
}

void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
	auto const src_str = [source]() {
		switch (source)
		{
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		}
		}();

	auto const type_str = [type]() {
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
		}
		}();

	auto const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		}
	}();
	std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
}

Application::Application()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	glViewport(0, 0, START_WIDTH, START_HEIGHT);
	glfwSetCursorPosCallback(m_Window.getGLFWwindow(), mouse_callback);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	// glEnable(GL_DEBUG_OUTPUT);
    // glDebugMessageCallback(message_callback, nullptr);


	m_FBdown = new Framebuffer(
        Framebuffer::CreateInfo{
            std::span<const Framebuffer::Attachment>{
                std::array<Framebuffer::Attachment, 2>{
                    Framebuffer::Attachment{GL_COLOR_ATTACHMENT0, START_WIDTH, START_HEIGHT, 3},
                    Framebuffer::Attachment{GL_DEPTH_ATTACHMENT, START_WIDTH, START_HEIGHT}
                }
            }
        }
    );

	// m_FBup = new Framebuffer(
 //        Framebuffer::CreateInfo{
 //            std::span<const Framebuffer::Attachment>{
 //                std::array<Framebuffer::Attachment, 2>{
 //                    Framebuffer::Attachment{GL_COLOR_ATTACHMENT0, 100, 75, 3, 1/2},
 //                    Framebuffer::Attachment{GL_DEPTH_ATTACHMENT, START_WIDTH, START_HEIGHT}
 //                }
 //            }
 //        }
 //    );

	quad = new Quad();
	universe = new Universe();
}

Application::~Application()
{
}

void Application::run()
{
	float currentFrame = glfwGetTime();
	float lastFrame = currentFrame;
	float dt = 0;

	while (!m_Window.shouldClose())
	{
		glfwPollEvents();
		handleInputs();
		
		currentFrame = glfwGetTime();
		dt = std::max(currentFrame - lastFrame, 0.05f);
		lastFrame = currentFrame;

		if (m_Window.wasWindowResized())
		{
			m_Window.resetWindowResizeFlag();
			glViewport(0, 0, m_Window.getWindowRes().x, m_Window.getWindowRes().y);
			cam.setAspectRatio(m_Window.getWindowRes().x/m_Window.getWindowRes().y);
			m_FBdown->changeRes(m_Window.getWindowRes().x, m_Window.getWindowRes().y, 0);
		}

		m_FBdown->bind();

		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cam.update(m_Window.getGLFWwindow());
		universe->Draw(cam.getViewMatrix(), cam.getProjMatrix(), dt);

		m_FBdown->unbind();

        m_FBdown->bindTex(0);
        m_FBdown->bindImage(0, 0);
		quad->Draw(m_Window.getWindowRes());

		glfwSwapBuffers(m_Window.getGLFWwindow());
	}
}

void Application::handleInputs()
{
	if (glfwGetKey(m_Window.getGLFWwindow(), GLFW_KEY_O) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (glfwGetKey(m_Window.getGLFWwindow(), GLFW_KEY_P) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	// This causes seg fault cause I AM NOT DESTROYING ANYTHING. ok I will not idc.
	else if (glfwGetKey(m_Window.getGLFWwindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(m_Window.getGLFWwindow(), true);
	}
}
