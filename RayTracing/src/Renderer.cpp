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

	void Renderer::Render(const Camera& camera)
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

				glm::vec4 color = TraceRay(ray);
				color = glm::clamp(color, 0.0f, 1.0f);

				m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
			}
		}
		
		m_FinalImage->SetData(m_ImageData);
	}

	glm::vec4 Renderer::TraceRay(const Ray& ray)
	{
		glm::vec3 colorOut = glm::vec3(0.7f, 0.7f, 1.0f);

		// a = rayorigin
		// b = ray directionn

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(ray.Origin, ray.Direction);
		float c = glm::dot(ray.Origin, ray.Origin) - radius * radius;

		// Quadratic Forumula discriminant:
		// b^2 - 4ac
		float discriminant = b * b - 4.0f * a * c;

		if (discriminant < 0.0f)
			return glm::vec4(colorOut, 1.0f);

		//float t0 = -b + sqrt(discriminant) / (2.0f * a);
		float ClosestT = (-b - sqrt(discriminant)) / (2.0f * a);

		glm::vec3 hitpoint = ray.Origin + ray.Direction * ClosestT;
		glm::vec3 normal = hitpoint - sphereOrigin;
		normal = glm::normalize(normal);

		float light = glm::max(glm::dot(normal, -lightDir), 0.0f);
		colorOut = light * (0.5f * glm::vec3(normal.x + 1, normal.y + 1, normal.z + 1));
		return glm::vec4(colorOut, 1.0f);
	}
}