#pragma once
#include <vector>
#include "Entity.h"

namespace Engine::ECS
{
	template <typename T>
	struct ComponentPool
	{
		std::vector<T> data;
		std::vector<Entity> owner;

		T* Get(Entity e)
		{
			for (size_t i = 0; i < owner.size(); ++i)
			{
				if (owner[i] == e) return &data[i];
			}
			return nullptr;
		}

		bool Has(Entity e) const
		{
			for (auto id : owner)
			{
				if (id == e)
				{
					return true;
				}
			}
			return false;
		}

		T& Emplace(Entity e, const T& v = T{})
		{
			owner.push_back(e);
			data.push_back(v);
			return data.back();
		}

		void Remove(Entity e)
		{
			for (size_t i = 0; i < owner.size(); ++i)
			{
				if (owner[i] == e)
				{
					owner[i] = owner.back();
					owner.pop_back();
					data[i] = data.back();
					data.pop_back();
					return;
				}
			}
		}
	};

	struct Registry
	{
		Entity next{ 1 };
		std::vector<Entity> alive;

		Entity Create()
		{
			alive.push_back(next);
			return next++;
		}

		void Destroy(Entity e)
		{
			for (size_t i = 0; i < alive.size(); ++i)
			{
				if (alive[i] == e)
				{
					alive[i] = alive.back();
					alive.pop_back();
					break;
				}
			}
		}

		template <typename T>
		ComponentPool<T>& Pool() 
		{
			static ComponentPool<T> pool;
			return pool;
		}
	};
}