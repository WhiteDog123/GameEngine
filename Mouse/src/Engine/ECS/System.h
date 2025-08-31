#pragma once
namespace Engine::ECS
{
	//ISystem은 Registry를 참조로만 사용하고 있기 때문에, #include보다 전방선언만으로 충분하다.
	struct Registry;
	struct ISystem
	{
		virtual void Update(Registry& reg, float dt) = 0;
		virtual ~ISystem() = default;
	};
}