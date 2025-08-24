#pragma once
#include "Engine/Core/Application.h"
#include "Engine/Core/Time.h"
#include "Engine/Input/Actions.h"
#include "Engine/Rendering/Renderer2D.h"
#include "imgui.h"
#include <vector>
#include <string>
#include <random>

class GameApp : public Engine::Core::Application
{
public:
	void OnInit(GLFWwindow* window) override;
	void OnUpdate(float deltaTime) override;
	void OnImGuiRender() override;
	void OnShutdown();

private:
	void DrawPerfStats();
	void DrawLogWindow();

private:
	void DrawSceneView(float deltaTime);
	void ResetRun(const ImVec2& avail);
	struct Rect { float x, y, w, h;		ImVec4 color; };
	struct Bullet { float x, y, w, h;		float vx, vy;ImVec4 color; };
	bool OverlapRB(const Rect& a, const Bullet& b);

private:
	bool playMode = false;
	bool needReset = false;
	enum class GameState { Idle, Playing, GameOver } gameState = GameState::Idle;

	Engine::Core::Time gTime;

	Rect player{ 380, 280, 24, 24, ImVec4(1, 1, 0, 1) };

	std::vector<Bullet> bullets;

	float spawnTimer = 0.0f;
	float spawnRate = 3.0f;
	float scoreSec = 0.0f;

	std::vector<std::string> logs;

	GLFWwindow* window = nullptr;
	float bgColor[3] = { 0.2f, 0.3f, 0.4f };
};