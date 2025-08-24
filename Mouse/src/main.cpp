#include "Engine/Core/EntryPoint.h"
#include "Game/GameApp.h"

int main() 
{
	auto* app = new GameApp();
	Engine::Core::Run(app);
	return 0;
}