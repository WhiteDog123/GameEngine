#pragma once
#include <GLFW/glfw3.h>

namespace Engine::Input
{
	enum class Action
	{
		MoveUp,
		MoveDown,
		MoveLeft,
		MoveRight,
		Restart,
		COUNT
	};

	struct InputState
	{
		bool held[(int)Action::COUNT]{};
	};

	inline InputState PollInput(GLFWwindow* w)
	{
		InputState s{};
		s.held[(int)Action::MoveUp] = glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(w, GLFW_KEY_UP) == GLFW_PRESS;
		s.held[(int)Action::MoveDown] = glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(w, GLFW_KEY_DOWN) == GLFW_PRESS;
		s.held[(int)Action::MoveLeft] = glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(w, GLFW_KEY_LEFT) == GLFW_PRESS;
		s.held[(int)Action::MoveRight] = glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(w, GLFW_KEY_RIGHT) == GLFW_PRESS;
		s.held[(int)Action::Restart] = glfwGetKey(w, GLFW_KEY_R) == GLFW_PRESS;
		return s;
	}
}