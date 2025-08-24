#include <vector>
#include <string>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <random>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "main.h"
#include "Engine/Core/Time.h"
#include "Engine/Input/Actions.h"	
#include "Engine/Rendering/Renderer2D.h"

static std::vector<std::string> logs;

static Engine::Core::Time gTime;


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

void DrawColorPicker(float* bgColor)
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
	ImGui::Text("Right Button: %s", rightDown ? "Held" : "Released");

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		logs.push_back("Left Click at (" + std::to_string(mousePos.x) + ", " + std::to_string(mousePos.y) + ")");
	}
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		logs.push_back("Right Click at (" + std::to_string(mousePos.x) + ", " + std::to_string(mousePos.y) + ")");
	}


	ImGui::End();
}

static bool lastKeyState[1024] = { false };

void DrawKeyDebug(GLFWwindow* window)
{
	ImGui::Begin("Keyboard Debug");

	struct {
		int key;
		const char* name;
	} keys[] = {
		{ GLFW_KEY_W, "W" },
		{ GLFW_KEY_A, "A" },
		{ GLFW_KEY_S, "S" },
		{ GLFW_KEY_D, "D" },
		{ GLFW_KEY_ESCAPE, "ESC" }
	};

	for (auto& k : keys)
	{
		int state = glfwGetKey(window, k.key);

		ImGui::Text("%s: %s", k.name, state == GLFW_PRESS ? "Pressed" : "Released");

		if (k.key == GLFW_KEY_ESCAPE && state == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}

		if (state == GLFW_PRESS && !lastKeyState[k.key])
		{
			logs.push_back(std::string(k.name) + " Pressed");
		}
		if (state == GLFW_RELEASE && lastKeyState[k.key])
		{
			logs.push_back(std::string(k.name) + " Released");
		}
		lastKeyState[k.key] = (state == GLFW_PRESS);
	}
	ImGui::End();
}

struct Rect {
	float x, y, w, h;
	ImVec4 color;
};

static std::vector<Rect> objects;
static int selectedIndex = -1;
static ImVec2 dragOffset;

inline float Clamp(float v, float min, float max) {
	return v < min ? min : (v > max ? max : v);
}

void DrawInspector()
{
	if (selectedIndex < 0) return;

	auto& R = objects[selectedIndex];
	ImGui::Begin("Inspector");

	ImGui::DragFloat("X", &R.x, 1.0f, 0.0f, ImGui::GetWindowWidth() - R.w);
	ImGui::DragFloat("Y", &R.y, 1.0f, 0.0f, ImGui::GetWindowHeight() - R.h);

	ImGui::DragFloat("Width", &R.w, 1.0f, 1.0f, 100);
	ImGui::DragFloat("Height", &R.h, 1.0f, 1.0f, 100);

	ImGui::ColorEdit4("Color", (float*)&R.color);
	ImGui::End();
}
static bool playMode = false;
static Rect player = { 380, 280, 24, 24, ImVec4(1,1,0,1) };



struct Bullet
{
	float x, y, w, h;
	float vx, vy;
	ImVec4 color;
};
static std::vector<Bullet> bullets;
static float spawnTimer = 0.0f;
static float spawnRate = 3.0;
static float scoreSec = 0.0f;

enum class GameState { Idle, Playing, GameOver };
static GameState gameState = GameState::Idle;
static bool needReset = false;



inline bool OverlapRB(const Rect& a, const Bullet& b)
{
	return !(a.x + a.w < b.x || b.x + b.w < a.x ||
		a.y + a.h < b.y || b.y + b.h < a.y);
}

static void ResetRun(const ImVec2& avail)
{
	bullets.clear();
	spawnTimer = 0.0f;
	spawnRate = 3.0f;
	scoreSec = 0.0f;
	player.w = player.h = 24.0f;
	player.x = std::max(0.0f, avail.x * 0.5f - player.w * 0.5f);
	player.y = std::max(0.0f, avail.y * 0.5f - player.h * 0.5f);
	gameState = GameState::Playing;

}



