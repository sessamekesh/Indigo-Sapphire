#include <model\geo\rectangle.h>

namespace model
{
	namespace geo
	{
		Rectangle::Rectangle(
			const glm::vec3& pos,
			const glm::quat& rot,
			float width,
			float height
		)
			: WithWorldTransform(pos, rot, glm::vec3(1.f, 1.f, 1.f))
			, width_(width)
			, height_(height)
		{}

		float Rectangle::width() const
		{
			return width_;
		}

		float Rectangle::height() const
		{
			return height_;
		}
	}
}