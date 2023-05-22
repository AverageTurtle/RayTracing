#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"

#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.01f, 100.0f) 
	{
		
		RefractiveMaterial* glass = new RefractiveMaterial();
		m_Scene.Materials.push_back(glass);

		DiffuseMaterial* white = new DiffuseMaterial();
		white->Albedo = { 1.0f, 1.0f, 1.0f };
		white->Roughness = 1.0f;
		m_Scene.Materials.push_back(white);

		DiffuseMaterial* red = new DiffuseMaterial();
		red->Albedo = { 1.0f, 0.2f, 0.2f };
		red->Roughness = 1.0f;
		m_Scene.Materials.push_back(red);

		DiffuseMaterial* green = new DiffuseMaterial();
		green->Albedo = { 0.2f, 1.0f, 0.2f };
		green->Roughness = 1.0f;
		m_Scene.Materials.push_back(green);

		DiffuseMaterial* blue = new DiffuseMaterial();
		blue->Albedo = { 0.2f, 0.2f, 1.0f };
		blue->Roughness = 1.0f;
		m_Scene.Materials.push_back(blue);


		DiffuseMaterial* pink = new DiffuseMaterial();
		pink->Albedo = { 1.0f, 0.3, 1.0f };
		pink->Roughness = 1.0f;
		m_Scene.Materials.push_back(pink);

		{
			Sphere sphere;
			sphere.Position = { 0.0f, -0.2f, 0.0f };
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 5;
			m_Scene.Spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.Position = { 2.0f, 2.0f, 0.0f };
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 0;
			m_Scene.Spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.Position = { 0.0f, -1010.0f, 0.0f };
			sphere.Radius = 1000.0f;
			sphere.MaterialIndex = 1;
			m_Scene.Spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Position = { 0.0f, 1010.0f, 0.0f };
			sphere.Radius = 1000.0f;
			sphere.MaterialIndex = 1;
			m_Scene.Spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Position = { -1010.0f, 0.0f, -5.0f };
			sphere.Radius = 1000.0f;
			sphere.MaterialIndex = 2;
			m_Scene.Spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Position = { 1010.0f, 0.0f, -5.0f };
			sphere.Radius = 1000.0f;
			sphere.MaterialIndex = 2;
			m_Scene.Spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Position = { 0.0f, 0.0f, -1010.0f };
			sphere.Radius = 1000.0f;
			sphere.MaterialIndex = 3;
			m_Scene.Spheres.push_back(sphere); 
		}
		{
			Sphere sphere;
			sphere.Position = { 0.0f, 0.0f, 1010.0f };
			sphere.Radius = 1000.0f;
			sphere.MaterialIndex = 4;
			m_Scene.Spheres.push_back(sphere);
		}


		PointLight& pointLight = m_Scene.PointLights.emplace_back();
		pointLight.Intesity = 20.0f;
		pointLight.Position = { 0.0f, 4.0f, 0.0f };
	}
	virtual void OnUpdate(float ts) override {
		if (m_Camera.OnUpdate(ts)) {
			m_Renderer.ResetFrameIndex();
		}
		Render();
	}
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		if (ImGui::Button("Render")) {
			Render();
		}
		if (ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate))
			m_Renderer.ResetFrameIndex();
		if (ImGui::Checkbox("Preview Renderer", &m_Renderer.GetSettings().PreviewRenderer))
			m_Renderer.ResetFrameIndex();

		if (ImGui::Button("Reset")) {
			m_Renderer.ResetFrameIndex();
		}
		ImGui::Text("Last Render Time: %.3fms", m_LastRenderTime);
		ImGui::SliderFloat("Render Scale", &m_RenderScale, 0.01f, 2.0f);

		if (ImGui::Button("Add Sphere")) {
			Sphere sphere;
			m_Scene.Spheres.push_back(sphere);
			m_Renderer.ResetFrameIndex();
		}
		if (ImGui::Button("Add Diffuse Mat")) {
			m_Scene.Materials.push_back(new DiffuseMaterial());
			m_Renderer.ResetFrameIndex();
		}
		ImGui::SameLine();
		if (ImGui::Button("Add Refractive Mat")) {
			m_Scene.Materials.push_back(new RefractiveMaterial());
			m_Renderer.ResetFrameIndex();
		}

		if (ImGui::Button("Add Point Light")) {
			PointLight pointLight;
			m_Scene.PointLights.push_back(pointLight);
			m_Renderer.ResetFrameIndex();
		}

		ImGui::End();

		ImGui::Begin("Lights");
		for (size_t i = 0; i < m_Scene.PointLights.size(); i++)
		{
			ImGui::PushID("PointLight: "+i);

			PointLight& pointLight = m_Scene.PointLights[i];
			if (ImGui::DragFloat3("Position", glm::value_ptr(pointLight.Position), 0.1f))
				m_Renderer.ResetFrameIndex();
			if (ImGui::DragFloat("Intesity", &pointLight.Intesity, 0.1f))
				m_Renderer.ResetFrameIndex();
			if (ImGui::ColorEdit3("Color", glm::value_ptr(pointLight.Color)))
				m_Renderer.ResetFrameIndex();

			ImGui::Separator();

			ImGui::PopID();
		}
		ImGui::End();

		ImGui::Begin("Materials");
		for (size_t i = 0; i < m_Scene.Materials.size(); i++)
		{
			ImGui::PushID(i);

			Material* material = m_Scene.Materials[i];
			if (material->GetMaterialType() == MaterialType::Diffuse) {
				DiffuseMaterial* diffuse = (DiffuseMaterial*)material;
				if (ImGui::ColorEdit3("Albedo", glm::value_ptr(diffuse->Albedo)))
					m_Renderer.ResetFrameIndex();
				if (ImGui::DragFloat("Roughness", &diffuse->Roughness, 0.05f, 0.0f, 1.0f))
					m_Renderer.ResetFrameIndex();
				if (ImGui::DragFloat("Metallic", &diffuse->Metallic, 0.05f, 0.0f, 1.0f))
					m_Renderer.ResetFrameIndex();
			}
			else {
				RefractiveMaterial* refractive = (RefractiveMaterial*)material;
				if (ImGui::DragFloat("Refractive Index", &refractive->RefractiveIndex, 0.05f))
					m_Renderer.ResetFrameIndex();
			}

			ImGui::Separator();
			ImGui::PopID();
		}
		ImGui::End();

		ImGui::Begin("Objects");
		for (size_t i = 0; i < m_Scene.Spheres.size(); i++)
		{
			ImGui::PushID(i);

			Sphere& sphere = m_Scene.Spheres[i];
			if(ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f))
				m_Renderer.ResetFrameIndex();
			if(ImGui::DragFloat("Radius", &sphere.Radius, 0.1f))
				m_Renderer.ResetFrameIndex();
			if(ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_Scene.Materials.size() - 1))
				m_Renderer.ResetFrameIndex();

			ImGui::Separator();
			ImGui::PopID();
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");
		
		m_ViewportHeight = ImGui::GetContentRegionAvail().x;
		m_ViewportWidth = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();
		if(image)
			ImGui::Image(image->GetDescriptorSet(), { (float)m_ViewportHeight, (float)m_ViewportWidth }, ImVec2(0,1), ImVec2(1,0));

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void Render() {
		Walnut::Timer timer;

		m_Renderer.OnResize(m_ViewportHeight * m_RenderScale, m_ViewportWidth * m_RenderScale);
		m_Camera.OnResize(m_ViewportHeight * m_RenderScale, m_ViewportWidth * m_RenderScale);
		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	RayTracing::Renderer m_Renderer;
	float m_LastRenderTime = 0.0f;
	float m_RenderScale = 1.0f;

	Camera m_Camera;
	Scene m_Scene;

	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}