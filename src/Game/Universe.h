#pragma once

#include "../Game/Planet.h"

#include <glm/glm.hpp>
#include <vector>
#include <memory>

class Universe
{
public:
    Universe();
    ~Universe();

    Universe(const Universe&) = delete;
	Universe& operator=(const Universe&) = delete;

    void Draw(glm::mat4 view, glm::mat4 proj, float dt);

private:
    std::vector<std::unique_ptr<Planet>> m_Planets;
};