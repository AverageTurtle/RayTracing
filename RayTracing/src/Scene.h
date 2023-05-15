#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Material {
	glm::vec3 Albedo{ 1.0f };
	float Roughness = 1.0f;
	float Metallic = 0.0f;
};

struct Sphere
{
	glm::vec3 Position{0.0f};
	float Radius = 0.5f;

	int MaterialIndex = 0;
};

struct DirectionalLight
{
	glm::vec3 Direction = glm::vec3(0.7f, -0.9f, 0.5f);
	float Intesity = 2.0f;
	glm::vec3 Color = glm::vec3(1.0f);
};
struct PointLight
{
	glm::vec3 Position{ 0.0f, 6.0f, 0.0f };
	float Intesity = 2.0f;
	glm::vec3 Color = glm::vec3(1.0f);
};

struct Scene
{
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;
	std::vector<DirectionalLight> DirectionalLights;
	std::vector<PointLight> PointLights;
};