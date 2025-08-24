#pragma once
#include <algorithm>

namespace Engine::Core
{
	struct Time
	{
		float delta = 0.f;
		float total = 0.f;
		float fixedStep = 1.f / 120.f;
		float accumulator = 0.f;
		float maxAccum = 0.25f;
		int maxStepsPerFrame = 12;
	};

	template <typename StepFn>
	inline int StepFixed(Time& t, float frameDelta, StepFn&& step)
	{
		t.delta = frameDelta;
		t.total += frameDelta;
		t.accumulator += frameDelta;

		if (t.accumulator > t.maxAccum)
		{
			const int maxSteps = std::max(1, (int)(t.maxAccum / t.fixedStep));
			t.accumulator = maxSteps * t.fixedStep;
		}

		int steps = 0;
		while (t.accumulator >= t.fixedStep && steps < t.maxStepsPerFrame)
		{
			step(t.fixedStep);
			t.accumulator -= t.fixedStep;
			++steps;
		}
		if (steps == t.maxStepsPerFrame) t.accumulator = 0.f;
		return steps;
	}
}