#include "Quad.h"

Quad::Quad()
{
	m_Shader.AttachShader({ "res/shaders/quad.frag", GL_FRAGMENT_SHADER });
	m_Shader.AttachShader({ "res/shaders/quad.vert", GL_VERTEX_SHADER });
    m_Shader.setI("screenTex", 0);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	m_VBO.UploadData(vertices, sizeof(vertices));

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

Quad::~Quad()
{
	glDeleteBuffers(1, &m_VAO);
	m_Shader.DestroyShader();
	m_VBO.DestroyBuffer();
}

void Quad::Draw(glm::vec2 screenExtent)
{
	m_Shader.Bind();
    m_Shader.setVec2("screenExtent", screenExtent);

	glDisable(GL_CULL_FACE);
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);

	m_Shader.UnBind();
}