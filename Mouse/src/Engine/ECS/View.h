#pragma once
#include "Registry.h"

namespace Engine::ECS
{
	template <typename A, typename B>
	struct View2
	{
		ComponentPool<A>& pa;
		ComponentPool<B>& pb;
		template <typename Fn>
		void Each(Fn&& fn)
		{
			for (size_t i = 0; i < pa.owner.size(); ++i)
			{
				auto e = pa.owner[i];
				if (auto* b = pb.Get(e))
				{
					fn(e, pa.data[i], *b);
				}
			}
		}
	};

	template <typename A, typename B>
	View2<A, B> View(Registry& reg)
	{
		return {
			reg.Pool<A>(), reg.Pool<B>();
		}
	}
}