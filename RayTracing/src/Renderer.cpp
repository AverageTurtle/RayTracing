#include "Renderer.h"

#include "Walnut/Random.h"
#include <iostream>

#include <cmath> 

namespace RayTracing {
	namespace Utils {
		static uint32_t ConvertToRGBA(const glm::vec4& color) {
			uint8_t r = (uint8_t)(color.r * 255.0f);
			uint8_t g = (uint8_t)(color.g * 255.0f);
			uint8_t b = (uint8_t)(color.b * 255.0f);
			uint8_t a = (uint8_t)(color.a * 255.0f);

			return (a << 24) | (b << 16) | (g << 8) | r;
		}
	}

	void Renderer::OnResize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
			return;

		if (m_FinalImage)
		{
			if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
				return;

			m_FinalImage->Resize(width, height);
		}
		else
		{
			m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
		}
		
		delete[] m_ImageData;
		m_ImageData = new uint32_t[width * height];
	}

	void Renderer::Render(const Scene& scene, const Camera& camera)
	{
		if (m_FinalImage == nullptr)
			return;

		Ray ray;
		ray.Origin = camera.GetPosition();

		const std::vector<glm::vec3>& rayDirections = camera.GetRayDirections();

		for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) 
		{
			for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
			{
				ray.Direction = rayDirections[x + y * m_FinalImage->GetWidth()];

				glm::vec4 color = TraceRay(scene, ray);
				color = glm::clamp(color, 0.0f, 1.0f);

				m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
			}
		}
		
		m_FinalImage->SetData(m_ImageData);
	}

	glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
	{
		glm::vec3 colorOut = glm::vec3(0.6f, 0.6f, 1.0f);

		if(scene.Spheres.size() == 0)
			return glm::vec4(colorOut, 1.0f);

		const Sphere* closestSphere = nullptr;
		float hitDistance = FLT_MAX;
		for (const Sphere& sphere : scene.Spheres) {
			glm::vec3 origin = ray.Origin - sphere.Position;

			glm::vec3 lightDir = glm::vec3(-0.5f, -0.8, -0.5);

			float a = glm::dot(ray.Direction, ray.Direction);
			float b = 2.0f * glm::dot(origin, ray.Direction);
			float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

			// Quadratic Forumula discriminant:
			// b^2 - 4ac
			float discriminant = b * b - 4.0f * a * c;
			if (discriminant < 0.0f)
				continue;

			//float t0 = -b + sqrt(discriminant) / (2.0f * a);
			float closestT = (-b - sqrt(discriminant)) / (2.0f * a);
			if (closestT < 0)
				continue;

			if (closestT < hitDistance) {
				hitDistance = closestT;
				closestSphere = &sphere;
			}
		}

		if (closestSphere == nullptr)
			return glm::vec4(colorOut, 1.0f);

		glm::vec3 origin = ray.Origin - closestSphere->Position;
		glm::vec3 hitpoint = origin + ray.Direction * hitDistance;
		glm::vec3 normal = hitpoint - glm::vec3{0.0f, 0.0f, 0.0f};
		normal = glm::normalize(normal);

		float light = glm::max(glm::dot(normal, -lightDir), 0.0f);
		colorOut = light * closestSphere->Albedo;
		return glm::vec4(colorOut, 1.0f);
	}
}