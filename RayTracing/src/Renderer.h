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
		struct Settings
		{
			bool Accumulate = true;
			bool PreviewRenderer = false;
		};

	public:
		Renderer() = default;
		
		void OnResize(uint32_t width, uint32_t height);
		void Render(const Scene& scene, const Camera& camera);

		std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

		void ResetFrameIndex() { m_FrameIndex = 1; }
		Settings& GetSettings() { return m_Settings; }
	private:
		struct HitPayload
		{
			float HitDistance;
			glm::vec3 WorldPosition;
			glm::vec3 WorldNormal;

			uint32_t ObjectIndex;
		};

		void TraceColorRay(Ray& ray, glm::vec3& color, glm::vec3& contribution, const int& maxDepth, int& depth);
		glm::vec4 PerPixel(uint32_t x, uint32_t y); // RayGen

		glm::vec3 CaculatePointLight(const PointLight& pointLight, const HitPayload& payload);
		
		//TEMP
		HitPayload TraceShadowRay(const Ray& ray);
		HitPayload TraceRay(const Ray& ray);
		HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex);
		HitPayload Miss(const Ray& ray);
	private:
		std::shared_ptr<Walnut::Image> m_FinalImage;
		Settings m_Settings;

		std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;

		const Scene* m_ActiveScene = nullptr;
		const Camera* m_ActiveCamera = nullptr;
		uint32_t* m_ImageData = nullptr;
		glm::vec4* m_AccumulationData = nullptr;

		uint32_t m_FrameIndex = 1;
	};
}