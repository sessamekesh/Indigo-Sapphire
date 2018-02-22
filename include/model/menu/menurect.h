#pragma once

/**
 * Menu Rect
 *
 * Logical representation of a rectangle in menu space. It has a top, left, width, and height. That's it.
 * X coordinates go left-to-right, with 0 being the far left
 * Y coordinates go top-to-bottom, with 0 being the far top
 */

#include <glm/glm.hpp>

namespace model
{
	namespace menu
	{
		struct MenuRect
		{
			glm::vec2 TopLeftPos;
			glm::vec2 Size;
		};
	}
}