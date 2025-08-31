#include "Engine/Core/EntryPoint.h"
#include "Game/GameApp.h"
#include "Engine/ECS/Registry.h"
#include "Engine/Components/Transform.h"

static void __ecs__registry_smoketest()
{
	Engine::ECS::Registry reg;
	auto e = reg.Create();
	reg.Pool<Engine::Components::Transform>().Emplace(e);
}

int main() 
{
	auto* app = new GameApp();
	Engine::Core::Run(app);
	return 0;
}