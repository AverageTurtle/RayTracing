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
		struct HitPayload
		{
			float HitDistance;
			glm::vec3 WorldPosition;
			glm::vec3 WorldNormal;

			uint32_t ObjectIndex;
		};

		glm::vec4 PerPixel(uint32_t x, uint32_t y); // RayGen

		HitPayload TraceRay(const Ray& ray);
		HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);
		HitPayload Miss(const Ray& ray);
	private:
		uint32_t* m_ImageData = nullptr;
		std::shared_ptr<Walnut::Image> m_FinalImage;

		const Scene* m_ActiveScene = nullptr;
		const Camera* m_ActiveCamera = nullptr;

		glm::vec3 lightDir = glm::vec3(-0.5f, -0.8, -0.5);
	};
}