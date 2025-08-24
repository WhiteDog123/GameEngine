#pragma once
#include "Engine/Core/Application.h"

class GameApp : public Engine::Core::Application
{
public:
	void OnInit() override;
	void OnUpdate(float dt) override;
	void OnImGuiRender() override;
};