#include "Renderer.h"

#include "Walnut/Random.h"
#include <iostream>

#include <cmath> 

namespace RayTracing {
	void Renderer::OnResize(uint32_t width, uint32_t height)
	{
		if (m_FinalImage)
		{
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
				m_ImageData[x + y * m_FinalImage->GetWidth()] = PerPixel(coord);
			}
		}
		

		m_FinalImage->SetData(m_ImageData);
	}

	uint32_t Renderer::PerPixel(glm::vec2 coord)
	{
		glm::vec3 colorOut = glm::vec3(0.1f);
	
		glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);
		glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
		rayDirection = glm::normalize(rayDirection);

		float radius = 0.5;
		glm::vec3 sphereOrigin = glm::vec3(0.0f);
		glm::vec3 lightDir = glm::vec3(-1.0f, -0.9, 0.5);

		float a = glm::dot(rayDirection, rayDirection);
		float b = 2.0f * glm::dot(rayOrigin, rayDirection);
		float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

		float discriminant = b * b - 4.0f * a * c;

		if (discriminant >= 0.0f)
		{
			float t0 = -b + sqrt(discriminant) / (2.0f * a);
			//float t1 = -b + sqrt(discriminant) / (2.0f * a);
			
			glm::vec3 hitpos = rayOrigin + rayDirection * t0;
			glm::vec3 normal = hitpos - sphereOrigin;
			normal = glm::normalize(normal);

			float light = glm::dot(normal, -lightDir);
			colorOut = light * glm::vec3(0.f, 1.0f, 0.f);

		}


		colorOut = glm::clamp(colorOut, 0.0f, 1.0f);

		uint8_t intr = (uint8_t)(colorOut.x * 255.0f);
		uint8_t intg = (uint8_t)(colorOut.y * 255.0f);
		uint8_t intb = (uint8_t)(colorOut.z * 255.0f);

		

		return 0xff0000000 | (intb << 16) | (intg << 8) | intr;
	}
}