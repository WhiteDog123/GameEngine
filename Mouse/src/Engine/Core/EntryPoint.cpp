#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Application.h"
#include "EntryPoint.h"

namespace Engine::Core
{
	static GLFWwindow* window = nullptr;

	static bool InitWindow()
	{
		if (!glfwInit()) return false;

		//OpenGL로 하여금 '이 창에서 렌더링 하도록' 설정한다.
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		window = glfwCreateWindow(800, 600, "Mouse Engine", nullptr, nullptr);
		if (!window)
		{
			glfwTerminate();
			return false;
		}
		//OpenGL로 하여금 '이 창에서 렌더링 하도록' 설정한다.
		glfwMakeContextCurrent(window);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return false;
		return true;
	}

	static void InitImGui()
	{
		IMGUI_CHECKVERSION();
		//ImGUI의 세션/내부상태를 저장하기 위한 상태 저장소(context)를 생성 - 버튼 상태, 마우스 위치 등의 다양한 값을 전역으로 관리
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	static void ShutdownImGui()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Run(Application* app)
	{
		if (!InitWindow()) return;
		InitImGui();

		app->OnInit(window);

		float lastTime = (float)glfwGetTime();
		while (!glfwWindowShouldClose(window))
		{
			float now = (float)glfwGetTime();
			float delta = now - lastTime;
			lastTime = now;

			glfwPollEvents();

			glViewport(0, 0, 800, 600);
			glClearColor(0.2f, 0.3, 0.4f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			app->OnUpdate(delta);
			app->OnImGuiRender();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(window);
		}
		app->OnShutDown();
		delete app;

		ShutdownImGui();
		glfwDestroyWindow(window);
		glfwTerminate();
	}
}