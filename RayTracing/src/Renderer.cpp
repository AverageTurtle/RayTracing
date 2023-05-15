#include "Renderer.h"

#include "Walnut/Random.h"
#include <iostream>

#include <cmath> 
#include <execution>
#include <math.h>
#include <corecrt_math_defines.h>

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

		delete[] m_AccumulationData;
		m_AccumulationData = new glm::vec4[width * height];

		m_ImageHorizontalIter.resize(width);
		m_ImageVerticalIter.resize(height);
		for (uint32_t i = 0; i < width; i++)
			m_ImageHorizontalIter[i] = i;
		for (uint32_t i = 0; i < height; i++)
			m_ImageVerticalIter[i] = i;

	}

	void Renderer::Render(const Scene& scene, const Camera& camera)
	{
		if (m_FinalImage == nullptr)
			return;
		m_ActiveScene = &scene;
		m_ActiveCamera = &camera;

		if (m_FrameIndex == 1)
			memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));

		
		std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
			[&](uint32_t y)
			{
				std::for_each(std::execution::par, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
				[this, y](uint32_t x)
					{
						glm::vec4 color = PerPixel(x, y);
						color = glm::sqrt(color);

						m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;

						glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
						accumulatedColor /= (float)m_FrameIndex;

						accumulatedColor = glm::clamp(accumulatedColor, 0.0f, 1.0f);

						

						m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
					});
			});

		m_FinalImage->SetData(m_ImageData);

		if (m_Settings.Accumulate)
			m_FrameIndex++;
		else
			m_FrameIndex = 1;
	}


	glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
	{
		Ray ray;
		ray.Origin = m_ActiveCamera->GetPosition();
		ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()] + Walnut::Random::Vec3(-0.001f, 0.001f);
		 
		// Shading at point Sp
		glm::vec3 color(0.0f);

		Renderer::HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0.0001f)
		{
			//return glm::vec4(0.75f, 0.8f, 1.0f, 1.0f);
			return glm::vec4(0.01f, 0.01f, 0.01f, 1.0f);
		}


		const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
		const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];
		glm::vec3 sphereColor = material.Albedo;


		glm::vec3 lightIntensity(0.0f);
		for each (auto pointLight in m_ActiveScene->PointLights)
		{
			lightIntensity += CaculatePointLight(pointLight, payload);
		}
		color = sphereColor / ((float)M_PI) * lightIntensity;

		//float facingRatio = std::max(0.0f, glm::dot(payload.WorldNormal, -ray.Direction));
		/*
		DirectionalLight dirLight;
		Ray shadowRay;
		shadowRay.Origin = payload.WorldPosition + (payload.WorldNormal*0.00001f);
		shadowRay.Direction = -dirLight.Direction;
		glm::normalize(shadowRay.Direction);

		Renderer::HitPayload shadowPayload = TraceRay(shadowRay);
		if (shadowPayload.HitDistance > 0.0001f)
		{
			return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		}

		color = sphereColor / ((float)M_PI) * dirLight.Intesity * dirLight.Color * std::max(0.0f, glm::dot(payload.WorldNormal, -lightDir));
		*/


		/*
		glm::vec3 lightColor(0.0f);
		glm::vec3 reflectionColor(0.0f);

		const int depth = 10;
		float multiplier = 1.0f;
		const static glm::vec3 lightPos = { 0.0f, 10.0f, 0.0f };
		for (int i = 0; i < depth; i++)
		{
			Renderer::HitPayload payload = TraceRay(ray);
			if (payload.HitDistance < 0.0001f)
			{
				glm::vec3 skyColor = glm::vec3(0.75f, 0.8f, 1.0f);
				//lightColor += skyColor * multiplier;
				//reflectionColor += skyColor * multiplier;
				break;
			}

			const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
			const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];
			glm::vec3 sphereColor = material.Albedo;// lightIntensity * material.Albedo;
			reflectionColor += sphereColor * multiplier;

			Ray shadowRay;
			shadowRay.Origin = payload.WorldPosition + (payload.WorldNormal * 0.001f);
			shadowRay.Direction = lightPos - payload.WorldPosition;
			glm::normalize(shadowRay.Direction);

			Renderer::HitPayload shadowPayload = TraceRay(shadowRay);
			if (shadowPayload.HitDistance < 0.0001f) {
				lightColor += glm::vec3(multiplier);//* sphereColor;
			}


			multiplier *= 0.5f;
			ray.Origin = payload.WorldPosition + (payload.WorldNormal * 0.0001f);
			auto v = Walnut::Random::Vec3(-0.5f, 0.5f);
			ray.Direction = glm::reflect(ray.Direction,
				payload.WorldNormal + material.Roughness * v);
		}
		*/

		//Shadow
		/*
		const static glm::vec3 lightPos = { 0.0f, 10.0f, 0.0f };
		Renderer::HitPayload payload = TraceRay(ray);
		if (payload.HitDistance < 0.0001f)
		{
			glm::vec3 skyColor = glm::vec3(0.75f, 0.8f, 1.0f);
			color += skyColor * multiplier;
		}
		else {
			const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
			const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];
			glm::vec3 sphereColor = material.Albedo;
			Ray shadowRay;
			shadowRay.Origin = payload.WorldPosition + (payload.WorldNormal*0.001f);
			shadowRay.Direction = lightPos - payload.WorldPosition;
			glm::normalize(shadowRay.Direction);

			Renderer::HitPayload payload = TraceRay(shadowRay);
			if (payload.HitDistance < 0.0001f) {
				color =  sphereColor;
			}
			else {
				color = sphereColor * 0.1f;
			}
		}
		*/

		// Reflection
		/*
		for (int i = 0; i < depth; i++)
		{
			Renderer::HitPayload payload = TraceRay(ray);
			if (payload.HitDistance < 0.0001f)
			{
				glm::vec3 skyColor = glm::vec3(0.75f, 0.8f, 1.0f);
				color += skyColor * multiplier;
				break;
			}

			const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
			const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];

			glm::vec3 sphereColor = material.Albedo;// lightIntensity * material.Albedo;
			///color += sphereColor * multiplier;
			
			multiplier *= 0.5f;
			ray.Origin = payload.WorldPosition + (payload.WorldNormal * 0.0001f);

			auto v = Walnut::Random::Vec3(-0.5f, 0.5f);
			ray.Direction = glm::reflect(ray.Direction,
				payload.WorldNormal + material.Roughness * v);
			
		}*/

		return glm::vec4(color, 1.0f);
	}

	glm::vec3 Renderer::CaculatePointLight(const PointLight& pointLight, const Renderer::HitPayload& payload)
	{
		glm::vec3 lightDir = pointLight.Position - payload.WorldPosition;
		float r2 = glm::length(lightDir);
		float dist = sqrtf(r2);
		lightDir = lightDir / glm::vec3(dist);
		glm::vec3 lightIntensity = pointLight.Intesity * pointLight.Color / glm::vec3(4 * M_PI * r2);

		Ray shadowRay;
		shadowRay.Origin = payload.WorldPosition + (payload.WorldNormal * 0.00001f);
		shadowRay.Direction = lightDir;
		shadowRay.Length = dist;
		glm::normalize(shadowRay.Direction);

		Renderer::HitPayload shadowPayload = Renderer::TraceRay(shadowRay);
		if (shadowPayload.HitDistance > 0.0001f)
		{
			return glm::vec3(0.0f, 0.0f, 0.0f);
		}
		return lightIntensity * std::max(0.0f, glm::dot(payload.WorldNormal, lightDir));
	}
	Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
	{
		int closestSphere = -1;
		float hitDistance = ray.Length;
		for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++) {
			const Sphere& sphere = m_ActiveScene->Spheres[i];
			glm::vec3 origin = ray.Origin - sphere.Position;

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
				closestSphere = (int)i;
			}
		}

		if (closestSphere < 0)
			return Miss(ray);

		return ClosestHit(ray, hitDistance, closestSphere);
	}

	Renderer::HitPayload Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex)
	{
		Renderer::HitPayload payload;
		payload.HitDistance = hitDistance;
		payload.ObjectIndex = objectIndex;

		const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];

		glm::vec3 origin = ray.Origin - closestSphere.Position;
		payload.WorldPosition = origin + ray.Direction * hitDistance;
		payload.WorldNormal = glm::normalize(payload.WorldPosition);
		
		payload.WorldPosition += closestSphere.Position;
		return payload;
	}

	Renderer::HitPayload Renderer::Miss(const Ray& ray)
	{
		Renderer::HitPayload payload;
		payload.HitDistance = -1.0f;
		return payload;
	}
}