#include <GLFW/glfw3.h>
#include "Application.h"
#include "EntryPoint.h"

namespace Engine::Core
{
	static GLFWwindow* window = nullptr;

	static bool InitWindow()
	{
		if (!glfwInit()) return false;
		
		window = glfwCreateWindow(800, 600, "Mouse Engine", nullptr, nullptr);

		if (!window) return false;

		glfwMakeContextCurrent(window);

		return true;
	}

	void Run(Application* app)
	{
		if (!InitWindow()) return;

		app->OnInit();

		float lastTime = 0.0f;
		while (!glfwWindowShouldClose(window))
		{
			float now = glfwGetTime();
			float delta = now - lastTime;
			lastTime = now;

			glfwPollEvents();

			app->OnUpdate(delta);
			app->OnImGuiRender();

			glfwSwapBuffers(window);
		}

		delete app;
		glfwDestroyWindow(window);
		glfwTerminate();
	}
}