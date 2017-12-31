#pragma once

#include <glm/glm.hpp>
#include <model/withworldtransform.h>

namespace model
{
	namespace geo
	{
		class Rectangle : public WithWorldTransform
		{
		public:
			// Width: x, Height: y
			Rectangle(
				const glm::vec3& pos,
				const glm::quat& rot,
				float width,
				float height
			);
			Rectangle(const Rectangle&) = default;
			~Rectangle() = default;

			glm::vec3 pos() const;
			float width() const;
			float height() const;

		protected:
			float width_;
			float height_;
		};
	}
}