void DrawSceneView(GLFWwindow* window, float deltaTime)
{
	ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoMove);

	ImVec2 p0 = ImGui::GetCursorScreenPos();
	ImVec2 avail = ImGui::GetContentRegionAvail();

	ImDrawList* draw = ImGui::GetWindowDrawList();
	Engine::Rendering::Renderer2D r;
	r.Begin(draw, p0);
	r.Rect(0.0, 0.0, avail.x, avail.y, IM_COL32(50, 50, 50, 255));

	if (playMode)
	{
		if (needReset || gameState == GameState::Idle)
		{
			ResetRun(avail);
			needReset = false;
		}
		Engine::Input::InputState in = Engine::Input::PollInput(window);

		Engine::Core::StepFixed(gTime, deltaTime, [&](float dt)
			{
				if (gameState != GameState::Playing) return;

				const float speed = 260.0f;
				float dx = 0.f, dy = 0.f;

				if (in.held[(int)Engine::Input::Action::MoveLeft]) dx -= 1.f;
				if (in.held[(int)Engine::Input::Action::MoveRight]) dx += 1.f;
				if (in.held[(int)Engine::Input::Action::MoveUp]) dy -= 1.f;
				if (in.held[(int)Engine::Input::Action::MoveDown]) dy += 1.f;

				player.x += speed * dx * dt;
				player.y += speed * dy * dt;
				player.x = Clamp(player.x, 0.0f, avail.x - player.w);
				player.y = Clamp(player.y, 0.0f, avail.y - player.h);

				spawnTimer += dt;
				float interval = 1.0f / spawnRate;
				static std::mt19937 rng{ std::random_device{}() };
				std::uniform_real_distribution<float> rx(0.0f, 1.0f);

				while (spawnTimer >= interval)
				{
					spawnTimer -= interval;
					spawnRate = std::min(12.0f, spawnRate + 0.05f);

					int side = int(rx(rng) * 4.0f);
					Bullet b;
					b.w = 10.f;
					b.h = 10.f;
					b.color = ImVec4(1, 0.25f, 0.25f, 1);
					float spd = 140.0f + rx(rng) * 160.0f;

					if (side == 0) { b.x = rx(rng) * (avail.x - b.w); b.y = -b.h; b.vx = 0; b.vy = spd; }
					if (side == 1) { b.x = rx(rng) * (avail.x - b.w); b.y = avail.y + b.h; b.vx = 0;     b.vy = -spd; }
					if (side == 2) { b.x = -b.w;                    b.y = rx(rng) * (avail.y - b.h); b.vx = spd;  b.vy = 0; }
					if (side == 3) { b.x = avail.x + b.w;           b.y = rx(rng) * (avail.y - b.h); b.vx = -spd;  b.vy = 0; }
					b.vx += (rx(rng) - 0.5f) * 60.0f;
					b.vy += (rx(rng) - 0.5f) * 60.0f;
					bullets.push_back(b);
				}

				for (int i = (int)bullets.size() - 1; i >= 0; --i)
				{
					auto& b = bullets[i];
					b.x += b.vx * dt;
					b.y += b.vy * dt;
					if (b.x < -20 || b.y < -20 || b.x > avail.x + 20 || b.y > avail.y + 20)
					{
						bullets.erase(bullets.begin() + i);
					}
				}

				for (auto& b : bullets)
				{
					if (!(player.x + player.w < b.x || b.x + b.w < player.x ||
						player.y + player.h < b.y || b.y + b.h < player.y))
					{
						gameState = GameState::GameOver;
						logs.push_back("Game Over! Score: " + std::to_string(scoreSec) + "s");
						break;
					}
				}

				if (gameState == GameState::Playing) scoreSec += dt;
			});

		r.Text(ImVec2(8, 8), IM_COL32_WHITE, (std::string("Score: ") + std::to_string(scoreSec) + "s").c_str());
		if (gameState == GameState::GameOver)
		{
			ImVec2 center = ImVec2(p0.x + avail.x * 0.5f - 90, p0.y + avail.y * 0.5f - 30);
			ImGui::SetCursorScreenPos(center);
			ImGui::BeginChild("GameOver", ImVec2(180, 60), true);
			ImGui::Text("Game Over");
			ImGui::Text("Score: %.1fs", scoreSec);
			if (ImGui::Button("Restart (R)")) { ResetRun(avail); }
			ImGui::EndChild();
			if (Engine::Input::PollInput(window).held[(int)Engine::Input::Action::Restart])
			{
				ResetRun(avail);
			}
		}

		if (playMode)
		{
			r.Rect(player.x, player.y, player.w, player.h, IM_COL32(255, 255, 0, 255));
			for (auto& b : bullets)
			{
				r.Rect(b.x, b.y, b.w, b.h, IM_COL32(255, 70, 70, 255));
			}
		}
	}

	r.End();
	ImGui::End();
}

int main() {
	GLFWwindow* window = nullptr;
	if (!InitGLFW(&window) || !InitGLAD())
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

		ImGui::Begin("Control", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_AlwaysAutoResize);

		if (ImGui::Button(playMode ? "Stop" : "Play"))
		{
			playMode = !playMode;
			if (playMode)
			{
				gTime.accumulator = 0.0f;
				gameState = GameState::Idle;
				needReset = true;
			}
			else
			{
				gameState = GameState::Idle;
				bullets.clear();
			}
		}
		ImGui::End();

		DrawColorPicker(bgColor);
		DrawPerfStats(deltaTime);
		DrawLogWindow();
		DrawMouseDebug(window);
		DrawKeyDebug(window);
		DrawSceneView(window, deltaTime);
		DrawInspector();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}
	ShutdownImGui();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}