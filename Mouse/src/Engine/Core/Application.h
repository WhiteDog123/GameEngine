#pragma once

namespace Engine::Core
{
	class Application
	{
	public:
		virtual void OnInit() = 0;
		virtual void OnUpdate(float dt) = 0;
		virtual void OnImGuiRender() = 0;
		virtual ~Application() = default;
	};
}