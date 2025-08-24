#pragma once
struct GLFWwindow;

namespace Engine::Core
{
	class Application
	{
	public:
		virtual void OnInit(GLFWwindow* window) = 0;
		virtual void OnUpdate(float dt) = 0;
		virtual void OnImGuiRender() = 0;
		virtual void OnShutDown() {}
		virtual ~Application() = default;
	};
}