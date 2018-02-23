#include <sim/lakescene/entities/verticalsubmenu.h>
#include <view/rawentities/planegenerator.h>

namespace sim
{
	namespace lake
	{
		namespace entity
		{
			VerticalSubmenuEntity::VerticalSubmenuEntity(
				model::menu::VerticalSubmenu model,
				const glm::vec4& inactiveBackgroundColor,
				const glm::vec4& selectedBackgroundColor,
				const glm::vec4& disabledBackgroundColor,
				const glm::vec4& inactiveTextColor,
				const glm::vec4& selectedTextColor,
				const glm::vec4& disabledTextColor,
				const glm::vec2& overMenuWorldDimensions,
				const glm::vec2& overMenuLogicalSize
			)
				: model::WithWorldTransform(glm::vec3(0.f, 0.f, 0.f), glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.f)), glm::vec3(1.f, 1.f, 1.f))
				, model_(model)
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
				, disabledEntities_()
				, selectedEntities_()
				, log(util::DEBUG, util::WARN, "[VerticalSubmenuEntity] ")
			{}

			VerticalSubmenuEntity::~VerticalSubmenuEntity()
			{
				release();
			}

			void VerticalSubmenuEntity::setScroll(float logicalScrollAmt)
			{
				auto worldScrollAmount = (logicalScrollAmt / model_.rect().Size.y) * worldDimensions_.y;
				scrollTransform_->pos(glm::vec3(0.f, worldScrollAmount, 0.f));
			}

