#include <sim/lakescene/model/tree.h>

namespace sim
{
	namespace lake
	{
		TreeModel::TreeModel(
			TREE_TYPE type,
			const glm::vec3& pos,
			const glm::quat& rot,
			const glm::vec3& scl
		)
			: model::WithWorldTransform(pos, rot, scl)
			, type_(type)
		{}

		TreeModel::TREE_TYPE TreeModel::type() const
		{
			return type_;
		}
	}
}