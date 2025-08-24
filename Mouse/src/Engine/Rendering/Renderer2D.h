#pragma once
#include "imgui.h"

namespace Engine::Rendering
{
	struct Renderer2D
	{
		void Begin(ImDrawList* dl, ImVec2 origin)
		{
			draw = dl;
			p0 = origin;
		}

		void Rect(float x, float y, float w, float h, ImU32 col)
		{
			ImVec2 a
			{
				p0.x + x,
				p0.y + y
			};
			ImVec2 b
			{
				a.x + w,
				a.y + h
			};
			draw->AddRectFilled(a, b, col);
		}

		void RectOutline(float x, float y, float w, float h, ImU32 col, float thickness = 1.f)
		{
			ImVec2 a
			{
				p0.x + x,
				p0.y + y
			};
			ImVec2 b
			{
				a.x + w,
				a.y + h
			};
			draw->AddRect(a, b, col, 0.0f, 0, thickness);
		}

		void Text(ImVec2 localPos, ImU32 col, const char* text)
		{
			draw->AddText(ImVec2(p0.x + localPos.x, p0.y +localPos.y), col, text);
		}

		void End() {}

		ImDrawList* draw{};
		ImVec2 p0{};
	};
}