#include "GameApp.h"
#include <GLFW/glfw3.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>

void GameApp::OnInit(GLFWwindow* window)
{
	GameApp::window = window;
}

void GameApp::OnUpdate(float deltaTime)
{
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

	DrawSceneView(deltaTime);
}
void GameApp::OnImGuiRender()
{
	DrawPerfStats();
	DrawLogWindow();
}
void GameApp::DrawPerfStats()
{
	ImGui::Begin("Performance");
	float fps = 1.0f / gTime.delta;
	float frameTimeMs = gTime.delta * 1000.0f;
	ImGui::Text("FPS: %.1f", fps);
	ImGui::Text("Frame Time: %.2f ms", frameTimeMs);
	ImGui::End();
}
void GameApp::DrawLogWindow()
{
	ImGui::Begin("Log");
	if (ImGui::Button("Clear"))
	{
		logs.clear();
	}

	ImGui::BeginChild("LogRegion", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
	for (const auto& line : logs)
	{
		ImGui::TextUnformatted(line.c_str());
	}
	ImGui::EndChild();
	ImGui::End();
}

void GameApp::DrawSceneView(float deltaTime)
{
	ImGui::Begin("Scene", nullptr);

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
				player.x = std::max(0.0f, std::min(player.x, avail.x - player.w));
				player.y = std::max(0.0f, std::min(player.y, avail.y - player.h));

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
					if (OverlapRB(player, b))
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


bool GameApp::OverlapRB(const Rect& a, const Bullet& b)
{
	return !(a.x + a.w < b.x || b.x + b.w < a.x ||
		a.y + a.h < b.y || b.y + b.h < a.y);
}

void GameApp::ResetRun(const ImVec2& avail)
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

void GameApp::OnShutdown()
{

}