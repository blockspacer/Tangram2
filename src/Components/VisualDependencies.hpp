#pragma once

#include <entt/entt.hpp>
#include <cereal/access.hpp>
#include <cereal/types/vector.hpp>

namespace Cmp {
struct VisualDependencies {
	std::vector<entt::entity> list;

private:
	//Serialization
	friend class cereal::access;
	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(list);
	}
};
}