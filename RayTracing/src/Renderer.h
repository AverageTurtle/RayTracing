#pragma once

#include "Walnut/Image.h"

#include <memory>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Ray.h"

namespace RayTracing {
	class Renderer {
	public:
		Renderer() = default;
		
		void OnResize(uint32_t width, uint32_t height);
		void Render(const Camera& camera);

		std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
	private:
		glm::vec4 TraceRay(const Ray& ray);
	private:
		uint32_t* m_ImageData = nullptr;
		std::shared_ptr<Walnut::Image> m_FinalImage;
		glm::vec3 lightDir = glm::vec3(-0.5f, -0.8, -0.5);
		float radius = 0.5;
		glm::vec3 sphereOrigin = glm::vec3(0.0f);
	};
}