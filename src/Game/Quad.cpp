#include "Quad.h"

Quad::Quad()
{
	m_Shader.AttachShader({ "res/shaders/quad.frag", GL_FRAGMENT_SHADER });
	m_Shader.AttachShader({ "res/shaders/quad.vert", GL_VERTEX_SHADER });
    m_Shader.Bind();
    m_Shader.setI("screenTex", 0);
    m_Shader.setI("depthTex", 1);

    // m_DownScaleShader.AttachShader({"res/shaders/downscale.comp", GL_COMPUTE_SHADER });

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

void Quad::Draw(glm::vec2 screenExtent, glm::vec3 camPos, glm::mat4 inverseView, glm::mat4 inverseProj)
{
    // m_DownScaleShader.Bind();
    // glDispatchCompute((unsigned int)screenExtent.x, (unsigned int)screenExtent.y, 1);
    // glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

	m_Shader.Bind();
    m_Shader.setVec2("screenExtent", screenExtent);
    m_Shader.setVec3("camPos", camPos);
    m_Shader.setMat4("inverseView", inverseView);
    m_Shader.setMat4("inverseProj", inverseProj);

    glCullFace(GL_BACK);
	glBindVertexArray(m_VAO);
    glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
    glCullFace(GL_FRONT);

	m_Shader.UnBind();
}
