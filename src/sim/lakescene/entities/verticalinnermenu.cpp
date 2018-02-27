#include <sim/lakescene/entities/verticalinnermenu.h>

namespace sim
{
	namespace lake
	{
		namespace entity
		{
			VerticalInnerMenuEntity::VerticalInnerMenuEntity(
				std::shared_ptr<model::menu::VerticalInnerMenu> model,
				const glm::vec4& inactiveBackgroundColor,
				const glm::vec4& selectedBackgroundColor,
				const glm::vec4& disabledBackgroundColor,
				const glm::vec4& inactiveTextColor,
				const glm::vec4& selectedTextColor,
				const glm::vec4& disabledTextColor,
				const glm::vec2& overMenuWorldDimensions,
				const glm::vec2& overMenuLogicalSize
			)
				: model_(model)
				, inactiveBackgroundColor_(inactiveBackgroundColor)
				, selectedBackgroundColor_(selectedBackgroundColor)
				, disabledBackgroundColor_(disabledBackgroundColor)
				, inactiveTextColor_(inactiveTextColor)
				, selectedTextColor_(selectedTextColor)
				, disabledTextColor_(disabledTextColor)
				, scrollTransform_(nullptr)
				, submenuTransform_(nullptr)
				, worldDimensions_(overMenuWorldDimensions)
				, worldMenuLogicalSize_(overMenuLogicalSize)
				, inactiveEntities_()
				, selectedEntities_()
				, disabledEntities_()
			{}

			VerticalInnerMenuEntity::~VerticalInnerMenuEntity()
			{
				release();
			}

			void VerticalInnerMenuEntity::setScroll(float logicalScrollAmt)
			{
				scrollTransform_->pos(glm::vec3(0.f, logicalScrollAmt, 0.f));
			}

			// TODO SESS: Resume here
		}
	}
}