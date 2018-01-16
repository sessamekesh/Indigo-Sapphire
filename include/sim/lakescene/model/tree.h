#pragma once

#include <model/withworldtransform.h>

namespace sim
{
	namespace lake
	{
		class TreeModel : public model::WithWorldTransform
		{
		public:
			enum TREE_TYPE
			{
				TREE_TYPE_0
			};

		public:
			TreeModel(TREE_TYPE type, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scl);
			~TreeModel() = default;
			TreeModel(const TreeModel&) = default;

			TREE_TYPE type() const;

		private:
			TREE_TYPE type_;
		};
	}
}