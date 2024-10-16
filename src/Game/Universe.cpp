#include "Universe.h"

// #include <imgui/imgui.h>
// #include <imgui/imgui_impl_glfw.h>
// #include <imgui/imgui_impl_opengl3.h>

Universe::Universe()
{
    float sunMass = 1000.f;
    float sunRadius = 500.f;
    glm::vec3 sunPosition = {0.f, 2.f, 0.f};
    glm::vec3 sunVelocity = {0.f, 0.f, 0.f};

    m_Planets.emplace_back(std::make_unique<Planet>(
        PlanetCreateInfo{
            sunPosition,
            sunVelocity,
            {4.0f, 4.0f, 3.0f},
            sunRadius,
            sunMass,
            true
        }
    ));

    m_Planets[0]->noiseInfo.emplace_back(
        PlanetNoiseInfo{
            .enabled = true,
            .useFirstLayerAsMask = false,
            .baseRoughness = 6.05,
            .roughness = 9.6,
            .persistance = 0.6f,
            .strength = 0.1,
            .minValue = 0.45f,
            .octaves = 5,
            .isNoiseRigid = false,
            .center = {96.f, -33.f, 40.f}
        }
    );
    m_Planets[0]->noiseInfo.emplace_back(
        PlanetNoiseInfo{
            .enabled = true,
            .useFirstLayerAsMask = true,
            .baseRoughness = 4.16,
            .roughness = 40.65,
            .persistance = 0.5f,
            .strength = 0.4,
            .minValue = 1.25f,
            .octaves = 6,
            .isNoiseRigid = true,
            .center = {2.85f, 0.8f, 2.1f}
        }
    );

    m_Planets[0]->Generate();

    float earthMass = 6.0e-3;
    float earthRadius = 100.f;
    glm::vec3 earthPosition = {3000.f, 0.f, 0.f};
    float gConst = 1.0f;
    float escVelocityMag = std::sqrt(gConst * sunMass / glm::length(earthPosition - sunPosition));
    glm::vec3 earthVelocity = {0.f, escVelocityMag, 0.f};

    m_Planets.emplace_back(std::make_unique<Planet>(
        PlanetCreateInfo{
            earthPosition,
            earthVelocity,
            {0.4f, 0.2f, 0.3f},
            earthRadius,
            earthMass,
            false
        }
    ));

	m_Planets[1]->noiseInfo.emplace_back(
		PlanetNoiseInfo{
			.enabled = true,
			.useFirstLayerAsMask = false,
			.baseRoughness = 6.05,
			.roughness = 9.6,
			.persistance = 0.6f,
			.strength = 0.1,
			.minValue = 0.45f,
			.octaves = 5,
			.isNoiseRigid = false,
			.center = {96.f, -33.f, 40.f}
		}
	);
	m_Planets[1]->noiseInfo.emplace_back(
		PlanetNoiseInfo{
			.enabled = true,
			.useFirstLayerAsMask = true,
			.baseRoughness = 4.16,
			.roughness = 40.65,
			.persistance = 0.5f,
			.strength = 0.4f,
			.minValue = 1.25f,
			.octaves = 6,
			.isNoiseRigid = true,
			.center = {2.85f, 0.8f, 2.1f}
		}
	);

	m_Planets[1]->Generate();

    // ImGui Garbage

    // IMGUI_CHECKVERSION();
	// ImGui::CreateContext();
	// ImGuiIO &io = ImGui::GetIO(); (void)io;
	// ImGui::StyleColorsDark();
	// ImGui_ImplGlfw_InitForOpenGL(m_Window.getGLFWwindow(), true);
	// ImGui_ImplOpenGL3_Init("#version 330");
}

Universe::~Universe()
{
    for (int i=0; i<m_Planets.size(); i++)
    {
        m_Planets[i].release();
    }
    m_Planets.clear();


	// ImGui_ImplOpenGL3_Shutdown();
	// ImGui_ImplGlfw_Shutdown();
	// ImGui::DestroyContext();
}

void Universe::Draw(glm::mat4 view, glm::mat4 proj, float dt)
{
    for (const auto &planet : m_Planets)
    {
        planet->UpdateVel(m_Planets, dt);
    }

    for (const auto &planet : m_Planets)
    {
        planet->UpdatePos(dt);
        planet->Draw(view, proj);
    }


    // Imgui garbage

    // ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplGlfw_NewFrame();
    // ImGui::NewFrame();

    // m_FB->bindTex();
    // quad->Draw();

    // ImGui::Begin("Planet Settings");

    // if (ImGui::InputInt("No. of layers", &numberOfLayers))
    // {
    //     m_Planets[0]->noiseInfo.resize(numberOfLayers);
    // }

    // shouldGeneratePlanet = false;
    // for (int i = 0; i < m_Planets[0]->noiseInfo.size(); i++)
    // {
    //     auto &noiseInfo = m_Planets[0]->noiseInfo[i];

    //     ImGui::Begin(std::to_string(i).c_str());
    //     if (ImGui::Checkbox("Enabled", &noiseInfo.enabled) ||
    //         ImGui::Checkbox("Use First Layer As Mask", &noiseInfo.useFirstLayerAsMask) ||
    //         ImGui::DragFloat("Base Roughness", &noiseInfo.baseRoughness, 0.05f) ||
    //         ImGui::DragFloat("Roughness", &noiseInfo.roughness, 0.05f) ||
    //         ImGui::DragFloat("Strength", &noiseInfo.strength, 0.05f) ||
    //         ImGui::DragFloat("Min value", &noiseInfo.minValue, 0.05f) ||
    //         ImGui::DragFloat("Persistance", &noiseInfo.persistance, 0.05f) ||
    //         ImGui::DragInt("Octaves", &noiseInfo.octaves) ||
    //         ImGui::DragFloat3("Center", &noiseInfo.center[0], 0.05f) ||
    //         ImGui::Checkbox("Rigid Noise", &noiseInfo.isNoiseRigid))
    //     {
    //         shouldGeneratePlanet = true;
    //     }

    //     if (noiseInfo.isNoiseRigid)
    //     {
    //         if (ImGui::DragFloat("Weight Multiplier", &noiseInfo.weightMultiplier, 0.05))
    //         {
    //             shouldGeneratePlanet = true;
    //         }
    //     }

    //     ImGui::End();
    // }

    // ImGui::End();

    // ImGui::Render();
    // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // if (shouldGeneratePlanet)
    // {
    //     m_Planets[0]->Generate();
    // }
}
