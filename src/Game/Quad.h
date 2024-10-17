#pragma once

#include "../OpenGL/Shader.h"
#include "../OpenGL/Buffer.h"

class Quad
{
public:
	Quad();
	~Quad();
	
	void Draw(glm::vec2 screenExtent);

private:
	const float vertices[30] = {
        -1.0F, -1.0F, 0.0F,  0.0F, 0.0F,
        1.0F, -1.0F, 0.0F,   1.0F, 0.0F,
        1.0F,  1.0F, 0.0F,   1.0F, 1.0F,
        1.0F,  1.0F, 0.0F,   1.0F, 1.0F,
        -1.0F,  1.0F, 0.0F,  0.0F, 1.0F,
        -1.0F, -1.0F, 0.0F,  0.0F, 0.0F,
    };

	Shader m_Shader;
    Shader m_DownScaleShader;
	Buffer m_VBO{ {GL_ARRAY_BUFFER, NULL, sizeof(vertices), GL_STATIC_DRAW}};
	unsigned int m_VAO=0;
};
