#include "Planet.h"
#include <glm/geometric.hpp>

Planet::Planet(PlanetCreateInfo const &createInfo)
    :mass(createInfo.mass), radius(createInfo.radius), pos(createInfo.pos), velocity(createInfo.velocity)
{
	m_Shader.AttachShader({ "res/shaders/planet.frag", GL_FRAGMENT_SHADER });
	m_Shader.AttachShader({ "res/shaders/planet.vert", GL_VERTEX_SHADER });
    m_Shader.setVec3("u_color", createInfo.color);
    m_Shader.setI("u_isLightSource", createInfo.isLightSource);

    m_Noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_Noise.SetSeed(rand());

    model = glm::translate(model, pos);
    
    glGenVertexArrays(1, &m_VAO);

    Generate();
}

Planet::~Planet()
{
    m_Vertices.clear();
    m_Indices.clear();
	glDeleteBuffers(1, &m_VAO);
	m_Shader.DestroyShader();
	m_VBO.DestroyBuffer();
}

void Planet::Generate()
{
    m_Vertices.clear();
    m_Indices.clear();

    m_Vertices.reserve(m_Resolution * m_Resolution * 3);
    m_Indices.reserve((m_Resolution - 1) * (m_Resolution - 1) * 6);

    glm::vec3 localUp[6] = {{0,1,0}, {0,-1,0}, {1,0,0}, {-1,0,0}, {0,0,-1}, {0,0,1}};

    for (int i = 0; i < 6; i++)
    {
        glm::vec3 forward = glm::vec3(localUp[i].y, localUp[i].z, localUp[i].x);
        glm::vec3 right = glm::cross(localUp[i], forward);

        ConstructFaceMesh(localUp[i], forward, right);
    }

	glBindVertexArray(m_VAO);

	m_VBO.UploadData(m_Vertices.data(), m_Vertices.size() * sizeof(float));
	m_IBO.UploadData(m_Indices.data(), m_Indices.size() * sizeof(unsigned int));

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

    m_Shader.setVec2("u_minMax", minMax);
}

void Planet::ConstructFaceMesh(glm::vec3 localUp, glm::vec3 forward, glm::vec3 right)
{
    int vertexOffset = m_Vertices.size() / 3;

    for (int y = 0; y < m_Resolution; y++)
    {
        for (int x = 0; x < m_Resolution; x++)
        {
            glm::vec2 percent = glm::vec2(x, y) / float(m_Resolution - 1);
            glm::vec3 pointOnCube = localUp + (percent.x - 0.5f) * 2.f * forward + (percent.y - 0.5f) * 2.f * right;
            glm::vec3 pointOnUnitSphere = glm::normalize(pointOnCube);

            glm::vec3 calculatedPoint = CalcPointOnPlanet(pointOnUnitSphere);

            m_Vertices.push_back(calculatedPoint.x);
            m_Vertices.push_back(calculatedPoint.y);
            m_Vertices.push_back(calculatedPoint.z);

            if (x < m_Resolution - 1 && y < m_Resolution - 1)
            {
                int topLeft = vertexOffset + x + y * m_Resolution;
                int bottomLeft = vertexOffset + x + (y + 1) * m_Resolution;
                int topRight = vertexOffset + (x + 1) + y * m_Resolution;
                int bottomRight = vertexOffset + (x + 1) + (y + 1) * m_Resolution;

                m_Indices.push_back(topLeft);
                m_Indices.push_back(bottomLeft);
                m_Indices.push_back(topRight);

                m_Indices.push_back(bottomLeft);
                m_Indices.push_back(bottomRight);
                m_Indices.push_back(topRight);
            }
        }
    }
}

float Planet::GetNoise(glm::vec3 point, int i)
{
    float noiseVal = 0;
    float frequency = noiseInfo[i].baseRoughness;
    float amplitude = 1;
    float weight = 1;
    
    for (int i = 0; i < noiseInfo[i].octaves; i++)
    {
        point = point * frequency + noiseInfo[i].center;

        if (noiseInfo[i].isNoiseRigid)
        {
            float v = 1 - std::abs(m_Noise.GetNoise(point.x, point.y, point.z));
            v *= v;
            v *= weight * noiseInfo[i].weightMultiplier;
            weight = v;
            noiseVal += v * amplitude;
        }
        else
        {
            float v = m_Noise.GetNoise(point.x, point.y, point.z);
            noiseVal += (v + 1) * 0.5f * amplitude;
        }

        frequency *= noiseInfo[i].roughness;
        amplitude *= noiseInfo[i].persistance;
    }

    noiseVal = std::max(0.f, noiseVal - noiseInfo[i].minValue);
    return noiseVal * noiseInfo[i].strength;
}

glm::vec3 Planet::CalcPointOnPlanet(glm::vec3 pointOnUnitSphere)
{
    float firstLayerValue = 0;
    float elevation = 0;

    if (noiseInfo.size() > 0)
    {
        firstLayerValue = GetNoise(pointOnUnitSphere, 0);
        if (noiseInfo[0].enabled)
        {
            elevation += firstLayerValue;
        }
    }

    for (int i = 0; i < noiseInfo.size(); i++)
    {
        if (!noiseInfo[i].enabled) continue;

        float mask = (noiseInfo[i].useFirstLayerAsMask) ? firstLayerValue : 1;
        elevation += GetNoise(pointOnUnitSphere, i) * mask;
    }
    elevation = radius * (1 + elevation);

    if (elevation > minMax.y)
    {
        minMax.y = elevation;
    }
    else if (elevation < minMax.x)
    {
        minMax.x = elevation;
    }

    return pointOnUnitSphere * elevation;
}

void Planet::UpdateVel(std::vector<std::unique_ptr<Planet>> &planets, float dt)
{
    glm::vec3 totalGravForce = glm::vec3(0);

    for (const auto& planet2 : planets)
    {
        if (pos == planet2->pos) continue;

        float sqrDistance = std::pow(glm::distance(planet2->pos, pos), 2);
        glm::vec3 forceDir = glm::normalize(planet2->pos - pos);
        glm::vec3 force = forceDir * gConst * mass * planet2->mass / sqrDistance;
        glm::vec3 accel = force / mass;
        
        if (glm::distance(pos, planet2->pos) > radius + planet2->radius)
        {
            velocity += accel * dt;
        }
        else
        {
            velocity = glm::vec3(0.f);
        }
    }
}

void Planet::UpdatePos(float dt)
{
    pos += velocity * dt;
}

void Planet::Draw(const glm::mat4 &view, const glm::mat4 &proj)
{
	m_Shader.Bind();
    m_Shader.setMat4("view", view);
    m_Shader.setMat4("proj", proj);

    model = glm::translate(glm::mat4(1.f), this->pos);
    m_Shader.setMat4("model", model);

	glBindVertexArray(m_VAO);
    m_IBO.Bind();
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, NULL);
    m_IBO.UnBind();
	glBindVertexArray(0);

	m_Shader.UnBind();
}
