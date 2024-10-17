#pragma once

#include "../OpenGL/Buffer.h"
#include "../OpenGL/Shader.h"

#include <glm/glm.hpp>
#include <FastNoise/FastNoiseLite.h>

#include <cstdint>
#include <vector>
#include <memory>

static constexpr uint32_t m_Resolution = 64;
static constexpr float gConst = 0.01;

struct PlanetCreateInfo
{
    glm::vec3 pos={0,0,0};
    glm::vec3 velocity={0,0,0};
    glm::vec3 color={1,1,1};
    float radius=1.f;
    float mass=1.f;
    bool isLightSource = false;
};

struct PlanetNoiseInfo
{
    bool enabled = false;
    bool useFirstLayerAsMask = false;
    float baseRoughness = 0.91f;
    float roughness = 5.4f;
    float persistance = 0.6f;
    float strength = 0.5f;
    float minValue = 5.5f;
    int octaves = 5;
    float weightMultiplier = 1.0f;
    bool isNoiseRigid = false;
    glm::vec3 center = {2,2,2};
};

class Planet
{
public:
	Planet(PlanetCreateInfo const &createInfo={});
	~Planet();

    void Generate();

    void UpdateVel(std::vector<std::unique_ptr<Planet>> &planets, float dt);
    void UpdatePos(float dt);

    void Draw(const glm::mat4 &view, const glm::mat4 &proj);

    float mass = 0.f;
    float radius = 0.f;
    glm::vec3 pos{0.f};
    glm::vec3 velocity{0.f};

    // Planet Noise
    
    std::vector<PlanetNoiseInfo> noiseInfo;

private:
    void ConstructFaceMesh(glm::vec3 localUp, glm::vec3 forward, glm::vec3 right);
    float GetNoise(glm::vec3 point, int i);
    glm::vec3 CalcPointOnPlanet(glm::vec3 pointOnUnitSphere);

    std::vector<float> m_Vertices;
    std::vector<unsigned int> m_Indices;
	Buffer m_VBO{ {GL_ARRAY_BUFFER, NULL, sizeof(m_Vertices), GL_STATIC_DRAW}};
    Buffer m_IBO{ {GL_ELEMENT_ARRAY_BUFFER, NULL, sizeof(m_Indices), GL_STATIC_DRAW}};
    Shader m_Shader{};
    unsigned int m_VAO=0;
    
    glm::vec2 minMax{0.f, 0.f};

    glm::mat4 model{1};

    FastNoiseLite m_Noise;
};
