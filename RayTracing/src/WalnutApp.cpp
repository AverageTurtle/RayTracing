#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.01f, 100.0f) {}
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
		m_Renderer.Render(m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	RayTracing::Renderer m_Renderer;
	float m_LastRenderTime = 0.0f;
	float m_RenderScale = 1.0f;

	Camera m_Camera;

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