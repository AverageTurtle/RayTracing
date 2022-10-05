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
		{
			Sphere sphere;
			sphere.Position = { 0.0f, 0.0f, 0.0f };
			sphere.Radius = 0.5f;
			sphere.Albedo = { 1.0f, 0.3, 1.0f };
			m_Scene.Spheres.push_back(sphere);
		}
		{
			Sphere sphere;
			sphere.Position = { 1.0f, 0.0f, -5.0f };
			sphere.Radius = 0.7f;
			sphere.Albedo = { 0.0f, 0.3, 1.0f };
			m_Scene.Spheres.push_back(sphere);
		}
	}
	virtual void OnUpdate(float ts) override {
		m_Camera.OnUpdate(ts);
		Render();
	}
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		if (ImGui::Button("Render")) {
			Render();
		}
		ImGui::Text("Last Render Time: %.3fms", m_LastRenderTime);
		ImGui::SliderFloat("Render Scale", &m_RenderScale, 0.01f, 2.0f);

		if (ImGui::Button("Add Sphere")) {
			Sphere sphere;
			m_Scene.Spheres.push_back(sphere);
		}

		ImGui::End();

		ImGui::Begin("Scene");
		for (size_t i = 0; i < m_Scene.Spheres.size(); i++) {
			if(ImGui::TreeNode(std::to_string(i).c_str())) {
				ImGui::DragFloat3("Position", glm::value_ptr(m_Scene.Spheres[i].Position), 0.1f);
				ImGui::DragFloat("Radius", &m_Scene.Spheres[i].Radius, 0.1f);
				ImGui::ColorEdit3("Albedo", glm::value_ptr(m_Scene.Spheres[i].Albedo), 0.1f);

				ImGui::TreePop();
			}
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