#include <vector>
#include <string>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "main.h"

static std::vector<std::string> logs;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

bool InitGLFW(GLFWwindow** window)
{
	if (glfwInit() == false)
	{
		std::cerr << "GLFW 초기화 실패!" << std::endl;
		return -1;
	}

	//설정값 지정(Hint 주기)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//이 시점에서 창은 '만들어진다', 그러나, 렌더링 루프, 이벤트 처리도 없으므로 화면에 나타나지 않는다.
	*window = glfwCreateWindow(800, 600, "Mouse Engine v0.1", NULL, NULL);
	if (!window)
	{
		std::cerr << "Failed to create window!" << std::endl;
		glfwTerminate();
		return false;
	}
	//OpenGL로 하여금 '이 창에서 렌더링 하도록' 설정한다.
	glfwMakeContextCurrent(*window);
	glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback); // 사용자가 크기 조절을 해야 비로소 호출됨.
	return true;
}
bool InitGLAD()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to Iniciallize GLAD" << std::endl;
		return false;
	}
	return true;
}
void InitImGui(GLFWwindow* window)
{
	glViewport(0, 0, 800, 600); // OpenGL은 기본적으로 초기 상태의 뷰포트가 0x0으로 되어있어서 필요함.

	const char* glsl_version = "#version 330";
	//IMGUI 초기화
	IMGUI_CHECKVERSION();
	//ImGUI의 세션/내부상태를 저장하기 위한 상태 저장소(context)를 생성 - 버튼 상태, 마우스 위치 등의 다양한 값을 전역으로 관리
	ImGui::CreateContext();
	//의도적으로 사용하지 않는 변수로 지정
	//ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}

void ShutdownImGui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void DrawColorPicker(float*  bgColor)
{

	ImGui::Begin("Background Color");
	ImGui::ColorEdit3("", bgColor);
	ImGui::End();
}

void DrawPerfStats(float deltaTime)
{
	ImGui::Begin("Performance");
	float fps = 1.0f / deltaTime;
	float frameTimeMs = deltaTime * 1000.0f;
	ImGui::Text("FPS: %.1f", fps);
	ImGui::Text("Frame Time: %.2f ms", frameTimeMs);
	ImGui::End();
}

void DrawLogWindow()
{
	ImGui::Begin("Log");

	if (ImGui::Button("Click to Log"))
	{
		float now = glfwGetTime();
		logs.push_back("Clicked at " + std::to_string(now) + "s");
	}

	ImGui::SameLine();
	if (ImGui::Button("Clear"))
	{
		logs.clear();
	}

	ImGui::BeginChild("LogRegion", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);

	for (const auto& line : logs)
	{
		ImGui::TextUnformatted(line.c_str());
	}

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
	{
		ImGui::SetScrollHereY(1.0f);
	}

	ImGui::EndChild();
	ImGui::End();
}

void DrawMouseDebug(GLFWwindow* window)
{
	ImGui::Begin("Mouse Debug");

	//ImGui 좌표계 기준
	ImVec2 mousePos = ImGui::GetMousePos();
	ImGui::Text("ImGui Mouse Pos: (%.1f, %.1f)", mousePos.x, mousePos.y);

	//GLFW 기준 좌표계
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	ImGui::Text("GLFW Mouse Pos: (%.1f, %.1f)", xpos, ypos);

	bool leftDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
	bool rightDown = ImGui::IsMouseDown(ImGuiMouseButton_Right);

	ImGui::Text("Left Button: %s", leftDown ? "Held" : "Released");
	ImGui::Text("Right Button: %s", rightDown? "Held" : "Released");

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		logs.push_back("Left Click at ("+std::to_string(mousePos.x)+", " + std::to_string(mousePos.y)+")");
	}
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		logs.push_back("Right Click at (" + std::to_string(mousePos.x) + ", " + std::to_string(mousePos.y) + ")");
	}


	ImGui::End();
}

int main() {
	GLFWwindow* window = nullptr;
	if(!InitGLFW(&window) || !InitGLAD())
	{
		return -1;
	}
	InitImGui(window);

	static float bgColor[3] = { 0.2f, 0.3f, 0.4f };

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	// 렌더링 루프
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		DrawColorPicker(bgColor);
		DrawPerfStats(deltaTime);
		DrawLogWindow();
		DrawMouseDebug(window);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}
	ShutdownImGui();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}