#pragma once

#include "Walnut/Image.h"

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

#include <memory>
#include <glm/glm.hpp>

namespace RayTracing {
	class Renderer {
	public:
		Renderer() = default;
		
		void OnResize(uint32_t width, uint32_t height);
		void Render(const Scene& scene, const Camera& camera);

		std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
	private:
		glm::vec4 TraceRay(const Scene& scene, const Ray& ray);
	private:
		uint32_t* m_ImageData = nullptr;
		std::shared_ptr<Walnut::Image> m_FinalImage;

		glm::vec3 lightDir = glm::vec3(-0.5f, -0.8, -0.5);
	};
}