			bool VerticalSubmenuEntity::prepare(
				std::shared_ptr<view::solidshader::SolidShader> solidShader,
				std::shared_ptr<view::text::MSDFTextShader> textShader,
				util::PipelineState& pso,
				const model::text::TextAtlas& atlas,
				std::shared_ptr<model::WithWorldTransform> menuTransform,
				std::shared_ptr<view::Texture> textAtlasTexture
			) {
				// Release anything that may have been previously prepared
				release();

				// Offset to put 0,0 at the upper-left hand corner of the menu (in world space)
				submenuTransform_ = std::make_shared<model::WithWorldTransform>(
					glm::vec3(-worldDimensions_.x / 2.f, worldDimensions_.y / 2.f, 0.f),
					glm::angleAxis(0.f, glm::vec3(0.f, 0.f, 0.f)),
					glm::vec3(1.f, 1.f, 1.f)
				);

				// Offset for the text to put its upper left hand corner in the right place
				auto textOffset = modelToWorldSize(glm::vec2(0.f, -model_.submenuEntryHeight()));
				auto textTransform = std::make_shared<model::WithWorldTransform>(
					glm::vec3(textOffset, 0.f), glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.f)), glm::vec3(1.f, 1.f, 1.f)
				);

				for (auto&& menuItem : model_.getMenuItems())
				{
					// Transformation to put the upper-left corner of the background where it should be
					auto upperLeft = modelToWorldLocation(menuItem.Location.TopLeftPos);
					auto size = modelToWorldSize(menuItem.Location.Size);
					auto backgroundTransform = std::make_shared<model::WithWorldTransform>(
						glm::vec3(size.x * 0.5f, -size.y, 0.f),
						glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.f)),
						glm::vec3(1.f, 1.f, 1.f)
					);

					//
					// Inactive entity
					//
					MenuItemEntity inactiveEntity;
					inactiveEntity.BackgroundEntity = std::make_shared<view::solidshader::GenericSolidEntity>(glm::vec3(upperLeft.x, upperLeft.y, 0.01f), glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.)), glm::vec3(1.f, 1.f, 1.f));
					inactiveEntity.BackgroundEntity->addMesh(std::make_shared<view::GenericMesh>(view::raw::PlaneGenerator::getGenericMeshData(
						model::geo::Plane(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f)),
						glm::vec3(0.f, 1.f, 0.f),
						size
					)), inactiveBackgroundColor_);
					if (!inactiveEntity.BackgroundEntity->prepare(solidShader, pso))
					{
						log.error << "Failed to prepare inactive background entity for menu item " << menuItem.Entry->Name << util::endl;
						return false;
					}
					inactiveEntity.TextEntity = std::make_shared<view::text::MSDFStringEntity>(glm::vec3(upperLeft.x, upperLeft.y, 0.02f), glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.)), glm::vec3(1.f, 1.f, 1.f));
					{
						auto p1 = inactiveEntity.TextEntity->prepareCPUDeferrable(
							menuItem.Entry->Caption,
							size.y * 0.6f, size.y * 0.6f,
							atlas,
							inactiveTextColor_
						);
						if (!inactiveEntity.TextEntity->prepareFromDeferrable(p1, textShader, pso, textAtlasTexture))
						{
							log.error << "Failed to prepare inactive text entity for menu item " << menuItem.Entry->Name << util::endl;
							return false;
						}
					}

					//
					// Disabled entity
					//
					MenuItemEntity disabledEntity;
					disabledEntity.BackgroundEntity = std::make_shared<view::solidshader::GenericSolidEntity>(glm::vec3(upperLeft.x, upperLeft.y, 0.01f), glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.)), glm::vec3(1.f, 1.f, 1.f));
					disabledEntity.BackgroundEntity->addMesh(std::make_shared<view::GenericMesh>(view::raw::PlaneGenerator::getGenericMeshData(
						model::geo::Plane(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f)),
						glm::vec3(0.f, 1.f, 0.f),
						size
					)), disabledBackgroundColor_);
					if (!disabledEntity.BackgroundEntity->prepare(solidShader, pso))
					{
						log.error << "Failed to prepare inactive background entity for menu item " << menuItem.Entry->Name << util::endl;
						return false;
					}
					disabledEntity.TextEntity = std::make_shared<view::text::MSDFStringEntity>(glm::vec3(upperLeft.x, upperLeft.y, 0.02f), glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.)), glm::vec3(1.f, 1.f, 1.f));
					{
						auto p1 = disabledEntity.TextEntity->prepareCPUDeferrable(
							menuItem.Entry->Caption,
							size.y * 0.6f, size.y * 0.6f,
							atlas,
							disabledTextColor_
						);
						if (!disabledEntity.TextEntity->prepareFromDeferrable(p1, textShader, pso, textAtlasTexture))
						{
							log.error << "Failed to prepare inactive text entity for menu item " << menuItem.Entry->Name << util::endl;
							return false;
						}
					}

					//
					// Selected entity
					//
					MenuItemEntity selectedEntity;
					selectedEntity.BackgroundEntity = std::make_shared<view::solidshader::GenericSolidEntity>(glm::vec3(upperLeft.x, upperLeft.y, 0.01f), glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.)), glm::vec3(1.f, 1.f, 1.f));
					selectedEntity.BackgroundEntity->addMesh(std::make_shared<view::GenericMesh>(view::raw::PlaneGenerator::getGenericMeshData(
						model::geo::Plane(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f)),
						glm::vec3(0.f, 1.f, 0.f),
						size
					)), selectedBackgroundColor_);
					if (!selectedEntity.BackgroundEntity->prepare(solidShader, pso))
					{
						log.error << "Failed to prepare inactive background entity for menu item " << menuItem.Entry->Name << util::endl;
						return false;
					}
					selectedEntity.TextEntity = std::make_shared<view::text::MSDFStringEntity>(glm::vec3(upperLeft.x, upperLeft.y, 0.02f), glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.)), glm::vec3(1.f, 1.f, 1.f));
					{
						auto p1 = selectedEntity.TextEntity->prepareCPUDeferrable(
							menuItem.Entry->Caption,
							size.y * 0.6f, size.y * 0.6f,
							atlas,
							selectedTextColor_
						);
						if (!selectedEntity.TextEntity->prepareFromDeferrable(p1, textShader, pso, textAtlasTexture))
						{
							log.error << "Failed to prepare inactive text entity for menu item " << menuItem.Entry->Name << util::endl;
							return false;
						}
					}

					submenuTransform_->setParent(menuTransform);

					backgroundTransform->setParent(submenuTransform_);
					textTransform->setParent(submenuTransform_);

					inactiveEntity.BackgroundEntity->setParent(backgroundTransform);
					inactiveEntity.TextEntity->setParent(textTransform);
					disabledEntity.BackgroundEntity->setParent(backgroundTransform);
					disabledEntity.TextEntity->setParent(textTransform);
					selectedEntity.BackgroundEntity->setParent(backgroundTransform);
					selectedEntity.TextEntity->setParent(textTransform);
					inactiveEntities_.insert({ menuItem.Entry->Name, inactiveEntity });
					disabledEntities_.insert({ menuItem.Entry->Name, disabledEntity });
					selectedEntities_.insert({ menuItem.Entry->Name, selectedEntity });
				}

				return true;
			}

			bool VerticalSubmenuEntity::release()
			{
				for (auto&& inactiveEntities : inactiveEntities_)
				{
					inactiveEntities.second.BackgroundEntity->release();
					inactiveEntities.second.TextEntity->release();
				}

				for (auto&& disabledEntities : disabledEntities_)
				{
					disabledEntities.second.BackgroundEntity->release();
					disabledEntities.second.TextEntity->release();
				}

				for (auto&& selectedEntities : selectedEntities_)
				{
					selectedEntities.second.BackgroundEntity->release();
					selectedEntities.second.TextEntity->release();
				}

				return true;
			}

			void VerticalSubmenuEntity::renderBackground(std::shared_ptr<view::solidshader::SolidShader> solidShader)
			{
				for (auto&& menuItem : model_.getMenuItems())
				{
					switch (menuItem.Entry->State)
					{
					case model::menu::SubmenuEntry::DISABLED:
					{
						auto entity = disabledEntities_.find(menuItem.Entry->Name);
						if (entity != disabledEntities_.end())
						{
							entity->second.BackgroundEntity->render(solidShader);
						}
					}
					break;
					case model::menu::SubmenuEntry::INACTIVE:
					default:
					{
						auto entity = inactiveEntities_.find(menuItem.Entry->Name);
						if (entity != inactiveEntities_.end())
						{
							entity->second.BackgroundEntity->render(solidShader);
						}
					}
					break;
					case model::menu::SubmenuEntry::SELECTED:
					{
						auto entity = selectedEntities_.find(menuItem.Entry->Name);
						if (entity != selectedEntities_.end())
						{
							entity->second.BackgroundEntity->render(solidShader);
						}
					}
					break;
					}
				}
			}

			void VerticalSubmenuEntity::renderText(std::shared_ptr<view::text::MSDFTextShader> textShader)
			{
				for (auto&& menuItem : model_.getMenuItems())
				{
					switch (menuItem.Entry->State)
					{
					case model::menu::SubmenuEntry::DISABLED:
					{
						auto entity = disabledEntities_.find(menuItem.Entry->Name);
						if (entity != disabledEntities_.end())
						{
							entity->second.TextEntity->render(textShader);
						}
					}
					break;
					case model::menu::SubmenuEntry::INACTIVE:
					default:
					{
						auto entity = inactiveEntities_.find(menuItem.Entry->Name);
						if (entity != inactiveEntities_.end())
						{
							entity->second.TextEntity->render(textShader);
						}
					}
					break;
					case model::menu::SubmenuEntry::SELECTED:
					{
						auto entity = selectedEntities_.find(menuItem.Entry->Name);
						if (entity != selectedEntities_.end())
						{
							entity->second.TextEntity->render(textShader);
						}
					}
					break;
					}
				}
			}

			glm::vec2 VerticalSubmenuEntity::modelToWorldLocation(const glm::vec2& modelPoint) const
			{
				return glm::vec2(
					modelPoint.x * (worldDimensions_.x / worldMenuLogicalSize_.x),
					-modelPoint.y * (worldDimensions_.y / worldMenuLogicalSize_.y)
				);
			}

			glm::vec2 VerticalSubmenuEntity::modelToWorldSize(const glm::vec2& modelPoint) const
			{
				return glm::vec2(
					modelPoint.x * (worldDimensions_.x / worldMenuLogicalSize_.x),
					modelPoint.y * (worldDimensions_.y / worldMenuLogicalSize_.y)
				);
			}
		}
	}
}