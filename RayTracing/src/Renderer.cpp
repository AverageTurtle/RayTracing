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

	void Renderer::Render()
	{
		for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) 
		{
			for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
			{
				glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };
				coord = coord * 2.0f - 1.0f;
				coord.x *= m_FinalImage->GetWidth() / (float)m_FinalImage->GetHeight();

				glm::vec4 color = PerPixel(coord);
				color = glm::clamp(color, 0.0f, 1.0f);

				m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
			}
		}
		
		m_FinalImage->SetData(m_ImageData);
	}

	glm::vec4 Renderer::PerPixel(glm::vec2 coord)
	{
		glm::vec3 colorOut = glm::vec3(0.8f + (( - coord.y - 1.0f) * 0.2), 0.8f + ((-coord.y - 1.0f) * 0.2), 1.0f);
	
		glm::vec3 rayOrigin(0.0f, 0.0f, 1.5f);
		glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
		rayDirection = glm::normalize(rayDirection);

		float radius = 0.5;
		glm::vec3 sphereOrigin = glm::vec3(0.0f);
		glm::vec3 lightDir = glm::vec3(-0.5f, -0.8, -0.5);

		// a = rayorigin
		// b = ray directionn

		float a = glm::dot(rayDirection, rayDirection);
		float b = 2.0f * glm::dot(rayOrigin, rayDirection);
		float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

		// Quadratic Forumula discriminant:
		// b^2 - 4ac
		float discriminant = b * b - 4.0f * a * c;

		if (discriminant < 0.0f)
			return glm::vec4(colorOut, 1.0f);


		//float t0 = -b + sqrt(discriminant) / (2.0f * a);
		float ClosestT = (-b - sqrt(discriminant)) / (2.0f * a);

		glm::vec3 hitpoint = rayOrigin + rayDirection * ClosestT;
		glm::vec3 normal = hitpoint - sphereOrigin;
		normal = glm::normalize(normal);

		float light = glm::max(glm::dot(normal, -lightDir), 0.0f);
		colorOut = light * (0.5f * glm::vec3(normal.x + 1, normal.y + 1, normal.z + 1));
		return glm::vec4(colorOut, 1.0f);
	}
}