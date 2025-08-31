#pragma once
#include <cstdint>
namespace Engine::ECS
{
	//uint32_t는 고정된 폭을 가진 부호 없는 정수 타입
	//항상 32비트. 컴파일러가 달라도 크기가 보장됨.
	//Entity는 식별자로 쓰이기 때문에, 직렬화나 동기화에서 호환성을 보장하기 위해 고정된 폭을 사용하기로 함.
	//using과 typedef는 큰 차이 없지만 using은 템플릿도 쓸 수 있는 등 소소한 이점이 있음. 가독성도 조금 더 좋은 듯.
	using Entity = uint32_t;
	//constexpr은 컴파일 타임 상수. 컴파일 타임에 계산되는 값이됨.
	//inline과 함께 하면 '헤더 전역 상수'처럼 사용할 수 있음. 
	//kInvalid는 컴파일 타임에서 부터 0으로 정의하고 싶음.
	//constexpr만 쓰면 ODR 위반이 날 수 있음.
	//inline을 붙여서 정의가 여러번 나오더라도 같ㅌ은 정의는 하나로 취급하게 됨.
	inline constexpr Entity kInvalid = 0;
}